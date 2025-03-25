#ifndef MAX6675_INIT_H
#define MAX6675_INIT_H

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h" 

#define MAX6675_CS_PIN   GPIO_NUM_15
#define MAX6675_CLK_PIN  GPIO_NUM_14
#define MAX6675_MISO_PIN GPIO_NUM_12
#define MAX6675_SPI_HOST SPI2_HOST

typedef struct {
    spi_device_handle_t spi;  // SPI handle
    gpio_num_t cs_pin;        // GPIO пін для CS
} max6675_t;

// Функції для роботи з MAX6675
esp_err_t max6675_init(max6675_t *sensor, spi_host_device_t host, gpio_num_t clk_pin, gpio_num_t cs_pin, gpio_num_t miso_pin);
float read_celsius(max6675_t *sensor);
float read_fahrenheit(max6675_t *sensor);

#endif // MAX6675_INIT_H
