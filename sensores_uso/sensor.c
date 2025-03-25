#include "sensor.h"

//Para controlar cuando se deba hacer una medicion de temperatura
EventGroupHandle_t i2c_event_group;

i2c_master_bus_handle_t bus_handle;

i2c_master_dev_handle_t dev_handle;

long signed int temperature = 0;
long signed int humidity = 0;

long signed int t_fine;

long signed int DIG_T1 = 27698;
long signed int DIG_T2= -410;
long signed int DIG_T3= 189;

unsigned char DIG_H1 = 75;
signed int DIG_H2= 356;
unsigned char DIG_H3= 0;
signed int DIG_H4= 0;
signed int DIG_H5= 0;
signed char DIG_H6= 30;

void i2c_master_init() {
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
        .i2c_port = I2C_MASTER_NUM,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));
    
}

void i2c_slaves_devices_init() {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x76,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
}

long signed int BME280_temperature_compensate_T_double(long signed int adc_T) {
    long signed int var1, var2;
    var1 = ((((adc_T>>3) - ((long signed int)DIG_T1<<1))) * ((long signed int)DIG_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((long signed int)DIG_T1)) * ((adc_T>>4) - ((long signed int)DIG_T1))) >> 12) * ((long signed int)DIG_T3)) >> 14;
    return var1 + var2;
}

long signed int BME280_humidity_compensate_T_double (long signed int adc_H) {
    long signed int value;
    value = (t_fine - ((long signed int)76800));
    value = (((((adc_H << 14) - (((long signed int)DIG_H4) << 20) - (((long signed int)DIG_H5) * value)) + ((long signed int)16384)) >> 15) * 
    (((((((value * ((long signed int)DIG_H6)) >> 10) * (((value * ((long signed int)DIG_H3)) >> 11) + ((long signed int)32768)) >> 10) + 
    ((long signed int)2097152)) * ((long signed int)DIG_H2) + 8192) >> 14)));
    value = (value - (((((value >> 15) * (value >> 15)) >> 7) * ((long signed int)DIG_H1)) >> 4));
    value = (value < 0 ? 0 : value);
    value = (value > 419430400 ? 419430400 : value);
    return value>>12;
}

void get_temperature_compensation_values() {
    uint8_t r_buffer[6] = {0 , 0 , 0, 0 , 0 , 0 };
    uint8_t data= 0x88;
    
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &data, 1, r_buffer, 6, -1));
    DIG_T1 = r_buffer[0] | r_buffer[1] << 8;
    DIG_T2 = r_buffer[2] | r_buffer[3] << 8;
    DIG_T3 = r_buffer[4] | r_buffer[5] << 8;
}

void get_humidity_compenstation_values() {
    uint8_t r_buffer[8] = {0 , 0 , 0, 0 , 0 , 0, 0, 0};
    
    uint8_t data= 0xA1;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &data, 1, r_buffer, 1, -1));
    
    data= 0xE1;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &data, 1, (r_buffer + 1), 7, -1));
    
    DIG_H1 = r_buffer[0];
    DIG_H2 = r_buffer[1] | r_buffer[2] << 8;
    DIG_H3 = r_buffer[3];
    DIG_H4 = r_buffer[4] << 4 | (r_buffer[5] & 0x0F);
    DIG_H5 = r_buffer[6] << 4 | r_buffer[5] >> 4;
    DIG_H6 = r_buffer[7];

}

long signed int get_temperature() {
    //instrucciones necesarias para solo obtener la medicion de temperatura, direccion/valor 
    uint8_t data_wr[8] = {0xF5, 0x00, 0xF2, 0x00, 0xF4, 0xA1};
    uint8_t r_buffer[6] = {0 , 0 , 0, 0 , 0 , 0 };
    
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_wr, 6, -1));

    data_wr[0]= 0xF3;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, data_wr, 1, r_buffer, 1, -1));

    while (r_buffer[0] & (1<<0|1<<3)) {
        ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, data_wr, 1, r_buffer, 1, -1));
    }

    data_wr[0]= 0xFA;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, data_wr, 1, r_buffer, 3, -1));

    long signed int adc_T = 0;
    adc_T = r_buffer[0] << 12 | r_buffer[1] << 4 | r_buffer[2] >> 4;

    t_fine = BME280_temperature_compensate_T_double(adc_T);

    long signed int T = (t_fine * 5 + 128) >> 8;

    return T;
}

long signed int get_humidity() {
    //instrucciones necesarias para solo obtener la medicion de humedad, direccion/valor 
    get_temperature();
    
    uint8_t data_wr[8] = {0xF5, 0x00, 0xF2, 0x05, 0xF4, 0x01};
    uint8_t r_buffer[6] = {0 , 0 , 0, 0 , 0 , 0 };
    
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_wr, 6, -1));

    data_wr[0]= 0xF3;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, data_wr, 1, r_buffer, 1, -1));

    while (r_buffer[0] & (1<<0|1<<3)) {
        ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, data_wr, 1, r_buffer, 1, -1));
    }

    data_wr[0]= 0xFD;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, data_wr, 1, r_buffer, 2, -1));

    long signed int adc_H = 0;

    adc_H = r_buffer[0] << 8 | r_buffer[1];

    long signed int h_fine = BME280_humidity_compensate_T_double(adc_H);

    long signed int H = h_fine;

    return H>>10;

}