#include "camera.h"
#include "esp_camera.h"

void send_image_task(void *parameter);

camera_config_t config;

void init_camera()
{
    //  init camera
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // small resolution
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;

    // medium resolution
    // config.frame_size = FRAMESIZE_VGA;
    // config.jpeg_quality = 10;

    // high quality, too slow??
    // config.frame_size = FRAMESIZE_XGA;
    // config.jpeg_quality = 10;

    // not sure if this matters
    config.fb_count = 1;

    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    // start send image task
    xTaskCreatePinnedToCore(
        send_image_task,   /* Function to implement the task */
        "send_image_task", /* Name of the task */
        10000,             /* Stack size in words */
        NULL,              /* Task input parameter */
        1,                 /* Priority of the task */
        NULL,              /* Task handle. */
        1);                /* Core where the task should run */

    Serial.println("Camera init success");
}

void send_image()
{
    // send the image to the server
    camera_fb_t *fb = NULL;

    // Take Picture with Camera
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        delay(1000);

        return;
    }


    // check if the websocket is connected
    if (!websockets_client.isConnected())
    {
        Serial.println("Websocket not connected");
        delay(1000);
        esp_camera_fb_return(fb);
        return;
    }

    // send the image to the server
    websockets_client.sendBIN(fb->buf, fb->len);

    // Free the frame buffer space
    esp_camera_fb_return(fb);
    delay(100);
}

void send_image_task(void *parameter)
{
    while (1)
    {
        send_image();
        delay(50);
    }
} // end send_image_task