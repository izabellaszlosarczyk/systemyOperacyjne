#include "producent.h"
#include "konsument.h"
#include "glowny.h"

int main(int argc, char *argv[]){
	if(argc != 2) {
		printf("uruchomienie: ./%s [TYP - k lub p]", argv[0]);
		return -1;
	}

	if (strcmp(argv[1],"p") == 0) {
		return producent(argc, argv);
	} else if(strcmp(argv[1],"k") == 0) {
		return konsument(argc, argv);
	}

	return 0;
}