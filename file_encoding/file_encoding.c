#include <fcntl.h>
#include <unistd.h>
#include <file_encoding.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

extern struct encoding_operations utf8_encoding_opr;

/*排列顺序*/
static struct encoding_operations *encoding_opr_list[] = {

	&utf8_encoding_opr, //默认的方法,所有文件都会匹配成功,一定要放最后
	NULL,
};

//struct encoding_file *
int encoding_open(struct encoding_file *file, const char *name)
{
	int fd;
	struct encoding_operations *opr;
	struct stat file_stat;
	int ret = 0;
	unsigned char *start;

	fd = open(name, O_REONLY);
	if (fd < 0) {
		return NULL;
	}
	ret = fstat(fd, &file_stat);
	if (ret < 0) {
		goto err_close;
	}

	/* 文件映射 */
	start = mmap(NULL, file_stat.st_size, PROT_READ, 0, fd, 0);
	if (start == MAP_FAILED) {
		ret = -1;
		goto err_close;
	}

	for (opr = encoding_opr_list; opr; opr++) {
		if (opr->is_support(start, file_stat.st_size)) {
			file->fd = fd;
			file->start = start;
			file->size = file_stat.st_size;
			file->type = opr->type;
			file->opr = opr;
			return 0;
		}
	}
	ret = -1;
	munmap(start,file_stat.st_size);
err_close:
	close(fd);
	return ret;
}

int encoding_close(struct encoding_file *file)
{
	munmap(file->start, file->size);
	close(file->fd);
	return 0;
}


int get_code(struct encoding_file *file, const char **string, int bytes)
{
	return file->opr->get_code(string, bytes);
}



/**/
int utf8_is_support(const char **string, int nbytes)
{
	if (nbytes >= 3 && !strncmp(*string, "\xef\xbb\xbf", 3)) {
		*string += 3;
	}
	return 1;
}

/*
 * get a unicode from uft8
 * string: address for utf8 char string,return the address after a unicode;
 * bytes: *string len
 * return one unicode
 */
unsigned int utf8_get_unicode(const char **string, int nbytes)
{
	int clo = 0; /* count leading one */
	char c;
	unsigned int unicode = 0;
	int len = 0;

	if (bytes  == 0)
		return 0;

	for(c = **string; c & 0x80; c = c << 1) {
		clo++;
	}

	c = **string;
	unicode = c << clo >> clo;
	*string++;

	if (clo == 0) {
		return unicode;
	} else if (clo > 6) { /* 大于 6字节返回*/
		return unicode;
	}

	clo--;
	for (; clo; clo--) {
		c = **string;
		if (c & 0x80 && !(c & 0x40)) {
			unicode = (unicode << 6) | (c & 0x3f);
			*string++;
		} else {
			return unicode;
		}
	}
	return unicode;
}

struct encoding_operations utf8_encoding_opr = {
	.type = ENCODING_UTF8,
	.is_support = utf8_is_support,
	.get_code = utf8_get_unicode,
};
