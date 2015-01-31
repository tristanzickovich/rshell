#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

void printa(const char *path){
	DIR * pdir = opendir(path); //return a pointer to dir
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}
	dirent *direntp;
	while(direntp = readdir(pdir))
		cout << direntp->d_name << "  " ;
	cout << endl;
	closedir(pdir);
}

void printl(){
	struct stat sb;
	if(stat(".", &sb) == -1){
		perror("Stat Error:");
		exit(1);
	}
	cout << sb.st_dev << endl
		<< sb.st_ino << endl
		<< sb.st_mode << endl
		<< sb.st_nlink << endl
		<< sb.st_uid << endl
		<< sb.st_gid << endl
		<< sb.st_rdev << endl
		<< sb.st_size << endl
		<< sb.st_blksize << endl
		<< sb.st_blocks << endl
		<< sb.st_atime << endl
		<< sb.st_mtime << endl
		<< sb.st_ctime << endl;
}

int main(int argc, char *argv[]){
	string aflag = "-a", lflag = "-l", rflag = "-R";
	for(unsigned i = 1; i < argc; ++i){
		if(string(argv[i]) == aflag)
			printa(".");
		else if(string(argv[i]) == lflag)
			printl();
	}
	return 0;
}
