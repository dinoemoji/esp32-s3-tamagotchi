idf_component_register(
    SRCS 
        "main.c" 
        "assets/images/tamagotchi_bg.c" 
        "assets/images/write.c" 
        "assets/images/log.c" 
        "assets/images/trophy.c" 
        "assets/images/settings.c"
        "assets/animations/animations.c"
        "assets/animations/tamagotchi_state.c"
        "assets/animations/prehatch/prehatch_00.c"
        "assets/animations/prehatch/prehatch_01.c"
        "assets/animations/prehatch/prehatch_02.c"
        "assets/animations/prehatch/prehatch_03.c"
        "assets/animations/hatched/hatched_00.c"
        "assets/animations/hatched/hatched_01.c"
        "assets/animations/hatched/hatched_02.c"
        "assets/animations/idle/idle_00.c"
        "assets/animations/idle/idle_01.c"
        "assets/animations/idle/idle_02.c"
        "assets/animations/idle/idle_03.c"
        "assets/animations/idle/idle_04.c"
        "assets/animations/writing/writing_00.c"
        "assets/animations/writing/writing_01.c"
        "assets/animations/writing/writing_02.c"
        "assets/animations/writing/writing_03.c"
        "assets/animations/writing/writing_04.c"
        "assets/animations/sick/sick_00.c"
        "assets/animations/sick/sick_01.c"
        "assets/animations/dead/dead_00.c"
        "assets/animations/dead/dead_01.c"
        "assets/animations/dead/dead_02.c"
        "assets/animations/dead/dead_03.c"
        "assets/animations/dead/dead_04.c"
        "assets/animations/dead/dead_05.c"
        "assets/animations/dead/dead_06.c"
        "assets/animations/celebrate/celebrate_00.c"
        "assets/animations/celebrate/celebrate_01.c"
        "assets/animations/celebrate/celebrate_02.c"
        "assets/animations/celebrate/celebrate_03.c"
        "assets/animations/celebrate/celebrate_04.c"
    INCLUDE_DIRS "."
    REQUIRES freertos driver esp_driver_gpio lvgl esp_codec_dev esp32_s3_touch_amoled_1_8 nvs_flash esp_pm
)

# Embed the WAV files
target_add_binary_data(${COMPONENT_LIB} "inc.wav" BINARY)
target_add_binary_data(${COMPONENT_LIB} "dec.wav" BINARY)
target_add_binary_data(${COMPONENT_LIB} "reset.wav" BINARY)

