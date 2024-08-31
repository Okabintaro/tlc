#include "../main.h"

static void init(void) {
	// TODO: Load title images/art if possible
}

static void update(void) {
	scene_base_update();

	if (input_pressed(A_SHOOT) || input_pressed(A_JUMP)) {
		sound_set_time(g.music, 0);
		sound_set_volume(g.music, 1.0);
		sound_unpause(g.music);

		game_set_level_path("assets/levels/01-wasteland.json");
		engine_set_scene(&scene_game);
	}
}

static void draw(void) {
	scene_base_draw();

	float d = engine.time - 1;
	if (d > 2 || (int)(d * 2) % 2 == 0) {
		font_draw(g.font, vec2(RENDER_WIDTH / 2, RENDER_HEIGHT - 16), "Press X or C to Play", FONT_ALIGN_CENTER);
	}
}

scene_t scene_title = {
    .init = init,
    .update = update,
    .draw = draw,
};
