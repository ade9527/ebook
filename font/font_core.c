#include <font.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

extern struct font_operations freetype_opr;

static struct font_operations *font_operations_list[] = {
	&freetype_opr,
};

struct font_operations *font_match(enum font_type type)
{
	int font_opr_count = sizeof(font_operations_list) / \
			     sizeof(struct font_operations*);

	for (int i = 0; i < font_opr_count; i++) {
		if (font_operations_list[i]->type == type) {
			return font_operations_list[i];
		}
	}
	return NULL;
}

/*
 * match and init font;
 */
int font_init(struct font *font)
{
	font->opr = font_match(font->type);
	if (font == NULL)
		return ERR_FONT_NO_OPR;
	return font->opr->init(font);
}

int font_exit(struct font *font)
{
	if (font->opr->exit) {
		font->opr->exit(font);
	}
	return 0;
}

/* set font size */
/* size (mm)*/
int set_font_size(struct font *font, unsigned int size)
{
	font->size = size;
	if (font->opr->set_size)
		return font->opr->set_size(font);
	return 0;
}
/* get font bitmap */
int font_get_bitmap(struct font *font, unsigned int code,
		struct font_bitmap *bitmap)
{
	return font->opr->get_bitmap(font, code, bitmap);
}

int font_put_bitmap(struct font_bitmap *bitmap)
{
	if (bitmap->font->opr->put_bitmap)
		bitmap->font->opr->put_bitmap(bitmap);
	return 0;
}

