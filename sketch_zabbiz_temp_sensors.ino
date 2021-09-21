#include <OneWire.h>
#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0x05, 0xB6, 0x27, 0x39, 0x20 }; // random MAC
byte ip[] = { 192, 168, 3, 7 }; // IP address in local network
byte myDns[] = {192, 168, 3, 252}; // адрес DNS-сервера
byte gateway[] = {192, 168, 0, 254}; // адрес сетевого шлюза
byte subnet[] = {255, 255, 252, 0}; // SubnetMask

String readString = String(20); 
byte addr1[8] = { 0x28, 0x81, 0x51, 0x94, 0x97, 0x06, 0x03, 0x6E }; // адрес 1 датчика
byte addr2[8] = { 0x28, 0x83, 0x41, 0x94, 0x97, 0x0C, 0x03, 0x9B }; // адрес 2 датчика
byte addr3[8] = { 0x28, 0x83, 0xF7, 0x01, 0x00, 0x00, 0x00, 0xD5 }; // адрес 3 датчика

OneWire ds(4); // DS18B20 at pin 2
EthernetServer server(10050); // Zabbix port

void setup() {
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
        server.begin();
  Serial.begin(9600);
  Serial.print("Server address:");
  Serial.println(Ethernet.localIP()); // выводим IP-адрес контроллера
  //Serial.println(Ethernet.subnetMask());
  ds.reset_search();
  ds.search(addr1);
}

void loop() {
  byte data[9];
  float celsius;
  int dataLength = 0;
  String dataString = String(20);
  char c;
  byte i;

  dataString = "";
  readString = "";
  if (EthernetClient client = server.available())
  {
    while (client.connected()) {
      if (dataString.length() <= 21 and dataString.length() <= dataLength) {
        c = client.read();
        if (readString.startsWith("ZBXD\x01") and readString.length() > 13 and dataLength > 0 and dataString.length() == dataLength and dataString.length() <= 20) // end of query from zabbix server 
        {
          client.print("ZBXD\x01"); // response header
          if (dataString == "agent.ping") {
            byte responseBytes [] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '1'}; 
            client.write(responseBytes, 9);
          // получаем данные по 1 датчику
          } else if (dataString == "env.temp.sensor1") {
            ds.reset();
            ds.select(addr1);
            ds.write(0x44, 1);  // start conversion with regular (non-parasite!) power
            delay(1000);  
            ds.reset();
            ds.select(addr1);
            ds.write(0xBE);  // read Scratchpad
            for ( i = 0; i < 9; i++) {
                data[i] = ds.read();
            };
            int16_t raw = (data[1] << 8) | data[0];
            byte cfg = (data[4] & 0x60);
            if (cfg == 0x00) raw = raw & ~7; // точность 9-разрядов, 93,75 мс
            else if (cfg == 0x20) raw = raw & ~3; // точность 10-разрядов, 187,5 мс
            else if (cfg == 0x40) raw = raw & ~1; // точность 11-разрядов, 375 мс
            celsius = (float)raw / 16.0;
            byte responseBytes [] = {(byte) String(celsius).length(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            client.write(responseBytes, 8);
            client.print(celsius);
            Serial.println(celsius);
          // получаем данные по 2 датчику
          } else if (dataString == "env.temp.sensor2") {
            ds.reset();
            ds.select(addr2);
            ds.write(0x44, 1);  // start conversion with regular (non-parasite!) power
            delay(1000);  
            ds.reset();
            ds.select(addr2);
            ds.write(0xBE);  // read Scratchpad
            for ( i = 0; i < 9; i++) {
                data[i] = ds.read();
            };
            int16_t raw = (data[1] << 8) | data[0];
            byte cfg = (data[4] & 0x60);
            if (cfg == 0x00) raw = raw & ~7; // точность 9-разрядов, 93,75 мс
            else if (cfg == 0x20) raw = raw & ~3; // точность 10-разрядов, 187,5 мс
            else if (cfg == 0x40) raw = raw & ~1; // точность 11-разрядов, 375 мс
            celsius = (float)raw / 16.0;
            byte responseBytes [] = {(byte) String(celsius).length(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            client.write(responseBytes, 8);
            client.print(celsius);
            Serial.println(celsius);
          // получаем данные по 3 датчику  
          } else if (dataString == "env.temp.sensor3") {
            ds.reset();
            ds.select(addr3);
            ds.write(0x44, 1);  // start conversion with regular (non-parasite!) power
            delay(1000);  
            ds.reset();
            ds.select(addr3);
            ds.write(0xBE);  // read Scratchpad
            for ( i = 0; i < 9; i++) {
                data[i] = ds.read();
            };
            int16_t raw = (data[1] << 8) | data[0];
            byte cfg = (data[4] & 0x60);
            if (cfg == 0x00) raw = raw & ~7; // точность 9-разрядов, 93,75 мс
            else if (cfg == 0x20) raw = raw & ~3; // точность 10-разрядов, 187,5 мс
            else if (cfg == 0x40) raw = raw & ~1; // точность 11-разрядов, 375 мс
            celsius = (float)raw / 16.0;
            byte responseBytes [] = {(byte) String(celsius).length(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            client.write(responseBytes, 8);
            client.print(celsius);
            Serial.println(celsius);
          } else {
            byte responseBytes [] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            client.write(responseBytes, 8);
            client.print("ZBX_NOTSUPPORTED");
          }
          break;
        }
        else if (readString.length() < 20) {
          if (readString == "ZBXD\x01" and dataLength == 0) {
            dataLength = int(c);
            //Serial.println(dataLength);
          }
          readString = readString + c;
        }
        if (readString.startsWith("ZBXD\x01") and readString.length() > 13 and dataLength > 0 and dataString.length() < dataLength) {
          dataString = dataString + c;
        }
      } else {
        client.print("ZBXD\x01"); // response header
        byte responseBytes [] = {0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        client.write(responseBytes, 8);
        client.print("ERROR_MAX_KEY_LENGTH_IS_20");
      }
    }
    delay(10);
    client.stop();
  }
}
