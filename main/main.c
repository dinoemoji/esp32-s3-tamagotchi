#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "bsp/esp32_s3_touch_amoled_1_8.h"
#include "lvgl.h"

#define BTN_INC   GPIO_NUM_17
#define BTN_DEC   GPIO_NUM_39
#define BTN_RESET GPIO_NUM_42

static void buttons_init(void)
{
    gpio_config_t io = {0};
    io.mode = GPIO_MODE_INPUT;
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    io.pin_bit_mask =
        (1ULL << BTN_INC) |
        (1ULL << BTN_DEC) |
        (1ULL << BTN_RESET);
    gpio_config(&io);
}

void app_main(void)
{
    lv_disp_t *disp = bsp_display_start();

    bsp_display_lock(0);
    lv_disp_set_rotation(disp, LV_DISP_ROTATION_270);
    bsp_display_unlock();

    bsp_display_backlight_on();
    vTaskDelay(pdMS_TO_TICKS(200));

    bsp_display_lock(0);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(screen);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_36, LV_PART_MAIN);

    int counter = 0;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", counter);
    lv_label_set_text(label, buf);
    lv_obj_center(label);

    bsp_display_unlock();

    buttons_init();

    int last_inc = 0, last_dec = 0, last_rst = 0;

    while (1) {
        int inc = (gpio_get_level(BTN_INC) == 0);
        int dec = (gpio_get_level(BTN_DEC) == 0);
        int rst = (gpio_get_level(BTN_RESET) == 0);

        int inc_edge = inc && !last_inc;
        int dec_edge = dec && !last_dec;
        int rst_edge = rst && !last_rst;

        if (inc_edge || dec_edge || rst_edge) {
            if (rst_edge) counter = 0;
            else if (dec_edge) counter--;
            else if (inc_edge) counter++;

            bsp_display_lock(0);
            snprintf(buf, sizeof(buf), "%d", counter);
            lv_label_set_text(label, buf);
            lv_obj_center(label);
            bsp_display_unlock();
        }

        last_inc = inc;
        last_dec = dec;
        last_rst = rst;

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}
