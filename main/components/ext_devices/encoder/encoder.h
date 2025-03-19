#ifndef ENCODER_LVGL_H
#define ENCODER_LVGL_H

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "lvgl.h"

// Define some constants
#define PCNT_HIGH_LIMIT 100
#define PCNT_LOW_LIMIT  -100
#define GPIO_A 27
#define GPIO_B 26

static lv_indev_t *encoder_indev;
// Declare the LVGL group handlers
static lv_group_t *groups[2];

// Declare the Pulse Counter unit handle
static pcnt_unit_handle_t pcnt_unit;

// Declare the last count variable for the encoder
static int last_count;

// Function prototypes
void encoder_task(void *arg);
void setup_encoder(void);
void set_group(void);
void encoder_init(void);

#endif // ENCODER_LVGL_H
