#include "assets/animations/tamagotchi_state.h"
#include "assets/animations/animations.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

// NVS keys
#define NVS_KEY_LAST_CHECK_DATE      "last_check"
#define NVS_KEY_CONSECUTIVE_MISSED    "missed_days"
#define NVS_KEY_LAST_WRITING_DATE     "last_write"
#define NVS_KEY_CONSECUTIVE_WRITING   "write_days"
#define NVS_KEY_CELEBRATION_PLAYED     "celebrated"

void tamagotchi_state_init(tamagotchi_state_t *state) {
    memset(state, 0, sizeof(tamagotchi_state_t));
    state->lifecycle = TAMA_LIFECYCLE_PREHATCH;
    state->health = TAMA_HEALTH_HEALTHY;
    state->current_anim = TAMA_ANIM_PREHATCH;
    state->consecutive_missed_days = 0;
    state->consecutive_writing_days = 0;
    state->last_check_date = 0;
    state->last_writing_date = 0;
    state->celebration_played = false;
}

void tamagotchi_state_load(tamagotchi_state_t *state) {
    nvs_handle_t handle;
    if (nvs_open("tama", NVS_READONLY, &handle) == ESP_OK) {
        size_t required_size = sizeof(uint32_t);
        if (nvs_get_blob(handle, NVS_KEY_LAST_CHECK_DATE, &state->last_check_date, &required_size) == ESP_OK) {
            // Loaded successfully
        }
        int32_t int_value = 0;
        if (nvs_get_i32(handle, NVS_KEY_CONSECUTIVE_MISSED, &int_value) == ESP_OK) {
            state->consecutive_missed_days = int_value;
        }
        required_size = sizeof(uint32_t);
        if (nvs_get_blob(handle, NVS_KEY_LAST_WRITING_DATE, &state->last_writing_date, &required_size) == ESP_OK) {
            // Loaded successfully
        }
        if (nvs_get_i32(handle, NVS_KEY_CONSECUTIVE_WRITING, &int_value) == ESP_OK) {
            state->consecutive_writing_days = int_value;
        }
        uint8_t bool_value = 0;
        if (nvs_get_u8(handle, NVS_KEY_CELEBRATION_PLAYED, &bool_value) == ESP_OK) {
            state->celebration_played = (bool_value != 0);
        }
        nvs_close(handle);
    }
}

void tamagotchi_state_save(const tamagotchi_state_t *state) {
    nvs_handle_t handle;
    if (nvs_open("tama", NVS_READWRITE, &handle) == ESP_OK) {
        nvs_set_blob(handle, NVS_KEY_LAST_CHECK_DATE, &state->last_check_date, sizeof(uint32_t));
        nvs_set_i32(handle, NVS_KEY_CONSECUTIVE_MISSED, state->consecutive_missed_days);
        nvs_set_blob(handle, NVS_KEY_LAST_WRITING_DATE, &state->last_writing_date, sizeof(uint32_t));
        nvs_set_i32(handle, NVS_KEY_CONSECUTIVE_WRITING, state->consecutive_writing_days);
        nvs_set_u8(handle, NVS_KEY_CELEBRATION_PLAYED, state->celebration_played ? 1 : 0);
        nvs_commit(handle);
        nvs_close(handle);
    }
}

tamagotchi_lifecycle_t calculate_lifecycle(int32_t words_count) {
    if (words_count < WORDS_PREHATCH_THRESHOLD) {
        return TAMA_LIFECYCLE_PREHATCH;
    } else if (words_count < WORDS_HATCHED_THRESHOLD) {
        return TAMA_LIFECYCLE_HATCHED;
    } else {
        return TAMA_LIFECYCLE_ADULT;
    }
}

tamagotchi_health_t calculate_health_status(int32_t consecutive_missed_days) {
    if (consecutive_missed_days >= DAYS_DEAD_THRESHOLD) {
        return TAMA_HEALTH_DEAD;
    } else if (consecutive_missed_days >= DAYS_SICK_THRESHOLD) {
        return TAMA_HEALTH_SICK;
    } else {
        return TAMA_HEALTH_HEALTHY;
    }
}

tamagotchi_anim_t get_current_animation(const tamagotchi_state_t *state, int32_t words_count, bool writing_anim_active) {
    // Priority order:
    // 1. Dead (always show if dead)
    if (state->health == TAMA_HEALTH_DEAD) {
        return TAMA_ANIM_DEAD;
    }
    
    // 2. Celebration (if goal reached and not yet played)
    if (words_count >= WORDS_GOAL && !state->celebration_played) {
        return TAMA_ANIM_CELEBRATE;
    }
    
    // 3. Writing animation (if active and adult)
    if (writing_anim_active && state->lifecycle == TAMA_LIFECYCLE_ADULT) {
        return TAMA_ANIM_WRITING;
    }
    
    // 4. Sick (if sick)
    if (state->health == TAMA_HEALTH_SICK) {
        return TAMA_ANIM_SICK;
    }
    
    // 5. Lifecycle-based animations
    switch (state->lifecycle) {
        case TAMA_LIFECYCLE_PREHATCH:
            return TAMA_ANIM_PREHATCH;
        case TAMA_LIFECYCLE_HATCHED:
            return TAMA_ANIM_HATCHED;
        case TAMA_LIFECYCLE_ADULT:
            return TAMA_ANIM_IDLE;
        default:
            return TAMA_ANIM_IDLE;
    }
}

// Simple date key generation (YYYYMMDD)
// Note: This is a simplified version. For production, use RTC or proper time API
// For now, uses days since boot as a simple counter
// TODO: Replace with actual RTC/SNTP implementation for real date tracking
uint32_t get_current_date_key(void) {
    // Use days since boot as date key (will work for consecutive day tracking)
    // Format: days since boot as uint32_t
    // This allows tracking consecutive days without needing RTC
    // When RTC is available, replace with: year * 10000 + month * 100 + day
    TickType_t ticks = xTaskGetTickCount();
    uint32_t days = (uint32_t)(ticks / (configTICK_RATE_HZ * 60 * 60 * 24));
    return days;
}

bool check_daily_writing(tamagotchi_state_t *state) {
    uint32_t current_date = get_current_date_key();
    
    // Check if it's a new day (or past 11:49 PM)
    // For simplicity, check if date changed
    if (current_date != state->last_check_date) {
        // New day - check if writing occurred yesterday
        if (state->last_writing_date < state->last_check_date) {
            // No writing yesterday - increment missed days
            state->consecutive_missed_days++;
            state->consecutive_writing_days = 0;  // Reset writing streak
        } else {
            // Writing occurred yesterday - reset missed days
            state->consecutive_missed_days = 0;
            state->consecutive_writing_days++;
        }
        
        state->last_check_date = current_date;
        return true;  // Day check occurred
    }
    
    return false;  // Same day, no check needed
}

void update_consecutive_missed_days(tamagotchi_state_t *state) {
    uint32_t current_date = get_current_date_key();
    
    if (state->last_writing_date < current_date) {
        state->consecutive_missed_days++;
        state->consecutive_writing_days = 0;
    } else {
        state->consecutive_missed_days = 0;
    }
}

void mark_writing_activity(tamagotchi_state_t *state) {
    uint32_t current_date = get_current_date_key();
    
    if (state->last_writing_date != current_date) {
        // New writing day
        state->last_writing_date = current_date;
        state->consecutive_missed_days = 0;
        state->consecutive_writing_days++;
    }
    // If same day, don't change anything (already counted)
}

const animation_t* get_animation_for_type(tamagotchi_anim_t anim_type) {
    switch (anim_type) {
        case TAMA_ANIM_PREHATCH:
            return &prehatch_animation;
        case TAMA_ANIM_HATCHED:
            return &hatched_animation;
        case TAMA_ANIM_IDLE:
            return &idle_animation;
        case TAMA_ANIM_WRITING:
            return &writing_animation;
        case TAMA_ANIM_SICK:
            return &sick_animation;
        case TAMA_ANIM_DEAD:
            return &dead_animation;
        case TAMA_ANIM_CELEBRATE:
            return &celebrate_animation;
        default:
            return &idle_animation;
    }
}
