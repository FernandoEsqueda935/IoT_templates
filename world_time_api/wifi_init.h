#include "esp_wifi.h"
#include "esp_log.h"
#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/event_groups.h"
#include "nvs_flash.h"
#include "string.h"

#define TAG_WIFI "WIFI"

static const int CONNECTED_BIT = BIT0;
static EventGroupHandle_t wifi_event_group;

void wifi_init (const char * ssid, const char * password);
static void event_handler_wifi(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);