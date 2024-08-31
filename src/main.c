#include "main.h"

global_t g;

#define DBG_SKIP_TITLE (1) // set to (1) to skip title

void main_init(void) {
	// Gamepad
	input_bind(INPUT_GAMEPAD_DPAD_LEFT, A_LEFT);
	input_bind(INPUT_GAMEPAD_DPAD_RIGHT, A_RIGHT);
	input_bind(INPUT_GAMEPAD_L_STICK_LEFT, A_LEFT);
	input_bind(INPUT_GAMEPAD_L_STICK_RIGHT, A_RIGHT);
	input_bind(INPUT_GAMEPAD_X, A_JUMP);
	input_bind(INPUT_GAMEPAD_B, A_JUMP);
	input_bind(INPUT_GAMEPAD_A, A_SHOOT);

	// Keyboard
	input_bind(INPUT_KEY_LEFT, A_LEFT);
	input_bind(INPUT_KEY_RIGHT, A_RIGHT);
	input_bind(INPUT_KEY_X, A_JUMP);
	input_bind(INPUT_KEY_C, A_SHOOT);

	g.noise = noise(8);
	g.player = entity_ref_none();
	g.font = font("assets/font_04b03.qoi", "assets/font_04b03.json");

	g.music = sound(sound_source("assets/music/pixel12.qoa"));
	sound_set_loop(g.music, true);

	sound_set_global_volume(0.75);

#if DBG_SKIP_TITLE
	game_set_level_path("assets/levels/01-wasteland.json");
	engine_set_scene(&scene_game);
#else
	engine_set_scene(&scene_title);
#endif
}

void main_cleanup(void) {}
