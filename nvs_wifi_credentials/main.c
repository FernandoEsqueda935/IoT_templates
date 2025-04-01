
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "wifi_credentials_nvs.h"
#include "esp_log.h"

#define TAG "MAIN"

void app_main(void) {
    init_nvs_flash();

    nvs_set_get_wifi_credentials("hola123", "jajatl");

    nvs_flash_deinit();

    esp_restart();
}


