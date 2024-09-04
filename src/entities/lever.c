#include "../main.h"

EDITOR_SIZE(48, 48);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 64, 64);

static anim_def_t *anim_lever_off;
static anim_def_t *anim_lever_on;
static anim_def_t *anim_lever_switching_on;
static anim_def_t *anim_lever_switching_off;

static void load(void) {
	image_t *tiles = image("assets/tiles/tiles.qoi");

	anim_lever_off = anim_def(tiles, vec2i(16, 16), 1.0, {262});
	anim_lever_on = anim_def(tiles, vec2i(16, 16), 1.0, {265});
}


static void draw(entity_t *self) {
	if (self->lever.is_on) {
		self->anim = anim(anim_lever_on);
	} else {
		self->anim = anim(anim_lever_off);
	}
	self->anim.flip_y = self->lever.flip;
}


static void init(entity_t *self) {
	self->lever.is_on = false;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(48, 48);
	self->offset = vec2(-16, -16);
	draw(self);
}

static void notify_targets(entity_t *self, entity_t *other);

static void settings(entity_t *self, json_t *settings) {
	self->lever.targets = entities_from_json_names(json_value_for_key(settings, "target"));
	json_t *is_on = json_value_for_key(settings, "is_on");
	self->lever.is_on = is_on ? json_bool(is_on) : false;

	if (self->lever.is_on) {
		notify_targets(self, NULL);
	}

	json_t *delay = json_value_for_key(settings, "delay");
	self->lever.delay = delay ? json_number(delay) : -1;
	self->lever.can_fire = true;

	json_t *flip = json_value_for_key(settings, "flip");
	self->lever.flip = flip ? json_bool(flip) : false;
}


static void update(entity_t *self) {
	draw(self);
}


static void notify_targets(entity_t *self, entity_t *other) {
	for (int i = 0; i < self->lever.targets.len; i++) {
		entity_t *target = entity_by_ref(self->lever.targets.entities[i]);
		if (target) {
			entity_trigger(target, other);
			entity_message(target, self->lever.is_on ? EM_ACTIVATE : EM_DEACTIVATE, NULL);
		}
	}
}

static void touch(entity_t *self, entity_t *other) {
	entity_t *player = entity_by_ref(g.player);

	if (!input_released(A_SHOOT)) {
		return;
	}

	self->lever.is_on = !self->lever.is_on;
	draw(self);

	notify_targets(self, other);

	if (self->lever.delay == -1) {
		self->lever.can_fire = false;
	} else {
		self->lever.delay_time = self->lever.delay;
	}
}


/// React on ACTIVATE and DEACTIVATE messages
static void message(entity_t *self, entity_message_t message, void *data) {
	if (message == EM_ACTIVATE) {
		self->lever.is_on = true;
	} else if (message == EM_DEACTIVATE) {
		self->lever.is_on = false;
	}
	draw(self);
}


entity_vtab_t entity_vtab_lever = {
    .load = load,
    .init = init,
    .settings = settings,
    .message = message,
    .update = update,
    .touch = touch,
};
