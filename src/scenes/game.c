#include "../main.h"

static sound_t *music;
static entity_ref_t last_checkpoint = entity_ref_none();
static vec2_t initial_spawn_pos = vec2(0, 0);
static char level_path[64] = {0};
// static camera_t camera;

static void init(void) {
	engine_load_level(level_path);
	engine.gravity = 240;

	g.camera.speed = 5;
	g.camera.min_vel = 1;

	last_checkpoint = entity_ref_none();

	entity_list_t players = entities_by_type(ENTITY_TYPE_PLAYER);

	if (players.len > 0) {
		g.player = players.entities[0];
		camera_follow(&g.camera, g.player, true);
		entity_t *player_ent = entity_by_ref(g.player);
		initial_spawn_pos = player_ent->pos;
	}

	sound_set_time(g.music, 0);
	sound_set_volume(g.music, 0.4);
	sound_unpause(g.music);
}

static void cleanup(void) {
	g.level_time = engine.time;
}

static void update(void) {
	if (input_pressed(A_DBG_BB)) {
		g.draw_bb = !g.draw_bb;
	}

	if (input_pressed(A_PAUSE)) {
		g.paused = !g.paused;
	}

	if (g.paused) {
		return;
	}
	scene_base_update();
	camera_update(&g.camera);
}

static void draw(void) {
	scene_base_draw();

	if (g.paused) {
		font_draw(g.font, vec2(RENDER_WIDTH / 2, RENDER_HEIGHT - 16), "Game Paused. Press START/ESC to resume.",
		          FONT_ALIGN_CENTER);
	}

	// Draw some debug info...
	if (g.draw_bb) {
		font_draw(g.font, vec2(2, 2),
		          str_format("total: %.2fms, update: %.2fms, draw: %.2fms\n"
		                     "draw calls: %d, entities: %d, checks: %d",
		                     engine.perf.total * 1000, engine.perf.update * 1000, engine.perf.draw * 1000,
		                     engine.perf.draw_calls, engine.perf.entities, engine.perf.checks),
		          FONT_ALIGN_LEFT);
	}
	// font_draw(g.font, vec2(2, 2), str_format("camera: %f %f\n", camera.pos.x, camera.pos.y), FONT_ALIGN_LEFT);
	// entity_t *player_ent = entity_by_ref(g.player);
	// font_draw(g.font, vec2(2, 2 + 16), str_format("player: %f %f\n", player_ent->pos.x, player_ent->pos.y),
	//           FONT_ALIGN_LEFT);
}

entity_t *game_spawn_particle(vec2_t pos, float vel, float vel_variance, float angle, float angle_variance,
                              anim_def_t *sheet) {
	entity_t *particle = entity_spawn(ENTITY_TYPE_PARTICLE, pos);
	if (!particle) {
		return NULL;
	}

	particle->anim = anim(sheet);
	anim_goto_rand(&particle->anim);
	particle->anim.flip_x = rand_int(0, 1);
	particle->anim.flip_y = rand_int(0, 1);

	float a = rand_float(angle - angle_variance, angle + angle_variance);
	float v = rand_float(vel - vel_variance, vel + vel_variance);
	particle->vel = vec2_mulf(vec2_from_angle(a), v);
	return particle;
}

void game_set_checkpoint(entity_ref_t checkpoint) {
	last_checkpoint = checkpoint;
}

void game_respawn(void) {
	vec2_t pos = initial_spawn_pos;

	// entity_t *respawn_pod = entity_by_ref(last_checkpoint);
	// if (respawn_pod) {
	// 	pos = vec2_add(respawn_pod->pos, vec2(11, 0));
	// 	entity_message(respawn_pod, EM_ACTIVATE, NULL);
	// }

	entity_t *player = entity_spawn(ENTITY_TYPE_PLAYER, pos);
	g.player = entity_ref(player);
	camera_follow(&g.camera, g.player, false);
}

void game_set_level_path(char *path) {
	error_if(strlen(path) >= sizeof(level_path) - 1, "level path too long");
	strcpy(level_path, path);
}

scene_t scene_game = {.init = init, .update = update, .draw = draw, .cleanup = cleanup};
