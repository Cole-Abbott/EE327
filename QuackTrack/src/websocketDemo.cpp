// #include <WiFi.h>
// #include <WebSocketsClient.h>

// #define SSID "Device-Northwestern" // Make sure to register with device northwestern prior to using this
// #define WEB_SOCKET_SERVER "10.105.1.116"
// #define WEB_SOCKET_PORT 8888
// #define WEB_SOCKET_PATH "/esp32"


// WebSocketsClient websockets_client;
// void websockets_event(WStype_t type, uint8_t* payload, size_t length);

// void setup() {
//     Serial.begin(115200);
// 	Serial.println(WiFi.macAddress()); // displays MAC address to register with device northwestern, if desired
	
// 	// Connect to WiFi
//     WiFi.mode(WIFI_STA);
// 	WiFi.begin(SSID);
	
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         Serial.println("Connecting to WiFi...");
//     }
//     Serial.println("");
//     Serial.print("Connected to ");
//     Serial.println(SSID);
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());

//     websockets_client.begin(WEB_SOCKET_SERVER, WEB_SOCKET_PORT, WEB_SOCKET_PATH); // replace with your server's info
//     websockets_client.onEvent(websockets_event);
// }

// void loop() {

  

// }

// void websockets_event(WStype_t type, uint8_t* payload, size_t length) {
//     switch(type) {
//         case WStype_DISCONNECTED:
//             Serial.println("Disconnected from WebSocket server");
//             break;
//         case WStype_CONNECTED:
//             Serial.println("Connected to WebSocket server");
//             break;
//         case WStype_TEXT:
//             Serial.println("Received message from WebSocket server: " + String((char*)payload));
//             break;
//     }
// }