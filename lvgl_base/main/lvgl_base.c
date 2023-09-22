#include <stdio.h>
#include "lcd_driver.h"

void app_main(void)
{
    xTaskCreatePinnedToCore(run, "gui", 4096, NULL, 4, NULL, 0);
    while (1) {
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        delay(1000);
    }

}
