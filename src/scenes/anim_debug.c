#include "../main.h"


static anim_def_t *anim_idle;
static anim_def_t *anim_run;
static anim_def_t *anim_shoot;
static anim_def_t *anim_land;
static anim_def_t *anim_jump;
static anim_def_t *anim_fall;
static anim_def_t *anim_die;
static anim_t my_anim;

static void init(void) {
	// TODO: Load title images/art if possible
	image_t *sheet = image("assets/sprites/player.qoi");

	// clang-format off
	anim_idle    = anim_def(sheet, vec2i(16, 16), 0.1, {0, 1, 2});
	anim_run     = anim_def(sheet, vec2i(16, 16), 0.1, {11, 12, 13, 14, 15});
	anim_shoot   = anim_def(sheet, vec2i(16, 16), 1.0, {16, 17, 18, 19});
	anim_land    = anim_def(sheet, vec2i(16, 16), 0.1, {0, 1, 2, 3, 4, 5, 6, 7});
	anim_jump    = anim_def(sheet, vec2i(16, 16), 0.15, {9});
	anim_fall    = anim_def(sheet, vec2i(16, 16), 0.4, {9, 10});
	anim_die     = anim_def(sheet, vec2i(16, 16), 1.0, {0, 1, 2});
	// clang-format on
	my_anim = anim(anim_run);
}

static void update(void) {
	scene_base_update();
}

static void draw(void) {
	scene_base_draw();
	my_anim.flip_x = true;
	anim_draw(&my_anim, vec2(100, 100));

	// float d = engine.time - 1;
	// if (d > 2 || (int)(d * 2) % 2 == 0) {
	// 	font_draw(g.font, vec2(RENDER_WIDTH / 2, RENDER_HEIGHT - 16), "Press X or C to Play", FONT_ALIGN_CENTER);
	// }
}

scene_t scene_anim = {
    .init = init,
    .update = update,
    .draw = draw,
};
