#include "PS2Handler.h"
#include "MotionController.h"
#include "config.h"

#include "WiFi.h"
#include "AsyncUDP.h"

#define PS2_DAT 5
#define PS2_CMD 6
#define PS2_SEL 7
#define PS2_CLK 4

MotionController motionController;
PS2Handler ps2;
AsyncUDP udpServer;
PS2X ps2x;

void setup() {
  Serial.begin(115200);
  motionController.begin(60);
  // ps2.begin(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
  WiFi.mode(WIFI_STA);
  WiFi.begin("Home2.4G", "a278742221");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected WIFI");
  if (udpServer.listen(1234)) {
    udpServer.onPacket([](AsyncUDPPacket packet) {
      String data = packet.readString();
      Serial.println(data);
      int pwm = 0, channel = 0, value = 0, offset = 0;
      int numFields = sscanf(data.c_str(), "%d,%d,%d,%d", &pwm, &channel, &value, &offset);
      motionController.single_debugging(pwm, channel, value, offset);
    });
  }
}
void loop() {
  delay(1000);
}
