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
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <queue>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace std;

//globals for color change
char blue [] = {"\033[1;34m"};
char bluegrey [] = {"\033[1;47;34m"};
char green [] = {"\033[1;32m"};
char greengrey [] = {"\033[1;47;32m"};
char normal [] = {"\033[0;00m"};
//Following struct and bool taken from: https://ideone.com/fork/6j56qR
struct case_insensitive_less : public std::binary_function< char,char,bool >
{
	bool operator () (char x, char y) const
	{
		return toupper( static_cast< unsigned char >(x)) <
		toupper( static_cast< unsigned char >(y));
	}
};
bool NoCaseLess(const std::string &a, const std::string &b)
{
	return std::lexicographical_compare( a.begin(),a.end(),
	b.begin(),b.end(), case_insensitive_less() );
}

vector<char *> alporder(const char* start){
	vector<char *> newvec;
	DIR * pdir = opendir(start);
	if(pdir == NULL){
		perror("Can't open directory");
		exit(1);
	}

	dirent *direntp;
	while((direntp = readdir(pdir))){
		char *c = new char[strlen(direntp->d_name) + 1];
		strcpy(c, direntp->d_name);
		newvec.push_back(c);
	}
	sort(newvec.begin(), newvec.end(), NoCaseLess);
	
	closedir(pdir);
	return newvec;
}

void printa(bool listall){
	struct stat sb;
	bool hidden = false;
	vector<char *> filesvec = alporder(".");
	for(unsigned i = 0; i < filesvec.size(); ++i){
		char *path = filesvec.at(i);
		if(stat(path, &sb) == -1){
			perror("Stat Error");
			exit(1);
		}
		if(path[0] == '.')
			hidden = true;
		else
			hidden = false;
		//check if -a or no flag.  If no flag skip files starting with '.'
		if((!listall && !hidden) || listall){
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
	}
	cout << endl;
}

// -l flag helper for total output
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
	while((direntp = readdir(pdir))){
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
	bool hidden = false;
	cout << "total " << totalflag(listall) << endl;
	vector<char *> filesvec = alporder(".");
	for(unsigned i = 0; i < filesvec.size(); ++i){
		char *path = filesvec.at(i);
		
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
}

void printrRecursed(const char *dirname, bool listall){ 
	struct stat sb;
	string case1 = ".";
	string case2 = "..";
	bool hidden = false;
	queue<char *> additionalpaths;
	cout << endl /*<< "./" */ << dirname << ':' << endl;
	vector<char *> filesvec = alporder(dirname);
	for(unsigned i = 0; i < filesvec.size(); ++i){
		char *hlpr = filesvec.at(i);
		string paths = (string)dirname + '/' + filesvec.at(i); 
		char *path = (char *)paths.c_str();
		
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
				if(path[strlen(path)-1] != '.'){
					char* p = new char[PATH_MAX + 1];
					realpath(path, p);
					additionalpaths.push(p);
				}
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
		string next = additionalpaths.front();
		delete[] additionalpaths.front();
		additionalpaths.pop();
			printrRecursed(next.c_str(), listall);
	}
}

void printr(const char *dirname, bool listall){
	struct stat sb;
	bool hidden = false;
	string case1 = ".";
	string case2 = "..";
	queue<char *> additionalpaths;
	cout << dirname << ':' << endl;
	vector<char *> filesvec = alporder(".");
	for(unsigned i = 0; i < filesvec.size(); ++i){
		char *path = filesvec.at(i);
	
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
				if(path[strlen(path)-1] != '.'){
					char* p = new char[PATH_MAX + 1];
					realpath(path, p);
					additionalpaths.push(p);
				}
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
		delete[] additionalpaths.front();
		additionalpaths.pop();
		printrRecursed(next.c_str(), listall);
	}
}

void printlrRecursed(const char *dirname, bool listall){
	struct stat sb;
	string case1 = ".";
	string case2 = "..";
	bool hidden = false;
	queue<char *> additionalpaths;
	cout << endl /*<< "./" */ << dirname << ':' << endl;
	cout << "total " << totalflag(listall) << endl;
	vector<char *> filesvec = alporder(dirname);
	for(unsigned i = 0; i < filesvec.size(); ++i){
		char *hlpr = filesvec.at(i);
		string paths = (string)dirname + '/' + filesvec.at(i); 
		char *path = (char *)paths.c_str();
		
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
				if(path[strlen(path)-1] != '.'){
					char* p = new char[PATH_MAX + 1];
					realpath(path, p);
					additionalpaths.push(p);
				}
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
	while(!additionalpaths.empty()){
		string next =  additionalpaths.front();
		delete[] additionalpaths.front();
		additionalpaths.pop();
		printlrRecursed(next.c_str(), listall);
	}

}

void printlr(const char *dirname, bool listall){
	struct stat sb;
	bool hidden = false;
	string case1 = ".";
	string case2 = "..";
	queue<char *> additionalpaths;
	cout << dirname << ':' << endl;
	cout << "total " << totalflag(listall) << endl;
	vector<char *> filesvec = alporder(".");
	for(unsigned i = 0; i < filesvec.size(); ++i){
		char *path = filesvec.at(i);
	
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
				if(path[strlen(path)-1] != '.'){
					char* p = new char[PATH_MAX + 1];
					realpath(path, p);
					additionalpaths.push(p);
				}
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
	while(!additionalpaths.empty()){
		string next =  additionalpaths.front();
		delete[] additionalpaths.front();
		additionalpaths.pop();
		printlrRecursed(next.c_str(), listall);
	}

}

int countflags(unsigned size, char **args){
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
		printa(true);
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
		printlr(".", false);
	//if flag is -aRl (or some combo thereof)
	else if (numflags == 7)
		printlr(".", true);
	//if no flags
	else if (numflags == 0){
		printa(false);
	}
	return 0;
}







