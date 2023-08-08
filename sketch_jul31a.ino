#define BLYNK_TEMPLATE_ID           "TMPLQAc0hDlD"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "lbtUJBTNxTJYbsXFlevHY-ym7Gce-kEt"

#define BLYNK_PRINT Serial  
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WificlientSecure.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>

//----------------------------
String URLL =  "http://myprojectiot11.000webhostapp.com/dbwrite.php";
//----------------------------

HTTPClient http ;  // http boject of class HTTPCLient
WiFiClientSecure wclient ; // wclient object of class HTTPClient  
 
#define DHTPIN      D1   //กำหนดให้ D1 เป็น DHTPIN
#include "DHT.h"
#define DHTTYPE DHT11   //ชนิดของ DHT
DHT dht(DHTPIN, DHTTYPE);

#define PUMP D2   //กำหนดให้ D2 เป็น PUMP
#define LAMP D3   //กำหนดให้ D3 เป็น LAMP
#define FAN  D5   //กำหนดให้ D5 เป็น FAN
int Soilmoisture = A0;   //กำหนดให้ A0 เป็น Soilmoisture
#define LDR  D6   //กำหนดให้ D6 เป็น LDR

#define MANUAL  0   //กำหนดให้ MANUAL เป็น 0
#define AUTO    1   //กำหนดให้ AUTO เป็น 1
boolean modeChT = MANUAL;   //กำหนดให้ modeChT เป็น MANUAL

float TempHIGH = 30;    //กำหนดให้ TempHIGH ให้อยู่ที่ 30 องศา ถ้าน้อยกว่า 30 พัดลมทำงาน
float MoisHIGH = 900;  //กำหนดให้ MoisHIGH ให้อยู่ที่ 900 ความชื้นในดิน ถ้าน้อยกว่า 900 ก็สั่งปั้มทำงาน

int val = 0;   //กำหนดให้ val เป็น 0
int val1 = 0;  //กำหนดให้ val1 เป็น 0
int val2 = 99; //กำหนดให้ val2 เป็น 99

String sendval, sendval2 ,sendval3 ,postData , postData1;

int Temp;   //กำหนดตัวแปร Temp 
int Humi;   //กำหนดตัวแปร Humi 
int Light;  //กำหนดตัวแปร Light

char ssid[] = "Bookphuket_2.4G";  //ชื่อ WIFI
char pass[] = "xxx";   //รหัส WIFI
String GAS_ID = ""; //--> spreadsheet script ID

//Your Domain name with URL path or IP address with path
const char* host = "script.google.com"; // only google.com not https://google.com
const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
//------------------------------------------------------

//-----------------------------------------------------------------------------------------
#define UPDATE_INTERVAL_HOUR  (0) // จัดการทุก h ชม
#define UPDATE_INTERVAL_MIN   (5) // จัดการทุก m นาที
#define UPDATE_INTERVAL_SEC   (0) // จัดการทุก s วินาที

#define UPDATE_INTERVAL_MS    ( ((UPDATE_INTERVAL_HOUR*60*60) + (UPDATE_INTERVAL_MIN * 60) + UPDATE_INTERVAL_SEC ) * 1000 )

//-----------------------------------------------------------------------------------------

BlynkTimer timer;  

BLYNK_WRITE(V0)   //MODE
{
  int pinValue = param.asInt(); 
  if(pinValue !=1)
  {
    modeChT = MANUAL;  
    Serial.println("MANUAL MODE!!!");
    Blynk.virtualWrite(V1, "inactive"); // PUMP
    Blynk.setProperty(V1, "color", "#7cf051");
    Blynk.virtualWrite(V2, "inactive"); // LAMP
    Blynk.setProperty(V2, "color", "#203996");
    Blynk.virtualWrite(V3, "inactive"); // FAN
    Blynk.setProperty(V3, "color", "#c21b3f");   
  }
  else
  {
    modeChT = AUTO;  
    Serial.println("AUTO MODE!!!");
    Blynk.virtualWrite(V1, "inactive");
    Blynk.setProperty(V1, "color", "#3D3D3D");
    Blynk.virtualWrite(V2, "inactive");
    Blynk.setProperty(V2, "color", "#3D3D3D");
    Blynk.virtualWrite(V3, "inactive");
    Blynk.setProperty(V3, "color", "#3D3D3D");
  }
}

BLYNK_WRITE(V1)  //PUMP
{
  int pinValue = param.asInt(); 
  if(modeChT == MANUAL)
  {
    if(pinValue == 1)
    {
      digitalWrite(PUMP, LOW);
    }
    else
    {
      digitalWrite(PUMP, HIGH);
    }
  }
  else
  {
    Blynk.virtualWrite(V1, "inactive");
    Blynk.setProperty(V1, "color", "#3D3D3D");
  }
}

BLYNK_WRITE(V2)   //LAMP
{
  int pinValue = param.asInt(); 
  if(modeChT == MANUAL)
  {
    if(pinValue == 1)
    {
      digitalWrite(LAMP, LOW);
    }
    else
    {
      digitalWrite(LAMP, HIGH);
    }
  }
  else
  {
    Blynk.virtualWrite(V2, "inactive");
    Blynk.setProperty(V2, "color", "#3D3D3D");
  }
}

BLYNK_WRITE(V3)   //FAN
{
  int pinValue = param.asInt(); 
  if(modeChT == MANUAL)
  {
    if(pinValue == 1)
    {
      digitalWrite(FAN, LOW);
    }
    else
    {
      digitalWrite(FAN, HIGH);
    }
  }
  else
  {
    Blynk.virtualWrite(V3, "inactive");
    Blynk.setProperty(V3, "color", "#3D3D3D");
  }
}



void setup()
{
  pinMode(PUMP, OUTPUT);  digitalWrite(PUMP, HIGH);
  pinMode(LAMP, OUTPUT);  digitalWrite(LAMP, HIGH);
  pinMode(FAN,  OUTPUT);  digitalWrite(FAN, HIGH);
  pinMode(Soilmoisture, INPUT);
  pinMode(LDR, INPUT);
  pinMode(DHTPIN, INPUT);
  
  Serial.begin(115200);
  dht.begin();  //dht เริ่มทำงาน
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); 
}

// ----------------------------------------------------------------------------------------------
unsigned long time_ms;
unsigned long time_1000_ms_buf;
unsigned long time_sheet_update_buf;
unsigned long time_dif;


void loop()
{
   HTTPClient http1;    // http object of clas HTTPClient
   WiFiClient wclient1; // wclient object of clas HTTPClient    


   float Temp = dht.readTemperature();
   float Humi = dht.readHumidity();   
   int Moisture = analogRead(Soilmoisture);
   
  // Convert integer variables to string
  sendval = String(Temp);  
  sendval2 = String(Humi);      
  sendval3 = String(Moisture);      

  //
  
 //-------------------------------
     time_ms = millis();
     time_dif = time_ms - time_1000_ms_buf;

  //Read and print serial data every 1 sec
  if ( time_dif >= 5000 ) // 5 sec
  {
     time_1000_ms_buf = time_ms;     
     // Print serial messages

    Serial.print("time_dif >= 5000 ");
    Serial.print("\t");     
    Serial.print("Humidity: " + String(Humi) + " %");
    Serial.print("\t");
    Serial.println("Temperature: " + String(Temp) + " C");
  }
    
  // Update data to google sheet in specific period
    time_ms = millis();
    time_dif = time_ms - time_sheet_update_buf;  
    
  if ( time_dif >= UPDATE_INTERVAL_MS ) // Specific period
  {   
     
   time_sheet_update_buf = time_ms;
     
   // Send DATA to GOOGLE SHEET
   // update_google_sheet(Temp,Humi);
   //
   //Update MY SQL
   //http_POST(Temp,Humi);
   
    postData1 = "temperature="+sendval+"&humidity="+sendval2+"&moisture="+sendval3; ; 
  
  
    String requestUrl = "https://myprojectiot11.000webhostapp.com/dbwrite.php";
  // Update Host URL here:-  
  
    http1.begin(wclient1, "http://myprojectiot11.000webhostapp.com/dbwrite.php");              // Connect to host where MySQL databse is hosted
    http1.addHeader("Content-Type", "application/x-www-form-urlencoded");            //Specify content-type header

   
    int httpCode1 = http1.POST(postData1);   // Send POST request to php file and store server response code in variable named httpCode
    Serial.println("Values are, sendval = " + sendval + " and sendval2 = "+sendval2 );


  // if connection eatablished then do this
    if (httpCode1 == 200) { Serial.println("Values uploaded successfully."); Serial.println(httpCode1); 
    String webpage = http1.getString();    // Get html webpage output and store it in a string
    Serial.println(webpage + "\n"); 
    }

// if failed to connect then return and restart

   else { 
    Serial.println(httpCode1); 
    Serial.println("Failed to upload values. \n"); 
    http1.end(); 
    return; }

     
     // 
    } else
      {
      Serial.print("NO Update_GOOGLE_SHEET  " + String(time_ms) + " s");
      Serial.print("\t");          
      }
  
 //-----------------------------
     
  Blynk.run();
 
  Blynk.virtualWrite(V4, Temp);
  Blynk.virtualWrite(V5, Humi);
  
  Serial.print("Moisture : ");
  Serial.println(Moisture);
  Blynk.virtualWrite(V6, Moisture);

  switch(modeChT)
  {
    case AUTO:
    {
       if(Temp >= TempHIGH)
       {
        digitalWrite(FAN, LOW);
        Blynk.virtualWrite(V3, HIGH);
       }
       else
       {
        digitalWrite(FAN, HIGH);
        Blynk.virtualWrite(V3, LOW);
       }

       Moisture = analogRead(Soilmoisture);      
       if(Moisture >= MoisHIGH)
       {
        digitalWrite(PUMP, LOW);
        Blynk.virtualWrite(V1, HIGH);
       }
       else 
       {
        digitalWrite(PUMP, HIGH);
        Blynk.virtualWrite(V1, LOW);
       }

       val = analogRead(LDR);
       Serial.print("LDR = ");
       Serial.println(val);
       if(val < 500)
       {     
        digitalWrite(LAMP, HIGH);
        Blynk.virtualWrite(V2, LOW);
       }
       else 
       {
        digitalWrite(LAMP, LOW);
        Blynk.virtualWrite(V2, HIGH);
       } 
       break;
    }
    case MANUAL: 
    {     
      Blynk.setProperty(V1, "color", "#7cf051");
      Blynk.setProperty(V2, "color", "#203996");
      Blynk.setProperty(V3, "color", "#c21b3f");
      break;
    }
  }
   
  delay(5000);
  //val+=1; val2+=10; // Incrementing value of variables
}

void http_POST(float val_temp,float val_humi){
    
}

// ----------------------------------------------------------------------------------------------
void update_google_sheet(float val_temp,float val_humi)
{

    wclient.setInsecure(); // this is the magical line that makes everything work
    
    Serial.print("connecting to ");
    Serial.println(host);
     
    
    if (!wclient.connect(host, httpPort)) { //works!
      Serial.println("connection failed");
      return;
    }
       
    //----------------------------------------Processing data and sending data
    float string_temp = val_temp; 
    float string_humi = val_humi;
    String urll = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temp + "&humidity="+string_humi; //  2 variables 
    Serial.print("requesting URL: ");
    Serial.println(urll);

    wclient.print(String("GET ") + urll + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

    Serial.println();
  //----------------------------------------

  //---------------------------------------
  while (wclient.connected()) {
    String line = wclient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = wclient.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //------------------------

}
