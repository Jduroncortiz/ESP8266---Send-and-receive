/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "WTS-Guest";
const char *password = "SU2017EZ";
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
long valuesArray1[100] = {0};
int j = 0;
int ic = 1;
float media = 0;

ESP8266WebServer server ( 80 );

const int led = 14;

void handleRoot() {
	digitalWrite ( led, 1 );
	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Remote datalogger v0.5</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Analog signal 1</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

		hr, min % 60, sec % 60
	);
	server.send ( 200, "text/html", temp );
	digitalWrite ( led, 0 );
}

void handleNotFound() {
	digitalWrite ( led, 1 );
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
	digitalWrite ( led, 0 );
}

void setup ( void ) {
	pinMode ( led, OUTPUT );
	digitalWrite ( led, 0 );
	Serial.begin ( 9600 );
	WiFi.mode ( WIFI_STA );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );
  inputString.reserve(200);
  
	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( MDNS.begin ( "esp8266" ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on ( "/", handleRoot );
	server.on ( "/test.svg", drawGraph );
	server.on ( "/inline", []() {
		server.send ( 200, "text/plain", "this works as well" );
	} );
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );
  Serial.println("START_OK");
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void loop ( void ) {
	server.handleClient();
  
  if(Serial.available()){
  serialEvent();
  }
  
   if (stringComplete) {
    String arrayLength = String(inputString.length()-2, DEC);
    int commasTemp[101] = {0};
    int i = 0;
    ic = 1; 
    
    while(i<inputString.length()){
      int commas = inputString.indexOf(";",i+1);
      
      if(commas!=-1){
        i = commas;
        commasTemp[ic] = commas;
        ic++;
        i++;
      }else{
        commasTemp[ic] = inputString.length()-2;
        break;
      }
    }
    
    for(j=0; j<ic; j++){
      String beginNumber = "";
      if(j==0){
        beginNumber = inputString.substring(commasTemp[j], commasTemp[j+1]);  
      }else{
        beginNumber = inputString.substring(commasTemp[j]+1, commasTemp[j+1]);  
        }
      valuesArray1[j] = beginNumber.toInt();
    }
    inputString = "";
    stringComplete = false;
    media = 0;
    for(int k=0; k<ic; k++){
      Serial.println(String(valuesArray1[k], DEC));
      media += valuesArray1[k];
    }
    media /= ic;
    Serial.print("Media= "); Serial.println(media);
  }
}

void drawGraph() {
	String out = "";
	char temp[100];
	out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1020\" height=\"220\">\n";
 	out += "<rect width=\"1020\" height=\"220\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
 	out += "<g stroke=\"black\">\n";
 	int y = valuesArray1[0]; //rand() % 130;
 	for (int x = 10; x < 1000; x+= 10) {
 		int y2 = valuesArray1[x/10];
 		sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 200 - y, x + 10, 200 - y2);
 		out += temp;
 		y = y2;
 	}
	out += "</g>\n</svg>\n";

  // GRAPH PLOTTING TEMPLATE
  /*String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";*/
  

	server.send ( 200, "image/svg+xml", out);
}
