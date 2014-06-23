#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "lookdir.h"

Songstruct *insert(Songstruct *sPtr,char *data)
{
	Songstruct *newptr;
	Songstruct *previousptr;
	Songstruct *currentptr;
	newptr = malloc(sizeof(Songstruct));
  	if(newptr != NULL)
  	{
    	newptr->song_name = data;
		newptr->nextPtr = NULL;
		previousptr = NULL;
		currentptr = sPtr;
		while(currentptr != NULL)
    	{
      		previousptr = currentptr;
      		currentptr = currentptr->nextPtr;
    	}
    	if(previousptr == NULL)
    	{
      		newptr->nextPtr = sPtr;
      		sPtr = newptr;
    	}else{
      		previousptr->nextPtr=newptr;
      		newptr->nextPtr=currentptr;
    	}
  	}else{
    	printf("not insert.No memory size");
  	}
  	return sPtr;
}


Songstruct *lookdir(const char *dir)
{
	struct dirent *entry;
  	struct stat statbuf;
  	Songstruct *startptr = NULL;
  	DIR *dp;

	if(chdir(dir) == -1)
	{
		printf("error or no exist pathname: %s\n", dir);
		return;
	}

  	if((dp=opendir(dir))==NULL)
  	{
    	//fprintf(stderr,"cannot open directory: %s\n",dir);
    	printf("cannot open directory: %s\n",dir);
    	return;
 	}
  	while((entry=readdir(dp))!=NULL)
  	{ 
    	if(lstat(entry->d_name,&statbuf) < 0)
		{
			printf("lstat error\n");
			return;
		}

    	if(!S_ISDIR(statbuf.st_mode)){ 
			/*
			if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0 )
				continue;
			*/
      		//printf("%s\n",entry->d_name);
      		startptr = insert(startptr,entry->d_name);
    	}
    }
  	closedir(dp);
  	return startptr;
}

