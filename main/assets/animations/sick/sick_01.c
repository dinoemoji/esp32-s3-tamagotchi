#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMAGE_SICK_01
#define LV_ATTRIBUTE_IMAGE_SICK_01
#endif

// Dummy placeholder data - replace with actual image data (60x60 ARGB8888 = 14400 bytes)
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE_SICK_01 uint8_t sick_01_map[14400] = {0};

const lv_image_dsc_t sick_01 = {
    .header.cf = LV_COLOR_FORMAT_ARGB8888,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 60,
    .header.h = 60,
    .data_size = 60 * 60 * 4,
    .data = sick_01_map,
};
