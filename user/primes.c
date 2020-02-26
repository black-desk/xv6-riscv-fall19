#include"kernel/types.h"
#include"kernel/stat.h"
#include"user/user.h"
void fliter(int x,int in_pipe_fd,int out_pipe_fd)
{
    fprintf(2,"prime %d\n",x);
    char buf;
    while(read(in_pipe_fd,&buf,1)>=0){
        if(buf%x!=0){
            write(out_pipe_fd,&buf,1);
        }
    }
}
void sub(int* lpipe_fd){
    close(lpipe_fd[1]);
    char buf=0;
    int rpipe_fd[2];
    pipe(rpipe_fd);
    if(fork()==0){
        sub(rpipe_fd);
    }
    close(rpipe_fd[0]);
    while(!buf) read(lpipe_fd[0],&buf,1);
    fliter(buf,lpipe_fd[0],rpipe_fd[1]);
    return;
}
int main()
{
    char buf;
    int pipe_fd[2];
    pipe(pipe_fd);
    if(fork()==0){
        sub(pipe_fd);
    }
    close(pipe_fd[0]);
    for(int i=2;i<=35;i++){
        buf = i;
        write(pipe_fd[1],&buf,1);
    }
    sleep(1);
    exit();
}