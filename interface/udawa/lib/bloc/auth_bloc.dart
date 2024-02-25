import 'dart:io';

import 'package:bloc/bloc.dart';
import 'package:meta/meta.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:udawa/models/mdns_device_model.dart';
import 'package:multicast_dns/multicast_dns.dart';
import 'dart:convert';

part 'auth_event.dart';
part 'auth_state.dart';

class AuthBloc extends Bloc<AuthEvent, AuthState> {
  AuthBloc() : super(AuthInitial()) {
    on<AuthDeviceScannerRequested>(_onAuthDeviceScannerRequested);
  }

  void _onAuthDeviceScannerRequested(
    AuthDeviceScannerRequested event,
    Emitter<AuthState> emit,
  ) async {
    List deviceList = [];

    emit(AuthDeviceScannerStarted());

    final serviceType = '_http._tcp';
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
                MdnsDevice(name: ptr.domainName, address: ip.address, port: 80);
            deviceList.add(device);

            await Future.delayed(const Duration(milliseconds: 500), () {
              emit(AuthDeviceScannerOnProcess(mdnsDevice: device));
            });
          }
        }
      }
    } finally {
      client.stop();
    }

    await Future.delayed(const Duration(seconds: 1), () {
      emit(AuthDeviceScannerFinished(mdnsDeviceList: deviceList));
    });
  }
}
