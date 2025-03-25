#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"


#define I2C_MASTER_NUM              I2C_NUM_0

//bits de temperatura ready o de temperatura solicitada
#define I2C_GET_TEMPERATURE BIT0
#define I2C_TEMPERATURE_READY BIT1

//los pines para esp en modo maestro
#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21

extern long signed int temperature;
extern long signed int humidity;

extern long signed int t_fine;

extern long signed int DIG_T1;
extern long signed int DIG_T2;
extern long signed int DIG_T3;

extern unsigned char DIG_H1;
extern signed int DIG_H2;
extern unsigned char DIG_H3;
extern signed int DIG_H4;
extern signed int DIG_H5;
extern signed char DIG_H6;

long signed int get_temperature();
long signed int get_humidity();


void i2c_master_init();
void i2c_slaves_devices_init();

void get_temperature_compensation_values();
void get_humidity_compenstation_values();