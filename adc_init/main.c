#include "adc_init.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

uint8_t channels[2] = {ADC_CHANNEL_0, ADC_CHANNEL_6};
uint8_t adc_channels_num = 2;   
adc_oneshot_chan_cfg_t config[2] = {
    {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_0,
    },
    {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_0,
    }
};

void app_main(void) {
    int adc_reading = 0;
    adc_oneshot_unit_handle_t adc_handle;
    adc_init(config, channels, adc_channels_num, &adc_handle);
    while(1) {
        for (uint8_t channel_num=0; channel_num<adc_channels_num; channel_num++) {
            adc_oneshot_read(adc_handle, channels[channel_num], &adc_reading);
            ESP_LOGI("ADC", "Channel %d: %d", channels[channel_num], adc_reading);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
