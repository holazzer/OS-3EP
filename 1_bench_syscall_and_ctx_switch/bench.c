#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>

#define milsec 1000*1000

int main(int argc,char** argv){
    if(argc!=2){
        printf("usage: %s <number-of-read-call>\n",argv[0]);
        exit(0);
    }
    long long i,n;
    struct timeval tv0,tv1;
    for(i=0;i<strlen(argv[1]);i++){
        if(!isdigit(argv[1][i])){
            printf("bad read-call times: %s\n",argv[1]);
            exit(-1);
        }
    }
    n = atoll(argv[1]);
    if(errno==ERANGE)
    printf("syscall number too big for long long.\nset to:%lld\n",n);
    int fd = open("text.txt",O_RDONLY);
    if(fd<0){
        printf("Fail to get file descriptor.\n");
        exit(-1);
    }
    printf("Get fd:%d\n",fd);
    if(gettimeofday(&tv0,NULL)!=0){
        printf("Fail to get time.\n");
        exit(-1);
    }
    for(int i = 0;i < n; i++){
        read(fd,NULL,0); // empty read call
    }
    if(gettimeofday(&tv1,NULL)!=0){
        printf("Fail to get time.\n");
        exit(-1);
    }
    printf("Syscall %lld times.\ntv0 :%ld %ld\n"\
    "tv1 :%ld %ld\ndiff:%ld ms\n", \
    n,\
    tv0.tv_sec,tv0.tv_usec,\
    tv1.tv_sec,tv1.tv_usec,\
    (tv1.tv_sec-tv0.tv_sec)*milsec+(tv1.tv_usec-tv0.tv_usec) );
    return 0;
    close(fd);
}