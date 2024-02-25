part of 'auth_bloc.dart';

@immutable
sealed class AuthEvent {}

final class AuthDeviceScannerRequested extends AuthEvent {}

final class AuthLocalRequested extends AuthEvent {
  final String ip;
  final String webApiKey;

  AuthLocalRequested({
    required this.ip,
    required this.webApiKey,
  });
}
