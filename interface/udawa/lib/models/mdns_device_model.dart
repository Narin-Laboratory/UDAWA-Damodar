import 'dart:io';

// A class to represent a single mDNS device
class MdnsDevice {
  final String name;
  final InternetAddress address;
  final int port;
  final Map<String, String> attributes; // For additional device information

  MdnsDevice({
    required this.name,
    required this.address,
    required this.port,
    this.attributes = const {},
  });
}
