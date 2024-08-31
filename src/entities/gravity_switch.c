#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

/// React on ACTIVATE and DEACTIVATE messages
static void message(entity_t *self, entity_message_t message, void *data) {
	if (message == EM_ACTIVATE) {
		engine.gravity = -240;
	} else if (message == EM_DEACTIVATE) {
		engine.gravity = 240;
	}
}

entity_vtab_t entity_vtab_gravity_switch = {
    .message = message,
};
