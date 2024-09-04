#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 255, 255);

static void init(entity_t *self) {
	self->size = vec2(16, 16);
}

static void draw(entity_t *self, vec2_t viewport) {
	vec2_t pos = vec2_sub(self->pos, viewport);

	// TODO: Load from json and/or entity data?
	if (g.game_won) {
		font_draw(g.font, pos,
		          "You found the color green!\n"
		          "There is still hope for the world.\n"
		          "Thanks for playing!",
		          FONT_ALIGN_CENTER);
	}
}


entity_vtab_t entity_vtab_text = {
    .draw = draw,
    .init = init,
};
