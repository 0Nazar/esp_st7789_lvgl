#ifndef ENCODER_H
#define ENCODER_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"

// Визначення пінів
#define ENCODER_PIN_A  27    // Змінити на свій пін для A
#define ENCODER_PIN_B  26    // Змінити на свій пін для B
#define BUTTON_PIN     25    // Змінити на свій пін для кнопки

// Оголошення функцій
void encoder_isr_handler(void* arg);   // Функція обробки переривань енкодера
bool button_pressed(void);             // Функція для перевірки натискання кнопки енкодера
int32_t get_counter(void);             // Функція для отримання значення лічильника
void reset_counter(void);              // Функція для скидання лічильника
void set_counter(int32_t value);       // Функція для встановлення значення лічильника

#endif // ENCODER_H
