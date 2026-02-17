#include "assets/animations/animations.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Prehatch animation frames
static const lv_image_dsc_t *prehatch_frames[] = {
    &prehatch_00,
    &prehatch_01,
    &prehatch_02,
    &prehatch_03,
};

// Hatched animation frames
static const lv_image_dsc_t *hatched_frames[] = {
    &hatched_00,
    &hatched_01,
    &hatched_02,
};

// Idle animation frames
static const lv_image_dsc_t *idle_frames[] = {
    &idle_00,
    &idle_01,
    &idle_02,
    &idle_03,
    &idle_04,
};

// Writing animation frames
static const lv_image_dsc_t *writing_frames[] = {
    &writing_00,
    &writing_01,
    &writing_02,
    &writing_03,
    &writing_04,
};

// Sick animation frames
static const lv_image_dsc_t *sick_frames[] = {
    &sick_00,
    &sick_01,
};

// Dead animation frames
static const lv_image_dsc_t *dead_frames[] = {
    &dead_00,
    &dead_01,
    &dead_02,
    &dead_03,
    &dead_04,
    &dead_05,
    &dead_06,
};

// Celebrate animation frames
static const lv_image_dsc_t *celebrate_frames[] = {
    &celebrate_00,
    &celebrate_01,
    &celebrate_02,
    &celebrate_03,
    &celebrate_04,
};

// Animation definitions with FPS from animation_fps.h
const animation_t prehatch_animation = {
    .frames = prehatch_frames,
    .frame_count = sizeof(prehatch_frames) / sizeof(prehatch_frames[0]),
    .fps = ANIM_FPS_PREHATCH,
};

const animation_t hatched_animation = {
    .frames = hatched_frames,
    .frame_count = sizeof(hatched_frames) / sizeof(hatched_frames[0]),
    .fps = ANIM_FPS_HATCHED,
};

const animation_t idle_animation = {
    .frames = idle_frames,
    .frame_count = sizeof(idle_frames) / sizeof(idle_frames[0]),
    .fps = ANIM_FPS_IDLE,
};

const animation_t writing_animation = {
    .frames = writing_frames,
    .frame_count = sizeof(writing_frames) / sizeof(writing_frames[0]),
    .fps = ANIM_FPS_WRITING,
};

const animation_t sick_animation = {
    .frames = sick_frames,
    .frame_count = sizeof(sick_frames) / sizeof(sick_frames[0]),
    .fps = ANIM_FPS_SICK,
};

const animation_t dead_animation = {
    .frames = dead_frames,
    .frame_count = sizeof(dead_frames) / sizeof(dead_frames[0]),
    .fps = ANIM_FPS_DEAD,
};

const animation_t celebrate_animation = {
    .frames = celebrate_frames,
    .frame_count = sizeof(celebrate_frames) / sizeof(celebrate_frames[0]),
    .fps = ANIM_FPS_CELEBRATE,
};

// Animation player functions
void animation_player_init(animation_player_t *player, const animation_t *anim, lv_obj_t *img_obj, bool loop) {
    player->animation = anim;
    player->image_obj = img_obj;
    player->current_frame = 0;
    player->last_update_ms = 0;
    player->playing = false;
    player->loop = loop;
    
    // Set initial frame
    if (anim && anim->frame_count > 0 && img_obj) {
        lv_image_set_src(img_obj, anim->frames[0]);
    }
}

void animation_player_start(animation_player_t *player) {
    if (player->animation && player->animation->frame_count > 0) {
        player->playing = true;
        player->current_frame = 0;
        player->last_update_ms = 0;
    }
}

void animation_player_stop(animation_player_t *player) {
    player->playing = false;
}

void animation_player_set_frame(animation_player_t *player, uint16_t frame_index) {
    if (!player->animation || !player->image_obj) return;
    
    if (frame_index < player->animation->frame_count) {
        player->current_frame = frame_index;
        lv_image_set_src(player->image_obj, player->animation->frames[frame_index]);
    }
}

void animation_player_set_animation(animation_player_t *player, const animation_t *anim, bool loop) {
    player->animation = anim;
    player->loop = loop;
    player->current_frame = 0;
    player->last_update_ms = 0;
    player->playing = true;
    
    if (anim && anim->frame_count > 0 && player->image_obj) {
        lv_image_set_src(player->image_obj, anim->frames[0]);
    }
}

void animation_player_update(animation_player_t *player, uint32_t current_time_ms) {
    if (!player->playing || !player->animation || !player->image_obj) return;
    if (player->animation->frame_count == 0) return;
    
    // Calculate milliseconds per frame based on animation's FPS
    uint32_t ms_per_frame = 1000 / player->animation->fps;
    
    // Check if it's time to advance to next frame
    if (player->last_update_ms == 0) {
        player->last_update_ms = current_time_ms;
        return;
    }
    
    uint32_t elapsed = current_time_ms - player->last_update_ms;
    
    if (elapsed >= ms_per_frame) {
        // Advance to next frame
        player->current_frame++;
        
        // Handle looping or stopping at end
        if (player->current_frame >= player->animation->frame_count) {
            if (player->loop) {
                player->current_frame = 0;
            } else {
                player->current_frame = player->animation->frame_count - 1;
                player->playing = false;
                return;
            }
        }
        
        // Update the image
        lv_image_set_src(player->image_obj, player->animation->frames[player->current_frame]);
        
        // Update timing (account for frame time overflow)
        player->last_update_ms += ms_per_frame;
        if (player->last_update_ms > current_time_ms) {
            player->last_update_ms = current_time_ms;
        }
    }
}
