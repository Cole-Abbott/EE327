#include "myWebsocket.h"

// SECTION: GLOBALS
WebSocketsClient websockets_client;

// SECTION: FUNCTIONS
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
void client_loop_task(void *pvParameters);

// SECTION: CALLBACKS
void (*handleMessage)(char *message);


/**
 * @brief Connects to the wifi network specified in websockets.h
 *
 */
void init_wifi()
{
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress()); // displays MAC address to register with device northwestern, if desired

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID);

    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void init_websockets(void (*func)(char *))
{
    // Connect to the websocket server
    websockets_client.begin(WEB_SOCKET_SERVER, WEB_SOCKET_PORT, WEB_SOCKET_PATH);
    websockets_client.onEvent(webSocketEvent);

    // set Handlemessage callback
    handleMessage = func;
    
    // Start task to handle server loops
    xTaskCreatePinnedToCore(
        client_loop_task,             // Function to implement the task
        "client_loop_task",           // Name of the task
        8192,                         // Stack size in words
        NULL,                         // Task input parameter
        1,                            // Priority of the task
        NULL,                         // Task handle.
        0); // Core where the task should run

    Serial.println("Websocket client started");
}

/**
 * @brief Task to handle websocket events
 *
 * @param pvParameters freeRTOS task parameters (not used)
 */
void client_loop_task(void *pvParameters)
{
    while (1)
    {
        websockets_client.loop();
        delay(50);
    }
} // end client_loop_task


/**
 * @brief handles websocket events by calling the handleMessage callback passed in init_websockets
 *
 */
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        break;
    case WStype_TEXT:
        Serial.printf("[WSc] get text: %s\n", payload);
        handleMessage((char *)payload);
        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        break;
    }
} // end webSocketEvent


