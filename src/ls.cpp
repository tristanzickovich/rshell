#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

int main(){
	const char *path = ".";
	DIR * pdir = opendir(path); //return a pointer to dir
	if(pdir == NULL)
		perror("Can't open directory:");

	dirent *direntp;
	while(direntp = readdir(pdir))
		cout << direntp->d_name << endl;

	closedir(pdir);
	return 0;
}
