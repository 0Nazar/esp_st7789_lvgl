#ifndef ENCODER_H
#define ENCODER_H

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "lvgl.h"
#include "ui.h"
#include "sdkconfig.h"

// Encoder button pin definition (should match your hardware)
#define ENCODER_A GPIO_NUM_27   // Change GPIO as per your wiring
#define ENCODER_B GPIO_NUM_26   // Change GPIO as per your wiring
#define ENCODER_BTN GPIO_NUM_25 // Change GPIO as per your wiring

// Initializes the encoder, including the PCNT unit, GPIO, and LVGL input device
void encoder_init(void);

// The task that handles encoder polling
void encoder_task(void *arg);

// ISR handler for the encoder button press
static void IRAM_ATTR encoder_btn_isr_handler(void *arg);

// The read callback for the encoder used by LVGL
static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);

#endif // ENCODER_H