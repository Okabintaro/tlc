#include "../main.h"

EDITOR_IGNORE(true)

#define DEFAULT_SPEED 200

static anim_def_t *anim_proj_energy;
static anim_def_t *anim_proj_water;

static void load(void) {
	image_t *energy_sheet = image("assets/sprites/smal_proj.qoi");
	anim_proj_energy = anim_def(energy_sheet, vec2i(8, 8), 0.07, {0, 1, 2, 3});
	image_t *water_sheet = image("assets/sprites/smal_proj_water.qoi");
	anim_proj_water = anim_def(water_sheet, vec2i(8, 8), 0.07, {0, 1, 2, 3});
}


static void init(entity_t *self) {
	self->size = vec2(8, 8);
	self->gravity = 0;
	self->physics = ENTITY_PHYSICS_WORLD;
	self->group = ENTITY_GROUP_PROJECTILE;
	self->smal_proj.speed = DEFAULT_SPEED;
}

static void update(entity_t *self) {
	// FIXME: Should be set somewhen after init and settings but only once
	self->anim = anim(self->smal_proj.is_water ? anim_proj_water : anim_proj_energy);
	entity_base_update(self);
}

static void collide(entity_t *self, vec2_t normal, trace_t *trace) {
	entity_kill(self);
}

static void draw(entity_t *self, vec2_t viewport) {
	entity_base_draw(self, viewport);

	if (g.draw_bb) {
		render_draw(vec2_sub(self->pos, viewport), self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 255, 255, 128));
	}
}

static void touch(entity_t *self, entity_t *other) {
	entity_kill(self);
}

entity_vtab_t entity_vtab_smal_proj = {
    .load = load,
    .init = init,
    .update = update,
    .collide = collide,
    .touch = touch,
    .draw = draw,
};
