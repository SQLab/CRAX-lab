#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <s2e.h>

int main(int argc, char **argv)
{
    if(argc < 2) {
        puts("Usage: ./symio program_path [args]");
        return 0;
    }

    int n;
    char buf[1024];
    puts("[Get crash input....]");
    n = read(0, buf, sizeof(buf));
    if (n < 0) {
        puts("payload error");
        return 0;
    }
    
    int pipe_fd[2];
    pid_t pid;
    
    //s2e_disable_forking();
    s2e_make_concolic(buf, n, "crax");

    if(pipe(pipe_fd) < 0) {
        printf("pipe error\n");
        exit(1);
    }

    write(pipe_fd[1], buf, n);

    char *args[argc - 1];
    int i;
    for (i = 0; i < argc - 1; i++)
        args[i] = argv[i + 1];
    args[i] = NULL;

    if(!(pid = fork())) {
        dup2(pipe_fd[0], 0);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execve(args[0], args, NULL);
    } else {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        wait();
        s2e_kill_state(0, "program terminated");
    }

    return 0;
}
