// This code has been written to create a simple device to alert Fort Collins Utilities customers about the status of peakload events
// Please refer to readme file for hardware requirements and configuration
// This solution has been co-created by the City of Fort Collins and Apieron Logic LLC



#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Set wifi credentials here
const char* ssid = "NETWORKNAME";                     //Replace with your network name
const char* password = "PASSWORD";                   //Replace with network password

const char* link= "http://www.fcgov.com/peakload/load-control-status?token=9081029c3ca4e0d8a0be965a086a73ba";    //URL to query conservation event status


int underway_events = 0;                              //1 if conservation event is underway, 0 if not
int scheduled_events =0;                              //1 if there is a scheduled conservation event, 0 if not 
 
void setup () {
 
  pinMode(12, OUTPUT);  //green -- D6
  pinMode(13, OUTPUT);  //yellow -- D7
  pinMode(14, OUTPUT);  //blue -- D5
  pinMode(15, OUTPUT);  //red -- D8

  paintPixels (LOW, LOW, LOW, HIGH);                  //set to blue initially

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  delay (2000); 

  getPayload();                                      //to set values of underway_events and scheduled_events based on returned values from peakload website

}


void connectWifi() {                                             //function to connect to wifi
    //disconnect and reconnect to network
    Serial.println("Reconnecting to WiFi");
    WiFi.disconnect();
    delay (2000);
    WiFi.begin(ssid, password); 
    delay (2000); 
    return;

}


void paintPixels (char g, char y, char r, char b) {                 //function to change colors of LEDs
    digitalWrite(12, g); //green
    digitalWrite(13, y); //yellow
    digitalWrite(15, r); //red
    digitalWrite(14, b);  //blue
        
    return;
}





void getPayload () {                                                  //Query website and return parsed values
  
 
    HTTPClient http;                                                  //Declare an object of class HTTPClient
    Serial.println("talking to fcgov");
 
    http.begin(link);                                                  //Specify request destination. Does NOT like https
    int httpCode = http.GET();                                         //Send the request
    
    if (httpCode > 0) {                                                //Check the returning code
        String payload = http.getString();                             //Get the request response payload

        StaticJsonBuffer<300> JSONBuffer;                              //Memory pool
        JsonObject& parsed = JSONBuffer.parseObject(payload);          //Parse message

        if (!parsed.success()) {                                       //Check for errors in parsing
 
            Serial.println("Parsing failed");
              connectWifi();                                           //try to reconnect to wifi to resolve issue
              return;
 
            }
 
        underway_events = parsed["underway_events"];               //Get sensor type value
        scheduled_events = parsed["scheduled_events"];             //Get value of sensor measurement
 
        Serial.println(underway_events);
        Serial.println(scheduled_events);

        http.end();  
    }
   
  return;
  }
   
                                                                      //Close connection
        


 
void loop() {

   if (WiFi.status() == WL_CONNECTED) {                                //Check WiFi connection status   
       
        paintPixels (LOW, LOW, LOW, LOW);                     //makes LEDs blink to show it is checking
        getPayload();                                         //check peakload website for status



        if (underway_events==0 && scheduled_events==0){
          Serial.println("No Event");
          paintPixels (HIGH, LOW, LOW, LOW);                            //No event so turn on green only

        }

        if (underway_events==0 && scheduled_events!=0){
          Serial.println("Event Scheduled");
          paintPixels (LOW, HIGH, LOW, LOW);                           //Scheduled event so turn on yellow only
        }

        if (underway_events!=0 && scheduled_events==0){
          Serial.println("Event Underway");
          paintPixels (LOW, LOW, HIGH, LOW);                           //Event underway so turn on red only
        }
        
        if (underway_events!=0 && scheduled_events!=0){
          Serial.println("Event Scheduled and Currently Underway");
          paintPixels (LOW, HIGH, HIGH, LOW);                          //Event underway and scheduled so turn on Yellow and Red 
        }


       delay(60000);                                                   //Send a request every 60 seconds  
   
   }else{
       Serial.println("No Connection");
       paintPixels (LOW, LOW, LOW, HIGH);                             //blue only
       connectWifi();                                                 //reconnect to wifi 
    }
    
}

 
  
  
  
 


