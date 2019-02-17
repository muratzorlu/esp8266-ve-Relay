#include <ESP8266WiFi.h>
const char* ssid = "wifi Adı";
const char* password = "şifre";

WiFiServer server(80);

int val;
void setup() {
  Serial.begin(9600);
  delay(10);

  // GPIO2 çıkış olarak ayarlanıyor.
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
  // Wifi'ye bağlanıyor
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  //Wifi'ye bağlanmak için bekliyor
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Server başlıyor
  server.begin();
  Serial.println("Server started");

  // Modemden aldığı IP adresini yazıyor
  Serial.println(WiFi.localIP());
}

void loop() {
  // Server kontrol
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Bağlı bilgisayardan veri bekleniyor
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // ilk satır okunuyor
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // röle'ye gönderilecek bilgileri hex olarak saklanıyor
  const byte miBufferON[] = {0xA0, 0x01, 0x01, 0xA2};
  const byte miBufferOFF[] = {0xA0, 0x01, 0x00, 0xA1};
  
  //gelen satır içinde kontrol yapılıyor
  if (req.indexOf("?pin=OFF1") != -1)
  {
    val = 0;
    Serial.write(miBufferOFF, sizeof(miBufferOFF));
  }
  else if (req.indexOf("?pin=ON1") != -1)
  {
    val = 1;
    Serial.write(miBufferON, sizeof(miBufferON));
  }

  // GPIO2 pini ayarlanıyor
  digitalWrite(2, val);
  
  client.flush();

  // Server'a yazırılacak html metni
  String s = "<h1>ESP8266 Relay Kontrol</h1><p>Relay On/Off <a href=\"?pin=ON1\"><button>ON</button></a>&nbsp;<a href=\"?pin=OFF1\"><button>OFF</button></a></p>";
  s += (val)?"Relay On":"Relay Off";
  s += "</html>\n";

  // Html metin yazdırılıyor
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}
