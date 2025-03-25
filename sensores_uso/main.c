#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "wifi_init.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "sensor.h"
#include "esp_adc/adc_oneshot.h"

#define SSID "INFINITUM1E33"
#define PASSWORD "NgsNkCdK55"

#define TOPIC "iot"

#define LED GPIO_NUM_2

static const char *TAG = "MQTT";
static EventGroupHandle_t mqtt_event_group;

esp_mqtt_client_handle_t client;

char resp[128] = {0};

uint8_t led_state = 0;

char msg[50] = {0};


adc_oneshot_unit_handle_t adc1_handle;

void adc_init()  {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);
    
    adc_oneshot_chan_cfg_t config = { 
        (adc_atten_t)ADC_ATTEN_DB_11, 
        (adc_bitwidth_t)ADC_BITWIDTH_DEFAULT 
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(client, TOPIC, 0);
            xEventGroupSetBits(mqtt_event_group, CONNECTED_BIT);
            esp_mqtt_client_publish(client, "iot/state", "ESP32 conectado", 0, 0, 0);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Descoectado de MQTT");
            break;

        case MQTT_EVENT_DATA:
            memcpy(resp, event->data, event->data_len);
            resp[event->data_len] = '\0';

            break;
        
            default:
            break;
    }
}

void mqtt_task ( void ) {
    
    while (true) {
        gpio_set_level(LED, led_state);
        if (!strcmp(resp, "ON")) {
            led_state=1;
        }
        else if (!strcmp(resp, "OFF")) {
            led_state=0;
        }

        vTaskDelay(20/portTICK_PERIOD_MS);
    }

}

void send_led_state ( void ) {
    while (true) {

        if (gpio_get_level(LED)) {
            strcpy(msg, "LED: ON desde ESP32");
        }
        else {
            strcpy(msg, "LED: OFF desde ESP32");
        }
        esp_mqtt_client_publish(client, "iot/state", msg, 0, 0, 0);

        vTaskDelay(500/portTICK_PERIOD_MS);

    }
}

void send_temperature_humedity ( void ) {
    while (true) {
        char h[10]={0};
        char t[10]={0};

        temperature=get_temperature();
        humidity=get_humidity();


        sprintf(h, "%d", (int) humidity);
        sprintf(t, "%.2f", (float) temperature / 100);

        esp_mqtt_client_publish(client, "iot/temperature", t, 0, 0, 0);
        esp_mqtt_client_publish(client, "iot/humidity", h, 0, 0, 0);

        vTaskDelay(10000/portTICK_PERIOD_MS);
    }
}

void send_adc_value ( void ) {
    int adc_value = 0;
    float voltage = 0;
    char v[15] = {0};
    while (true) {
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_value);
        voltage =  ((float)adc_value * 3.9 ) / (2<<12);
        sprintf(v, "%.2f", voltage);
        esp_mqtt_client_publish(client, "iot/adc", v, 0, 0, 0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    i2c_master_init();
    i2c_slaves_devices_init();

    adc_init();
    
    get_temperature_compensation_values();
    get_humidity_compenstation_values();

    wifi_init(SSID, PASSWORD);

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_INPUT_OUTPUT);

    mqtt_event_group = xEventGroupCreate();

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.1.75:1883", 
        .credentials.username = "mqtt",
        .credentials.authentication.password = "mqtt",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    xEventGroupWaitBits(mqtt_event_group, CONNECTED_BIT, true, true, portMAX_DELAY);

    xTaskCreate( (TaskFunction_t) &mqtt_task, "mqtt_task", 4096, NULL, 5, NULL );
    xTaskCreate( (TaskFunction_t) &send_led_state, "send_led_state", 4096, NULL, 5, NULL );
    xTaskCreate( (TaskFunction_t) &send_temperature_humedity, "send_temperature_humedity", 4096, NULL, 5, NULL );
    xTaskCreate( (TaskFunction_t) &send_adc_value, "send_adc_value", 4096, NULL, 5, NULL );
}
