#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp/esp32_s3_touch_amoled_1_8.h"
#include "lvgl.h"

void app_main(void)
{
    bsp_display_start();
    bsp_display_backlight_on();
    
    vTaskDelay(pdMS_TO_TICKS(500));
    
    bsp_display_lock(0);
    
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Hello, World!");
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_36, LV_PART_MAIN);
    lv_obj_center(label);
    
    lv_obj_invalidate(screen);
    
    bsp_display_unlock();
    
    vTaskDelay(pdMS_TO_TICKS(100));
    
    bsp_display_lock(0);
    lv_refr_now(NULL);
    bsp_display_unlock();
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}