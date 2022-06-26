#include <TridentTD_LineNotify.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

WiFiClient client;


String SMOKE;
String response;
const char* ssid     = "MHEEMHEE";
const char* password = "MHEE0614346242";

String IPCAM_IP  =  "192.168.1.2:8080/shot.jpg";

#define TOKENCOUNT  1  //จำนวนTOKENของไลน์
String tokens[TOKENCOUNT] = {"Szmqei6nZgNRL6eBHuc45ZU1ShEMyKiblm6iECEhHj9"};

int ledPin = D0;
int analogPin = A0; //ประกาศตัวแปร ให้ analogPin 
int val = 0;
bool ipCameraEnabled = true;

void downloadAndSaveFile(String fileName, String  url){
  
  HTTPClient http;

  Serial.println("[HTTP] begin...\n");
  Serial.println(fileName);
  Serial.println(url);
  http.begin(url);
  
  Serial.printf("[HTTP] GET...\n", url.c_str());
  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      Serial.printf("[FILE] open file for writing %d\n", fileName.c_str());
      
      File file = SPIFFS.open(fileName, "w");

      // file found at server
      if(httpCode == HTTP_CODE_OK) {

          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = http.getSize();

          // create buffer for read
          uint8_t buff[128] = { 0 };

          // get tcp stream
          WiFiClient * stream = http.getStreamPtr();

          // read all data from server
          while(http.connected() && (len > 0 || len == -1)) {
              // get available data size
              size_t size = stream->available();
              if(size) {
                  // read up to 128 byte
                  int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                  // write it to Serial
                  //Serial.write(buff, c);
                  file.write(buff, c);
                  if(len > 0) {
                      len -= c;
                  }
              }
              delay(1);
          }

          Serial.println();
          Serial.println("[HTTP] connection closed or file end.\n");
          Serial.println("[FILE] closing file\n");
          file.close();
          
      }
      
  }
  http.end();
}


void setup() 
{
   Serial.begin(115200);
   Serial.println("Starting...");

   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) 
   {
      delay(250);
      Serial.print(".");
   }

   Serial.println("WiFi connected");  
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   
  
}
void sendLineNotify(){
  digitalWrite(D0,LOW);

  

  for (int i = 0; i < TOKENCOUNT; i++){
      Serial.println("Send message " + i +1);
      LINE.setToken(tokens[i]);
      LINE.notify("มีคนสูบบุหรี่"); //ข้อความที่ส่งเข้าไลน์
  }
    if(ipCameraEnabled){
     downloadAndSaveFile("/shot.jpg","http://"+ IPCAM_IP );
    //listFiles();  
    for (int i = 0; i < TOKENCOUNT; i++){
      Serial.println("Send image " + i +1);
      LINE.setToken(tokens[i]);
      LINE.notifyPicture("Camera snapshot", SPIFFS, "/shot.jpg");  //รูปที่ส่งเข้าไลน์
      }
    
  }
  
   digitalWrite(D0,HIGH);
  
}





 void loop() 
{
  
  
  
   val = analogRead(analogPin);  //อ่านค่าสัญญาณ analog 
  Serial.print("val = "); // พิมพ์ข้อมความส่งเข้าคอมพิวเตอร์ "val = "
  Serial.println(val); // พิมพ์ค่าของตัวแปร val
  if (val > 60)
  {
    Serial.println("ส่งข้อมูล");
    sendLineNotify();
  }
if(ipCameraEnabled){
    //Initialize File System
    if(SPIFFS.begin()){
      Serial.println("SPIFFS Initialize....ok");
    }else{
      Serial.println("SPIFFS Initialization...failed");
    }
   
    //Format File System
    if(SPIFFS.format()){
      Serial.println("File System Formated");
    }else{
      Serial.println("File System Formatting Error");
    }
  }

  Serial.println("--พร้อมจับควัน--");
  delay(10);        // delay in between reads for stability
}
