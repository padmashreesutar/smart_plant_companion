#include <SoftwareSerial.h>  

#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
#define ALTITUDE 1655.0 

#include <dht11.h>

#define DHT11PIN 5
dht11 DHT11;

SoftwareSerial ser(11,12);   

#define moist A0

#define relay 10

int i = 1;

String apiKey = "P69IN29Y28L2GQ2R";   
String Host_Name = "SATURDAY";         
String Password = "SATURDAY";  

String cmd;



void  setup()
      {    
Serial.begin(115200);                  
  ser.begin(115200);
  ser.println("AT+RST");              

    Serial.begin(115200);

  
  char inv = '"';
  String cmd = "AT+CWJAP";

  cmd += "=";
  cmd += inv;
  cmd += Host_Name;
  cmd += inv;
  cmd += ",";
  cmd += inv;
  cmd += Password;
  cmd += inv;
  ser.println(cmd);                  
  

        Serial.println(" JCR COLLEGE");                      
        delay(2000);


        Serial.println(" E/C DEPARTMENT");                      
        delay(2000);
        
        Serial.println("SMART PLANT COMPANION ");                         
        delay(2000);

        pinMode(moist,INPUT_PULLUP);
        pinMode(relay,OUTPUT);

 if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while(1); 
  }


        
      }

void loop()
{
  moist11();
  moisturetemp();
  pressure11();
  EstablishTCPconnection();
  UpdatetoTS();
  
}

void moist11()
      {
        int moistvalue=analogRead(moist);
        Serial.println("MOIST VALUE:    ");
        Serial.println(moistvalue);
        delay(2000);
        if(moistvalue>500)
            {
               Serial.println("WATER PUMP ON ");
               delay(500);
              digitalWrite(relay,HIGH);
              
            }
        else
        {
          digitalWrite(relay,LOW);
        }
      }


void moisturetemp()
      {
        Serial.println();
        int chk = DHT11.read(DHT11PIN);
        Serial.print("Humidity (%): ");
        Serial.println((float)DHT11.humidity, 2);
    
        Serial.print("HUMIDITY: ");   
         Serial.print((float)DHT11.humidity, 2);             
        delay(2000);
        
        Serial.print("Temperature  (C): ");
        Serial.println((float)DHT11.temperature, 2);
    
        Serial.print("TEMPERATURE: ");   
        Serial.print((float)DHT11.temperature, 2);             
        delay(2000);
      
      }

      
 void pressure11()
{
    char status;
      double T,P,p0,a;
  
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
     

      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}


void EstablishTCPconnection()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";          // Establishing TCP connection
  cmd += "184.106.153.149";                       // api.thingspeak.com
  cmd += "\",80";                                 // port 80

  ser.println(cmd);
  Serial.println(cmd);
  if (ser.find("Error")) 
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

  
    void UpdatetoTS()
    {
    int chk = DHT11.read(DHT11PIN);
      int moistvalue=analogRead(moist);
    
     char status;
      double T,P,p0,a;

status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {     
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        


      Serial.println(P*0.0295333727,2);
      
      String state1 = String((float)DHT11.humidity, 2);               // Converting them to string as to send it through URL
      String state2 = String((float)DHT11.temperature, 2);
      String state3 = String(moistvalue);
      
    String state4 = String(ALTITUDE,0);
  String state5 = String(P*0.0295333727,2);

    
      String getStr = "GET /update?api_key=";         // prepare GET string
      getStr += apiKey;
      getStr += "&field1=";
      getStr += String(state1);
      getStr += "&field2=";
      getStr += String(state2);
      getStr += "&field3=";
      getStr += String(state3);
      getStr += "&field4=";
      getStr += String(state4);
      getStr += "&field5=";
      getStr += String(state5);

      getStr += "\r\n\r\n";
      
      cmd = "AT+CIPSEND=";
      cmd += String(getStr.length());                // Total Length of data
    
      ser.println(cmd);
      Serial.println(cmd);
    
    
      if (ser.find(">")) 
      {
          ser.print(getStr);
          Serial.print(getStr);
          
          Serial.println("data sent");
          delay(100);
     
          ser.print(getStr);
          Serial.print(getStr);
      }
      else 
      {
            ser.println("AT+CIPCLOSE");            
            Serial.println("AT+CIPCLOSE");
          Serial.print("check Internet");
          delay(100);
        
      }
      }
      
    }
  }}}