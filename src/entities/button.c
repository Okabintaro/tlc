#include "../main.h"

EDITOR_SIZE(48, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(64, 255, 255);

static anim_def_t *anim_button_off;
static anim_def_t *anim_button_on;

static sound_source_t *snd_hit;

static void load(void) {
	image_t *sheet = image("assets/sprites/button.qoi");

	anim_button_off = anim_def(sheet, vec2i(16, 16), 1.0, {0});
	anim_button_on = anim_def(sheet, vec2i(16, 16), 1.0, {1});
	snd_hit = sound_source("assets/sfx/hit.qoa");
}


static void init(entity_t *self) {
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(48, 16);
	self->offset = vec2(-16, 0);
	self->anim = anim(anim_button_off);
}


static void settings(entity_t *self, json_t *settings) {
	self->button.targets = entities_from_json_names(json_value_for_key(settings, "target"));

	// json_t *is_on = json_value_for_key(settings, "is_on");
	// self->button.is_on = is_on ? json_bool(is_on) : false;

	json_t *delay = json_value_for_key(settings, "delay");
	self->button.delay = delay ? json_number(delay) : -1;
	self->button.can_fire = true;
}


static void touch(entity_t *self, entity_t *other) {
	self->anim = anim(anim_button_off);
	entity_t *player = entity_by_ref(g.player);
	player->player.can_interact_time = 0.1;

	// TODO: Respect delay
	if (!input_released(A_SHOOT)) {
		return;
	}
	self->anim = anim(anim_button_on);

	for (int i = 0; i < self->button.targets.len; i++) {
		entity_t *target = entity_by_ref(self->button.targets.entities[i]);
		if (target) {
			// entity_trigger(target, other);
			// entity_message(target, self->button.is_on ? EM_ACTIVATE : EM_DEACTIVATE, NULL);

			entity_message(target, EM_ROTATE_RIGHT, NULL);
		}
	}
	sound_play_ex(snd_hit, 0.3f, 0.0f, 1.0f);

	if (self->button.delay == -1) {
		self->button.can_fire = false;
	} else {
		self->button.delay_time = self->button.delay;
	}
}

static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 60));
	}
}


entity_vtab_t entity_vtab_button = {
    .load = load,
    .init = init,
    .settings = settings,
    .touch = touch,
    .draw = draw,
};
