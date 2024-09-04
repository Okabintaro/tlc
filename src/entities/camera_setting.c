#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 0, 32);

static void message(entity_t *self, entity_message_t message, void *data) {}

static void settings(entity_t *self, json_t *settings) {
	vec2_t offset = vec2(0, 0);
	json_t *offset_x = json_value_for_key(settings, "offset_x");
	offset.x = offset_x ? json_number(offset_x) : 0;

	json_t *offset_y = json_value_for_key(settings, "offset_y");
	offset.y = offset_y ? json_number(offset_y) : 0;
	self->camera_setting.offset = offset;

	json_t *move = json_value_for_key(settings, "move_on");
	self->camera_setting.move = move ? json_bool(move) : true;
}

static void trigger(entity_t *self, entity_t *other) {
	g.camera.offset = self->camera_setting.offset;
}

entity_vtab_t entity_vtab_camera_setting = {
    .trigger = trigger,
    .message = message,
    .settings = settings,
};
