#include <font.h>
#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct freetype_data {
	FT_Face face;
	FT_Matrix matrix;
	FT_Library library;
	FT_Vector pen;
	double angle; /* 字的角度 */
};

int freetype_init(struct font *font)
{
	int ret = 0;
	FT_Error error;
	struct freetype_data *data;
	
	data = malloc(sizeof(*data));
	if (!data)
		return -ENOMEM;
	memset(data, 0, sizeof(*data));

	font->private_data;


/* init and set freetype */
	error = FT_Init_FreeType(&data->library);
	if (error != 0) {
		printf("freetype init failed, error:%d\n", error);
		ret = -1;
		goto err_free;
	}
	error = FT_New_Face(data->library, font->path, 0, &data->face);
	if (error != 0) {
		printf("freetype new face failed, error:%d\n", error);
		ret = -1;
		goto err_done_freetype;
	}

	data->pen.x = 0;
	data->pen.y = 0;

	data->angle = (0.0 / 360) * 3.14159 * 2;

	/* set up matrix */
	data->matrix.xx = (FT_Fixed)( cos(data->angle) * 0x10000L );
	data->matrix.xy = (FT_Fixed)(-sin(data->angle) * 0x10000L );
	data->matrix.yx = (FT_Fixed)( sin(data->angle) * 0x10000L );
	data->matrix.yy = (FT_Fixed)( cos(data->angle) * 0x10000L );

	return ret;
err_done_freetype:
	FT_Done_FreeType(data->library);
err_free:
	free(data);
	return ret;
}
void freetype_exit(struct font *font)
{
	struct freetype_data *data = font->private_data;

	FT_Done_Face(data->face);
	FT_Done_FreeType(data->library);
	free(font->private_data);
	font->private_data = NULL;
}

int freetype_get_bitmap(struct font *font, unsigned int code,
			struct font_bitmap *bitmap)
{
	FT_Error error;
	struct freetype_data *data = font->private_data;
	FT_GlyphSlot slot;

	FT_Set_Transform(data->face, &data->matrix, &data->pen);
	error = FT_Load_Char(data->face, code, FT_LOAD_RENDER);
	if (error) {
		return -1;
	}
	slot = data->face->glyph;
	bitmap->buffer = slot->bitmap.buffer;
	bitmap->width = slot->bitmap.width;
	bitmap->rows = slot->bitmap.rows;
	bitmap->advance_x = slot->advance.x;
	bitmap->advance_y = slot->advance.y;
	bitmap->vert_advance = (slot->linearVertAdvance + 65535) / 65536;
	bitmap->hori_advance = (slot->linearHoriAdvance + 65535) / 65536;
	/* 应为原点设置为(0,0) */
	bitmap->left = slot->bitmap_left; /**/
	bitmap->top = -slot->bitmap_top;
	bitmap->x = bitmap->base_x + bitmap->left;
	bitmap->y = bitmap->base_y + bitmap->top;
	bitmap->bpp = 8;
	bitmap->pitch = bitmap->width * 1;

	return 0;
}

int freetype_put_bitmap(struct font_bitmap *bitmap)
{
	/*释放 bitmap 的buffer?*/
FT_Done_Glyph
	return 0;
}

int freetype_setsize(struct font *font)
{
	FT_Error error;
	struct freetype_data *data = font->private_data;

	error = FT_Set_Char_Size(data->face, 0, font->size*64, 0, font->dpi);
	if (error != 0) {
		printf("freetype set char size failed, error:%d\n", error);
		return -1;
	}

	error = FT_Load_Char(font->face, 'A', FT_LOAD_RENDER);
	if (error != 0) {
		printf("freetype set size ,get advance failed, error:%d\n",
				error);
		font->vert_advance = 0;
		font->hori_advance = 0;
		return -2;
	}
	font->vert_advance = (font->face->glyph->linearVertAdvance + 65535)
					/ 65536;
	font->hori_advance = (font->face->glyph->linearHoriAdvance + 65535)
					/ 65536;
	return 0;
}







struct font_operations freetype_opr = {
	.font_type	= FONT_FREETYPE;
	.init		= freetype_init;
	.exit		= freetype_exit;
	.get_bitmap	= freetype_get_bitmap;
	.put_bitmap	= freetype_put_bitmap;
	.set_size	= freetype_setsize;
};

