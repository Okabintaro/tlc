#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(64, 255, 0);

#define PROJ_SPEED 200

static anim_def_t *anim_recv;
static anim_def_t *anim_recv_water;

static sound_source_t *snd_beep;
static sound_source_t *snd_beep_sine;

static void load(void) {
	image_t *sheet = image("assets/sprites/recv.qoi");
	anim_recv = anim_def(sheet, vec2i(16, 16), 1.0, {0, 1, 2, 3, 4, 5, 6, 7});
	image_t *sheet_water = image("assets/sprites/recv_water.qoi");
	anim_recv_water = anim_def(sheet_water, vec2i(16, 16), 1.0, {0, 1, 2, 3, 4, 5, 6, 7});

	snd_beep = sound_source("assets/sfx/beep_st.qoa");
	snd_beep_sine = sound_source("assets/sfx/beep_sine.qoa");
}

static void init(entity_t *self) {
	self->check_against = ENTITY_GROUP_PROJECTILE;
	self->size = vec2(8, 8);
	self->anim = anim(anim_recv);
	self->proj_recv.decay = -1.0f;
}


static void settings(entity_t *self, json_t *settings) {
	self->proj_recv.targets = entities_from_json_names(json_value_for_key(settings, "target"));

	json_t *is_water = json_value_for_key(settings, "is_water");
	self->proj_recv.is_water = is_water ? json_bool(is_water) : false;

	json_t *decay = json_value_for_key(settings, "decay");
	self->proj_recv.decay = decay ? json_number(decay) : -1.0f;

	if (is_water) {
		self->anim = anim(anim_recv_water);
	}

	// TODO: Load num_needed, ...
}


static void notify_targets(entity_t *self, entity_t *other, bool on) {
	for (int i = 0; i < self->proj_recv.targets.len; i++) {
		entity_t *target = entity_by_ref(self->proj_recv.targets.entities[i]);
		if (target) {
			entity_trigger(target, other);
			entity_message(target, on ? EM_ACTIVATE : EM_DEACTIVATE, NULL);
		}
	}
}

static void update(entity_t *self) {
	if (self->proj_recv.decay > 0.0f) {
		self->proj_recv.decay_timer -= engine.tick;
		if (self->proj_recv.decay_timer <= 0.0) {
			int lnr = self->proj_recv.num_received;
			self->proj_recv.num_received -= 1;
			self->proj_recv.num_received = clamp(self->proj_recv.num_received, 0, 7);
			self->proj_recv.decay_timer = self->proj_recv.decay;

			int nr = self->proj_recv.num_received;
			// printf("lnr: %d\n", lnr);
			// printf("nr: %d\n", nr);
			if (lnr >= 1 && nr <= 0) {
				notify_targets(self, NULL, false);
			}
		}
	}
}


static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	anim_def_t *def = self->anim.def;

	int progress = self->proj_recv.num_received;
	progress = clamp(progress, 0, 7);

	int tile = def->sequence[progress];
	vec2_t pos = vec2_sub(self->pos, viewport);
	bool flip_x = false;
	bool flip_y = false;
	image_draw_tile_ex(def->sheet, tile, def->frame_size, pos, flip_x, flip_y, rgba_white());

	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 10));
	}
}

static void damage(entity_t *self, entity_t *other, float damage) {}


static void touch(entity_t *self, entity_t *other) {
	entity_kill(other);

	int last = self->proj_recv.num_received;
	self->proj_recv.num_received = clamp(self->proj_recv.num_received + 1, 0, 7);
	int nr = self->proj_recv.num_received;

	if (nr > last) {
		float pitch = 1.0f + (float)nr / 8.0f;
		sound_play_ex(self->proj_recv.is_water ? snd_beep_sine : snd_beep, 0.4f, 0.0f, pitch);
	}
	if (last == 6 && nr == 7) {
		notify_targets(self, other, true);
	}
}

entity_vtab_t entity_vtab_proj_recv = {
    .load = load,
    .init = init,
    .settings = settings,
    .draw = draw,
    .damage = damage,
    .update = update,
    .touch = touch,
};
