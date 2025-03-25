#include "adc_init.h"

void adc_init(adc_oneshot_chan_cfg_t * config,  uint8_t * channels, uint8_t channels_num, adc_oneshot_unit_handle_t * adc_handle) {
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit( &adc_config, adc_handle);

    for (uint8_t channel_num=0; channel_num<channels_num; channel_num++) {
        adc_oneshot_config_channel(*adc_handle, channels[channel_num] ,&config[channel_num]);
    }
}