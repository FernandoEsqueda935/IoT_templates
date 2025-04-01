#ifndef WIFI_CREDENTIALS_NVS_H
#define WIFI_CREDENTIALS_NVS_H

void init_nvs_flash (void);

void nvs_set_get_wifi_credentials (char * ssid, char * password);

void nvs_set_string(char *key, char *str);

int nvs_get_string(char *key, char **str);

void nvs_set_uint8(char *key, uint8_t value);

void nvs_get_uint8(char *key, uint8_t *value);

#endif