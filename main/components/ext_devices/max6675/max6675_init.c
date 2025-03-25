#include "max6675_init.h"
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"

#define TAG "MAX6675"

lv_obj_t *error_temp_window;
lv_obj_t *ok_temp_button;

esp_err_t max6675_init(max6675_t *sensor, spi_host_device_t host, gpio_num_t clk_pin, gpio_num_t cs_pin, gpio_num_t miso_pin) {
    spi_bus_config_t buscfg = {
        .miso_io_num = miso_pin,
        .mosi_io_num = -1, // Not used
        .sclk_io_num = clk_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4000000, // 1 MHz
        .mode = 0, // SPI mode 0
        .spics_io_num = cs_pin,
        .queue_size = 1
    };

    gpio_set_direction(cs_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(cs_pin, 1); // CS inactive

    esp_err_t ret = spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus");
        return ret;
    }

    ret = spi_bus_add_device(host, &devcfg, &sensor->spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add MAX6675 to SPI bus");
        return ret;
    }

    sensor->cs_pin = cs_pin;
    return ESP_OK;
}

static uint16_t read_raw(max6675_t *sensor) {
    uint8_t data[2] = {0};
    spi_transaction_t trans = {
        .length = 16,
        .rxlength = 16,
        .tx_buffer = NULL,
        .rx_buffer = data
    };

    gpio_set_level(sensor->cs_pin, 0);
    esp_err_t ret = spi_device_transmit(sensor->spi, &trans);
    gpio_set_level(sensor->cs_pin, 1);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI read failed");
        return 0xFFFF; // Error value
    }

    uint16_t raw = (data[0] << 8) | data[1];
    return raw;
}

void error_window_temp(const char *error_message)
{
    // Create error window
    error_temp_window = lv_obj_create(lv_scr_act());
    lv_obj_set_size(error_temp_window, LV_HOR_RES, LV_VER_RES / 2); 
    lv_obj_align(error_temp_window, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(error_temp_window, lv_color_make(255, 0, 0), LV_PART_MAIN);
    lv_obj_t *label = lv_label_create(error_temp_window);
    lv_label_set_text(label, "Temperature sensor Error!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);

    // Create "OK" button
    ok_temp_button = lv_btn_create(error_temp_window);
    lv_obj_align(ok_temp_button, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_size(ok_temp_button, 100, 50);
    lv_obj_t *btn_label = lv_label_create(ok_temp_button);
    lv_label_set_text(btn_label, "OK");
}

void ok_button_temp_event(lv_event_t *e)
{
    lv_obj_t *error_temp_window = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_del(error_temp_window);
    }
}

float read_celsius(max6675_t *sensor) 
{
    uint16_t raw = read_raw(sensor);
    ESP_LOGI(TAG, "Raw data: 0x%04X", raw);
    if (raw & 0x4) 
    {
        ESP_LOGW(TAG, "No thermocouple detected");
        error_window_temp("No thermocouple detected");
        return 0;
    }

    raw >>= 3;
    return raw * 0.25;
}

float read_fahrenheit(max6675_t *sensor) {
    return read_celsius(sensor) * 9.0 / 5.0 + 32;
}
