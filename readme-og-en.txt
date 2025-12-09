Miku Miku Online 0.4.1

// ******************************** WARNING *********************************** //
// ***
// *** This program is in beta. It may be unstable or have incomplete features.
// *** Specifications may also be subject to significant changes.
// ***
// ***************************************************************************** //

*** For models and motions included in the binary package, see mmd.txt.
*** For software license information, see license.txt.

=== About This Software

This is chat software that allows you to use models from MMD (Miku Miku Dance).

=== System Requirements
Windows XP, Vista, 7
2GB RAM
DirectX 9
Shader Model 2.0 or higher

=== How to Use

Launch Client.exe.
The server will launch simultaneously in the background.

* Login
You will be asked for a host address.
If you are connecting to another server, enter the server's address.

If you are waiting as the server or playing alone, enter 127.0.0.1.

* Name
By default, no name is set, so it will be ???.
Set a name using the command below.

* Logout
When logging out, close the entire client.
Please note that if you are waiting as the server, the server will also be terminated.

If you are in full-screen mode, press Alt + F4 to exit.

=== Commands

The following commands can be used in the chat window.

/nick [name] - Set your name (e.g., /nick Anonymous)

/model [model name] - Set your model (e.g., /model Akita Neru)

/escape - Reset your position if you end up in a strange place.

/2d6 - Roll two six-sided dice. You can specify any number before or after the "d."

/trip [password] - Set a trip.

=== Controls
* Keyboard
W - Forward
S - Backward
A - Turn Left
D - Turn Right
Space - Jump
Enter - Toggle Chat Input
Alt + F4 - Exit Client

F1 - Show Menu
F2 - Show Layout Menu

* Mouse
Left Drag - Rotate Character
Right Drag - Rotate Camera
Mouse Wheel - Zoom Camera

* Gamepad
Left Analog - Move
Right Analog - Camera Zoom
Button 1 - Jump
POV - Camera Rotation

Please connect the gamepad before launching the client.
Some gamepads may not be supported.

=== Port Opening
You may need to open ports to listen as a server.
External Port 39390 → Internal Port 39390 (TCP)
External Port 39390 → Internal Port 39390 (UDP)
For instructions on how to open ports, please refer to your router's documentation.

=== Standalone Server Launch
With Client.exe closed, running server/Server.exe will launch the server.
You can launch Client.exe in this state, but closing Client.exe will not terminate the server.
To shut down the server, close the Command Prompt window.

--
mailto:mmo@h2so5.net
http://m2op.net/
http://sourceforge.jp/projects/mmo/