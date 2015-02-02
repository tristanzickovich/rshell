#include <iostream>
#include <iomanip>
#include <cstdio>
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
#include <queue>

using namespace std;
//globals for color change
char blue [] = {"\033[1;34m"};
char bluegrey [] = {"\033[1;100;34m"};
char green [] = {"\033[1;32m"};
char greengrey [] = {"\033[1;100;32m"};
char normal [] = {"\033[0;00m"};


void printa(){
	struct stat sb;
	DIR * pdir = opendir("."); //return a pointer to dir
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}
	bool hidden = false;
	dirent *direntp;
	while(direntp = readdir(pdir)){
		char *path = direntp->d_name;
		if(stat(path, &sb) == -1){
			perror("Stat Error");
			exit(1);
		}
		if(path[0] == '.')
			hidden = true;
		else
			hidden = false;

		if(S_ISDIR(sb.st_mode)){	
			if(hidden)
				cout <<  bluegrey << path << '/' << normal << "  "; 
			else
				cout << blue << path << '/' << normal << "  "; 
		}
		else if(S_IXUSR & sb.st_mode){
			if(hidden)
				cout << greengrey << path << normal << "  ";
			else
				cout << green << path << normal << "  ";
		}
		else
			cout << path << "  " ;

	}
	cout << endl;
	closedir(pdir);
}

int totalflag(bool listall){
	int total = 0;
	struct stat sb;
	DIR * pdir = opendir("."); //return a pointer to dir
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}
	bool hidden = false;
	dirent *direntp;
	while(direntp = readdir(pdir)){
		char *path = direntp->d_name;
		
		if(stat(path, &sb) == -1){
			perror("Stat Error");
			exit(1);
		}
		if(path[0] == '.')
			hidden = true;
		else
			hidden = false;
		//check if -l or -la.  If -l skip files starting with '.'
		if((!listall && !hidden) || listall){
			total += sb.st_blocks;
		}
	}
	return total/2;
}

void printl(bool listall){
	struct stat sb;
	DIR * pdir = opendir("."); //return a pointer to dir
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}
	bool hidden = false;
	dirent *direntp;
	cout << "total " << totalflag(listall) << endl;
	while(direntp = readdir(pdir)){
		char *path = direntp->d_name;
		
		if(stat(path, &sb) == -1){
			perror("Stat Error");
			exit(1);
		}
		if(path[0] == '.')
			hidden = true;
		else
			hidden = false;
		//check if -l or -la.  If -l skip files starting with '.'
		if((!listall && !hidden) || listall){
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
			//number of links
			cout << ' ' << sb.st_nlink;	
			//owner of item
			cout << ' ' << ownr;
			//group owner of item
			cout << ' ' << gownr;
			//size of item in bytes
			cout << ' ' << setw(7) << right <<  sb.st_size;
			//time item was last modified
			string lastmodtime = ctime(&sb.st_mtime);
			cout << ' ' << lastmodtime.substr(4,12);
			//output file name 
			//if file is a directory
			if(S_ISDIR(sb.st_mode)){	
				if(hidden)
					cout << ' ' <<  bluegrey << path << '/' << normal;
				else
					cout << ' ' << blue << path << '/' << normal;
			}
			else if(S_IXUSR & sb.st_mode){
				if(hidden)
					cout << ' ' << greengrey << path << normal;
				else
					cout << ' ' << green << path << normal;
			}
			else
				cout << ' ' << path;

			cout << endl;
		}
	}
	closedir(pdir);
}

void printrRecursed(const char *dirname, bool listall){ 
	struct stat sb;
	DIR * pdir = opendir(dirname); //return a pointer to dir
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}
	bool hidden = false;
	string filepath;
	queue<char *> additionalpaths;
	dirent *direntp;
	cout << endl << "./" << dirname << ':' << endl;
	while(direntp = readdir(pdir)){
		char *hlpr = direntp->d_name;
		string paths = (string)dirname + '/' + direntp->d_name;
		char *path = (char *)paths.c_str();
		//char *path = direntp->d_name;
		
		if(stat(path, &sb) == -1){
			perror("Stat Error");
			exit(1);
		}
		if(hlpr[0] == '.')
			hidden = true;
		else
			hidden = false;
		if((!listall && !hidden) || listall){
			if(S_ISDIR(sb.st_mode)){	
				additionalpaths.push(hlpr);
				if(hidden)
					cout <<  bluegrey << hlpr << '/' << normal << "  "; 
				else
					cout << blue << hlpr << '/' << normal << "  "; 
			}
			else if(S_IXUSR & sb.st_mode){
				if(hidden)
					cout << greengrey << hlpr << normal << "  ";
				else
					cout << green << hlpr << normal << "  ";
			}
			else
				cout << hlpr << "  " ;

		}
	}
	cout << endl;
	while(!additionalpaths.empty()){
		string next =  additionalpaths.front();
		//cerr << next << endl;
		additionalpaths.pop();
		printrRecursed(next.c_str(), listall);
	}
	closedir(pdir);
}

void printr(const char *dirname, bool listall){
	struct stat sb;
	DIR * pdir = opendir(dirname); //return a pointer to dir
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}
	bool hidden = false;
	string filepath;
	queue<char *> additionalpaths;
	dirent *direntp;
	cout << dirname << ':' << endl;
	while(direntp = readdir(pdir)){

		char *path = direntp->d_name;
		
		if(stat(path, &sb) == -1){
			perror("Stat Error");
			exit(1);
		}
		if(path[0] == '.')
			hidden = true;
		else
			hidden = false;
		if((!listall && !hidden) || listall){
			if(S_ISDIR(sb.st_mode)){	
				additionalpaths.push(path);
				if(hidden)
					cout <<  bluegrey << path << '/' << normal << "  "; 
				else
					cout << blue << path << '/' << normal << "  "; 
			}
			else if(S_IXUSR & sb.st_mode){
				if(hidden)
					cout << greengrey << path << normal << "  ";
				else
					cout << green << path << normal << "  ";
			}
			else
				cout << path << "  " ;

		}
	}
	cout << endl;
	while(!additionalpaths.empty()){
		string next =  additionalpaths.front();
		//cerr << next << endl;
		additionalpaths.pop();
		printrRecursed(next.c_str(), listall);
	}
	closedir(pdir);
}

void printlr(){
}

void printarl(){
}

int countflags(int size, char **args){
	int numflags = 0;
	bool ainc = false, linc = false, rinc = false;
	for(unsigned i = 1; i < size; ++i)
		if(args[1][0] != '-')
			break;
		else{
			for(unsigned j = 1; args[i][j] != 0; ++j){
				if(args[i][j] == 'a' && !ainc){
					ainc = true;
					++numflags;
				}
				else if(args[i][j] == 'l' && !linc){
					linc = true;
					numflags += 2;
				}
				else if(args[i][j] == 'R' && !rinc){
					rinc = true;
					numflags += 4;
				}
			}
		}
	return numflags;
}

int main(int argc, char *argv[]){
	int numflags = countflags(argc, argv);
	
	//if flag is -a
	if(numflags == 1)
		printa();
	//if flag is -l
	else if(numflags == 2)
		printl(false);
	//if flag is -la (or some combo thereof)
	else if (numflags == 3)
		printl(true);
	//if flag is -R
	else if (numflags == 4)
		printr(".", false);
	//if flag is -aR (or some combo thereof)
	else if (numflags == 5)
		printr(".", true);
	//if flag is -lR (or some combo thereof)
	else if (numflags == 6)
		printlr();
	//if flag is -aRl (or some combo thereof)
	else if (numflags == 7)
		printarl();
	return 0;
}







