#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"


static volatile int32_t counter = 0;
static volatile uint32_t lastIncReadTime = 0;
static volatile uint32_t lastDecReadTime = 0;
static const uint32_t pauseLength = 500;   // Тривалість паузи в мікросекундах
static const uint32_t fastIncrement = 10;  // Швидке інкрементування
static uint8_t old_AB = 3;
static int8_t encval = 0;

// Масив для визначення змін енкодера
static const int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

// Функція для обробки переривання енкодера
static void IRAM_ATTR encoder_isr_handler(void* arg) {
    static uint32_t lastDebounceTime = 0;
    uint32_t currentTime = esp_timer_get_time();

    if (currentTime - lastDebounceTime > 200) { // 200 µs debounce
        old_AB <<= 2;
        if (gpio_get_level(ENCODER_PIN_A)) old_AB |= 0x02;
        if (gpio_get_level(ENCODER_PIN_B)) old_AB |= 0x01;

        encval += enc_states[(old_AB & 0x0F)];

        if (encval > 3) {
            int changevalue = 1;
            if ((esp_timer_get_time() - lastIncReadTime) < pauseLength) {
                changevalue = fastIncrement * changevalue;
            }
            lastIncReadTime = esp_timer_get_time();
            counter += changevalue;
            encval = 0;
        }
        else if (encval < -3) {
            int changevalue = -1;
            if ((esp_timer_get_time() - lastDecReadTime) < pauseLength) {
                changevalue = fastIncrement * changevalue;
            }
            lastDecReadTime = esp_timer_get_time();
            counter += changevalue;
            encval = 0;
        }
        lastDebounceTime = currentTime;
    }
}

// Функція для обробки натискання кнопки енкодера
bool button_pressed(void) {
    static uint32_t lastPressTime = 0;
    uint32_t currentTime = esp_timer_get_time();

    bool currentState = gpio_get_level(BUTTON_PIN) == HIGH;
    if (currentState) {
        if (currentTime - lastPressTime > 50) { // 50 ms debounce
            lastPressTime = currentTime;
            return true;
        }
    }

    return false;
}

// Отримання значення лічильника
int32_t get_counter(void) {
    return counter;
}

// Скидання лічильника
void reset_counter(void) {
    counter = 0;
}

// Встановлення значення лічильника
void set_counter(int32_t value) {
    // Заборона переривань для безпечного оновлення лічильника
    taskENTER_CRITICAL();
    counter = value;
    taskEXIT_CRITICAL();
}