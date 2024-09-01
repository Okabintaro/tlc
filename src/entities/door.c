#include "../main.h"

EDITOR_SIZE(16, 32);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 229, 123);

static anim_def_t *anim_door_opened;
static anim_def_t *anim_door_closed;
static anim_def_t *anim_door_opening;
static anim_def_t *anim_door_closing;

static void load(void) {
	image_t *door = image("assets/sprites/door.qoi");

	// There are 4 frames in the door sprite sheet, closed, opening, opening, open
	anim_door_opened = anim_def(door, vec2i(16, 32), 1.0, {3});
	anim_door_closed = anim_def(door, vec2i(16, 32), 1.0, {0});
	anim_door_opening = anim_def(door, vec2i(16, 32), 0.1, {1, 2});
	anim_door_closing = anim_def(door, vec2i(16, 32), 0.1, {2, 1});
}


static void draw(entity_t *self) {
	if (self->door.is_open) {
		self->anim = anim(anim_door_opened);
	} else {
		self->anim = anim(anim_door_closed);
	}
}


static void init(entity_t *self) {
	self->size = vec2(16, 32);
	self->physics = ENTITY_PHYSICS_FIXED;
	self->door.is_open = false;
	self->group = ENTITY_GROUP_BREAKABLE;
	self->check_against = ENTITY_GROUP_NONE;

	draw(self);
}


static void settings(entity_t *self, json_t *settings) {
	json_t *is_open = json_value_for_key(settings, "is_open");
	self->door.is_open = is_open ? json_bool(is_open) : false;
}


static void update(entity_t *self) {
	if (self->anim.def == anim_door_opening && anim_looped(&self->anim)) {
		self->door.is_open = true;
		self->anim = anim(anim_door_opened);
	} else if (self->anim.def == anim_door_closing && anim_looped(&self->anim)) {
		self->door.is_open = false;
		self->anim = anim(anim_door_closed);
	}

	if (self->door.is_open) {
		self->physics = ENTITY_PHYSICS_NONE;
		self->size = vec2(0, 0);
	} else {
		self->physics = ENTITY_PHYSICS_FIXED;
		self->size = vec2(16, 32);
	}
}

/// React on ACTIVATE and DEACTIVATE messages
static void message(entity_t *self, entity_message_t message, void *data) {
	if (message == EM_ACTIVATE) {
		self->anim = anim(anim_door_opening);
	} else if (message == EM_DEACTIVATE) {
		self->anim = anim(anim_door_closing);
	}
}

static void damage(entity_t *self, entity_t *other, float damage) {
	// Doors can't be damaged
}

entity_vtab_t entity_vtab_door = {
    .load = load,
    .init = init,
    .damage = damage,
    .message = message,
    .settings = settings,
    .update = update,
};
