#ifndef __DISPLAY_H
#define __DISPLAY_H

enum display_type {
	DISPLAY_FB = 1;
};

struct display_method {
	enum display_type type;
	void (*set_pixel)(struct display_dev *dev,int x, int y,
				unsigned int color);/*color format is 0xRRGGBB*/
	void (*clean_screen)(struct display_dev *dev, unsigned int color);
	int (*open)(struct display_dev *);
	int (*close)(struct display_dev *);
};

struct display_dev {
	enum display_type type;
	const char *name;
	int fd;
	int xres;
	int yres;
	int line_bytes;
	int pixel_bytes;
	void *fbmem;
	struct display_method *method;
};


int display_open(struct display_dev *dev);
int display_close(struct display_dev *dev);
void display_set_pixel(struct display_dev *dev, int x, int y, unsigned int color);
void display_clean_screen(struct display_dev *dev, unsigned int color);

#endif
