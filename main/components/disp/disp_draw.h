#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "lvgl.h"
#include "st7789.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "disp_draw.h"

// Function declarations
uint32_t my_tick_get_cb(void);
static void increase_lvgl_tick(void *arg);
void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);

// Mutex functions
void lvgl_unlock(void);
bool lvgl_lock(int timeout_ms);

void update_sensor_task(void *pvParameters);

// Display initialization
void disp_draw_init(void);

// LVGL task
void lvgl_port_task(void *arg);

#endif // LVGL_DISPLAY_H
