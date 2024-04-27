#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>


// SECTION: DEFINITIONS
#define SSID "Device-Northwestern" // Make sure to register with device northwestern prior to using this
#define WEB_SOCKET_SERVER "10.105.1.116"
#define WEB_SOCKET_PORT 8888
#define WEB_SOCKET_PATH "/esp32"


// SECTION: FUNCTION DECLARATIONS

void init_wifi();
void init_websockets(void (*func)(char *));


extern WebSocketsClient websockets_client;