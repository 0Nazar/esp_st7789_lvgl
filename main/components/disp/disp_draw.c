#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "lvgl.h"
#include "lvgl/src/display/lv_display.h"
#include "st7789.h"
#include "ui.h"

#include "max6675_init.h"
#include "DHT.h"
#include "disp_draw.h"

#define TAG "DISP_DRAW"

TFT_t my_tft;

static lv_obj_t *label_humidity;

static lv_obj_t *label_temperature; 

// Calculate buffer size
#define BUF_SIZE (LV_HOR_RES * LV_VER_RES * sizeof(lv_color_t) / 2)

static lv_color_t buf1[1000];
static lv_color_t buf2[1000];

uint32_t my_tick_get_cb()
{
    return esp_timer_get_time() / 1000;
}
static void increase_lvgl_tick(void *arg)
{
    lv_tick_inc(2);
}

void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t * buf1 = (uint16_t *)px_map; 
    int32_t x, y;
    lcdDrawBufWithOffset(&my_tft,area->x1,area->y1, area->x2, area->y2, buf1);


    lv_display_flush_ready(display);
}

static SemaphoreHandle_t lvgl_mux = NULL;

void lvgl_unlock(void)
{
    xSemaphoreGiveRecursive(lvgl_mux);
}

bool lvgl_lock(int timeout_ms)
{
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks) == pdTRUE;
}

void lvgl_port_task(void *arg)
{
    uint32_t task_delay_ms = 500;
    while (1) {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            lvgl_unlock();
        }
        if (task_delay_ms > 500) {
            task_delay_ms = 500;
        } else if (task_delay_ms < 500) {
            task_delay_ms = 500;
        }
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}


void update_sensor_task(void *pvParameters) {
    while (1) {
        // Читання даних з датчика DHT
        int result = read_hum();
        if (result == DHT_OK) {
            float humidity = getHumidity();
            // Форматування рядка для вологи
            char humidity_str[32];
            snprintf(humidity_str, sizeof(humidity_str), "Humidity: %.1f%%", humidity);

            // Оновлення тексту для вологості на екрані
            if (lvgl_lock(-1)) {
                lv_label_set_text(label_humidity, humidity_str);
                lvgl_unlock();
            }
        } else {
            ESP_LOGE(TAG, "Failed to read DHT sensor");
            // errorHandler(DHT_TIMEOUT_ERROR);        
        }

        // Читання температури з MAX6675
        float temperature = read_celsius(&sensor);
        // Форматування рядка для температури
        char temp_str[32];
        snprintf(temp_str, sizeof(temp_str), "Temp: %.2f°C", temperature);

        // Оновлення тексту для температури на екрані
        if (lvgl_lock(-1)) {
            lv_label_set_text(label_temperature, temp_str);
            lvgl_unlock();
        } 
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void disp_draw_init(void)
{
    spi_master_init(&my_tft, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS, PIN_NUM_DC, PIN_NUM_RST, PIN_NUM_BL);

    max6675_init(&sensor, MAX6675_SPI_HOST, MAX6675_CLK_PIN, MAX6675_CS_PIN, MAX6675_MISO_PIN);

    lcdInit(&my_tft, 240, 240, 0, 0);

    lv_init();   
    
    lvgl_mux = xSemaphoreCreateRecursiveMutex();

    lv_display_t * display1 = lv_display_create(LV_HOR_RES, LV_VER_RES);

    lv_display_set_flush_cb(display1, my_flush_cb);

    lv_display_set_buffers(display1, buf1, buf2, 2000, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_tick_set_cb(my_tick_get_cb);

    BaseType_t task_creation_result = xTaskCreate(lvgl_port_task, "lvgl_port_task", 4096, NULL, 1, NULL);
    if (task_creation_result != pdPASS) {
        return;  
    } 

    if (lvgl_lock(-1)) 
    {
        ui_init(); 
        label_humidity = lv_label_create(lv_scr_act());
        lv_obj_align(label_humidity, LV_ALIGN_TOP_MID, 0, 180);
        lv_label_set_text(label_humidity, "Humidity: --");

        label_temperature = lv_label_create(lv_scr_act());
        lv_label_set_text(label_temperature, "Temp: --");
        lv_obj_align(label_temperature, LV_ALIGN_TOP_RIGHT, -75, 200);
        lvgl_unlock();
    }  

    setDHTgpio(GPIO_NUM_4);
    
    xTaskCreate(update_sensor_task, "update_humidity_task", 4096, NULL, 3, NULL);  
}