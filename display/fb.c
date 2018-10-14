#include <display.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int fb_open(struct display_dev *dev)
{
	struct fb_var_screeninfo var;
	int ret;

	dev->fd = open(dev->name, O_RDWR);
	if (dev->fd < 0) {
		return dev->fd;
	}

	/* get screen info */
	ret = ioctl(dev->fd, FBIOGET_VSCREENINFO, &var)
	if(ret < 0) {
		goto err_close;
	}
	dev->xres = var.xres;
	dev->yres = var.yres;
	dev->pixel_bytes = var.bits_per_pixel / 8;
	dev->line_bytes = dev->xres * dev->line_bytes;
	dev->fbmem = mmap(NULL, dev->xres * dev->yres * dev->pixel_bytes,
				PROT_READ|PROT_WRITE, MAP_SHARED,
				dev->fd, 0);
	if (dev->fbmem == MAP_FAILED) {
		ret = -1
		goto err_close;
	}

	return 0;
err_close:
	close(dev->fd);
	return ret;
}

int fb_close(struct display_dev *dev)
{
	munmap(dev->fbmem, dev->xres * dev->yres *dev->pixel_bytes);
	return close(dev->fd);
}

void fb_set_pixel(struct display_dev *dev, int x, int y, unsigned int color)
{
	unsigned char *pixel_8 = dev->fbmem + dev->yres * line_bytes
				+ dev->xres * dev->pixel_bytes;
	unsigned short *pixel_16 = (unsigned short *)pixel_8;
	unsigned int *pixel_32 = (unsigned int *)pixel_8;

	unsigned int red, green, blue;

	switch (dev->pixel_bytes) {
	case 1: /*需要查表?*/
		*pixel_8 = color;
		break;
	case 2:
		/* 565 */
		red = (color >> 16) & 0xff;
		green = (color >> 8) & 0xff;
		blue = color & 0xff;
		*pixel_16 = ((red >> 3) << 11) | ((green >> 2) << 5) |
				(blue >> 2);
		break;
	case 4:
		*pixel_32 = color;
		break;
	default:
		printf("Can't support this screen, bits_per_pixel:%d\n",
				dev->pixel_bytes * 8);
	}
}

void fb_clean_screen(struct display_dev *dev, unsigned int color)
{
	unsigned int red, green, blue;

	switch (dev->pixel_bytes) {
	case 1:
		memset(dev->fbmem, color, dev->line_bytes * dev->yres);
		break;
	case 2:
		/* 565 */
		red = (color >> 16) & 0xff;
		green = (color >> 8) & 0xff;
		blue = color & 0xff;
		color = ((red >> 3) << 11) | ((green >> 2) << 5) |
				(blue >> 2);
		for (unsigned short *pen = dev->fbmem;
				pen < fbmem + dev->line_bytes * dev->yres;
				pen++) {
			*pen = color;
		}
		break;
	case 4:
		for (unsigned int *pen = dev->fbmem; pen < fbmem
				+ dev->line_bytes * dev->yres;
				pen++) {
			*pen = color;
		}
		break;
	default:
		printf("Can't support this screen, bits_per_pixel:%d\n",
				dev->pixel_bytes * 8);
	}

}



struct display_method fb_method = {
	.type = DISPLAY_FB,
	.open = fb_open,
	.close = fb_close,
	.clean_screen = fb_clean_screen,
	.set_pixel = fb_set_pixel,
};

