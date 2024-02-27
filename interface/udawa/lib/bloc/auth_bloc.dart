import 'package:bloc/bloc.dart';
import 'package:meta/meta.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:udawa/data/data_provider/websocket_data_provider.dart';
import 'package:udawa/models/mdns_device_model.dart';
import 'package:multicast_dns/multicast_dns.dart';
import 'package:crypto/crypto.dart';
import 'dart:convert';
import 'dart:math';

part 'auth_event.dart';
part 'auth_state.dart';

class AuthBloc extends Bloc<AuthEvent, AuthState> {
  final WebSocketService webSocketService;

  AuthBloc(this.webSocketService) : super(AuthInitial()) {
    on<AuthDeviceScannerRequested>(_onAuthDeviceScannerRequested);
    on<AuthLocalRequested>(_onAuthLocalRequested);
  }

  void _onAuthDeviceScannerRequested(
    AuthDeviceScannerRequested event,
    Emitter<AuthState> emit,
  ) async {
    List<MdnsDevice> deviceList = [];

    emit(AuthDeviceScannerStarted());

    const serviceType = '_http._tcp';
    final MDnsClient client = MDnsClient();
    await client.start();

    try {
      await for (final ptr in client.lookup<PtrResourceRecord>(
          ResourceRecordQuery.serverPointer(serviceType))) {
        await for (final srv in client.lookup<SrvResourceRecord>(
            ResourceRecordQuery.service(ptr.domainName))) {
          await for (final ip in client.lookup<IPAddressResourceRecord>(
              ResourceRecordQuery.addressIPv4(srv.target))) {
            MdnsDevice device =
                MdnsDevice(name: srv.name, address: ip.address, port: srv.port);
            print(device.port);
            if (!deviceList.contains(device)) {
              deviceList.add(device);
              emit(AuthDeviceScannerOnProcess(mdnsDevice: device));
            }
          }
        }
      }
    } catch (error) {
      emit(AuthDeviceScannerError(error: error));
    } finally {
      client.stop();
    }

    emit(AuthDeviceScannerFinished(mdnsDeviceList: deviceList));
  }

  void _onAuthLocalRequested(
    AuthLocalRequested event,
    Emitter<AuthState> emit,
  ) async {
    if (event.ip == "") {
      emit(AuthLocalError(error: "Device IP Address can not be empty!"));
      return;
    }

    if (event.webApiKey == "") {
      emit(AuthLocalError(error: "Web API Key can not be empty!"));
      return;
    }

    emit(AuthLocalStarted());

    emit(
        AuthLocalOnProcess(message: "Trying to connect to device ${event.ip}"));

    try {
      await webSocketService.connect('ws://${event.ip}/ws');
    } catch (error) {
      emit(AuthLocalError(error: error.toString()));
    }

    String salt = _generateSalt();
    String auth = _generateAuth(salt, event.webApiKey);
    Map<String, dynamic> authObject = {
      'salt': salt,
      'auth': auth,
    };

    webSocketService.addListener(() {
      // Decode the JSON (if necessary)
      try {
        final data = jsonDecode(webSocketService.latestMessage ?? '');

        if (webSocketService.isConnected) {
          emit(AuthLocalSuccess());
        } else {
          emit(AuthLocalError(
              error:
                  "Failed to connect to ${event.ip} (${webSocketService.latestMessage ?? ''}). Make sure you are connected to the same network and try scanning again."));
        }
      } catch (e) {}
    });

    emit(AuthLocalOnProcess(message: "Sending an authentication request..."));
    webSocketService.send(authObject);
  }

  String _generateSalt() {
    final Random random = Random.secure();
    var values = List<int>.generate(32, (i) => random.nextInt(256));
    return base64Url.encode(values);
  }

  String _generateAuth(String salt, String apiKey) {
    final key = utf8.encode(apiKey);
    final bytes = utf8.encode(salt);
    final hmacSha256 = Hmac(sha256, key);
    final digest = hmacSha256.convert(bytes);

    return digest.toString();
  }
}
