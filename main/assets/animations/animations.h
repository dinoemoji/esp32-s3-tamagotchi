#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "lvgl.h"
#include "assets/animations/animation_fps.h"
#include <stdint.h>
#include <stdbool.h>

// Animation structure to hold frame data and metadata
typedef struct animation_t {
    const lv_image_dsc_t **frames;  // Array of pointers to frame image descriptors
    uint16_t frame_count;           // Number of frames in the animation
    uint16_t fps;                   // Frames per second (default playback speed)
} animation_t;

// Animation player state
typedef struct {
    const animation_t *animation;
    lv_obj_t *image_obj;            // LVGL image object to update
    uint16_t current_frame;         // Current frame index (0-based)
    uint32_t last_update_ms;        // Last frame update time in milliseconds
    bool playing;                    // Whether animation is currently playing
    bool loop;                       // Whether to loop the animation
} animation_player_t;

// Forward declarations for all animation frames
// Prehatch frames
LV_IMAGE_DECLARE(prehatch_00);
LV_IMAGE_DECLARE(prehatch_01);
LV_IMAGE_DECLARE(prehatch_02);
LV_IMAGE_DECLARE(prehatch_03);

// Hatched frames
LV_IMAGE_DECLARE(hatched_00);
LV_IMAGE_DECLARE(hatched_01);
LV_IMAGE_DECLARE(hatched_02);

// Idle frames
LV_IMAGE_DECLARE(idle_00);
LV_IMAGE_DECLARE(idle_01);
LV_IMAGE_DECLARE(idle_02);
LV_IMAGE_DECLARE(idle_03);
LV_IMAGE_DECLARE(idle_04);

// Writing frames
LV_IMAGE_DECLARE(writing_00);
LV_IMAGE_DECLARE(writing_01);
LV_IMAGE_DECLARE(writing_02);
LV_IMAGE_DECLARE(writing_03);
LV_IMAGE_DECLARE(writing_04);

// Sick frames
LV_IMAGE_DECLARE(sick_00);
LV_IMAGE_DECLARE(sick_01);

// Dead frames
LV_IMAGE_DECLARE(dead_00);
LV_IMAGE_DECLARE(dead_01);
LV_IMAGE_DECLARE(dead_02);
LV_IMAGE_DECLARE(dead_03);
LV_IMAGE_DECLARE(dead_04);
LV_IMAGE_DECLARE(dead_05);
LV_IMAGE_DECLARE(dead_06);

// Celebrate frames
LV_IMAGE_DECLARE(celebrate_00);
LV_IMAGE_DECLARE(celebrate_01);
LV_IMAGE_DECLARE(celebrate_02);
LV_IMAGE_DECLARE(celebrate_03);
LV_IMAGE_DECLARE(celebrate_04);

// Animation definitions (extern, defined in animations.c)
extern const animation_t prehatch_animation;
extern const animation_t hatched_animation;
extern const animation_t idle_animation;
extern const animation_t writing_animation;
extern const animation_t sick_animation;
extern const animation_t dead_animation;
extern const animation_t celebrate_animation;

// Function prototypes
void animation_player_init(animation_player_t *player, const animation_t *anim, lv_obj_t *img_obj, bool loop);
void animation_player_start(animation_player_t *player);
void animation_player_stop(animation_player_t *player);
void animation_player_update(animation_player_t *player, uint32_t current_time_ms);
void animation_player_set_frame(animation_player_t *player, uint16_t frame_index);
void animation_player_set_animation(animation_player_t *player, const animation_t *anim, bool loop);

#endif // ANIMATIONS_H
