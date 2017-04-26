#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <s2e.h>

#define FILE_LENGTH 120133

int main(int argc, char** argv)
{
    int fd;
    pid_t pid;
    char *p;

    if (argc < 3) {
        puts("Usage: ./symfile symfile program_path [args]");
        return 0;
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("open file error \n");
        return 0;
    }

    struct stat b;
    fstat(fd , &b);
    p = mmap(0, b.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    printf("%c", p[0]);
    //s2e_make_symbolic(p, b.st_size, "buf" , 0);
    s2e_make_concolic(p, b.st_size, "buf");

    int i;
    char *args[argc - 2];
    for (i = 0; i < argc - 2; i++)
        args[i] = argv[i + 2];
    args[i] = NULL;
    
    if(!(pid = fork()))
        execvp(args[0], args);
    else
        wait();

    close(fd);
    s2e_kill_state(0, "program terminated");
    return 0;
}

