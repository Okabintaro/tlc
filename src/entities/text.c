#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 255, 255);

static void init(entity_t *self) {
	self->size = vec2(16, 16);
}

static void draw(entity_t *self, vec2_t viewport) {
	vec2_t pos = vec2_sub(self->pos, viewport);

	if (g.game_won) {
		font_draw(g.font, pos,
		          "You found the color green!\n"
		          "There is still hope for the world.\n"
		          "Thanks for playing!",
		          FONT_ALIGN_CENTER);
	}
	// font_draw(g.font, pos, "This is a test...\nLine2\nLine3!!!", FONT_ALIGN_CENTER);
	// render_draw(pos, self->size, RENDER_NO_TEXTURE, vec2(0, 0), vec2(0, 0), rgba(255, 255, 255, 128));
}


entity_vtab_t entity_vtab_text = {
    .draw = draw,
    .init = init,
};
