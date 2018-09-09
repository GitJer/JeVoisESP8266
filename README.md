# JeVoisESP8266

This project adds wireless serial communication to the [JeVois Smart Machine vision Camera](http://jevois.org/) using an ESP8266.

It consists of an ESP8266, in my case a WEMOS d1 mini, connected to the JeVois camera as shown below:

![Serial connections](./serial_connections.png = 150x150)

# Configuration
This project uses [PlatformIO](https://platformio.org/), but it should be easy to use e.g. the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

## Wifi credentials
Your credentials for the wifi connection are needed. Edit the file 'jevoisEsp8266.ino' and change:
```
char* ssid = "YOUR SSID";
char* password = "YOUR PASSWORD";
```

## Set your IP address
I use a fixed IP address, you may want to change it to your needs. Edit the file 'jevoisEsp8266.ino' and change:
```
IPAddress ip(192, 168, 1, 115);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);
```
In the file 'getData.py' you have to change:
```
url = "http://192.168.1.115/data"
```
And in the file 'sendCommands.py' you have to change:
```
url = "http://192.168.1.115/set?cmd=" + urllib.quote(command)
```

## Over the air updating
I also use Over The Air (OTA) updating to program the ESP8266, so also edit the file 'platformio.ini' and change:
```
upload_port = 192.168.1.115
```
To make the OTA more secure a password is used. You should also change that. Edit 'jevoisEsp8266.ino' and change:
```  
ArduinoOTA.setPassword((const char *)"passwd");
```
And edit 'platform.ini' to change:
```  
--auth="passwd"
```

# Usage (on linux)
- Start the Camera and ESP8266 by connecting them to USB chargers. The ESP8266 flashes its built-in LED while it is not connected to the wifi network.
- In a terminal, start the script to read the serial data: 'python getData.py'
- Edit the script 'sendCommands.py' to send the right commands to make the camera do what you need, then, in another terminal, start it with 'python sendCommands.py'. In the current file it starts the 'DemoArUco' demo.

In the terminal where you started 'getData.py' you should now see the serial output of 'DemoArUco' as well as the camera's response to the commands. I noticed that I sometimes receive errors from the camera, just start the 'sendCommands.py' script again.

You can also use a browser and enter for example the following urls:
```
http://192.168.1.115/set?cmd=info
http://192.168.1.115/set?cmd=setpar serstyle Detail
```
Note that the spaces between the command and its parameters will be replaced by '%20'. Again, the output shows up in the terminal.
