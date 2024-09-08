#include "../main.h"
#include "../inspector.h"

static void init(void) {
	// engine_load_level("assets/levels/02-factory.json");
	engine_load_level_tiled("assets/map/intro.tmj", "assets/map");

	inspector_init((inspector_setup_t) {
		.cam = &g.camera,
		.font = g.font,
		.player = g.player,
		.actions = {
			.toggle = A_DBG_ROAM,
			.up = A_UP,
			.right = A_RIGHT,
			.down = A_DOWN,
			.left = A_LEFT,
			.select = A_CLICK,
			.drag = A_RIGHT_CLICK,
			.zoom_in = A_ZOOM_IN,
			.zoom_out = A_ZOOM_OUT,
		}
	});
}

static void update(void) {
	scene_base_update();

	// if (input_pressed(A_SHOOT) || input_pressed(A_JUMP)) {
	// 	sound_set_time(g.music, 0);
	// 	sound_set_volume(g.music, 1.0);
	// 	sound_unpause(g.music);
	// 	game_set_level_path("assets/levels/01-wasteland2.json");
	// 	engine_set_scene(&scene_game);
	// }
	inspector_update();
	camera_update(&g.camera);
}

static void draw(void) {
	scene_base_draw();
	inspector_draw();
}

scene_t scene_map_view = {
    .init = init,
    .update = update,
    .draw = draw,
};
