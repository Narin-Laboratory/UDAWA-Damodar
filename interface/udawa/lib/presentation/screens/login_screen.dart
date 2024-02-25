import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:udawa/bloc/auth_bloc.dart';
import 'package:udawa/models/mdns_device_model.dart';
import 'package:udawa/presentation/widgets/login_field_widget.dart';

class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  State<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final webApiKeyController = TextEditingController();
  final deviceIpAddressController = TextEditingController();

  @override
  void dispose() {
    webApiKeyController.dispose();
    deviceIpAddressController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        body: BlocConsumer<AuthBloc, AuthState>(
      listener: (context, state) {
        // TODO: implement listener
        if (state is AuthLocalError) {
          ScaffoldMessenger.of(context).showSnackBar(
            SnackBar(
              content: Text("Failed to connect: ${state.error}"),
            ),
          );
        }

        if (state is AuthLocalStarted) {
          ScaffoldMessenger.of(context).showSnackBar(
            SnackBar(
              content: Text(
                  "Trying to connect to ${deviceIpAddressController.text}"),
            ),
          );
        }
      },
      builder: (context, state) {
        return Scaffold(
            // Assuming you want to position it within a standard screen layout
            body: LayoutBuilder(builder: (context, constraints) {
          double maxWidth =
              constraints.maxWidth > 800 ? 800 : constraints.maxWidth;
          return Center(
            // Center the content within the Scaffold's body
            child: ConstrainedBox(
              constraints: BoxConstraints(maxWidth: maxWidth),
              child: Padding(
                padding: const EdgeInsets.all(20.0),
                child: SingleChildScrollView(
                  child: Column(
                    mainAxisAlignment:
                        MainAxisAlignment.center, // Center vertically
                    children: [
                      SizedBox(
                        width: 90.0, // Set your desired width
                        height: 90.0, // Set your desired height
                        child: Image.asset('assets/images/logo.png'),
                      ),
                      const SizedBox(height: 10),
                      const Text("UDAWA Smart System",
                          style: TextStyle(
                            fontWeight: FontWeight.w100,
                            fontSize: 22,
                            color: Colors.green,
                          )),
                      const SizedBox(height: 15),
                      LoginField(
                        labelText: "Device IP Address",
                        onChanged: (value) {},
                        controller: deviceIpAddressController,
                      ),
                      const SizedBox(height: 15),
                      LoginField(
                        labelText: "Web Api Key",
                        obscureText: true,
                        onChanged: (value) {},
                        controller: webApiKeyController,
                      ),
                      const SizedBox(height: 25),
                      Row(
                        children: [
                          Expanded(
                            child: TextButton.icon(
                                onPressed: () async {
                                  final selectedDevice =
                                      await showDialog<MdnsDevice>(
                                    context: context,
                                    builder: (context) =>
                                        const SelectionPopup(),
                                  );

                                  if (selectedDevice != null) {
                                    // Do something with the selectedItem
                                    deviceIpAddressController.text =
                                        selectedDevice.address.address;
                                  }
                                },
                                icon: Icon(Icons.search),
                                label: Text("Search")),
                          ),
                          const SizedBox(
                              width: 10), // Add spacing between buttons
                          Expanded(
                            child: ElevatedButton.icon(
                                onPressed: () {
                                  context.read<AuthBloc>().add(
                                      AuthLocalRequested(
                                          ip: deviceIpAddressController.text,
                                          webApiKey: webApiKeyController.text));
                                },
                                icon: Icon(Icons.connect_without_contact_sharp),
                                label: Text("Connect")),
                          ),
                        ],
                      )
                    ],
                  ),
                ),
              ),
            ),
          );
        }));
      },
    ));
  }
}

class SelectionPopup extends StatefulWidget {
  const SelectionPopup({Key? key}) : super(key: key);

  @override
  _SelectionPopupState createState() => _SelectionPopupState();
}

class _SelectionPopupState extends State<SelectionPopup> {
  @override
  void initState() {
    super.initState();
    context.read<AuthBloc>().add(AuthDeviceScannerRequested());
  }

  @override
  Widget build(BuildContext context) {
    return BlocBuilder<AuthBloc, AuthState>(
      builder: (context, state) {
        if (state is AuthDeviceScannerStarted) {
          return const AlertDialog(
              title: Text("Device Scanner"),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [Text("Starting Device Scanner...")],
              ));
        } else if (state is AuthDeviceScannerOnProcess) {
          return AlertDialog(
              title: Text("Scanning in Progress"),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                      "Found: ${state.mdnsDevice.name} at ${state.mdnsDevice.address.address}")
                ],
              ));
        } else if (state is AuthDeviceScannerFinished) {
          return AlertDialog(
            title: const Text("Select a Device"),
            content: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                for (final device in state.mdnsDeviceList)
                  ListTile(
                    title: Text("${device.name} - ${device.address.address}"),
                    onTap: () {
                      Navigator.pop(context, device);
                    },
                  ),
              ],
            ),
            actions: [
              TextButton(
                onPressed: () => Navigator.pop(context),
                child: const Text('Cancel'),
              ),
              ElevatedButton(
                onPressed: () {
                  context.read<AuthBloc>().add(AuthDeviceScannerRequested());
                },
                child: const Text('Rescan'),
              ),
            ],
          );
        } else {
          return const AlertDialog(
              title: Text("Select a Device"),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [Text("Unknown error has occured!")],
              ));
        }
      },
    );
  }
}
