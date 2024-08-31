#include "../main.h"

EDITOR_SIZE(8, 14);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

#define JUMP_INITIAL_VEL 40.0
#define JUMP_HIGH_TIME 0.14
#define JUMP_HIGH_ACCEL 780
#define ACCEL_GROUND 600
#define ACCEL_AIR 300
#define FRICTION_GROUND 10
#define FRICTION_AIR 5

static anim_def_t *anim_idle;
static anim_def_t *anim_run;
static anim_def_t *anim_land;
static anim_def_t *anim_jump;
static anim_def_t *anim_fall;
static anim_def_t *anim_die;
static anim_def_t *anim_shoot;

// static anim_def_t *anim_spawn;
// static anim_def_t *anim_gib;
// static anim_def_t *anim_gib_gun;

static anim_def_t *anim_plasma_idle;
static anim_def_t *anim_plasma_hit;


static void load(void) {
	image_t *sheet = image("assets/sprites/player.qoi");

	// clang-format off
	anim_idle    = anim_def(sheet, vec2i(16, 16), 0.2, {0, 1, 2});
	anim_run     = anim_def(sheet, vec2i(16, 16), 0.07, {11, 12, 13, 14, 15});
	anim_shoot   = anim_def(sheet, vec2i(16, 16), 1.0, {16, 17, 18, 19});
	anim_land    = anim_def(sheet, vec2i(16, 16), 0.03, {0, 1, 2, 3, 4, 5, 6, 7});
	anim_jump    = anim_def(sheet, vec2i(16, 16), 0.15, {9});
	anim_fall    = anim_def(sheet, vec2i(16, 16), 0.4, {9, 10});
	anim_die     = anim_def(sheet, vec2i(16, 16), 1.0, {0, 1, 2});

	// anim_die     = anim_def(sheet, vec2i(16, 16), 0.07, {18, 19, 20, 21, 22, 23, 16, 16, 16});
	// anim_scratch = anim_def(sheet, vec2i(16, 16), 0.3, {2, 1, 2, 1, 2, ANIM_STOP});
	// anim_shrug   = anim_def(sheet, vec2i(16, 16), 0.3, {3, 3, 3, 3, 3, 3, 4, 3, 3, ANIM_STOP});
	// anim_run     = anim_def(sheet, vec2i(16, 16), 0.07, {6, 7, 8, 9, 10, 11});
	// anim_jump    = anim_def(sheet, vec2i(16, 16), 1.0, {15});
	// anim_fall    = anim_def(sheet, vec2i(16, 16), 0.4, {12, 13});
	// anim_spawn   = anim_def(sheet, vec2i(16, 16), 0.07, {16, 16, 16, 23, 22, 21, 20, 19, 18});
	// anim_gib     = anim_def(sheet, vec2i(8, 8), 10, {82, 94});
	// anim_gib_gun = anim_def(sheet, vec2i(8, 8), 10, {11});

	image_t *plasma_sheet = image("assets/sprites/projectile.qoi");
	anim_plasma_idle      = anim_def(plasma_sheet, vec2i(32, 16), 1.0, {0});
	anim_plasma_hit       = anim_def(plasma_sheet, vec2i(32, 16), 0.07, {0, 1, 2, 3, ANIM_STOP});

	// sound_plasma = sound_source("assets/sounds/plasma.qoa");
	// sound_die    = sound_source("assets/sounds/die-respawn.qoa");
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
	// // spawning?
	// if (self->anim.def == anim_spawn) {
	// 	if (anim_looped(&self->anim)) {
	// 		self->anim = anim(anim_idle);
	// 	} else {
	// 		return;
	// 	}
	// }

	// // dying?
	// if (self->anim.def == anim_die) {
	// 	if (anim_looped(&self->anim)) {
	// 		entity_kill(self);
	// 	}
	// 	return;
	// }

	bool upside_down = engine.gravity < 0;

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

	if (input_state(A_JUMP)) {
		if (self->on_ground && self->player.can_jump) {
			self->vel.y = upside_down ? JUMP_INITIAL_VEL : -JUMP_INITIAL_VEL;
			self->player.can_jump = false;
			self->player.high_jump_time = JUMP_HIGH_TIME;
		} else if (self->player.high_jump_time > 0) {
			self->player.high_jump_time -= engine.tick;
			float d = self->player.high_jump_time;
			float f = max(0, d < 0 ? engine.tick + d : engine.tick);
			if (upside_down) {
				self->vel.y += JUMP_HIGH_ACCEL * f;
			} else {
				self->vel.y -= JUMP_HIGH_ACCEL * f;
			}
		}
	} else {
		self->player.high_jump_time = 0;
		self->player.can_jump = true;
	}

	if (input_pressed(A_SHOOT)) {
		vec2_t spawn_pos = vec2_add(self->pos, vec2(self->player.flip ? -3 : 5, 6));
		spawn_pos.y -= 5;
		entity_t *plasma = entity_spawn(ENTITY_TYPE_PROJECTILE, spawn_pos);
		if (plasma) {
			plasma->vel = vec2(self->player.flip ? -200 : 200, 0);
			plasma->check_against = ENTITY_GROUP_ENEMY | ENTITY_GROUP_BREAKABLE;
			plasma->anim = anim(anim_plasma_idle);
			plasma->projectile.flip = self->player.flip;
			plasma->anim.flip_x = self->player.flip;
			plasma->projectile.anim_hit = anim_plasma_hit;
		}
		// Add some recoil
		self->vel = vec2_add(self->vel, vec2(self->player.flip ? 50 : -50, 0));

		// sound_play(sound_plasma);
	}

	bool was_on_ground = self->on_ground;
	entity_base_update(self);

	// Just landed ?
	if (!was_on_ground && self->on_ground) {
		self->anim = anim(anim_land);
		self->offset = upside_down ? vec2(4, 0) : vec2(4, 4);
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

			// self->player.idle_time -= engine.tick;
			// if (self->player.is_idle && self->player.idle_time < 0) {
			// 	self->player.idle_time = rand_float(3, 7);
			// 	self->anim = rand_int(0, 1) == 0 ? anim(anim_scratch) : anim(anim_shrug);
			// }
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
	self->anim.flip_y = upside_down;
	// self->anim.tile_offset = self->player.flip ? 24 : 0;
}

static void damage(entity_t *self, entity_t *other, float damage) {
	if (self->anim.def != anim_die) {
		self->anim = anim(anim_die);
		for (int i = 0; i < 5; i++) {
			// entity_t *particle = game_spawn_particle(self->pos, 70, 30, -M_PI / 2, M_PI / 2, anim_gib);
			// if (particle) {
			// 	particle->physics = ENTITY_PHYSICS_MOVE;
			// 	particle->particle.life_time = 0.5;
			// 	particle->particle.fade_time = 0.5;
			// }
		}

		// entity_t *gun = game_spawn_particle(self->pos, 60, 10, -M_PI / 2, M_PI / 2, anim_gib_gun);
		// if (gun) {
		// 	gun->size = vec2(8, 8);
		// }

		// sound_play(sound_die);
	}
}

static void kill(entity_t *self) {
	game_respawn();
}

static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	// render_draw(&self->pos, vec2_sub(self->pos, viewport), self->offset));
	render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
	            rgba(255, 255, 255, 255));
}

entity_vtab_t entity_vtab_player = {
    .load = load,
    .init = init,
    .update = update,
    .damage = damage,
    // .draw = draw,
    .kill = kill,
};
