#include "wifi_init.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SSID "Mega-2.4G-BEAE"
#define PASSWORD "ZeVe5BeabN"

#define URL "http://worldtimeapi.org/api/timezone/America/Tijuana"


void http_init() {
    int64_t content_length = 0;
    char hora[9]={0};
    uint8_t f = 0;

    esp_http_client_config_t cfg = {
        .url = URL,
    };
    while (true) {
        esp_http_client_handle_t client = esp_http_client_init(&cfg);
        esp_http_client_open(client, 0);
        content_length =  esp_http_client_fetch_headers(client);
        char buffer[content_length];
        esp_http_client_read(client, buffer, (int) content_length);        
        esp_http_client_close(client);

        uint8_t cnt = 0;
        while (cnt < content_length && f < 2) {
            if (buffer[cnt] == 'T') {
                f++;
            }
            cnt++;
        }

        for (uint8_t i = 0; i < 8; i++) {
            hora[i] = buffer[cnt + i];
        }   
        hora[8] = '\0';

        printf("Hora: %s\n", hora);

        f = 0;

        vTaskDelay(15000 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    wifi_init(SSID, PASSWORD);
    xTaskCreate(http_init, "http_init", 4096, NULL, 5, NULL);
}
