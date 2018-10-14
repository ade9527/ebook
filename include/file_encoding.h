#ifndef __FILE_ENCODING_H
#define __FILE_ENCODING_H



enum encoding_type {
	ENCODING_UTF8 = 0x11;
	ENCODING_UTF16_BIG;
	ENCODING_UTF16_L;
};


struct encoding_file {
	enum encoding_type type;
	int fd;
	unsigned char *start;
	unsigned int size;
	struct encoding_operations *opr;
};

struct encoding_operations {
	enum encoding_type type;
	int (*is_support)(const char **string, int nbytes);
	int (*get_code)(const char **string, int nbytes);
};

int encoding_open(struct encoding_file *file, const char *name);
int encoding_close(struct encoding_file *file);
int encoding_get_code(struct encoding_file *file, const char **string, int bytes);
#endif
