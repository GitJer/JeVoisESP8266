/*
 Copyright (c) 2018 Jeroen Voogd

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#include <string.h>

// Since this ESP8266 will not be accessible, it should allow Over The Air updates
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define built_in_led 2

ESP8266WebServer server;
char* ssid = "YOUR SSID";
char* password = "YOUR PASSWORD";

void setup() {
  // Communication with the camera is via serial
  Serial.begin(115200); //Turn on Serial Port
  delay(1000);

  pinMode(built_in_led, OUTPUT);

  // configure a static IP address
  IPAddress ip(192, 168, 1, 115);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 1, 1);
  WiFi.config(ip, dns, gateway, subnet); //If you need Internet Access You should Add DNS also...
  // make the internet connection
  WiFi.begin(ssid,password);
  // int led_status = 0;
  while(WiFi.status()!=WL_CONNECTED) {
    // for debugging purposes, note that the jevois camera will also receive this message!
    // Serial.println("Waiting for wifi");
    digitalWrite(built_in_led, !digitalRead(built_in_led));
    delay(100);
  }
  // led off means there is a connection (note 1 = off)
  digitalWrite(built_in_led, 1);

  // configure the call back functions for the server
  server.on("/",[](){server.send(200,"text/plain","Jevois camera serial data");});
  server.on("/data",send_data);
  server.on("/set", receive_data);
  // start the server
  server.begin();

  // set OTA password
  ArduinoOTA.setPassword((const char *)"passwd");
  // start the OTA service
  ArduinoOTA.begin();
}

// implement a simple ring buffer of strings to store the incoming messages from
// the jevois camera

#define MAXRING 10
// note: max MAXSTRSIZE-1 characters, a null character is added
#define MAXSTRSIZE 200
// the ring buffer itself
char ring_buffer[MAXRING][MAXSTRSIZE];
// the place in the ring that needs to be send (the oldest info)
int to_send_pointer = 0;
// the place in the ring where new data needs to be added
int to_add_pointer = 0;
// a temporary buffer to receive characters via the serial port
char temp_buffer[MAXSTRSIZE];
// the place in the temp_buffer where characters should be added
int temp_buffer_position = 0;

void loop() {
  // let the Over The Air handler do its work
  ArduinoOTA.handle();
  // take care of the web-requests for data
  server.handleClient();

  // check for available serial data
  // store it in a temporary buffer
  if (Serial.available()) {
    // read the data one character at a time, and put it in a temp_buffer
    char input = Serial.read();
    temp_buffer[temp_buffer_position++] = input;
    // if the incoming data is complete (or the temporary buffer is full), add
    // it to the ring
    if ((input == '\n') or (temp_buffer_position == MAXSTRSIZE-1)) {
      // add a string termination character
      temp_buffer[temp_buffer_position] = '\0';
      // copy the temporary buffer to the ring buffer
      strcpy(ring_buffer[to_add_pointer], temp_buffer);
      // reset the temporary buffer
      temp_buffer_position = 0;
      // indicate that a string was added to the ring buffer
      to_add_pointer++;
      // check if the end of the ring is reached
      if (to_add_pointer == MAXRING) {
        // start adding at the beginning of the ring
        to_add_pointer = 0;
      }
    }
  }
}

void send_data()
{ // if the place in the ring to add and to send are the same there is no data
  if (to_send_pointer == to_add_pointer) {
    server.send(200,"text/plain","");
  } else {
    // there is data to send, so send it
    server.send(200,"text/plain", ring_buffer[to_send_pointer]);
    // increase the place in the ring to send
    to_send_pointer++;
    // check if the end of the ring was reached
    if (to_send_pointer == MAXRING) {
      // start reading from the beginning of the ring
      to_send_pointer = 0;
    }
  }
}

void receive_data()
{ // send an empty string back to the sender
  server.send(200,"text/plain","");
  // OR: send back the received command (for debugging)
  // String sendString="";
  // sendString=String("Received: cmd=" + server.arg("cmd"));
  // server.send(200,"text/plain",sendString);

  // send the received data to the camera
  Serial.println(server.arg("cmd"));
}
