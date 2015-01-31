#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

int main(){
	const char *path = ".";
	DIR * pdir = opendir(path); //return a pointer to dir
	dirent *direntp;
	while(direntp = readdir(pdir))
		cout << direntp->d_name << endl;

	closedir(pdir);
	return 0;
}
