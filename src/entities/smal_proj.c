#include "../main.h"

EDITOR_IGNORE(true)

#define DEFAULT_SPEED 200

static void init(entity_t *self) {
	self->size = vec2(8, 8);
	self->gravity = 0;
	self->physics = ENTITY_PHYSICS_WORLD;
	self->group = ENTITY_GROUP_PROJECTILE;
	self->smal_proj.speed = DEFAULT_SPEED;
}

static void set_hit(entity_t *self) {
	self->smal_proj.has_hit = true;
	self->vel.x = 0;

	if (self->smal_proj.anim_hit) {
		self->anim = anim(self->smal_proj.anim_hit);
	}
}

static void update(entity_t *self) {
	entity_base_update(self);

	if (self->smal_proj.has_hit && anim_looped(&self->anim)) {
		entity_kill(self);
	}
}

static void collide(entity_t *self, vec2_t normal, trace_t *trace) {
	if (self->smal_proj.has_hit) {
		return;
	}
	set_hit(self);
}


static void touch(entity_t *self, entity_t *other) {
	if (self->smal_proj.has_hit) {
		return;
	}
	entity_damage(other, self, 10);
	set_hit(self);
}


static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);

	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 128));
	}
}


entity_vtab_t entity_vtab_smal_proj = {
    .init = init,
    .update = update,
    .collide = collide,
    .touch = touch,
    .draw = draw,
};
