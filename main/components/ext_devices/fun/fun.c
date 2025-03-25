#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define FAN_GPIO 5
#define FAN_PWM_CHANNEL LEDC_CHANNEL_0
#define FAN_PWM_FREQ 25000
#define FAN_PWM_RES LEDC_TIMER_8_BIT

void fan_init() {
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = FAN_PWM_RES,
        .freq_hz = FAN_PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = FAN_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = FAN_PWM_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);
}

void fan_set_speed(uint8_t speed) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, FAN_PWM_CHANNEL, speed);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, FAN_PWM_CHANNEL);
}

void fan_init() {
    fan_init();
    
    while (1) {
        fan_set_speed(128);  // 50% швидкості
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        fan_set_speed(255);  // 100% швидкості
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        fan_set_speed(0);  // Вимкнути вентилятор
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
