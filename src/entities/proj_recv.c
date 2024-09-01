#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 128, 0);

#define PROJ_SPEED 200

static anim_def_t *anim_recv;

static void load(void) {
	image_t *sheet = image("assets/sprites/recv.qoi");
	anim_recv = anim_def(sheet, vec2i(16, 16), 1.0, {0, 1, 2, 3, 4, 5, 6, 7});
}

static void init(entity_t *self) {
	self->check_against = ENTITY_GROUP_PROJECTILE;
	self->size = vec2(8, 8);
	self->anim = anim(anim_recv);
}


static void settings(entity_t *self, json_t *settings) {
	self->lever.targets = entities_from_json_names(json_value_for_key(settings, "target"));
	// TODO: Load num_needed, ...
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

static void notify_targets(entity_t *self, entity_t *other) {
	for (int i = 0; i < self->proj_recv.targets.len; i++) {
		entity_t *target = entity_by_ref(self->proj_recv.targets.entities[i]);
		if (target) {
			entity_trigger(target, other);
			entity_message(target, EM_ACTIVATE, NULL);
		}
	}
}

static void touch(entity_t *self, entity_t *other) {
	entity_kill(other);
	self->proj_recv.num_received++;
	if (self->proj_recv.num_received == 7) {
		// TODO: Only once?
		notify_targets(self, other);
	}
}

entity_vtab_t entity_vtab_proj_recv = {
    .load = load,
    .init = init,
    .settings = settings,
    .draw = draw,
    .damage = damage,
    .touch = touch,
};
