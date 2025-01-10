#include <stdio.h>

int main(int argc, char **argv) {
	int a = 0xffffffff;
	unsigned char *p;
	p = &a;
	printf("%x\n", *p);
}
