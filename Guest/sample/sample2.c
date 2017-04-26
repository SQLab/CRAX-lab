
void foo(char *a)
{
	char b[40];
	strcpy(b, a);
}

int main(int argc, char** argv)
{
	char *a = argv[1];
	foo(a);
}
