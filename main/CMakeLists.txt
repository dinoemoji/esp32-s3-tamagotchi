idf_component_register(
    SRCS "main.c" "assets/images/tamagotchi_bg.c" "assets/images/write.c" "assets/images/log.c" "assets/images/trophy.c" "assets/images/settings.c"
    INCLUDE_DIRS "."
    REQUIRES freertos driver esp_driver_gpio lvgl esp_codec_dev esp32_s3_touch_amoled_1_8 nvs_flash esp_pm
)

# Embed the WAV files
target_add_binary_data(${COMPONENT_LIB} "inc.wav" BINARY)
target_add_binary_data(${COMPONENT_LIB} "dec.wav" BINARY)
target_add_binary_data(${COMPONENT_LIB} "reset.wav" BINARY)

