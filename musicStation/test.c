#include <stdio.h>
#include <stdlib.h>
#include "lookdir.h"

int main()
{
	Songstruct *test = NULL;
	test = lookdir("/home/kyle/");
	//printf("test address:%p\n", test);	

	while(test != NULL){
		printf("%s\n", test->song_name);
		test = test->nextPtr;
	}

}
