#ifndef _LOOKDIR_H_
#define _LOOKDIR_H_

struct songstruct{
	char *song_name;
	struct songstruct *nextPtr;
};
typedef struct songstruct Songstruct;

Songstruct *insert(Songstruct *sPtr, char *data);
Songstruct *lookdir(const char *dir);

#endif //end of _LOOKDIR_H_
