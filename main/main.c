#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "bsp/esp32_s3_touch_amoled_1_8.h"
#include "esp_codec_dev.h"
#include "lvgl.h"

#define BTN_INC   GPIO_NUM_18
#define BTN_DEC   GPIO_NUM_39
#define BTN_RESET GPIO_NUM_42

// Declare embedded audio files
extern const uint8_t inc_wav_start[] asm("_binary_inc_wav_start");
extern const uint8_t inc_wav_end[] asm("_binary_inc_wav_end");
extern const uint8_t dec_wav_start[] asm("_binary_dec_wav_start");
extern const uint8_t dec_wav_end[] asm("_binary_dec_wav_end");
extern const uint8_t reset_wav_start[] asm("_binary_reset_wav_start");
extern const uint8_t reset_wav_end[] asm("_binary_reset_wav_end");

static esp_codec_dev_handle_t spk_codec_dev = NULL;

// Audio playback state
static struct {
    const uint8_t *wav_start;
    const uint8_t *wav_end;
    bool play_requested;
    bool is_playing;
} audio_state = {0};

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

// Initialize audio using BSP
static void audio_init(void)
{
    printf("Initializing audio...\n");
    
    // Use BSP function to initialize codec
    spk_codec_dev = bsp_audio_codec_speaker_init();
    
    if (spk_codec_dev == NULL) {
        printf("ERROR: BSP audio init failed!\n");
        return;
    }
    
    // Configure sample format
    esp_codec_dev_sample_info_t fs = {
        .sample_rate = 16000,
        .channel = 1,
        .bits_per_sample = 16,
    };
    
    // Open codec
    esp_err_t ret = esp_codec_dev_open(spk_codec_dev, &fs);
    if (ret != ESP_OK) {
        printf("ERROR: Failed to open codec: %s\n", esp_err_to_name(ret));
        return;
    }
    
    // Set volume (0-100)
    esp_codec_dev_set_out_vol(spk_codec_dev, 80.0);
    
    printf("Audio initialized\n");
}

// Audio playback task
static void audio_task(void *args)
{
    while (1) {
        if (audio_state.play_requested && audio_state.wav_start && audio_state.wav_end) {
            audio_state.play_requested = false;
            audio_state.is_playing = true;

            size_t wav_size = audio_state.wav_end - audio_state.wav_start;
            const uint8_t *pcm_data = audio_state.wav_start + 44;  // Skip WAV header
            size_t pcm_size = wav_size - 44;

            printf("Playing audio: %zu bytes\n", pcm_size);
            
            // Write audio data directly to codec
            esp_codec_dev_write(spk_codec_dev, (void *)pcm_data, pcm_size);
            
            audio_state.is_playing = false;
            printf("Audio complete\n");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

static void play_wav(const uint8_t *wav_start, const uint8_t *wav_end)
{
    // If currently playing, stop it by closing and reopening codec
    if (audio_state.is_playing) {
        printf("Interrupting current playback\n");
        esp_codec_dev_close(spk_codec_dev);
        
        // Reopen codec
        esp_codec_dev_sample_info_t fs = {
            .sample_rate = 16000,
            .channel = 1,
            .bits_per_sample = 16,
        };
        esp_codec_dev_open(spk_codec_dev, &fs);
        esp_codec_dev_set_out_vol(spk_codec_dev, 80.0);
    }
    
    audio_state.wav_start = wav_start;
    audio_state.wav_end = wav_end;
    audio_state.play_requested = true;
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
    audio_init();

    xTaskCreate(audio_task, "audio_task", 4096, NULL, 5, NULL);

    int last_inc = 0, last_dec = 0, last_rst = 0;

    while (1) {
        int inc = (gpio_get_level(BTN_INC) == 0);
        int dec = (gpio_get_level(BTN_DEC) == 0);
        int rst = (gpio_get_level(BTN_RESET) == 0);

        int inc_edge = inc && !last_inc;
        int dec_edge = dec && !last_dec;
        int rst_edge = rst && !last_rst;

        if (inc_edge || dec_edge || rst_edge) {
            if (rst_edge) {
                counter = 0;
                play_wav(reset_wav_start, reset_wav_end);
            }
            else if (dec_edge) {
                counter--;
                play_wav(dec_wav_start, dec_wav_end);
            }
            else if (inc_edge) {
                counter++;
                play_wav(inc_wav_start, inc_wav_end);
            }

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
