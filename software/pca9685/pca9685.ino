#include <AsyncUDP.h>
#include <WiFi.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

Adafruit_PWMServoDriver left_pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver right_pwm = Adafruit_PWMServoDriver(0x41);

const char *ssid = "***********";
const char *password = "***********";
AsyncUDP udp;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    while (1) {
      delay(1000);
    }
  }

  left_pwm.begin();
  left_pwm.setPWMFreq(60);  // 设置PCA9685的PWM频率

  right_pwm.begin();
  right_pwm.setPWMFreq(60);  // 设置PCA9685的PWM频率

  if (udp.listen(1234)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();

      // 回复客户端
      packet.printf("Got %u bytes of data", packet.length());

      // 解析UDP数据
      String inputString = "";
      char* packet_ptr = (char *)packet.data();
      for (int c_idx = 0; c_idx < packet.length(); c_idx++) {
        char inChar = packet_ptr[c_idx];
        inputString += inChar;
      }

      inputString.trim(); // 去除可能的换行符和空格

      // 解析 channel, pin, value
      int comma1 = inputString.indexOf(',');
      int comma2 = inputString.indexOf(',', comma1 + 1);

      String channel = inputString.substring(0, comma1);
      String pin_str = inputString.substring(comma1 + 1, comma2);
      String value_str = inputString.substring(comma2 + 1);

      int pin = pin_str.toInt();
      int value = value_str.toInt();

      Adafruit_PWMServoDriver *channel_pwm;
      if (channel == "LEFT") {
        channel_pwm = &left_pwm;
      } else if (channel == "RIGHT") {
        channel_pwm = &right_pwm;
      } else {
        Serial.println("Invalid channel");
        return;
      }

      // 限制 pin 和 value 的范围
      if (pin < 0 || pin > 15) {
        pin = 0;
      }
      if (value < 0) {
        value = 0;
      } else if (value > 4095) {
        value = 4095;
      }

      // 设置 PWM
      channel_pwm->setPWM(pin, 0, value);

      Serial.print("Setting ");
      Serial.print(channel);
      Serial.print(" PWM: pin=");
      Serial.print(pin);
      Serial.print(", value=");
      Serial.println(value);
    });
  }
}

void loop() {
  delay(1000);  // 防止频繁运行
}