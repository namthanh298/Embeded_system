// code Arduino
// code này để đọc từ GPS NEO 7
//Các thư viện gồm
// ArduinoJson : để uno có thể hiểu được chuỗi JSON
// SoftwareSerial : để giao tiếp Serial giữa Uno và ESP; 
//                  thư viện này dùng chân Serial ảo để giao tiếp
// SerialCommand.h : định nghĩa các chuỗi gửi đi

//TinyGPS : để đọc các mã NMEA của GPS
// giá trị trả về của GPS là LON, LAT : format data như này để có thể cho lên server là dùng được API



#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command

#include <TinyGPS.h>
TinyGPS gps;

const byte rxUnoConnectESP = 3;          // Chân 3 được dùng làm chân RX
const byte txUnoConnectESP = 2;          // Chân 2 được dùng làm chân TX
const byte rxUnoConnectGPS = 5;
const byte txUnoConnectGPS = 4;
 
SoftwareSerial espSerial = SoftwareSerial(rxUnoConnectESP, txUnoConnectESP); 
SoftwareSerial gpsSerial = SoftwareSerial(rxUnoConnectESP, txUnoConnectESP); 
 
SerialCommand sCmd(espSerial); // Khai báo biến sử dụng thư viện Serial Command




/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
struct data_GPS {
  float LON = 105.835856 ; 
  float LAT = 21.012236;
};
data_GPS dataGPS{};  //default

const unsigned long loopReadData = 2000UL ;   //cứ sau 2000ms = 2s thì sẽ lặp lại 

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void setup() {
  // Khởi tạo Serial ở baudrate 57600 để debug ở serial monito
  Serial.begin(57600);
 // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP826
  espSerial.begin(57600);

  // khởi tạo gps Serial ở 9600
  gpsSerial.begin(9600);
}
//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


// đọc data GPS bằng Uno
data_GPS readDataGPS() {
  float flon; float flat;
  data_GPS data_LON_LAT;
  bool newData = false;

  // Mỗi 1s sẽ đọc GPS 
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    //float flat, flon; //flat = LAT ; flon = LON;
    gps.f_get_position(&flat, &flon);

    data_LON_LAT.LON = flon;
    data_LON_LAT.LAT = flat;
   // Serial.print("LAT=");
   // Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
   //Serial.print(" LON=");
   //Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
   return data_LON_LAT;
}
//else {
  
//    data_LON_LAT.LON = 105.84314343755584;
//  data_LON_LAT.LAT = 21.00683782081307;       // ĐHBK HN
//}
}


//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

unsigned long chuky = 0;

char json[] = 
"{\"geometry\":{\"type\":\"Point\",\"coordinates\":[105,20]},\"type\":\"Feature\",\"properties\":{\"title\":\"Xe Bus 16\"}";
// tạo mẫu dữ liệu ban đầu để chuyển sang JSON

void loop() {
// khởi tạo 1 chu kì lệnh = 2000s
if (millis() - chuky > loopReadData) {
    chuky = millis();
    dataGPS = readDataGPS();


//for lib JSONv6

// chuyển input sang JSON để gửi đi
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
       Serial.print(F("deserializeJson() failed: "));
       Serial.println(error.f_str());
       return;
     }
    doc["coordinates"][0] = dataGPS.LON;
    doc["coordinates"][1] = dataGPS.LAT;

//gửi data ra Serial ESP
    espSerial.print("sendData");   //gửi tên lệnh renderData
    espSerial.print('\r');           // gửi \r : cái này bắt buộc có
    serializeJson(doc,espSerial);        //gửi chuỗi JSON đến esp
    espSerial.print('\r'); 
    
// for lib JSONv5
/*
    StaticJsonBuffer<200> jsonBuffer; // tạo buffer cho JSON chứa tối đa 200 kí tư
    JsonObject& root = jsonBuffer.createObject();// đặt biến root kiểu JSON
    
   //đọc giá trị cảm biến GPS rồi in ra root
    root["geometry"] = { root["type"] = "Point", root["coordinates"] = [dataGPS.LON,dataGPS.LAT] };
    root["type"] = "Feature";
    root["properties"] = {root["title"] = "Xe Bus 16"};

  // gửi data
  // in ra cổng Serial ESP
  // cú pháp: <tên lệnh>\r<chuỗi JSON>\r
   espSerial.print("renderData");   //gửi tên lệnh renderData
   espSerial.print('\r');           // gửi \r : cái này bắt buộc có
   root.printTo(espSerial);        //gửi chuỗi JSON đến esp
   espSerial.print('\r');   
}
*/



sCmd.readSerial();  // vòng loop này cần có
}
}
