#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>
#include<sys/wait.h>

#define milsec 1000*1000


int main(int argc,char** argv){
    if(argc!=2){
        printf("usage: %s <number-of-pipe-transfer>\n",argv[0]);
        exit(0);
    }
    long long i,n;
    char msg = '$'; // receive ans send a one-char message 
    struct timeval tv0,tv1;
    for(i=0;i<strlen(argv[1]);i++){
        if(!isdigit(argv[1][i])){
            printf("bad pipe-transfer times: %s\n",argv[1]);
            exit(-1);
        }
    }
    n = atoll(argv[1]);
    if(errno==ERANGE)
    printf("number too big for long long.\nset to:%lld\n",n);

    int pA2B[2],pB2A[2],res,ok;
    if(pipe(pA2B)<0 || pipe(pB2A)<0){
        printf("Fail to create pipe.\n");
        close(pA2B[0]);close(pA2B[1]);
        close(pB2A[0]);close(pB2A[1]);
        exit(-1);
    }
    // !!! p[0] for read and p[1] for write, don't mix up
    printf("pipes created: [%d]A->B[%d] [%d]A<-B[%d]\n",pA2B[1],pA2B[0],pB2A[1],pB2A[0]);
    // time 
    if(gettimeofday(&tv0,NULL)!=0){
        printf("Fail to get time.\n");
        close(pA2B[0]);close(pA2B[1]);
        close(pB2A[0]);close(pB2A[1]);
        exit(-1);
    }
    int pid = fork(); // fork!
    if(pid==-1){printf("Failed to fork.\n");exit(-1);}
    if(pid==0){//child process [B]
        close(pA2B[1]);close(pB2A[0]); // close needless ends
        ok = 0;
        while(1){ // exit when pA2B[0] is closed
            res = read(pA2B[0],&msg,1);
            if(res==0)break;
            write(pB2A[1],&msg,1);// write to fail!
        }
        close(pA2B[0]);close(pB2A[1]);
        printf("child process exit.\n");
        return 0;
    }
    printf("child:%d\n",pid);
    close(pA2B[0]);close(pB2A[1]);
    i = 0;
    ok = 0;
    while(i<n){
        write(pA2B[1],&msg,1);
        read(pB2A[0],&msg,1);
        i++;
    }
    close(pA2B[1]);close(pB2A[0]);

    // time again
    if(gettimeofday(&tv1,NULL)!=0){
        printf("Fail to get time.\n");
        exit(-1);
    }
    printf("pipe-transfer %lld times.\ntv0 :%ld %ld\n"\
    "tv1 :%ld %ld\ndiff:%ld ms\n", \
    n,\
    tv0.tv_sec,tv0.tv_usec,\
    tv1.tv_sec,tv1.tv_usec,\
    (tv1.tv_sec-tv0.tv_sec)*milsec+(tv1.tv_usec-tv0.tv_usec) );
    wait(&res); // retrieve child info and prevent zombie
    printf("wait gets: %d %d\n",res,WIFEXITED(res));
    if(WIFSIGNALED(res))printf("WTERMSIG: %d\n",WTERMSIG(res));
    return 0;
}