#include "WiFi.h"
#include "AsyncUDP.h"

class UdpServer
{
private:
    /* data */
    AsyncUDP udp;

public:
    begin(char *ssid, char *password);
};

inline void UdpServer::begin(char *ssid, char *password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IPaddress: ");
    if (upd.listen(1234))
    {
        udp.onPacket([](AsyncUDPPacket packet)
                     {           
            Serial.print("Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length()); });
    }
}
