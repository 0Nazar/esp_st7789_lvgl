#ifndef DHT_H
#define DHT_H

#include "esp_err.h"

#define DHT_OK 0
#define DHT_TIMEOUT_ERROR -1
#define DHT_CHECKSUM_ERROR -2

#define MAXdhtData 5

void setDHTgpio(int gpio);
float getHumidity();
float getTemperature();
int readDHT();
void errorHandler(int response);

#endif // DHT_H