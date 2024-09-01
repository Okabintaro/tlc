#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 128, 0);

#define PROJ_SPEED 200

static anim_def_t *anim_prism_idle;
static anim_def_t *anim_prism_active;

static anim_def_t *anim_proj_idle;
static anim_def_t *anim_proj_hit;

static void load(void) {
	image_t *sheet = image("assets/sprites/prism.qoi");
	anim_prism_idle = anim_def(sheet, vec2i(16, 16), 1.0, {0});

	anim_prism_active = anim_def(sheet, vec2i(16, 16), 1.0, {1});
	anim_prism_idle->pivot = vec2(8, 8);
	anim_prism_active->pivot = vec2(8, 8);

	image_t *plasma_sheet = image("assets/sprites/smal_proj.qoi");
	anim_proj_idle = anim_def(plasma_sheet, vec2i(8, 8), 0.07, {0, 1, 2, 3});
}

static void update_anim(entity_t *self) {
	self->anim = anim(self->proj_prism.cooldown_timer > 0 ? anim_prism_active : anim_prism_idle);

	switch (self->proj_prism.direction) {
	case DIR_UP:
		self->anim.rotation = 0;
		break;
	case DIR_RIGHT:
		self->anim.rotation = -M_PI / 2;
		break;
	case DIR_DOWN:
		self->anim.rotation = M_PI;
		break;
	case DIR_LEFT:
		self->anim.rotation = M_PI / 2;
		break;
	}
}

static void init(entity_t *self) {
	self->proj_prism.direction = DIR_UP;
	self->proj_prism.is_active = false;
	self->check_against = ENTITY_GROUP_PROJECTILE;
	self->size = vec2(8, 8);
	update_anim(self);
}


static void update(entity_t *self) {
	self->proj_prism.cooldown_timer -= engine.tick;
	update_anim(self);
}


static void settings(entity_t *self, json_t *settings) {
	json_t *direction = json_value_for_key(settings, "direction");
	if (direction) {
		const char *dir = json_string(direction);
		if (strcmp(dir, "left") == 0) {
			self->proj_prism.direction = DIR_LEFT;
		} else if (strcmp(dir, "right") == 0) {
			self->proj_prism.direction = DIR_RIGHT;
		} else if (strcmp(dir, "up") == 0) {
			self->proj_prism.direction = DIR_UP;
		} else if (strcmp(dir, "down") == 0) {
			self->proj_prism.direction = DIR_DOWN;
		}
	}
	update_anim(self);
}

static void redirect_projectile(entity_t *self, entity_t *proj) {
	vec2_t dir = vec2_sub(vec2_add(self->pos, vec2(4, 4)), proj->pos);
	direction_t from_dir;
	if (dir.x < 0) {
		from_dir = DIR_RIGHT;
	} else if (dir.x > 0) {
		from_dir = DIR_LEFT;
	} else if (dir.y < 0) {
		from_dir = DIR_DOWN;
	} else {
		from_dir = DIR_UP;
	}

	vec2_t velocity = vec2(0, 0);
	direction_t to_dir;
	switch (self->proj_prism.direction) {
	case DIR_UP:
		if (from_dir == DIR_UP) {
			velocity = vec2(PROJ_SPEED, 0); // right
		} else if (from_dir == DIR_RIGHT) {
			velocity = vec2(0, -PROJ_SPEED); // up
		} else {
			entity_kill(proj);
			return;
		}
		break;
	case DIR_RIGHT:
		if (from_dir == DIR_DOWN) {
			velocity = vec2(PROJ_SPEED, 0); // right
		} else if (from_dir == DIR_RIGHT) {
			velocity = vec2(0, PROJ_SPEED); // down
		} else {
			entity_kill(proj);
			return;
		}

	case DIR_DOWN:
		if (from_dir == DIR_DOWN) {
			velocity = vec2(-PROJ_SPEED, 0); // left
		} else if (from_dir == DIR_LEFT) {
			velocity = vec2(0, PROJ_SPEED); // down
		} else {
			entity_kill(proj);
			return;
		}
		break;

	case DIR_LEFT:
		if (from_dir == DIR_UP) {
			velocity = vec2(-PROJ_SPEED, 0); // left
		} else if (from_dir == DIR_LEFT) {
			velocity = vec2(0, -PROJ_SPEED); // up
		} else {
			entity_kill(proj);
			return;
		}
		break;
	}

	proj->pos = vec2_add(self->pos, vec2(4, 4));
	proj->vel = velocity;
}


static void rotate_right(entity_t *self) {
	if (self->proj_prism.direction == DIR_LEFT) {
		self->proj_prism.direction = DIR_UP;
	} else if (self->proj_prism.direction == DIR_UP) {
		self->proj_prism.direction = DIR_RIGHT;
	} else if (self->proj_prism.direction == DIR_RIGHT) {
		self->proj_prism.direction = DIR_DOWN;
	} else if (self->proj_prism.direction == DIR_DOWN) {
		self->proj_prism.direction = DIR_LEFT;
	}
}


static void message(entity_t *self, entity_message_t message, void *data) {
	if (message == EM_ACTIVATE) {
	} else if (message == EM_DEACTIVATE) {
		self->proj_prism.is_active = false;
	} else if (message == EM_ROTATE_RIGHT) {
		rotate_right(self);
	}
	update_anim(self);
}


static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 10));
	}
}


static void damage(entity_t *self, entity_t *other, float damage) {}


static void touch(entity_t *self, entity_t *other) {
	if (self->proj_prism.cooldown_timer > 0) {
		return;
	}
	redirect_projectile(self, other);
	self->proj_prism.cooldown_timer = 0.5;
}


entity_vtab_t entity_vtab_proj_prism = {
    .load = load,
    .init = init,
    .settings = settings,
    .message = message,
    .update = update,
    .draw = draw,
    .damage = damage,
    .touch = touch,
};
