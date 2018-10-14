/*
 * This file is
 *
 *
 */
#include <stdio.h>

#include <font.h>
#include <display.h>
#include <file_encoding.h>
#include <draw.h>
#include <debug.h>


/*
 * initialization
 *
 * @param
 *        draw: should set path, display.type, display.name,font.type,
 *              font.path. Optional set font.size;
 * @return
 *        success return 0;
 *        falied return negative;
 */
int draw_init(struct file_draw *draw)
{
	int ret;

	/* file open */
	ret = encoding_open(&draw->file, path);
	if (ret != 0) {
		ebook_err("Open file failed.err:%d\n", ret);
		goto err_file;
	}

	/* display init */
	ret = display_open(&draw->display);
	if (ret < 0) {
		ebook_err("Display init failed.err:%d\n", ret);
		goto err_display;
	}


	/* font init */
	ret = font_init(&draw->font);
	if (ret < 0) {
		ebook_err("font init failed. err:%d\n", ret);
		goto err_font;
	}
	if (draw->font.hori_advance > draw->display.xres ||
		draw->font.vert_advance > draw->display.yres) {
		ebook_err("The font set is too large\n");
		goto err_font_size;
	}


	
	return 0;

err_font_size:
	font_exit(&draw->font);
err_font:
	display_close(&draw->display);
err_display:
	encoding_close(&draw->file);
err_file:
	return ret;
}

int draw_exit(struct file_draw *draw)
{
	font_exit(&draw->font);
	display_close(&draw->display);
	encoding_close(&draw->file);
}


struct page *get_page(struct draw *draw, int page_num)
{
	struct page *page;
	
	if (page_num < 0)
		return NULL;
	for (page = draw->pages_head; page != NULL; page = page->next) {
		if (page->num == page_num)
			return page;
	}
	return NULL;
}

/*
 *
 *
 */
int pages_scan(struct file_draw *draw)
{
	unsigned char *file_start = draw->file.start;
	unsigned int file_size = draw->file.size;
	unsigned char *file_p = file_start;
	unsigned int file_len;

	unsigned int xres = draw->display.xres;
	unsigned int yres = draw->display.yres;

	unsigned int x = 0;
	unsigned int y = 0;

	unsigned int font_vert = draw->font.vert_advance;
	unsigned int font_hori = draw->font.hori_advance;

	struct font_bitmap bitmap;

	unsigned int encode;
	int ret;
	unsigned int page_num = 0;
	struct page **page_tail = &draw->pages_head;

	/* check draw value */
	if (draw->pages_head != NULL) {
		ebook_err("err: pages_head already have value!\n");
		return -1;
	}


	/* 遇到 \n 换行*/
	x = 0;
	y = font_vert;
	for (page_num = 0; file_p < file_start + file_size; page_num++) {
		struct page *page;

		page = (struct page*)malloc(sizeof(*page));
		if (page == NULL) {
			ebook_err("mem alloc err\n");
			break;
		}
		page->num = page_num;
		page->start = file_p;
		page->next = NULL;
		*page_tail = page;
		page_tail = &page->next;

		x = 0;
		y = font_vert;
		while (y <= yres) {
			file_len = file_size - (file_p - file_start);
			if (file_len <= 0)
				break;
			encode = encoding_get_code(&draw->font, file_p, file_len);
			if (encode == '\0' || encode == '\r')
				continue;

			if (encode == '\n') {
				if (y + font_vert > yres)
					break;
				y += font_vert;
			}

			if (encode < 128) {
				if (x + font_hori > xres) {
					if (y + font_vert > yres)
						break;
					x = 0;
					y += font_vert;
				}
				x += font_hori;
			} else {
				if (x + font_hori * 2 > xres) {
					if (y + font_vert > yres)
						break;
					x = 0;
					y += font_vert;
				}
				x += font_hori* 2;
			}
		}
	}
	return page_num;
}

/*
 *
 * pram:
 *      x,y is font's upper left corner
 */
int show_characte(struct file_draw *draw, struct font_bitmap *bitmap,
		int x, int y)
{
	int draw_x;
	int draw_y;
	int x_max;
	int y_max;

	struct display_dev *display = &draw->display;
	int color;
	int i,j;

	/* determine the draw origin*/

	for (i = 0; draw_x < x_max; i++, draw_x++) {
		for (j = 0; draw_y < y_max; j++, draw_y++) {
			color = bitmap->buffer[j*bitmap->width + i];
			display->method->set_pixel(display, i, j, color);
		}
	}

	return 0;
}

int show_page(struct file_draw *draw, int page_num)
{
	unsigned int x = 0;
	unsigned int y = 0;

	unsigned int xres = draw->display.xres;
	unsigned int yres = draw->display.yres;

	unsigned int font_vert = draw->font.vert_advance;
	unsigned int font_hori = draw->font.hori_advance;

	struct font_bitmap bitmap;

	struct page *page;
	unsigned char *file_p;
	unsigned int len;

	unsigned int encode;

	int ret;

	page = get_page(draw, page_num);
	if (page == NULL) {
		ebook_err("jump page is not exist!\n");
		return -ERR_PAGE_INVALID;
	}
	draw->current_page = page;
	file_p = page->start;

	x = 0;
	y = 0;
	while (y + font_vert <= yres) {
		/* get encoding */
		len = draw->file.size - (file_p - draw->file.start);
		if (len <= 0) {
			/* 文件结尾 */
			break;
		}
		encode = encoding_get_code(&draw->file, &file_p, len);

		if (encode == '\0' || encode == '\r') {
			continue;
		}
		if (encode == '\n') {
			if (y + font_vert > yres)
				break;
			y += font_vert;
			x = 0;
			continue;
		}

		/* get font bitmap */
		ret = font_get_bitmap(&draw->font, encode, &bitmap);
		if (ret < 0) {
			ebook_err("get font err, word's encode:%x\n", encode);
			continue;
		}

		if (x + bitmap.hori_advance > xres) {
			x = 0;
			y += bitmap.vert_advance;
			if (y + bitmap.vert_advance > yres)
				break; //超出屏幕范围
		}

		/* show font */
		show_characte(draw, bitmap, x, y);
		x += bitmap.hori_advance;

		/* test vertical advance */
		if (bitmap->vert_advance != font_vert) {
			ebook_err("vertical advance is different: "
				"bitmap vert:%d,font vert:%d\n",
				bitmap->vert_advance, font_vert);
		}
	}
	return 0;
}

int next_page(struct draw *draw)
{
	return whow_page(draw, draw->current_page + 1);
}

int per_page(struct draw *draw)
{
	if (draw->current_page = 0)
		return -ERR_PAGE_INVALID;
	show_page(draw, draw->current_page - 1);
	return 0;
}


#if 0
/* 暂不实现 */
int set_font_size()
{

}
#endif

