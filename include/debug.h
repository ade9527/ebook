#ifndef __DEBUG_H
#define __DEBUG_H

#define ebook_err(...) /
do { /
	printf("[ERROR ]%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); /
	printf(__VA_ARGS__); /
}while(0)


#ifdef EBOOK_DEBUG
#define ebook_debug(...) /
do { /
	printf("[INFO ]: "); /
	printf(__VA_ARGS__); /
} while(0)
#else
#define ebook_debug(...)
#endif


enum err_code {
	DRAW_ERR = 1;
	ERR_FONT_NO_OPR;
	ERR_PAGE_INVALID;
};


#endif
