#ifndef ENCODER_H
#define ENCODER_H

#include "lvgl/lvgl.h"
#include "driver/gpio.h"
#include <stdint.h>
#include <stdbool.h>

// Функції для інтеграції з LVGL
void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);
void encoder_init(void);
void set_group(void);
void update_active_screen(lv_obj_t *new_screen);

#endif // ENCODER_H