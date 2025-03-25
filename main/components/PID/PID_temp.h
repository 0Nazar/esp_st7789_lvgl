#ifndef PID_H
#define PID_H

#include "driver/spi_master.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define FAN_PIN 5                  // Пін для керування вентилятором
#define TEMP_SETPOINT 15.0         // Бажана температура
#define PWM_RESOLUTION 255         // Роздільність PWM (від 0 до 255)
#define PID_KP 2.0                 // Параметр P PID контролера
#define PID_KI 5.0                 // Параметр I PID контролера
#define PID_KD 1.0                 // Параметр D PID контролера

extern float input_temperature;    // Вхідна температура (від термопари)
extern float output_fan_speed;     // Вихідний сигнал для вентилятора
extern float setpoint_temperature; // Бажана температура

/**
 * @brief Зчитування температури з термопари.
 * Функція для зчитування температури за допомогою MAX6675.
 */
void read_temperature(void);

/**
 * @brief Виконання одного циклу PID-контролю.
 * Обчислює значення для керування вентилятором.
 */
void pid_control_loop(void);

/**
 * @brief Ініціалізація PID контролера.
 * Налаштовує PID контролер з початковими параметрами.
 */
void setup_pid(void);

/**
 * @brief Керування швидкістю вентилятора за допомогою PWM.
 * Використовує значення PID для регулювання швидкості вентилятора.
 */
void control_fan_speed(void);

void pid_task(void *arg);

void pid_init(void);
#endif // PID_H
