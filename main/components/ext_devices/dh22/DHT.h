#ifndef DHT_H
#define DHT_H

#include "esp_log.h"
#include "driver/gpio.h"
#include "lvgl.h"

// == Error Codes ================================================

#define DHT_OK 0
#define DHT_TIMEOUT_ERROR -1
#define DHT_CHECKSUM_ERROR -2

// == Global Variables ===========================================

extern lv_obj_t *error_hum_window;
extern lv_obj_t *ok_hum_button;

extern float humidity;
extern float temperature;

// == Function Declarations ======================================

/**
 * @brief Set the GPIO pin to be used with DHT sensor
 * @param gpio GPIO pin number
 */
void setDHTgpio(int gpio);

/**
 * @brief Get the current humidity value
 * @return Current humidity
 */
float getHumidity();

/**
 * @brief Get the current temperature value
 * @return Current temperature
 */
float getTemperature();

/**
 * @brief Error handler for DHT sensor
 * @param response The error response code
 */
void errorHandler(int response);

/**
 * @brief Creates an error window for humidity sensor errors
 * @param error_message The error message to be displayed
 */
void error_window_hum(const char *error_message);

/**
 * @brief Reads signal from the DHT sensor to detect timeouts
 * @param usTimeOut Timeout in microseconds
 * @param state Desired signal state (0 or 1)
 * @return Time in microseconds, or -1 if timeout occurs
 */
int getSignalLevel(int usTimeOut, bool state);

/**
 * @brief Reads humidity and temperature data from the DHT sensor
 * @return DHT_OK on success, or error code on failure
 */
int read_hum();

#endif // DHT_H
