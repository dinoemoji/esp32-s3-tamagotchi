#ifndef TAMAGOTCHI_STATE_H
#define TAMAGOTCHI_STATE_H

#include <stdint.h>
#include <stdbool.h>

// Lifecycle stages
typedef enum {
    TAMA_LIFECYCLE_PREHATCH,  // < 1000 words
    TAMA_LIFECYCLE_HATCHED,   // < 16000 words (20% of 80k)
    TAMA_LIFECYCLE_ADULT      // >= 16000 words
} tamagotchi_lifecycle_t;

// Health status
typedef enum {
    TAMA_HEALTH_HEALTHY,  // Full health (6 segments)
    TAMA_HEALTH_SICK,     // 3+ consecutive days missed
    TAMA_HEALTH_DEAD      // 6+ consecutive days missed (no recovery)
} tamagotchi_health_t;

// Animation type
typedef enum {
    TAMA_ANIM_PREHATCH,
    TAMA_ANIM_HATCHED,
    TAMA_ANIM_IDLE,
    TAMA_ANIM_WRITING,
    TAMA_ANIM_SICK,
    TAMA_ANIM_DEAD,
    TAMA_ANIM_CELEBRATE
} tamagotchi_anim_t;

// Constants
#define WORDS_PREHATCH_THRESHOLD  1000
#define WORDS_HATCHED_THRESHOLD  16000  // 20% of 80k
#define WORDS_GOAL               80000
#define HEALTH_FULL               6
#define DAYS_SICK_THRESHOLD       3
#define DAYS_DEAD_THRESHOLD       6
#define WRITING_ANIM_DURATION_MS  5000  // 5 seconds

// Tamagotchi state structure
typedef struct {
    tamagotchi_lifecycle_t lifecycle;
    tamagotchi_health_t health;
    tamagotchi_anim_t current_anim;
    int32_t consecutive_missed_days;
    int32_t consecutive_writing_days;  // For health recovery
    uint32_t last_check_date;          // YYYYMMDD format
    uint32_t last_writing_date;        // YYYYMMDD format
    bool celebration_played;           // Track if 80k celebration already played
} tamagotchi_state_t;

// Forward declaration
struct animation_t;
typedef struct animation_t animation_t;

// Function prototypes
void tamagotchi_state_init(tamagotchi_state_t *state);
void tamagotchi_state_load(tamagotchi_state_t *state);
void tamagotchi_state_save(const tamagotchi_state_t *state);

tamagotchi_lifecycle_t calculate_lifecycle(int32_t words_count);
tamagotchi_health_t calculate_health_status(int32_t consecutive_missed_days);
tamagotchi_anim_t get_current_animation(const tamagotchi_state_t *state, int32_t words_count, bool writing_anim_active);

uint32_t get_current_date_key(void);  // Returns YYYYMMDD format
bool check_daily_writing(tamagotchi_state_t *state);
void update_consecutive_missed_days(tamagotchi_state_t *state);
void mark_writing_activity(tamagotchi_state_t *state);

const animation_t* get_animation_for_type(tamagotchi_anim_t anim_type);

#endif // TAMAGOTCHI_STATE_H
