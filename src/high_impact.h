#ifndef HIGH_IMPACT_H
#define HIGH_IMPACT_H

// -----------------------------------------------------------------------------
// High Impact configuration

// These defines are ALL optional. They overwrite the defaults set by
// high_impact and configure aspects of the library

// The values here (particularly resource limits) have been dialed in to this
// particular game. Increase them as needed. Allocating a few GB and thousands
// of entities is totally fine.


#define ALLOC_SIZE (16 * 1024 * 1024)
#define ALLOC_TEMP_OBJECTS_MAX 8

#define WINDOW_TITLE "Chromatic Restoration Protocol"

#define GAME_VENDOR "oka"
#define GAME_NAME "chromatic_restoration_protocol"

#define RENDER_WIDTH 320
#define RENDER_HEIGHT 180
#define RENDER_RESIZE_MODE RENDER_RESIZE_WIDTH
#define RENDER_SCALE_MODE RENDER_SCALE_DISCRETE

#define WINDOW_WIDTH (RENDER_WIDTH * 4)
#define WINDOW_HEIGHT (RENDER_HEIGHT * 4)

#define ENTITIES_MAX 1024
#define ENTITY_MAX_SIZE 64
#define ENTITY_MIN_BOUNCE_VELOCITY 10

#define SOUND_MAX_UNCOMPRESSED_SAMPLES (64 * 1024)
#define SOUND_MAX_SOURCES 64
#define SOUND_MAX_NODES 256

#define IMAGE_MAX_SOURCES 64

#define RENDER_ATLAS_SIZE 64
#define RENDER_ATLAS_GRID 16
#define RENDER_ATLAS_BORDER 0
#define RENDER_BUFFER_CAPACITY 2048
#define RENDER_TEXTURES_MAX 64

#define ENGINE_MAX_TICK 0.1
#define ENGINE_MAX_BACKGROUND_MAPS 4

#include "../high_impact/src/alloc.h"
#include "../high_impact/src/animation.h"
#include "../high_impact/src/camera.h"
#include "../high_impact/src/engine.h"
#include "../high_impact/src/entity_def.h"
#include "../high_impact/src/font.h"
#include "../high_impact/src/image.h"
#include "../high_impact/src/input.h"
#include "../high_impact/src/map.h"
#include "../high_impact/src/noise.h"
#include "../high_impact/src/platform.h"
#include "../high_impact/src/render.h"
#include "../high_impact/src/sound.h"
#include "../high_impact/src/trace.h"
#include "../high_impact/src/types.h"
#include "../high_impact/src/utils.h"
#include "utils.h"


// Careful, before we can include "../high_impact/src/entity.h", we need to set
// up our entity types.

// -----------------------------------------------------------------------------
// Entities

// This X-Macro will get called by ENTITY_DEFINE() and defines the entity_type_t
// enum, as well as the entity_vtab_t for each listed type.

// Each TYPE() must have a TYPE_NAME that can be used with e.g.
// entity_spawn(ENTITY_TYPE_PLAYER, ...), and a SHORT_NAME that will can be used
// for the stringified name with e.g. entity_type_by_name("player")

// Every entity is also expected to have a global entity_vtab_t with the name
// entity_vtab_SHORT_NAME, e.g. entity_vtab_t entity_vtab_player = {...};

#define ENTITY_TYPES(TYPE)                           \
	TYPE(ENTITY_TYPE_PLAYER, player)                 \
	TYPE(ENTITY_TYPE_PROJECTILE, projectile)         \
	TYPE(ENTITY_TYPE_SMAL_PROJ, smal_proj)           \
	TYPE(ENTITY_TYPE_PROJ_EMITTER, proj_emitter)     \
	TYPE(ENTITY_TYPE_PROJ_RECV, proj_recv)           \
	TYPE(ENTITY_TYPE_PROJ_PRISM, proj_prism)         \
	TYPE(ENTITY_TYPE_TRIGGER, trigger)               \
	TYPE(ENTITY_TYPE_LEVER, lever)                   \
	TYPE(ENTITY_TYPE_BUTTON, button)                 \
	TYPE(ENTITY_TYPE_DOOR, door)                     \
	TYPE(ENTITY_TYPE_GRAVITY_SWITCH, gravity_switch) \
	TYPE(ENTITY_TYPE_BAD_BOT, bad_bot)               \
	TYPE(ENTITY_TYPE_PARTICLE, particle)             \
	TYPE(ENTITY_TYPE_LEVEL_CHANGE, level_change)     \
	TYPE(ENTITY_TYPE_VOID, void)

// All entity types share the same struct. Calling ENTITY_DEFINE() defines that
// struct with the fields required by high_impact and the additional fields
// specified here.

ENTITY_DEFINE(
    // Entity private data
    union {
	    struct {
	    } gravity_switch;

	    struct {
		    char *path;
	    } level_change;

	    struct {
		    anim_def_t *anim_hit;
		    bool has_hit;
		    bool flip;
	    } projectile;

	    struct {
		    anim_def_t *anim_hit;
		    bool has_hit;
	    } smal_proj;


	    struct {
		    float high_jump_time;
		    float idle_time;
		    bool flip;
		    bool flip_y;
		    bool can_jump;
		    bool is_idle;
		    float can_interact_time;
	    } player;

	    struct {
		    float life_time;
		    float fade_time;
	    } particle;

	    struct {
		    entity_list_t targets;
		    float delay;
		    float delay_time;
		    bool can_fire;
	    } trigger;

	    struct {
		    entity_list_t targets;
		    float delay;
		    float delay_time;
		    bool can_fire;
		    bool is_on;
		    bool flip;
	    } lever;

	    struct {
		    entity_list_t targets;
		    float delay;
		    float delay_time;
		    bool can_fire;
		    /*bool is_on;*/
	    } button;

	    struct {
		    bool is_open;
	    } door;

	    struct {
		    bool in_jump;
		    bool seen_player;
		    float jump_timer;
	    } bad_bot;

	    struct {
		    direction_t direction;
		    bool is_active;
		    float cooldown_timer;
	    } proj_prism;

	    struct {
		    entity_list_t targets;
		    bool is_active;
		    float cooldown_timer;
		    int num_received;
		    int num_needed;
	    } proj_recv;

	    struct {
		    direction_t direction;
		    bool is_active;
		    float timer;
	    } proj_emitter;
    };);

// The entity_message_t is used with the entity_message() function. You can
// extend this as you wish.

typedef enum {
	EM_INVALID,
	EM_ACTIVATE,
	EM_DEACTIVATE,
	EM_ROTATE_RIGHT,
} entity_message_t;

// Now that we have all the prerequisites, we can include entity.h
#include "../high_impact/src/entity.h"

#endif
