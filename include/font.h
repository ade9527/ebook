#ifndef __FONT_H
#define __FONT_H

enum font_type {
	FONT_ASCII = 1,
	FONT_FREETYPE
};

struct font {
	enum font_type type;
	const char *path;
	unsigned int size;
	unsigned int dpi;
	unsigned int vert_advance;
	unsigned int hori_advance; /* Chinses should *2 */
	void *private_data;
	struct font_operations *opr;
};

struct font_bitmap{
	unsigned char *buffer;
	int width; //buffe中描述的宽度
	int rows;  //buffer中描述的行数
	unsigned int delta_x; /*buffer的绘制点相对左上角的偏移*/
	unsigned int delta_y;
	unsigned int vert_advance; /* 字的x轴 宽 */
	unsigned int hori_advance; /* 整个字的高 相同size高相同 */
	int pitch; /* 对于单色位图表示两行像素之间的跨度单位byte*/
	int bpp; /*每个像素所占位数*/
	struct font *font;
};


struct font_operations {
	enum font_type type;
	int (*init)(struct font *font);
	void (*exit)(struct font *font);
	int (*get_bitmap)(struct font *font, unsigned int code,
				struct font_bitmap *bitmap);
	int (*put_bitmap)(struct font_bitmap *bitmap);
	int (*set_size)(struct font *font);
};


int font_init(struct font *font);
int font_exit(struct font *font);
int set_font_size(struct font *font, unsigned int size);
int font_get_bitmap(struct font *font, unsigned int code,
		struct font_bitmap *bitmap);
int font_put_bitmap(struct font_bitmap *bitmap);

#endif
