//Wifi Module
#include <ESP8266WiFi.h>
const char* ssid = "Anomalocaris"; //wifi ssid
const char* pass = "1123581321"; //wifi password
WiFiServer Server(80); //set webserver port
String header; //header of HTTP request
String currentLine;

//ina3221 Module
#include <Wire.h>
#include "SDL_Arduino_INA3221.h"
SDL_Arduino_INA3221 ina3221;

#define LED1_CHANNEL 3 //LED1 ina3221 channel
#define LED2_CHANNEL 2 //LED2 ina3221 channel
#define LED1_SW D5 //LED1 relay pin
#define LED2_SW D6 //LED2 relay pin

class LED
{
private:
  int sw_pin;
  int sensor_ch;

public:
  LED(int pin, int channel){
    this -> sw_pin = pin;
    this -> sensor_ch = channel;
    init();
  }

  void init(){
    pinMode(sw_pin, OUTPUT);
    off();
  }

  void off(){
    digitalWrite(sw_pin, HIGH);
  }

  void on(){
    digitalWrite(sw_pin, LOW);
  }

  float bus_voltage(){
    return ina3221.getBusVoltage_V(sensor_ch);
  }

  float shunt_voltage(){
    return ina3221.getShuntVoltage_mV(sensor_ch);
  }

  float load_current(){
    return ina3221.getCurrent_mA(sensor_ch);
  }

  float load_voltage(){
    return bus_voltage() + (shunt_voltage()/1000);
  }
};

LED led1(LED1_SW, LED1_CHANNEL);
LED led2(LED2_SW, LED2_CHANNEL);

void setup() {
  ina3221.begin();
  Serial.begin(115200);

  //connecting to wifi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  //Print local IP Address and start web Server
  Serial.println("");
  Serial.println("Wifi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Server.begin();


}

void loop() {
  WiFiClient client = Server.available(); //Listen for incoming client
  
  if (client){
    Serial.println("New client.");
    currentLine = "";
    while (client.connected()){
      if (client.available()){
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n'){
          if (currentLine.length() == 0){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type: application/json");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: close");
            client.println();

            //turns LED On/off
            if (header.indexOf("GET /led1/on") >= 0){
              Serial.println("led1 on");
              led1.on();
            } else if (header.indexOf("GET /led1/off") >= 0){
              Serial.println("led1 off");
              led1.off();
            }

            if (header.indexOf("GET /led2/on") >= 0){
              Serial.println("led2 on");
              led2.on();
            } else if (header.indexOf("GET /led2/off") >= 0){
              Serial.println("led2 off");
              led2.off();
            }

            //Displaying reading data JSON
            //client.println("<!DOCTYPE html><html>");
            //client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            //client.println("<link rel=\"icon\" href=\"data:,\">");
            //client.println("<body>");
            //client.println("<pre>");
            client.println("{");
            client.println("  \"Load\":[");
            client.println("      {");
            client.println("        \"name\":\"LED1\",");
              client.print("        \"voltage\":\""); client.print(led1.load_voltage()); client.println("\",");
            client.println("        \"voltage_unit\":\"V\",");
              client.print("        \"current\":\""); client.print(led1.load_current()); client.println("\",");
            client.println("        \"current_unit\":\"mA\"");
            client.println("      },");
            client.println("      {");
            client.println("        \"name\":\"LED2\",");
              client.print("        \"voltage\":\""); client.print(led2.load_voltage()); client.println("\",");
            client.println("        \"voltage_unit\":\"V\",");
              client.print("        \"current\":\""); client.print(led2.load_current()); client.println("\",");
            client.println("        \"current_unit\":\"mA\"");
            client.println("      }");
            client.println("  ]");
            client.println("}");
            //client.println("</pre>");
            //client.println("</body>");
            //client.println("</html>");

            client.println(); //end HTTP response
            break;            

          } else {
            currentLine = "";
          }
        } else if (c != '\r'){
          currentLine += c;
        }
      }
    }
    header = ""; //clear header
    client.stop(); //close Connection
    Serial.println("Client disconnected");
    Serial.println("");
  }
}
