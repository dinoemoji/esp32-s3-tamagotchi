idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES freertos driver lvgl esp_codec_dev esp32_s3_touch_amoled_1_8
)

# Embed the WAV files
target_add_binary_data(${COMPONENT_LIB} "inc.wav" BINARY)
target_add_binary_data(${COMPONENT_LIB} "dec.wav" BINARY)
target_add_binary_data(${COMPONENT_LIB} "reset.wav" BINARY)
