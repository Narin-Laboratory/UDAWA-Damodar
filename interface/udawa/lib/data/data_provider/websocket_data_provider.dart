import 'dart:convert';
import 'package:flutter/widgets.dart';
import 'package:web_socket_channel/io.dart';

class WebSocketService extends ChangeNotifier {
  static final WebSocketService _instance = WebSocketService._internal();
  factory WebSocketService() => _instance;
  WebSocketService._internal();

  IOWebSocketChannel? _channel;
  bool _isConnected = false;

  bool get isConnected => _isConnected;

  String? _latestMessage;

  String? get latestMessage => _latestMessage;

  // Connect to your WebSocket server
  Future<void> connect(String url) async {
    try {
      _channel = IOWebSocketChannel.connect(url,
          connectTimeout: const Duration(seconds: 5));

      _channel!.stream.listen((message) {
        // Handle incoming messages
        _latestMessage = message;
        print(_latestMessage);
        try {
          final data = jsonDecode(_latestMessage ?? '');

          // Handle based on message type
          if (data['status'] && data['status']['code']) {
            if (data['status']['code'] == 200) {
              _isConnected = true;
            } else {
              _isConnected = false;
            }
          }
        } catch (e) {}

        // ... your message processing logic
        notifyListeners();
      }, onDone: () {
        _isConnected = false;
        // Handle connection closed
      }, onError: (error) {
        // Handle errors
        _isConnected = false;
      });
    } catch (e) {
      // Connection error handling
      _isConnected = false;
    }
  }

  void send(dynamic data) {
    if (_channel != null && data != null) {
      _channel!.sink.add(jsonEncode(data));
    }
  }

  void close() {
    if (_channel != null) {
      _isConnected = false;
      _channel!.sink.close();
    }
  }
}
