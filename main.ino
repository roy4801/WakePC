#include <LWiFi.h>

char ssid[] = "roy-5f";
char pass[] = "s82611141";
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup()
{
    Serial.begin(115200);         // initialize serial communication
    pinMode(LED_BUILTIN, OUTPUT); // set the LED pin mode

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to Network named: ");
        Serial.println(ssid); // print the network name (SSID);

        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
    }
    server.begin();    // start the web server on port 80
    printWifiStatus(); // you're connected now, so print out the status
}

void loop()
{
    WiFiClient client = server.available(); // listen for incoming clients

    if (client)
    {
        Serial.println("new client");
        String currentLine = "";
        bool turnOn = false;
        String res;

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                if (c == '\n')
                {
                    if (currentLine.length() == 0)
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type: application/json");
                        client.println("Access-Control-Allow-Origin: *");
                        client.println();

                        res = "{";
                        res += R"("ssid":")" + String(WiFi.SSID()) + "\",";
                        res += R"("ip":")" + WiFi.localIP().toString() + "\",";
                        res += R"("rssi":")" + String(WiFi.RSSI()) + "\",";
                        res += R"("state":")" + String(turnOn ? "on" : "off");
                        res += R"("})";

                        client.print(res.c_str());
                        res = "";

                        client.println();
                        break;
                    }
                    else
                    {
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {
                    currentLine += c;
                }

                if (currentLine.startsWith("GET /boot"))
                {
                    digitalWrite(LED_BUILTIN, HIGH);
                    turnOn = true;
                }
                if (currentLine.startsWith("GET /shutdown"))
                {
                    digitalWrite(LED_BUILTIN, LOW);
                    turnOn = false;
                }
            }
        }

        client.stop();
        Serial.println("client disonnected");
    }
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
    // print where to go in a browser:
    Serial.print("To see this page in action, open a browser to http://");
    Serial.println(ip);
}
