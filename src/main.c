#include "main.h"
#include <stdint.h>
#include <stdio.h>

global_t g = {0};

#define DBG_SKIP_TITLE (1) // set to (1) to skip title
#define DBG_KEYS (1)       // set to (1) to enable debug hotkeys

void main_init(void) {
	// Gamepad
	input_bind(INPUT_GAMEPAD_DPAD_LEFT, A_LEFT);
	input_bind(INPUT_GAMEPAD_DPAD_RIGHT, A_RIGHT);
	input_bind(INPUT_GAMEPAD_DPAD_UP, A_UP);
	input_bind(INPUT_GAMEPAD_DPAD_DOWN, A_DOWN);
	input_bind(INPUT_GAMEPAD_L_STICK_LEFT, A_LEFT);
	input_bind(INPUT_GAMEPAD_L_STICK_RIGHT, A_RIGHT);
	input_bind(INPUT_GAMEPAD_X, A_JUMP);
	input_bind(INPUT_GAMEPAD_B, A_JUMP);
	input_bind(INPUT_GAMEPAD_A, A_SHOOT);
	input_bind(INPUT_GAMEPAD_START, A_PAUSE);

	// Keyboard
	input_bind(INPUT_KEY_LEFT, A_LEFT);
	input_bind(INPUT_KEY_UP, A_UP);
	input_bind(INPUT_KEY_DOWN, A_DOWN);
	input_bind(INPUT_KEY_RIGHT, A_RIGHT);

	input_bind(INPUT_KEY_W, A_UP);
	input_bind(INPUT_KEY_A, A_LEFT);
	input_bind(INPUT_KEY_S, A_DOWN);
	input_bind(INPUT_KEY_D, A_RIGHT);

	// MOUSE
	input_bind(INPUT_MOUSE_LEFT, A_CLICK);
	input_bind(INPUT_MOUSE_RIGHT, A_RIGHT_CLICK);

	input_bind(INPUT_MOUSE_WHEEL_UP, A_ZOOM_IN);
	input_bind(INPUT_MOUSE_WHEEL_DOWN, A_ZOOM_OUT);

	input_bind(INPUT_KEY_X, A_JUMP);
	input_bind(INPUT_KEY_C, A_SHOOT);
	input_bind(INPUT_KEY_ESCAPE, A_PAUSE);

#if DBG_KEYS
	input_bind(INPUT_KEY_F2, A_DBG_BB);
	input_bind(INPUT_KEY_F3, A_SAVE);
	input_bind(INPUT_KEY_F4, A_LOAD);
	input_bind(INPUT_KEY_F1, A_DBG_ROAM);
#endif

	printf("Sizeof entity_t: %ld\n", sizeof(entity_t));

	g.noise = noise(8);
	g.player = entity_ref_none();
	g.font = font("assets/font_04b03.qoi", "assets/font_04b03.json");

	// g.music = sound(sound_source("assets/music/pixel7.qoa"));
	// sound_set_loop(g.music, true);
	// sound_set_volume(g.music, 0.0);

	sound_set_global_volume(0.75);

#if DBG_SKIP_TITLE
	// game_set_level_path("assets/levels/01-wasteland2.json");
	game_set_level_path("assets/levels/02-factory.json");
	engine_set_scene(&scene_map_view);
	// engine_set_scene(&scene_game);
#else
	engine_set_scene(&scene_title);
#endif
}

void main_cleanup(void) {}
