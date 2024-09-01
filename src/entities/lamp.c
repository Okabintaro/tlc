#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 255, 0);

static anim_def_t *anim_lamp_off;
static anim_def_t *anim_lamp_on;

static void load(void) {
	image_t *sheet = image("assets/sprites/lamp.qoi");

	anim_lamp_off = anim_def(sheet, vec2i(16, 16), 1.0, {0});
	anim_lamp_on = anim_def(sheet, vec2i(16, 16), 0.1, {2, 3});
}


static void init(entity_t *self) {
	self->size = vec2(16, 16);
	self->anim = anim(anim_lamp_off);
}

static void settings(entity_t *self, json_t *settings) {
	self->lamp.targets = entities_from_json_names(json_value_for_key(settings, "target"));
}


static void notify(entity_t *self, bool on) {
	for (int i = 0; i < self->lamp.targets.len; i++) {
		entity_t *target = entity_by_ref(self->lamp.targets.entities[i]);
		if (target) {
			entity_message(target, EM_LIGHT, (void *)on);
		}
	}
}


/// React on ACTIVATE and DEACTIVATE messages
static void message(entity_t *self, entity_message_t message, void *data) {
	int oc = self->door.open_count;
	if (message == EM_ACTIVATE) {
		self->anim = anim(anim_lamp_on);
		notify(self, true);
	} else if (message == EM_DEACTIVATE) {
		self->anim = anim(anim_lamp_off);
		notify(self, false);
	}
}

entity_vtab_t entity_vtab_lamp = {
    .settings = settings,
    .load = load,
    .init = init,
    .message = message,
};
