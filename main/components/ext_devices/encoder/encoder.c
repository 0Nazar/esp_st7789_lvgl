#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "lvgl.h"
#include "ui.h"
#include "encoder.h"
#include "sdkconfig.h"

static const char *TAG = "Encoder";
static pcnt_unit_handle_t pcnt_unit = NULL;
lv_group_t *groups[3];
lv_indev_t *encoder_indev = NULL;
bool encoder_pressed = false;
SemaphoreHandle_t encoder_semaphore;

static void IRAM_ATTR encoder_btn_isr_handler(void *arg) 
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    encoder_pressed = true;
    xSemaphoreGiveFromISR(encoder_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data) 
{
    static int16_t last_count = 0;
    int pulse_count = 0;

    // Отримуємо значення лічильника
    pcnt_unit_get_count(pcnt_unit, &pulse_count);

    // Визначаємо напрямок обертання
    if (pulse_count > last_count) 
    {
        data->enc_diff = 1;
        ESP_LOGI(TAG, "Encoder turned RIGHT, count: %d", pulse_count);
    } 
    else if (pulse_count < last_count) 
    {
        data->enc_diff = -1;
        ESP_LOGI(TAG, "Encoder turned LEFT, count: %d", pulse_count);
    }

    last_count = pulse_count;

    // Обробка натискання кнопки
    if (xSemaphoreTake(encoder_semaphore, 0) == pdTRUE) 
    {
        data->state = LV_INDEV_STATE_PR;
        data->key = LV_KEY_ENTER;
        ESP_LOGI(TAG, "Encoder button pressed!");
    } 
    else 
    {
        data->state = LV_INDEV_STATE_REL;
    }

    if (lv_scr_act() == ui_Screen1) {
        lv_indev_set_group(encoder_indev, groups[0]);
    } else if (lv_scr_act() == ui_Screen2) {
        lv_indev_set_group(encoder_indev, groups[1]);
    } else if (lv_scr_act() == ui_Screen3) {
        lv_indev_set_group(encoder_indev, groups[2]);
    }

}

void encoder_task(void *arg) 
{
    while (1) 
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void encoder_init(void) {
    encoder_semaphore = xSemaphoreCreateBinary();

    ESP_LOGI(TAG, "Install PCNT unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = 100,
        .low_limit = -100,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "Install PCNT channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = ENCODER_A,
        .level_gpio_num = ENCODER_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = ENCODER_B,
        .level_gpio_num = ENCODER_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_LOGI(TAG, "Set edge and level actions for PCNT channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_LOGI(TAG, "Enable PCNT unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "Clear PCNT unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "Start PCNT unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    // Налаштовуємо кнопку енкодера
    gpio_set_direction(ENCODER_BTN, GPIO_MODE_INPUT);
    gpio_set_intr_type(ENCODER_BTN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENCODER_BTN, encoder_btn_isr_handler, NULL);

    groups[0] = lv_group_create();
    lv_group_add_obj(groups[0], ui_Screen1);
    lv_group_add_obj(groups[0], ui_PAGE11);
    lv_group_add_obj(groups[0], ui_PAGE21);
    
    groups[1] = lv_group_create();
    lv_group_add_obj(groups[1], ui_Screen2);
    lv_group_add_obj(groups[1], ui_PAGE22);
    lv_group_add_obj(groups[1], ui_MENU1);

    groups[2] = lv_group_create();
    lv_group_add_obj(groups[2], ui_Screen3);
    lv_group_add_obj(groups[2], ui_PAGE1);
    lv_group_add_obj(groups[2], ui_MENU2);
    
    encoder_indev = lv_indev_create();
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, encoder_read);
    lv_indev_set_group(encoder_indev, groups[0]);
    
    xTaskCreate(encoder_task, "encoder_task", 4096, NULL, 5, NULL);
}
