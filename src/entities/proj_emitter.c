#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 128, 0);

static anim_def_t *anim_proj_emitter_idle;
static anim_def_t *anim_proj_emitter_active;

static anim_def_t *anim_proj_idle;
static anim_def_t *anim_proj_hit;

static void load(void) {
	image_t *sheet = image("assets/sprites/proj_emitter.qoi");
	anim_proj_emitter_idle = anim_def(sheet, vec2i(16, 16), 1.0, {0});
	anim_proj_emitter_active = anim_def(sheet, vec2i(16, 16), 1.0, {3});
	anim_proj_emitter_idle->pivot = vec2(8, 8);
	anim_proj_emitter_active->pivot = vec2(8, 8);

	image_t *plasma_sheet = image("assets/sprites/smal_proj.qoi");
	anim_proj_idle = anim_def(plasma_sheet, vec2i(8, 8), 0.07, {0, 1, 2, 3});
	// anim_proj_hit = anim_def(plasma_sheet, vec2i(32, 16), 0.07, {0, 1, 2, 3, ANIM_STOP});
}

static void draw(entity_t *self) {
	self->anim = anim(self->proj_emitter.is_active ? anim_proj_emitter_active : anim_proj_emitter_idle);

	switch (self->proj_emitter.direction) {
	case DIR_UP:
		self->anim.rotation = 0;
		// self->anim.flip_x = false;
		// self->anim.flip_y = false;
		break;
	case DIR_RIGHT:
		self->anim.rotation = -M_PI / 2;
		// self->anim.flip_x = false;
		// self->anim.flip_y = false;
		break;
	case DIR_DOWN:
		self->anim.rotation = M_PI;
		// self->anim.flip_y = true;
		// self->anim.flip_x = false;
		break;
	case DIR_LEFT:
		self->anim.rotation = M_PI / 2;
		// self->anim.flip_x = true;
		// self->anim.flip_y = false;
		break;
	}
}

static void init(entity_t *self) {
	self->proj_emitter.is_active = false;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(16, 16);
	draw(self);
}

static void settings(entity_t *self, json_t *settings) {
	json_t *direction = json_value_for_key(settings, "direction");
	if (direction) {
		const char *dir = json_string(direction);
		if (strcmp(dir, "left") == 0) {
			self->proj_emitter.direction = DIR_LEFT;
		} else if (strcmp(dir, "right") == 0) {
			self->proj_emitter.direction = DIR_RIGHT;
		} else if (strcmp(dir, "up") == 0) {
			self->proj_emitter.direction = DIR_UP;
		} else if (strcmp(dir, "down") == 0) {
			self->proj_emitter.direction = DIR_DOWN;
		}
	}
	draw(self);
}

static void emit_projectile(entity_t *self) {
	vec2_t spawn_pos = self->pos;
	vec2_t velocity = vec2(0, 0);

	switch (self->proj_emitter.direction) {
	case DIR_LEFT:
		spawn_pos.x -= 8;
		spawn_pos.y += 4;
		velocity.x = -200;
		break;
	case DIR_RIGHT:
		spawn_pos.x += 8;
		spawn_pos.y += 4;
		velocity.x = 200;
		break;
	case DIR_UP:
		spawn_pos.y -= 8;
		spawn_pos.x += 4;
		velocity.y = -200;
		break;
	case DIR_DOWN:
		spawn_pos.y += 8;
		spawn_pos.x += 4;
		velocity.y = 200;
		break;
	}

	entity_t *plasma = entity_spawn(ENTITY_TYPE_SMAL_PROJ, spawn_pos);
	if (plasma) {
		plasma->vel = velocity;
		plasma->check_against = ENTITY_GROUP_ENEMY | ENTITY_GROUP_BREAKABLE;
		plasma->anim = anim(anim_proj_idle);
		plasma->anim.flip_x = plasma->projectile.flip;
		plasma->projectile.anim_hit = anim_proj_hit;
	}
	self->proj_emitter.timer = 2.0f;
}


static void update(entity_t *self) {
	if (!self->proj_emitter.is_active) {
		return;
	}
	if (self->proj_emitter.timer > 0) {
		self->proj_emitter.timer -= engine.tick;
	} else {
		emit_projectile(self);
	}
	draw(self);
}

static void rotate_right(entity_t *self) {
	if (self->proj_emitter.direction == DIR_LEFT) {
		self->proj_emitter.direction = DIR_UP;
	} else if (self->proj_emitter.direction == DIR_UP) {
		self->proj_emitter.direction = DIR_RIGHT;
	} else if (self->proj_emitter.direction == DIR_RIGHT) {
		self->proj_emitter.direction = DIR_DOWN;
	} else if (self->proj_emitter.direction == DIR_DOWN) {
		self->proj_emitter.direction = DIR_LEFT;
	}
}


static void message(entity_t *self, entity_message_t message, void *data) {
	if (message == EM_ACTIVATE) {
		self->proj_emitter.is_active = true;
		emit_projectile(self);
	} else if (message == EM_DEACTIVATE) {
		self->proj_emitter.is_active = false;
	} else if (message == EM_ROTATE_RIGHT) {
		rotate_right(self);
	}
	draw(self);
}


static void drawa(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
	            rgba(255, 255, 255, 10));
}


entity_vtab_t entity_vtab_proj_emitter = {
    .load = load,
    .init = init,
    .settings = settings,
    .message = message,
    .draw = drawa,
    .update = update,
};
