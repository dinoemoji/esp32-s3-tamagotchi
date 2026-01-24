#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "bsp/esp32_s3_touch_amoled_1_8.h"
#include "lvgl.h"
#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

LV_IMAGE_DECLARE(tamagotchi_bg);
LV_IMAGE_DECLARE(write_icon);
LV_IMAGE_DECLARE(log_icon);
LV_IMAGE_DECLARE(trophy_icon);
LV_IMAGE_DECLARE(settings_icon);
extern const lv_font_t lv_font_unscii_16;

static lv_obj_t *icon_write;
static lv_obj_t *icon_log;
static lv_obj_t *icon_trophy;
static lv_obj_t *icon_settings;
static lv_obj_t *words_value_label;
static lv_obj_t *words_value_label_bold;
static lv_obj_t *words_bar;
static lv_obj_t *health_segments[6];
static int32_t words_count = 0;
static int32_t health_count = 6;

static void load_persisted_state(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    nvs_handle_t handle;
    if (nvs_open("tama", NVS_READWRITE, &handle) == ESP_OK) {
        int32_t value = 0;
        if (nvs_get_i32(handle, "words", &value) == ESP_OK) {
            words_count = value;
        }
        value = 0;
        if (nvs_get_i32(handle, "health", &value) == ESP_OK) {
            health_count = value;
        }
        nvs_close(handle);
    }
}

static void save_persisted_state(void)
{
    nvs_handle_t handle;
    if (nvs_open("tama", NVS_READWRITE, &handle) == ESP_OK) {
        nvs_set_i32(handle, "words", words_count);
        nvs_set_i32(handle, "health", health_count);
        nvs_commit(handle);
        nvs_close(handle);
    }
}

static void sync_bold_text(lv_obj_t *base_label, lv_obj_t *bold_label)
{
    lv_label_set_text(bold_label, lv_label_get_text(base_label));
}

static void update_words_ui(void)
{
    int32_t clamped = words_count;
    if (clamped < 0) clamped = 0;
    if (clamped > 80000) clamped = 80000;
    lv_bar_set_value(words_bar, clamped, LV_ANIM_OFF);
    lv_label_set_text_fmt(words_value_label, "%" PRId32 "/80k", clamped);
    if (words_value_label_bold) {
        sync_bold_text(words_value_label, words_value_label_bold);
    }
}

static void update_health_ui(void)
{
    int32_t clamped = health_count;
    if (clamped < 0) clamped = 0;
    if (clamped > 6) clamped = 6;
    for (int32_t i = 0; i < 6; i++) {
        lv_obj_set_style_opa(health_segments[i], i < clamped ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);
    }
}

static void icon_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    if (target == icon_write) {
        words_count += 250;
        update_words_ui();
        save_persisted_state();
    } else if (target == icon_log) {
        health_count += 1;
        update_health_ui();
        save_persisted_state();
    } else if (target == icon_settings) {
        health_count -= 1;
        update_health_ui();
        save_persisted_state();
    } else if (target == icon_trophy) {
        return;
    }
}

static lv_obj_t *create_icon(lv_obj_t *parent, int32_t x, int32_t y, const char *symbol)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_size(container, 60, 60);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_bg_opa(container, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
    lv_obj_add_flag(container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(container, icon_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, symbol);
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_36, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);

    return container;
}

static lv_obj_t *create_icon_image(lv_obj_t *parent, int32_t x, int32_t y, const lv_image_dsc_t *image)
{
    const int32_t icon_scale = (50 * 256) / 60;
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_size(container, 60, 60);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_bg_opa(container, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
    lv_obj_add_flag(container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(container, icon_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *img = lv_image_create(container);
    lv_image_set_src(img, image);
    lv_image_set_scale(img, icon_scale);
    lv_obj_center(img);

    return container;
}

void app_main(void)
{
    load_persisted_state();

    lv_disp_t *disp = bsp_display_start();

#if CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .light_sleep_enable = false,
    };
    esp_pm_configure(&pm_config);
#endif

    bsp_display_lock(0);
    lv_disp_set_rotation(disp, LV_DISP_ROTATION_270);
    bsp_display_unlock();

    bsp_display_backlight_on();
    vTaskDelay(pdMS_TO_TICKS(200));

    bsp_display_lock(0);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *bg_img = lv_image_create(screen);
    lv_image_set_src(bg_img, &tamagotchi_bg);
    lv_obj_align(bg_img, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_move_background(bg_img);

    int32_t screen_w = lv_disp_get_hor_res(disp);
    int32_t screen_h = lv_disp_get_ver_res(disp);
    int32_t icon_total_w = (60 * 4) + (33 * 3);
    int32_t start_x = (screen_w - icon_total_w) / 2;
    int32_t y = 9;

    icon_write = create_icon_image(screen, start_x + (60 + 33) * 0, y, &write_icon);
    icon_log = create_icon_image(screen, start_x + (60 + 33) * 1, y, &log_icon);
    icon_trophy = create_icon_image(screen, start_x + (60 + 33) * 2, y, &trophy_icon);
    icon_settings = create_icon_image(screen, start_x + (60 + 33) * 3, y, &settings_icon);

    int32_t bar_height = 25;
    int32_t words_bar_height = 31;
    int32_t bar_gap = 4;
    int32_t bar_right_pad = 54;
    int32_t label_gap = 10;
    int32_t label_x = 54;
    int32_t bar_x = 0;
    int32_t bar_w = 0;
    int32_t bottom_bar_y = screen_h - 10 - bar_height;
    int32_t top_bar_y = bottom_bar_y - bar_gap - words_bar_height;

    lv_obj_t *words_label = lv_label_create(screen);
    lv_label_set_text(words_label, "WORDS");
    lv_obj_set_style_text_color(words_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(words_label, &lv_font_unscii_16, LV_PART_MAIN);
    lv_obj_t *words_label_bold = lv_label_create(screen);
    lv_label_set_text(words_label_bold, "WORDS");
    lv_obj_set_style_text_color(words_label_bold, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(words_label_bold, &lv_font_unscii_16, LV_PART_MAIN);

    lv_obj_t *health_label = lv_label_create(screen);
    lv_label_set_text(health_label, "HEALTH");
    lv_obj_set_style_text_color(health_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(health_label, &lv_font_unscii_16, LV_PART_MAIN);
    lv_obj_t *health_label_bold = lv_label_create(screen);
    lv_label_set_text(health_label_bold, "HEALTH");
    lv_obj_set_style_text_color(health_label_bold, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(health_label_bold, &lv_font_unscii_16, LV_PART_MAIN);

    words_value_label = lv_label_create(screen);
    lv_label_set_text(words_value_label, "0/80k");
    lv_obj_set_style_text_color(words_value_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(words_value_label, &lv_font_unscii_16, LV_PART_MAIN);
    words_value_label_bold = lv_label_create(screen);
    lv_label_set_text(words_value_label_bold, "0/80k");
    lv_obj_set_style_text_color(words_value_label_bold, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(words_value_label_bold, &lv_font_unscii_16, LV_PART_MAIN);
    lv_obj_update_layout(screen);
    int32_t label_w = lv_obj_get_width(words_label_bold);
    int32_t health_label_w = lv_obj_get_width(health_label_bold);
    if (health_label_w > label_w) {
        label_w = health_label_w;
    }
    bar_x = label_x + label_w + label_gap;
    bar_w = screen_w - bar_x - bar_right_pad;

    lv_obj_align(words_label, LV_ALIGN_TOP_LEFT, label_x, top_bar_y + 2);
    lv_obj_align(words_label_bold, LV_ALIGN_TOP_LEFT, label_x + 1, top_bar_y + 2);

    lv_obj_align(health_label, LV_ALIGN_TOP_LEFT, label_x, bottom_bar_y + 2);
    lv_obj_align(health_label_bold, LV_ALIGN_TOP_LEFT, label_x + 1, bottom_bar_y + 2);

    lv_obj_align(words_value_label, LV_ALIGN_TOP_RIGHT, -54, top_bar_y - 18);
    lv_obj_align(words_value_label_bold, LV_ALIGN_TOP_RIGHT, -53, top_bar_y - 18);

    words_bar = lv_bar_create(screen);
    lv_obj_set_pos(words_bar, bar_x, top_bar_y);
    lv_obj_set_size(words_bar, bar_w, words_bar_height);
    lv_bar_set_range(words_bar, 0, 80000);
    lv_obj_set_style_bg_opa(words_bar, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_color(words_bar, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(words_bar, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(words_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(words_bar, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(words_bar, lv_color_black(), LV_PART_INDICATOR);

    lv_obj_t *health_bar = lv_obj_create(screen);
    lv_obj_set_pos(health_bar, bar_x, bottom_bar_y);
    lv_obj_set_size(health_bar, bar_w, bar_height);
    lv_obj_set_style_bg_opa(health_bar, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_width(health_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(health_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(health_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(health_bar, 2, LV_PART_MAIN);
    lv_obj_set_flex_flow(health_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(health_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    int32_t segment_gap = 2;
    int32_t segment_w = (bar_w - segment_gap * 5) / 6;
    for (int32_t i = 0; i < 6; i++) {
        health_segments[i] = lv_obj_create(health_bar);
        lv_obj_set_size(health_segments[i], segment_w, bar_height);
        lv_obj_set_style_bg_color(health_segments[i], lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_border_width(health_segments[i], 0, LV_PART_MAIN);
        lv_obj_set_style_radius(health_segments[i], 0, LV_PART_MAIN);
    }

    update_words_ui();
    update_health_ui();

    bsp_display_unlock();

    while (1) {
        bsp_display_lock(0);
        lv_timer_handler();
        bsp_display_unlock();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
