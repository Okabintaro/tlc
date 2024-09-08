#include "../main.h"
#include "../inspector.h"
#include <stdint.h>

EDITOR_SIZE(8, 14);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

#define JUMP_INITIAL_VEL 60.0
#define JUMP_HIGH_TIME 0.14
#define JUMP_HIGH_ACCEL 780
#define ACCEL_GROUND 800
#define ACCEL_AIR 500
#define FRICTION_GROUND 10
#define FRICTION_AIR 5

static anim_def_t *anim_idle;
static anim_def_t *anim_run;
static anim_def_t *anim_land;
static anim_def_t *anim_jump;
static anim_def_t *anim_fall;
static anim_def_t *anim_shoot;

static anim_def_t *anim_plasma_idle;
static anim_def_t *anim_plasma_hit;
static sound_source_t *sound_jump;


static void load(void) {
	image_t *sheet = image("assets/sprites/player.qoi");

	// clang-format off
	anim_idle    = anim_def(sheet, vec2i(16, 16), 0.2, {0, 1, 2});
	anim_run     = anim_def(sheet, vec2i(16, 16), 0.07, {11, 12, 13, 14, 15});
	anim_shoot   = anim_def(sheet, vec2i(16, 16), 1.0, {16, 17, 18, 19});
	anim_land    = anim_def(sheet, vec2i(16, 16), 0.03, {0, 1, 2, 3, 4, 5, 6, 7});
	anim_jump    = anim_def(sheet, vec2i(16, 16), 0.15, {9});
	anim_fall    = anim_def(sheet, vec2i(16, 16), 0.4, {9, 10});

	sound_jump = sound_source("assets/sfx/jump.qoa");
	// clang-format on
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	self->physics = ENTITY_PHYSICS_ACTIVE;
	self->offset = vec2(2, 4);
	self->size = vec2(8, 12);
	self->group = ENTITY_GROUP_PLAYER;
}

static void update(entity_t *self) {
	if (inspector_active()) {
		return;
	}
	self->friction.x = self->on_ground ? FRICTION_GROUND : FRICTION_AIR;

	bool did_move = false;
	if (input_state(A_LEFT)) {
		self->accel.x = self->on_ground ? -ACCEL_GROUND : -ACCEL_AIR;
		self->player.flip = true;
		did_move = true;
	} else if (input_state(A_RIGHT)) {
		self->accel.x = self->on_ground ? ACCEL_GROUND : ACCEL_AIR;
		self->player.flip = false;
		did_move = true;
	} else {
		self->accel.x = 0;
	}

	// TODO: Add jump buffering and coyote time
	if (input_state(A_JUMP)) {
		if (self->on_ground && self->player.can_jump) {
			self->vel.y = -JUMP_INITIAL_VEL;
			self->player.can_jump = false;
			self->player.high_jump_time = JUMP_HIGH_TIME;
			// TODO: Add some random pitch variation
			sound_play_ex(sound_jump, 0.3, 0.0, 0.8);
		} else if (self->player.high_jump_time > 0) {
			self->player.high_jump_time -= engine.tick;
			float d = self->player.high_jump_time;
			float f = max(0, d < 0 ? engine.tick + d : engine.tick);
			self->vel.y -= JUMP_HIGH_ACCEL * f;
		}
	} else {
		self->player.high_jump_time = 0;
		self->player.can_jump = true;
	}

	bool was_on_ground = self->on_ground;
	entity_base_update(self);

	// Just landed ?
	if (!was_on_ground && self->on_ground) {
		self->anim = anim(anim_land);
		self->offset = vec2(4, 4);
		self->size = vec2(8, 12);
	}

	// On ground?
	else if (was_on_ground && (self->anim.def != anim_land || anim_looped(&self->anim))) {
		if (did_move) {
			if (was_on_ground && self->anim.def != anim_run) {
				self->anim = anim(anim_run);
			}
			self->player.idle_time = 0;
			self->player.is_idle = false;
		} else {
			if (!self->player.is_idle || (!self->anim.def->loop && anim_looped(&self->anim))) {
				self->player.is_idle = true;
				self->player.idle_time = rand_float(3, 7);
				self->anim = anim(anim_idle);
			}
		}
	}

	// In air?
	else if (!was_on_ground) {
		if (self->vel.y < 0) {
			if (self->anim.def != anim_jump) {
				self->anim = anim(anim_jump);
			}
		} else {
			if (self->anim.def != anim_fall) {
				self->anim = anim(anim_fall);
			}
		}
		self->player.is_idle = false;
		self->offset = vec2(4, 0);
		self->size = vec2(8, 12);
	}

	self->anim.flip_x = self->player.flip;
}

static void damage(entity_t *self, entity_t *other, float damage) {}
static void kill(entity_t *self) {}

static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 128));
	}
}

entity_vtab_t entity_vtab_player = {
    .load = load,
    .init = init,
    .update = update,
    .damage = damage,
    .draw = draw,
    .kill = kill,
};
