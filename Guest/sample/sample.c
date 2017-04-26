#include <s2e.h>

void foo(char *a)
{
	char b[40];
	strcpy(b, a);
}

int main(int argc, char** argv)
{
	char *a = argv[1];
	s2e_make_concolic(a, 0x1000, "crax");
	foo(a);
}
