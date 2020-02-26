#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    char buf[10];
    int pipe_fd[2];
    pipe(pipe_fd);
    if(fork()==0){
        if(read(pipe_fd[0],buf,9)<0){
            fprintf(2,"error read child");
            exit();
        };
        fprintf(2,"%d: received %s\n",getpid(),buf);
        if(write(pipe_fd[1],"pong",4)<0){
            fprintf(2,"error write child");
            exit();
        };
    }
    else
    {
        if(write(pipe_fd[1],"ping",4)<0){
            fprintf(2,"error write parent");
            exit();
        }
        sleep(1);
        if(read(pipe_fd[0],buf,9)<0){
            fprintf(2,"errpr read parent");
            exit();
        }
        fprintf(2,"%d: received %s\n",getpid(),buf);
    }
    exit();
}