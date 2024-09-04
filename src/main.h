#ifndef MAIN_H
#define MAIN_H

#include "high_impact.h"

// -----------------------------------------------------------------------------
// Button actions
typedef enum {
	A_LEFT,
	A_RIGHT,
	A_UP,
	A_DOWN,
	A_JUMP,
	A_SHOOT,
	A_PAUSE,
	A_SAVE,
	A_LOAD,

	A_DBG_BB,
	A_DBG_ROAM,
	A_CLICK,
	A_RIGHT_CLICK,
	A_ZOOM_IN,
	A_ZOOM_OUT,
} action_t;

// -----------------------------------------------------------------------------
// Global data

typedef struct {
	font_t *font;
	noise_t *noise;
	sound_t music;
	entity_ref_t player;
	camera_t camera;

	float level_time;
	bool draw_bb;
	bool paused;
	bool game_won;
	float game_won_timer;
} global_t;

extern global_t g;


// -----------------------------------------------------------------------------
// Scenes

extern scene_t map_view_;
extern scene_t scene_game;
extern scene_t scene_credits;
extern scene_t scene_anim;
extern scene_t scene_title;
extern scene_t scene_map_view;

void game_set_level_path(char *path);
void game_set_checkpoint(entity_ref_t checkpoint);
void game_respawn(void);

entity_t *game_spawn_particle(vec2_t pos, float vel, float vel_variance, float angle, float angle_variance,
                              anim_def_t *sheet);


#endif
