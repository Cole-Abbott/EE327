#include "Arduino.h"
#include "myWebsocket.h"
#include "camera.h"



int i = 0;

// Function to handle the text message
void textHandler(char *message)
{
    

}

void setup()
{
    Serial.begin(115200);

    // init everything
    init_wifi();
    init_websockets(textHandler);
    init_camera();

    Serial.println("End of setup");
}

void loop()
{
    send_image();
    delay(10);
}