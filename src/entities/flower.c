#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 255, 0);

static anim_def_t *anim_off;
static anim_def_t *anim_on;
static bool has_light = false;
static bool has_water = false;

static void load(void) {
	image_t *sheet = image("assets/sprites/flower.qoi");

	anim_off = anim_def(sheet, vec2i(16, 16), 1.0, {0});
	anim_on = anim_def(sheet, vec2i(16, 16), 1.0, {1, 2, 3, 4, 5, 6, 7, ANIM_STOP});
}

static void init(entity_t *self) {
	self->size = vec2(16, 16);
	self->anim = anim(anim_off);
}

/// React on ACTIVATE and DEACTIVATE messages
static void message(entity_t *self, entity_message_t message, void *data) {
	int oc = self->door.open_count;
	if (message == EM_WATER) {
		has_water = (bool)data;
	} else if (message == EM_LIGHT) {
		has_light = (bool)data;
	}

	if (has_water && has_light) {
		self->anim = anim(anim_on);
	}
}


static void update(entity_t *self) {
	if (self->anim.def == anim_on && anim_looped(&self->anim)) {
		g.game_won = true;
		// Game won!
	}
}


entity_vtab_t entity_vtab_flower = {
    .load = load,
    .init = init,
    .update = update,
    .message = message,
};
