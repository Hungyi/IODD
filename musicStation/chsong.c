#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include "lookdir.h"
#include "tq2440_7seg.h"

void *get_pthread(void *arg);
void *print_pthread(void *arg);
void print();
int fd_pipe[2];
int fd_fifo;
int fd1;
Songstruct *listptr;

int main(int argc,int *argv[])
{
	//int fd;
  	pid_t pid,pid1;
  	char buf[100];
  	unlink("/my_fifo");
  	mkfifo("/my_fifo",O_CREAT|0666);
  	perror("mkfifo");
  	if(pipe(fd_pipe)<0){perror("pipe error\n");exit(-1);}
  	
	pid=fork();
  	if(pid<0)
  	{
    		perror("fork");
  	}else if(pid==0){
		/*pid1=fork();
		if(pid1<0)
		{
			perror("fork");
		}
		else if(pid1==0)
		{
			execlp("./leds","./leds",NULL);
		}else
		{
    			close(fd_pipe[0]);
   			dup2(fd_pipe[1],1);
    			printf("child1\n");
			fd_fifo=open("/my_fifo",O_RDWR);
			printf("child2\n");
			execlp("mplayer","mplayer","-slave","-quiet","-input","file=/my_fifo","/opt/Kalimba.mp3",NULL);
		}*/
		close(fd_pipe[0]);
   		dup2(fd_pipe[1],1);
    		printf("child1\n");
		fd_fifo=open("/my_fifo",O_RDWR);
		printf("child2\n");
		execlp("mplayer","mplayer","-slave","-quiet","-input","file=/my_fifo","/opt/Kalimba.mp3",NULL);
  	}
	else{
		fd1 = open("/dev/tq2440_7seg", 0);
		if(fd1 < 0){
			perror("open device 7seg");
			exit(1);
		}
		pthread_t tid1;
		pthread_t tid2;
		fd_fifo=open("/my_fifo",O_RDWR);
		pthread_create(&tid1,NULL,get_pthread,NULL);
		pthread_create(&tid2,NULL,print_pthread,NULL);
		pthread_join(tid1,NULL);
		pthread_join(tid2,NULL);
  	}
  	return 0;
}

void *get_pthread(void *arg){
	char buf[100];
	listptr=lookdir("/opt");
	_7seg_info_t segment;
	while(1){
   		printf("thread1\n");
   		printf("please input your cmd:");
   		
   		fflush(stdout);

   		printf("\nlistptr->songname=%s\n",listptr->song_name);
		printf("listptr->nextPtr->songname=%s\n",listptr->nextPtr->song_name);

   		fgets(buf,sizeof(buf),stdin);
		if(strlen(buf)==6)
		{
			printf("\n13456789\n");
			ioctl(fd1, 1, NULL);
		}else if(strlen(buf)>=8)
		{
			ioctl(fd1, 2, NULL);
		}
		printf("after fgets.............listptr->songname=%s\n",listptr->song_name);

		buf[strlen(buf)] = '\0';
		printf("*%s*\n", buf);
		if(write(fd_fifo, buf, strlen(buf)) != strlen(buf))
			perror("write");

  	}
}

void *print_pthread(void *arg)
{
  char buf[100];
  close(fd_pipe[1]);
  int size=0;
  while(1)
  {
     printf("thread2");
     size=read(fd_pipe[0],buf,sizeof(buf));
     buf[size]='\0';
     //sleep(3);
     printf("the msg read from pipe is %s\n",buf); 
  }
}
void print(){
   printf("listptr->song_name=%s\n",listptr->song_name);
}

