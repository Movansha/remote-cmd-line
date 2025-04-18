# ![rcl-icon](https://github.com/user-attachments/assets/ef9cd46a-e362-4b91-adcf-e210610037ab) Remote Command Line

Basic remote control application for Windows

This app allows to do these on a remote PC:

- ⚙️ Execute system commands
- 🔊 Play beeps
- ✉️ Send messages (with Windows's MessageBox)
- 📷 Take screenshots

> #### Used in this project:
> - Language: C++ (17)
> - Library: nlohmann/json (3.11.3)
> - Library: chriskohlhoff/asio (1.30.2)
> - Framework: Qt (6.7.3)

> [!CAUTION]
> Don't use "remote-hidden-client" as a remote access trojan!
> This project has been developed for legal use only.

### [*` >> Download for Windows `*](https://github.com/Movansha/remote-cmd-line/releases/latest)


# Screenshots

![rcl-server-533](https://github.com/user-attachments/assets/e21894ac-7c13-4ef4-b523-7e63bb997d08)

![rcl-client](https://github.com/user-attachments/assets/2ce48291-b220-4d41-857c-50eb6858e949)


#
> ### Easy to Use (Step by Step)
> #### Server:
> - Make sure your ISP does not perform CGNAT
> - Open a port on your router for port forwarding
> - You may use Duck DNS to use a domain if you want
> - Download and install the controller app ---> [for Windows](https://github.com/Movansha/remote-cmd-line/releases/latest)
> - Run the application
> - Change the port number and password
> - Click "Start server" button
> 
> #### Client:
> - Download and install the client app ---> [for Windows](https://github.com/Movansha/remote-cmd-line/releases/latest)
> - Run the application
> - Click "Connection preferences" button
> - Enter the server's IP adress, port number and password
> - Click "Connect" button

> [!TIP]
> "remote-hidden-client" runs in the background and always stay connected.
> 
> If you will use it; change the IP adress, port number and password in "config.json" file then run the executable.
