#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 128, 0);

static anim_def_t *anim_proj_emitter_idle;
static anim_def_t *anim_proj_emitter_active;

static void load(void) {
	image_t *sheet = image("assets/sprites/proj_emitter.qoi");
	anim_proj_emitter_idle = anim_def(sheet, vec2i(16, 16), 1.0, {0});
	anim_proj_emitter_active = anim_def(sheet, vec2i(16, 16), 1.0, {3});
	anim_proj_emitter_idle->pivot = vec2(8, 8);
	anim_proj_emitter_active->pivot = vec2(8, 8);
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
	self->proj_emitter.speed = 100;
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

	json_t *is_on = json_value_for_key(settings, "is_on");
	self->proj_emitter.is_active = is_on ? json_bool(is_on) : false;

	json_t *is_water = json_value_for_key(settings, "is_water");
	self->proj_emitter.is_water = is_water ? json_bool(is_water) : false;

	json_t *delay = json_value_for_key(settings, "delay");
	self->proj_emitter.delay = delay ? json_number(delay) : 1.0f;

	json_t *speed = json_value_for_key(settings, "speed");
	self->proj_emitter.speed = speed ? json_number(speed) : 100.0f;

	draw(self);
}

static void emit_projectile(entity_t *self) {
	vec2_t spawn_pos = self->pos;
	vec2_t velocity = vec2(0, 0);
	float speed = self->proj_emitter.speed;

	switch (self->proj_emitter.direction) {
	case DIR_LEFT:
		spawn_pos.x -= 8;
		spawn_pos.y += 4;
		velocity.x = -speed;
		break;
	case DIR_RIGHT:
		spawn_pos.x += 8;
		spawn_pos.y += 4;
		velocity.x = speed;
		break;
	case DIR_UP:
		spawn_pos.y -= 8;
		spawn_pos.x += 4;
		velocity.y = -speed;
		break;
	case DIR_DOWN:
		spawn_pos.y += 8;
		spawn_pos.x += 4;
		velocity.y = speed;
		break;
	}

	entity_t *proj = entity_spawn(ENTITY_TYPE_SMAL_PROJ, spawn_pos);
	if (proj) {
		proj->vel = velocity;
		proj->check_against = ENTITY_GROUP_ENEMY | ENTITY_GROUP_BREAKABLE;
		proj->smal_proj.speed = speed;
		proj->smal_proj.is_water = self->proj_emitter.is_water;
	}
	self->proj_emitter.timer = self->proj_emitter.delay;
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
	if (self->proj_emitter.is_water) {
		// Don't draw water emitter/pump
	} else {
		entity_base_draw(self, viewport);
	}
	render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
	            rgba(255, 255, 255, 10));
}

#define SERIALIZE_FIELD(field, type) \
do { \
  memcpy(buffer, field, sizeof(type)); \
  buffer += sizeof(type); \
  remaining_bytes -= sizeof(type); \
  bytes_written += sizeof(type); \
} while (0)

static int serialize(entity_t *self, uint8_t* buffer, uint32_t remaining_bytes) {
  int bytes_written = entity_base_serialize(self, buffer, remaining_bytes);
  buffer += bytes_written;
  SERIALIZE_FIELD(&self->proj_emitter.is_active, bool);
  SERIALIZE_FIELD(&self->proj_emitter.delay, float);
  SERIALIZE_FIELD(&self->proj_emitter.speed, float);
  return bytes_written;
}


#define DESERIALIZE_FIELD(field, type) \
do { \
  memcpy(field, buffer, sizeof(type)); \
  buffer += sizeof(type); \
  bytes_read += sizeof(type); \
} while (0)

static int deserialize(entity_t *self, uint8_t* buffer, uint32_t version) {
  int bytes_read = entity_base_deserialize(self, buffer, version);
  buffer += bytes_read;
  DESERIALIZE_FIELD(&self->proj_emitter.is_active, bool);
  DESERIALIZE_FIELD(&self->proj_emitter.delay, float);
  DESERIALIZE_FIELD(&self->proj_emitter.speed, float);
  return bytes_read;
}


// Called when the entity is serialized to a buffer.
// Return the number of bytes written to the buffer.
// int (*serialize)(entity_t *self, uint8_t* buffer, uint32_t remaining_bytes);

// Called when the entity is deserialized from a buffer
// The buffer pointer is advanced by the number of bytes read
// Returns the number of bytes read from the buffer
// version is the version of the savegame file that gets loaded.
// int (*deserialize)(entity_t *self, uint8_t* buffer, uint32_t version);



entity_vtab_t entity_vtab_proj_emitter = {
    .load = load,
    .init = init,
    .settings = settings,
    .message = message,
    .draw = drawa,
    .update = update,
    .serialize = serialize,
    .deserialize = deserialize,
};
