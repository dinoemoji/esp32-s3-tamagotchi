#include "esp_lcd_touch_ft3168.h"
#include "esp_lcd_touch_ft5x06.h"

esp_err_t esp_lcd_touch_new_i2c_ft3168(const esp_lcd_panel_io_handle_t io,
                                       const esp_lcd_touch_config_t *config,
                                       esp_lcd_touch_handle_t *out_touch)
{
    /* FT3168 uses a register map compatible with FT5x06 on this board. */
    return esp_lcd_touch_new_i2c_ft5x06(io, config, out_touch);
}

