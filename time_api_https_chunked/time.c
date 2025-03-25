#include "wifi_init.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SSID "Mega-2.4G-BEAE"
#define PASSWORD "ZeVe5BeabN"

#define URL "https://timeapi.io/api/time/current/zone?timeZone=America%2FTijuana"


char * buffer = NULL;
int64_t content_length = 0; 

extern const uint8_t time_pem_start[] asm("_binary_time_pem_start");
extern const uint8_t time_pem_end[] asm("_binary_time_pem_end");

void http_handler ( esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            printf("HTTP_EVENT_ERROR\n");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            printf("HTTP_EVENT_ON_CONNECTED\n");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            printf("HTTP_EVENT_HEADERS_SENT\n");
            break;
        case HTTP_EVENT_ON_HEADER:
            printf("HTTP_EVENT_ON_HEADER: %s: %s\n", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            if (evt->data_len > 0) {
                buffer = realloc(buffer, content_length + evt->data_len + 1);
                if (buffer == NULL) {
                    printf("Failed to allocate memory\n");
                    return;
                }
                memcpy(buffer + content_length, evt->data, evt->data_len);
                content_length += evt->data_len;
                buffer[content_length] = '\0'; // Null-terminate the string
                printf("HTTP_EVENT_ON_DATA: %s\n", buffer);
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            printf("HTTP_EVENT_DISCONNECTED\n");
            break;
        default:
            break;
    }

}


void http_init() {
    
    char hora[9]={0};
    uint8_t f = 0;

    esp_http_client_config_t cfg = {
        .url = URL,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .cert_pem = (char *)time_pem_start,
        .event_handler = http_handler,
        .buffer_size = 512,
    };
    while (true) {
        esp_http_client_handle_t client = esp_http_client_init(&cfg);
        esp_http_client_set_header(client, "Content-Type", "application/json");

        esp_http_client_perform(client);
        
        esp_http_client_close(client);
        esp_http_client_cleanup(client);

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


        free(buffer);
        content_length = 0;
        buffer = NULL;

        vTaskDelay(15000 / portTICK_PERIOD_MS);


    }
}

void app_main(void) {
    wifi_init(SSID, PASSWORD);
    xTaskCreate(http_init, "http_init", 4096, NULL, 5, NULL);
}
