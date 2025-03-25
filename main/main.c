#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "disp_draw.h"
#include "encoder.h"
#include "PID_temp.h"

void app_main(void)
{
    disp_draw_init(); // FUNCTION THAT DRAW EVERYTHING ON THE SCREEN AND INIT LVGL 

    // encoder_init();

    pid_init();
}