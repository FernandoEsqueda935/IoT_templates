#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"

#include "wifi_credentials_nvs.h"

#define TAG "NVS"

void nvs_set_get_wifi_credentials (char * ssid, char * password) {
    char * str = NULL;

    int nvs_err = 0;

    nvs_err = nvs_get_string("SSID", &str);

    if (nvs_err == ESP_ERR_NVS_NOT_FOUND) {
        nvs_set_string("SSID", ssid);
        nvs_set_string("PASSWORD", password);
        ESP_LOGI(TAG, "No se encontro la clave SSID, se escribio correctamente");
    }
    else {
        ESP_LOGI(TAG, "Se encontro la clave SSID: %s", str);
        free(str);
        str = NULL;
        nvs_get_string("PASSWORD", &str);
        ESP_LOGI(TAG, "Se encontro la clave PASSWORD: %s", str);
        free(str);
    }
}


void init_nvs_flash (void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}


void nvs_set_string( char * key, char * str ) {
    nvs_handle_t my_nvs_handle;

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "No se pudo abrir la particion NVS");
    }
    else {
        ESP_LOGI(TAG, "Se abrio la particion NVS correctamente");
        err = nvs_set_str(my_nvs_handle, key, str);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Se escribiio correctamente key: %s, %s", key, str);
        }
        else {
            ESP_LOGI(TAG, "Cadena escrita correctamente en NVS");
        }
    }
    nvs_close(my_nvs_handle);
}

//str tiene que ser un puntero a char, y despues de utilizarlo tiene que liberarse 
int nvs_get_string ( char * key, char ** str) {
    nvs_handle_t my_nvs_handle;

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);
    
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "No se pudo abrir la particion NVS");
    }
    else {
        size_t required_size = 0;
        
        //se obtiene el tamaño de la cadena con la clave
        err = nvs_get_str(my_nvs_handle, key, NULL, &required_size);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "No se encontro la clave %s", key);
            return ESP_ERR_NVS_NOT_FOUND;
        }
        else if (err != ESP_OK) {
            ESP_LOGI(TAG, "Error al leer la clave %s", key);
        }
        else {
            ESP_LOGI(TAG, "Se encontro la clave %s", key);
        }

        *str = malloc(required_size);
        if (str == NULL) {
            ESP_LOGI(TAG, "No se pudo reservar memoria para la cadena");
        }
        else {
            err = nvs_get_str(my_nvs_handle, key, *str, &required_size);
            if (err != ESP_OK) {
                ESP_LOGI(TAG, "No se pudo leer la cadena de NVS");
            }
            else {
                ESP_LOGI(TAG, "Cadena leida correctamente de NVS: %s", *str);
            }
        }
    }
    nvs_close(my_nvs_handle);
    return 0;
}

void nvs_set_uint8 (char * key, uint8_t value) {
    nvs_handle_t my_nvs_handle;

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "No se pudo abrir la particion NVS");
    }
    else {
        ESP_LOGI(TAG, "Se abrio la particion NVS correctamente");
        err = nvs_set_u8(my_nvs_handle, key, value);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Se escribiio correctamente key: %s, %d", key, value);
        }
        else {
            ESP_LOGI(TAG, "Valor escrito correctamente en NVS");
        }
    }
    nvs_close(my_nvs_handle);
}

void nvs_get_uint8 (char * key, uint8_t * value) {
    nvs_handle_t my_nvs_handle;

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);
    
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "No se pudo abrir la particion NVS");
    }
    else {
        err = nvs_get_u8(my_nvs_handle, key, value);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "No se encontro la clave %s", key);
        }
        else if (err != ESP_OK) {
            ESP_LOGI(TAG, "Error al leer la clave %s", key);
        }
        else {
            ESP_LOGI(TAG, "Se encontro la clave %s", key);
        }
    }
    nvs_close(my_nvs_handle);
}