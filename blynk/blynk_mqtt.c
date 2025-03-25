#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_init.h"
#include "esp_log.h"

#define SSID "Mega-2.4G-BEAE"
#define PASSWORD "ZeVe5BeabN"

#define TOPIC "ds/Switch Value"

#define TAG "MQTT"


esp_mqtt_client_handle_t client;

char value[2] = "1";

void mqtt_task (void) {
    while (true) {
        if (value[0]-'0') {
            value[0] = '0' ;
        }
        else {
            value[0] = '1';
        }

        esp_mqtt_client_publish(client, TOPIC, value, 1, 0, 0);
        ESP_LOGI(TAG, "Published: %s", value);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}


void mqtt_event_handler (void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    
        switch (event_id) {
            case MQTT_EVENT_CONNECTED:
                esp_mqtt_client_publish(client, TOPIC, value, 1, 0, 0);
                ESP_LOGI(TAG, "Published: %s", value);
                printf("MQTT_EVENT_CONNECTED\n");
                break;
            case MQTT_EVENT_DISCONNECTED:
                printf("MQTT_EVENT_DISCONNECTED\n");
                break;
        }
}


void mqtt_init ( void ) {

    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = "mqtt://blynk.cloud",
        .broker.address.port = 1883,
        .credentials.client_id = "",
        .credentials.username = "device",
        .credentials.authentication.password = "fSOOIr6uHIbUTpxP0l0xt5jUujSCD4e-",
        .session.keepalive = 45,
    };

    client = esp_mqtt_client_init(&cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

}


void app_main(void)
{
    wifi_init(SSID, PASSWORD);
    mqtt_init();
    xTaskCreate(mqtt_task, "mqtt_task", 4096, NULL, 5, NULL);

}
