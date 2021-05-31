// code ESP
// code này để tạo ESP là 1 socket client
// SerialCommand là để bắt các lệnh từ Uno gửi lên, sau đó gửi lại cho socket sever
//esp8266 là trung gian vận chuyển dữ liệu giữa Uno và socket sever
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <SerialCommand.h>  
 
//include thư viện để kiểm tra free RAM trên con esp8266
extern "C" {
  #include "user_interface.h"
}
 
 
const byte RX = D1;
const byte TX = D2;
SoftwareSerial mySerial(RX, TX, false);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
 
SocketIOClient client;
const char* ssid = "Happy House 2";          //Tên mạng Wifi mà Socket server của bạn đang kết nối
const char* password = "vuilenbannhe";  //Pass mạng wifi 
 
char host[] = "192.168.2.112";  //Địa chỉ IP dịch vụ, hãy thay đổi nó theo địa chỉ IP Socket server của bạn.
int port = 4000;                  //Cổng dịch vụ socket server do chúng ta tạo!
 
//từ khóa extern: dùng để #include các biến toàn cục ở một số thư viện khác. Trong thư viện SocketIOClient có hai biến toàn cục
// mà chúng ta cần quan tâm đó là
// RID: Tên hàm (tên sự kiện
// Rfull: Danh sách biến (được đóng gói lại là chuối JSON)
extern String RID;
extern String Rfull;
 
void defaultCommand(String command) {
  char *json = sCmd.next();
  client.send(command, (String) json);//gửi dữ liệu về cho Socket Serve
 
  //In ra serial monitor để debu
  Serial.print(command);
  Serial.print(' ');
  Serial.println(json);
  Serial.println("Tot lam, da gui du lieu roi, xem trong console cua Socket server di");
} 



void setup()
{
    //Bật baudrate ở mức 57600 để giao tiếp với máy tính qua Seria
    Serial.begin(57600);
    mySerial.begin(57600); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduin
    delay(10);
 
    //Việc đầu tiên cần làm là kết nối vào mạng Wifi
    Serial.print("Ket noi vao mang ");
    Serial.println(ssid);
 
    //Kết nối vào mạng Wifi
    WiFi.begin(ssid, password);
 
    //Chờ đến khi đã được kết nố
    while (WiFi.status() != WL_CONNECTED) { //Thoát ra khỏi vòng
        delay(500);
        Serial.print('.');
    }
 
    Serial.println();
    Serial.println(F("Da ket noi WiFi"));
    Serial.println(F("Dia chi IP cua ESP8266 (Socket Client ESP8266): "));
    Serial.println(WiFi.localIP());
 
    if (!client.connect(host, port)) {
        Serial.println(F("Ket noi den socket server that bai!"));
        return;
    }
 
    sCmd.addDefaultHandler(defaultCommand); //Lệnh nào đi qua nó cũng bắt hết, rồi chuyển xuống hàm defaultCommand
    Serial.println("Da san sang nhan lenh");
    
}


void loop()
{
 
    //Khi bắt được bất kỳ sự kiện nào thì chúng ta có hai tham số
    //  +RID: Tên sự kiệ
    //  +RFull: Danh sách tham số được nén thành chuỗi JSON
    if (client.monitor()) {
 
        //in ra serial cho Arduino
        mySerial.print(RID);
        mySerial.print('\r');
        mySerial.print(Rfull);
        mySerial.print('\r');
 
        //in ra serial monito
        Serial.print(RID);
        Serial.print(' ');
        Serial.println(Rfull);
        
        //Kiểm tra xem còn dư bao nhiêu RAM, để debu
        uint32_t free = system_get_free_heap_size();
        Serial.println(free);
    }
 
    //Kết nối lại
    if (!client.connected()) {
      client.reconnect(host, port);
    }
 
    sCmd.readSerial();
}
 
