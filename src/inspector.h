#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "high_impact.h"
#include <stdint.h>

typedef struct inspector_actions_t {
	uint8_t toggle;

	uint8_t left;
	uint8_t right;
	uint8_t up;
	uint8_t down;

	uint8_t select;
	uint8_t drag;

	uint8_t zoom_in;
	uint8_t zoom_out;
} inspector_actions_t;

typedef struct inspector_setup_t {
	camera_t *cam;
	entity_ref_t player;
	inspector_actions_t actions;
	font_t *font;
} inspector_setup_t;


// Initialize the inspector
void inspector_init(inspector_setup_t setup);
// Return true if inspector is currently active
bool inspector_active(void);
// Update the inspector
void inspector_update(void);
// Draw the inspector
void inspector_draw(void);

#endif // INSPECTOR_H

// #define INSPECTOR_IMPL
#ifdef INSPECTOR_IMPL

// Radius for finding entities under the cursor. Needs to be bigger than the biggest entity to select/hover to be reliable;
const int SEL_RADIUS = 48;
// Camera movement speed.
const float CAMERA_SPEED = 5.0;
// Min zoom level, max scale.
const int MAX_SCALE = 3;

static inspector_setup_t s;
static bool ready = false;
static bool active = false;
static bool dragging = false;
static vec2_t drag_pos = {0};
static int scale = 1;
entity_list_t hovered_entities = {0};
entity_ref_t selected_entity = {0};

vec2_t print_pos = vec2(2, 2);
#define STR_BUFLEN 1024
static char str_buf[STR_BUFLEN];

static void inspect_entity(void);

void inspector_init(inspector_setup_t setup) {
	s = setup;
	error_if(!setup.cam, "inspector_init(): camera is null");
	error_if(!setup.font, "inspector_init(): font is null");
	ready = true;
}

bool inspector_active(void) {
	return active;
}

void inspector_update(void) {
	error_if(!ready, "inspector not initialized");
	bool last_active = active;
	if (input_pressed(s.actions.toggle)) {
		active = !active;
	}
	if (!last_active && active) {
		// Inspector toggled on
		camera_unfollow(s.cam);
	} else if (last_active && !active) {
		// Inspector toggled off
		camera_follow(s.cam, s.player, true);
	}
	if (!active) {
		return;
	}

	// Move with actions/buttons
	vec2_t input_vec = {
	    .x = -input_state(s.actions.left) + input_state(s.actions.right),
	    .y = -input_state(s.actions.up) + input_state(s.actions.down),
	};
	input_vec = vec2_mulf(input_vec, CAMERA_SPEED);
	vec2_t target_pos = vec2_add(s.cam->pos, input_vec);

	// Zooming
	if (input_pressed(s.actions.zoom_in)) {
		scale = max(1, scale - 1);
		render_set_zoom(scale);
	}
	if (input_pressed(s.actions.zoom_out)) {
		scale = min(MAX_SCALE, scale + 1);
		render_set_zoom(scale);
	}

	// Dragging with mouse
	const uint8_t drag_action = s.actions.drag;
	if (input_pressed(drag_action)) {
		dragging = true;
		vec2_t mouse_pos = vec2_divf(input_mouse_pos(), render_get_scale());
		drag_pos = mouse_pos;
	} else if (input_released(drag_action)) {
		dragging = false;
	}
	if (dragging) {
		vec2_t mouse_pos = vec2_divf(input_mouse_pos(), render_get_scale());
		target_pos = vec2_add(target_pos, vec2_sub(drag_pos, mouse_pos));
		drag_pos = mouse_pos;
	}

	// TODO: Handle overdrag, set engine.view_port?
	camera_set(s.cam, target_pos);

	// Find entities at mouse
	vec2_t top_left = engine.viewport;
	vec2_t mouse_pos = vec2_divf(input_mouse_pos(), render_get_scale());
	vec2_t pointer = vec2_add(top_left, mouse_pos);
	hovered_entities = entities_by_location(pointer, SEL_RADIUS, ENTITY_TYPE_NONE, NULL);
}

void inspector_draw(void) {
	print_pos = vec2(2, 2);
	error_if(!ready, "inspector not initialized");
	if (!active) {
		return;
	}
	font_draw(s.font, print_pos, "Inspector active", FONT_ALIGN_LEFT);
	print_pos.y += s.font->line_height;
	font_draw(s.font, print_pos,
	          str_format("total: %.2fms, update: %.2fms, draw: %.2fms\n"
	                     "draw calls: %d, entities: %d, checks: %d",
	                     engine.perf.total * 1000, engine.perf.update * 1000, engine.perf.draw * 1000,
	                     engine.perf.draw_calls, engine.perf.entities, engine.perf.checks),
	          FONT_ALIGN_LEFT);
	print_pos.y += s.font->line_height * 2;


	vec2_t top_left = engine.viewport;
	vec2_t mouse_pos = vec2_divf(input_mouse_pos(), render_get_scale());
	vec2_t mouse_world = vec2_add(top_left, mouse_pos);

	// entity_t *player_ent = entity_by_ref(s.player);
	// const char *dbg_str =
	//     str_format("tl.x: %f, tl.y: %f\nmp: %f %f, p: %f %f\npl: %f %f", top_left.x, top_left.y, mouse_pos.x,
	//                mouse_pos.y, mouse_world.x, mouse_world.y, player_ent->pos.x, player_ent->pos.y);
	// font_draw(g.font, vec2(2, RENDER_HEIGHT - 32), dbg_str, FONT_ALIGN_LEFT);

	// DBG: Draws selection radius
	// render_draw(vec2_sub(mouse_pos, vec2(SEL_RADIUS, SEL_RADIUS)), vec2(SEL_RADIUS * 2, SEL_RADIUS * 2),
	// RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0), rgba(255, 0, 0, 60));

	// Draws mouse cursor in fullscreen
	if (platform_get_fullscreen()) {
		render_draw(vec2_sub(mouse_pos, vec2(0, 0)), vec2(4, 4), RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0),
		            rgba(255, 0, 0, 200));
	}

	for (int i = 0; i < engine.background_maps_len; i++) {
		map_t *map = engine.background_maps[i];
		int tile = map_tile_at_px(map, mouse_world);
		if (tile) {
			snprintf(str_buf, STR_BUFLEN, "map[%s]: %03d", map->name, tile);
			font_draw(s.font, print_pos, str_buf, FONT_ALIGN_LEFT);

			print_pos.y += s.font->line_height;
		}

	}

	vec2_t draw_pos = mouse_pos;
	for (int i = 0; i < hovered_entities.len; i++) {
		// Skip entities not over cursor
		entity_ref_t ent_ref = hovered_entities.entities[i];
		entity_t *ent = entity_by_ref(ent_ref);
		vec2_t ent_tl = ent->pos;
		vec2_t ent_br = vec2_add(ent->pos, ent->size);
		bool touching = mouse_world.x >= ent_tl.x && mouse_world.x <= ent_br.x && mouse_world.y >= ent_tl.y &&
		                mouse_world.y <= ent_br.y;
		if (!touching) {
			continue;
		}

		const char *tn = entity_type_name(ent->type);
		char *hover_str = str_format("%s", tn);
		if (input_pressed(s.actions.select)) {
			selected_entity = ent_ref;
		}
		font_draw(s.font, draw_pos, hover_str, FONT_ALIGN_LEFT);
		draw_pos.y += 8;
	}
	inspect_entity();
}

// Draw info about selected entity
static void inspect_entity(void) {
	if (selected_entity.id == 0 && selected_entity.index == 0) {
		return;
	}
	entity_t *ent = entity_by_ref(selected_entity);
	if (ent == NULL) {
		selected_entity = entity_ref_none();
		return;
	}

	snprintf(str_buf, STR_BUFLEN, "id: %03d | type: %s\npos: %.2f %.2f", ent->id, entity_type_name(ent->type),
	         ent->pos.x, ent->pos.y);
	font_draw(s.font, print_pos, str_buf, FONT_ALIGN_LEFT);
}

#endif // INSPECTOR_IMPL