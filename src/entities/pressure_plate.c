#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 229, 123);

static anim_def_t *anim_pp_off;
static anim_def_t *anim_pp_on;

static void load(void) {
	image_t *sheet = image("assets/sprites/pp.qoi");

	anim_pp_off = anim_def(sheet, vec2i(16, 16), 1.0, {0});
	anim_pp_on = anim_def(sheet, vec2i(16, 16), 1.0, {1});
}


static void init(entity_t *self) {
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(16, 16);
	self->offset = vec2(0, 0);
	self->anim = anim(anim_pp_off);
	self->pressure_plate.delay = 1.0f;
}


static void settings(entity_t *self, json_t *settings) {
	self->pressure_plate.targets = entities_from_json_names(json_value_for_key(settings, "target"));

	json_t *delay = json_value_for_key(settings, "delay");
	self->pressure_plate.delay = delay ? json_number(delay) : 1.0f;
}


static void touch(entity_t *self, entity_t *other) {
	if (self->pressure_plate.on_time <= 0) {
		for (int i = 0; i < self->button.targets.len; i++) {
			entity_t *target = entity_by_ref(self->button.targets.entities[i]);
			if (target) {
				entity_message(target, EM_ACTIVATE, NULL);
			}
		}
	}

	self->pressure_plate.on_time = self->pressure_plate.delay;
	self->anim = anim(anim_pp_on);
}

static void update(entity_t *self) {
	if (self->pressure_plate.on_time <= 0) {
		return;
	}

	// printf("Pressure Plate on_time: %f\n", self->pressure_plate.on_time);
	self->pressure_plate.on_time -= engine.tick;
	if (self->pressure_plate.on_time <= 0) {
		// printf("Pressure Plate reset\n");
		self->anim = anim(anim_pp_off);
		for (int i = 0; i < self->button.targets.len; i++) {
			entity_t *target = entity_by_ref(self->button.targets.entities[i]);
			if (target) {
				entity_message(target, EM_DEACTIVATE, NULL);
			}
		}
	}
}

static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 10));
	}
}


entity_vtab_t entity_vtab_pressure_plate = {
    .load = load,
    .init = init,
    .settings = settings,
    .touch = touch,
    .update = update,
    .draw = draw,
};
