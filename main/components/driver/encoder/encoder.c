#include "encoder.h"
#include "ui.h"
#include "lvgl/src/indev/lv_indev.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "EncoderRead.h"  // Include the EncoderRead functionality

lv_indev_t *encoder_indev;
lv_group_t *groups[2];

lv_obj_t *screen1;
lv_obj_t *screen2;

// Ініціалізація груп для кнопок
void set_group()
{
    groups[0] = lv_group_create();
    groups[1] = lv_group_create();

    lv_group_add_obj(groups[0], ui_NEXT);
    lv_group_add_obj(groups[0], ui_Button3);
    lv_group_add_obj(groups[0], ui_Button5);

    lv_group_add_obj(groups[1], ui_BACK);
    lv_group_add_obj(groups[1], ui_Button6);
    lv_group_add_obj(groups[1], ui_Button4);
}

// Перемикання активної групи на основі поточного екрану
void switch_group(lv_obj_t *active_screen)
{
    if (active_screen == screen1) {
        lv_indev_set_group(encoder_indev, groups[0]);
    } else if (active_screen == screen2) {
        lv_indev_set_group(encoder_indev, groups[1]);
    }
}

// Отримання змін енкодера
int32_t enc_get_new_moves(void)
{
    // Використовуємо функцію з EncoderRead.c
    return get_counter();  // Отримуємо зміни від лічильника з EncoderRead.c
}

// Перевірка натискання кнопки енкодера
bool enc_pressed(void)
{
    return button_pressed();  // Використовуємо функцію з EncoderRead.c
}

// Функція читання енкодера
void encoder_read(lv_indev_t * indev, lv_indev_data_t* data)
{
    data->enc_diff = enc_get_new_moves();
  
    if (enc_pressed()) {
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// Ініціалізація енкодера
void encoder_init(void)
{
    // Ініціалізація пінів за допомогою функцій з EncoderRead.c
    gpio_set_direction(ENCODER_PIN_A, GPIO_MODE_INPUT);
    gpio_set_direction(ENCODER_PIN_B, GPIO_MODE_INPUT);
    gpio_set_direction(ENCODER_SW_PIN, GPIO_MODE_INPUT);

    // Створення пристрою введення (енкодера)
    encoder_indev = lv_indev_create();
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, encoder_read);
    lv_indev_set_display(encoder_indev, lv_disp_get_default());

    // Ініціалізація груп
    set_group();

    // Встановлення початкової групи
    lv_indev_set_group(encoder_indev, groups[0]);  // Явно вказуємо групу

    // Встановлення фокусу на перший елемент групи
    lv_group_focus_next(groups[0]);  // Фокус на наступний об'єкт
}

