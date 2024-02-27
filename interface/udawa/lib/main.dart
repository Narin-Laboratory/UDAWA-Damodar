import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:udawa/app_bloc_observer.dart';
import 'package:udawa/bloc/auth_bloc.dart';
import 'package:udawa/data/data_provider/websocket_data_provider.dart';
import 'package:udawa/presentation/screens/login_screen.dart';
import 'package:provider/provider.dart';

void main() {
  Bloc.observer = AppBlocObserver();
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MultiBlocProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => WebSocketService()),
        BlocProvider(
          create: (context) =>
              AuthBloc(Provider.of<WebSocketService>(context, listen: false)),
        )
      ],
      child: MaterialApp(
        title: 'UDAWA Smart System',
        theme: ThemeData.dark(
          //colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
          useMaterial3: true,
        ),
        home: const LoginScreen(),
      ),
    );
  }
}
