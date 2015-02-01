#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <vector>

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

	if(stat("a.out", &sb) == -1){
		perror("Stat Error:");
		exit(1);
	}
	//set owner of item to ownr
	struct passwd *pass = getpwuid(sb.st_uid);
	char *ownr = pass->pw_name;	
	//set group owner to gownr
	struct group *grp = getgrgid(sb.st_gid);
	char *gownr = grp->gr_name; 
	//output permissions line
	if(S_ISDIR(sb.st_mode))
		cout << 'd';
	else if(S_ISLNK(sb.st_mode))
		cout << 's';
	else
		cout << '-';
	if(S_IRUSR & sb.st_mode)
		cout << 'r';
	else
		cout << '-';
	if(S_IWUSR & sb.st_mode)
		cout << 'w';
	else
		cout << '-';
	if(S_IXUSR & sb.st_mode)
		cout << 'x';
	else
		cout << '-';
	if(S_IRGRP & sb.st_mode)
		cout << 'r';
	else
		cout << '-';
	if(S_IWGRP & sb.st_mode)
		cout << 'w';
	else
		cout << '-';
	if(S_IXGRP & sb.st_mode)
		cout << 'x';
	else
		cout << '-';
	if(S_IROTH & sb.st_mode)
		cout << 'r';
	else
		cout << '-';
	if(S_IWOTH & sb.st_mode)
		cout << 'w';
	else
		cout << '-';
	if(S_IXOTH & sb.st_mode)
		cout << 'x';
	else
		cout << '-';
	cout << ' ' << sb.st_nlink;	
	cout << ' ' << ownr;
	cout << ' ' << gownr;
	cout << ' ' << sb.st_size;
	string lastmodtime = ctime(&sb.st_mtime);
	cout << ' ' << lastmodtime.substr(4,12);
	cout << endl;
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
