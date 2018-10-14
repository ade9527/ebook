#include <stdio.h>

void print_usage(char *name)
{
	printf("Usage: %s [size] [font_file] [file]\n", name);

}

int main(int argc, char *argv[])
{
	int size;
	char *font;
	char *file;
	int ret;

	if (argc != 4)
		print_usage(argv[0]);

	size = atoi(argv[1]);
	font = argv[2];
	file = argv[3];

	ret = show_file();

	return 0;
}


int show_file(int size, char *font, char *file)
{
	open_file()
}
