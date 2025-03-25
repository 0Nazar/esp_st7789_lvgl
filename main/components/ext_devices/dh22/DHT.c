#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "esp_log.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "DHT.h"

// == global defines =============================================

lv_obj_t *error_hum_window;
lv_obj_t *ok_hum_button;

static const char *TAG = "DHT";

int DHTgpio = GPIO_NUM_4;
float humidity = 0.;
float temperature = 0.;

// == set the DHT used pin=========================================

void setDHTgpio(int gpio)
{
    DHTgpio = gpio;
    gpio_set_pull_mode(DHTgpio, GPIO_PULLUP_ONLY);
}

// == get temp & hum =============================================

float getHumidity() { return humidity; }
float getTemperature() { return temperature; }

// == error handler ===============================================

void errorHandler(int response)
{
    switch (response)
    {
    case DHT_TIMEOUT_ERROR:
        ESP_LOGE(TAG, "Sensor Timeout\n");
        error_window_hum("Sensor Timeout");
        break;
    case DHT_CHECKSUM_ERROR:
        ESP_LOGE(TAG, "Checksum error\n");
        error_window_hum("Checksum Error");
        break;
    case DHT_OK:
        break;
    default:
        ESP_LOGE(TAG, "Unknown error\n");
        error_window_hum("Unknown Error");
    }
}
void ok_button_hum_event(lv_event_t *e);

void error_window_hum(const char *error_message)
{
    // Create error window
    error_hum_window = lv_obj_create(lv_scr_act());
    lv_obj_set_size(error_hum_window, LV_HOR_RES, LV_VER_RES / 2); 
    lv_obj_align(error_hum_window, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_outline_color(error_hum_window, lv_color_make(255, 0, 0), LV_PART_MAIN);

    // Create label to show the error message
    lv_obj_t *label = lv_label_create(error_hum_window);
    lv_label_set_text(label, "Humidity sensor Error!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);

    // Create "OK" button
    ok_hum_button = lv_btn_create(error_hum_window);
    lv_obj_align(ok_hum_button, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_size(ok_hum_button, 100, 50);

    lv_obj_t *btn_label = lv_label_create(ok_hum_button);
    lv_label_set_text(btn_label, "OK");
}

void ok_button_hum_event(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_del(obj);  // Видалення вікна помилки
    }
}

int getSignalLevel(int usTimeOut, bool state)
{

    int uSec = 0;
    while (gpio_get_level(DHTgpio) == state)
    {

        if (uSec > usTimeOut)
            return -1;

        ++uSec;
        esp_rom_delay_us(1); // uSec delay
    }

    return uSec;
}

#define MAXdhtData 5 // to complete 40 = 5*8 Bits

int read_hum()
{
    int uSec = 0;

    uint8_t dhtData[MAXdhtData];
    uint8_t byteInx = 0;
    uint8_t bitInx = 7;

    for (int k = 0; k < MAXdhtData; k++)
        dhtData[k] = 0;

    // == Send start signal to DHT sensor ===========

    gpio_set_direction(DHTgpio, GPIO_MODE_OUTPUT);

    // pull down for 3 ms for a smooth and nice wake up
    gpio_set_level(DHTgpio, 0);
    esp_rom_delay_us(3000);

    // pull up for 25 us for a gentile asking for data
    gpio_set_level(DHTgpio, 1);
    esp_rom_delay_us(25);

    gpio_set_direction(DHTgpio, GPIO_MODE_INPUT); // change to input mode

    // == DHT will keep the line low for 80 us and then high for 80us ====

    uSec = getSignalLevel(85, 0);
    // ESP_LOGI(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT_TIMEOUT_ERROR;

    // -- 80us up ------------------------

    uSec = getSignalLevel(85, 1);
    // ESP_LOGI(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT_TIMEOUT_ERROR;

    // == No errors, read the 40 data bits ================

    for (int k = 0; k < 40; k++)
    {

        // -- starts new data transmission with >50us low signal

        uSec = getSignalLevel(56, 0);
        if (uSec < 0)
            return DHT_TIMEOUT_ERROR;

        // -- check to see if after >70us rx data is a 0 or a 1

        uSec = getSignalLevel(75, 1);
        if (uSec < 0)
            return DHT_TIMEOUT_ERROR;

        // add the current read to the output data
        // since all dhtData array where set to 0 at the start,
        // only look for "1" (>28us us)

        if (uSec > 40)
        {
            dhtData[byteInx] |= (1 << bitInx);
        }

        // index to next byte

        if (bitInx == 0)
        {
            bitInx = 7;
            ++byteInx;
        }
        else
            bitInx--;
    }

    // == get humidity from Data[0] and Data[1] ==========================

    humidity = dhtData[0];
    humidity *= 0x100; // >> 8
    humidity += dhtData[1];
    humidity /= 10; // get the decimal

    // == get temp from Data[2] and Data[3]

    temperature = dhtData[2] & 0x7F;
    temperature *= 0x100; // >> 8
    temperature += dhtData[3];
    temperature /= 10;

    if (dhtData[2] & 0x80) // negative temp, brrr it's freezing
        temperature *= -1;

    // == verify if checksum is ok ===========================================
    // Checksum is the sum of Data 8 bits masked out 0xFF

    if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF))
        return DHT_OK;

    else
        return DHT_CHECKSUM_ERROR;
}