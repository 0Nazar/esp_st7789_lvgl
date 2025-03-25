#include <math.h>
#include <stdio.h>

#include "PID.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "max6675_init.h"
#include "disp_draw.h"

#define FAN_PIN 5 


struct pid_controller ctrldata;
pid_t pid;

float input = 0.0;
float output_fan_speed = 0.0;
float setpoint = 15.0;

// Control loop gains
float kp = 2.5, ki = 1.0, kd = 1.0;  

void setup_pid(void) 
{
    pid = pid_create(&ctrldata, &input, &output_fan_speed, &setpoint, kp, ki, kd);
    pid_limits(pid, 0, 255);  // Вихід PID обмежено від 0 до 255 (PWM)
    pid_auto(pid);
}

void control_fan_speed(void) 
{
    int pwm_value = (int)output_fan_speed;
    if (pwm_value < 0) pwm_value = 0;
    if (pwm_value > 255) pwm_value = 255;

    // Налаштовуємо швидкість вентилятора за допомогою PWM
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pwm_value);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

void pid_control_loop(void) 
{
    if (pid_need_compute(pid)) 
    {
        input = read_celsius(&sensor);
        pid_compute(pid);
        control_fan_speed();
    }
}

// FreeRTOS task для обробки температури
void pid_task(void *arg) {
    while (1) {

        pid_control_loop();
        
        control_fan_speed();
        
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}

void pid_init(void) {
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,  
        .gpio_num = FAN_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);

    setup_pid();

    xTaskCreate(pid_task, "PID Control Task", 4096, NULL, 10, NULL);
}