#include "../main.h"

EDITOR_IGNORE(true)

static void init(entity_t *self) {
	self->size = vec2(19, 10);
	self->gravity = 0;
	self->physics = ENTITY_PHYSICS_WORLD;
}

static void set_hit(entity_t *self) {
	self->projectile.has_hit = true;
	self->vel.x = 0;

	if (self->projectile.anim_hit) {
		self->anim = anim(self->projectile.anim_hit);
		self->anim.flip_x = self->projectile.flip;
	}
}

static void update(entity_t *self) {
	entity_base_update(self);

	if (self->projectile.flip) {
		self->offset = vec2(12, 4);
	} else {
		self->offset = vec2(1, 4);
	}

	if (self->projectile.has_hit && anim_looped(&self->anim)) {
		entity_kill(self);
	}
}

static void collide(entity_t *self, vec2_t normal, trace_t *trace) {
	if (self->projectile.has_hit) {
		return;
	}
	set_hit(self);
}


static void touch(entity_t *self, entity_t *other) {
	if (self->projectile.has_hit) {
		return;
	}
	entity_damage(other, self, 10);
	set_hit(self);
}


static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);
	render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
	            rgba(255, 255, 255, 255));
}


entity_vtab_t entity_vtab_projectile = {
    .init = init, .update = update, .collide = collide, .touch = touch,
    // .draw = draw,
};
