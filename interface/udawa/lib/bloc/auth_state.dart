part of 'auth_bloc.dart';

@immutable
sealed class AuthState {}

final class AuthInitial extends AuthState {}

final class AuthDeviceScannerStarted extends AuthState {}

final class AuthDeviceScannerOnProcess extends AuthState {
  final MdnsDevice mdnsDevice;
  AuthDeviceScannerOnProcess({required this.mdnsDevice});
}

final class AuthDeviceScannerFinished extends AuthState {
  final List mdnsDeviceList;
  AuthDeviceScannerFinished({required this.mdnsDeviceList});
}

final class AuthLocalStarted extends AuthState {}

final class AuthLocalError extends AuthState {
  final String error;

  AuthLocalError({required this.error});
}
