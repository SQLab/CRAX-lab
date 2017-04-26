#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <s2e.h>

int main(int argc, char **argv)
{
    if(argc < 2) {
        puts("Usage: ./symarg program_path arg0");
        return 0;
    }

    pid_t pid;
    
    //s2e_disable_forking();

    char *args[argc - 1];
    int i;
    for (i = 0; i < argc - 1; i++)
        args[i] = argv[i + 1];
    args[i] = NULL;

    s2e_make_concolic(args[1], strlen(args[1]), "crax");
    if(!(pid = fork())) {
        execve(args[0], args, NULL);
    } else {
        wait();
        s2e_kill_state(0, "program terminated");
    }

    return 0;
}
