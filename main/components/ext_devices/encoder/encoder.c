#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "lvgl.h"
#include "encoder.h"
#include "ui.h"


static const char *TAG = "pcnt_lvgl";
#define PCNT_HIGH_LIMIT 100
#define PCNT_LOW_LIMIT  -100
#define GPIO_A 27
#define GPIO_B 26

static int last_count = 0;

void encoder_read(lv_indev_t * indev, lv_indev_data_t * data){
    int pulse_count = 0;

    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &pulse_count));

    if (pulse_count > last_count) {
        data->state = LV_INDEV_STATE_PR; 
        lv_group_send_data(lv_group_get_default(), LV_KEY_RIGHT);
    }

    else if (pulse_count < last_count) {
        data->state = LV_INDEV_STATE_PR; 
        lv_group_send_data(lv_group_get_default(), LV_KEY_LEFT);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    last_count = pulse_count;
}

void encoder_task(void *arg) {
    while (1) {
        // You don't need to do anything specific here, as the encoder state is handled in the callback (encoder_read)
        // The task can simply loop with a delay to avoid overloading the CPU
        vTaskDelay(pdMS_TO_TICKS(50)); // Delay for a short period
    }
}

void setup_encoder() {
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));
    
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = GPIO_A,
        .level_gpio_num = GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = GPIO_B,
        .level_gpio_num = GPIO_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));
    
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
    
    xTaskCreate(encoder_task, "encoder_task", 4096, NULL, 5, NULL);
}

void set_group() {
    groups[0] = lv_group_create();
    groups[1] = lv_group_create();
    
    lv_group_add_obj(groups[0], ui_Screen1);
    lv_group_add_obj(groups[0], ui_NEXT);
    lv_group_add_obj(groups[0], ui_NEXT1);
    
    lv_group_add_obj(groups[1], ui_Screen2);
    lv_group_add_obj(groups[1], ui_BACK);
    lv_group_add_obj(groups[1], ui_BACK1);
    
    lv_group_set_default(groups[0]);
}

void encoder_init(void) {
    setup_encoder();
    set_group();

    encoder_indev = lv_indev_create();
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, encoder_read);
    lv_indev_set_group(encoder_indev, groups[0]);
}
