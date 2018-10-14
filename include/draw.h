#ifndef __DRAW_H
#define __DRAW_H

struct page {
	int num;
	char *start;
	struct page *next;
};

struct file_draw {
	struct encoding_file file;
	char *path;
	struct font font;
	struct display_dev display;

	struct page *pages_head;
	struct page *current_page;
};

#endif
