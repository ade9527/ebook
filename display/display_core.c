#include <display.h>
#include <stdlib.h>
#include <string.h>

#include <debug.h>


extern struct display_method fb_method;

struct display_method *display_method_list[] = {
	&fb_method,
};


static struct display_method * display_method_match(enum display_type type)
{
	struct display_method *method;
	int method_count = sizeof(display_method_list) / \
			sizeof(struct display_method*);

	for (int i = 0; i < method_count; i++) {
		if (display_method_list[i]->type == type)
			return display_method_list[i];
	}
	return NULL;
}


/*
 * open dispaly dev
 */
int display_open(struct display_dev *dev)
{
	struct display_method *method;

	/* match the right method */
	method = display_method_match(dev->type);
	if (!method)
		return -1;

	dev->method = method;
	if (dev->method->open(dev) < 0) {
		return -2;
	}

	return 0;

}

int display_close(struct display_dev *dev)
{
	return dev->method->close(dev);
}

// 要不要返回错误? 超出范围的直接不显示?
void display_set_pixel(struct display_dev *dev, int x, int y, unsigned int color)
{
	if (x < dev->xres && y < dev->yres)
		dev->method->set_pixel(dev, x, y, color);
}

void display_clean_screen(struct display_dev *dev, unsigned int color)
{
	if (!dev->method->clean_screen) {
		dev->method->clean_screen(dev, color);
		return;
	}
	
	for (int x = 0; i < dev->xres; x++) {
		for (int y = 0; i < dev->yres; y++)
			dev->method->set_pixel(dev, x, y, color);
	}
}


