#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>
#include <algorithm>

#define outlist O_RDWR|O_CREAT|O_TRUNC, 0644
#define inlist O_RDONLY
#define appendlist O_RDWR|O_CREAT|O_APPEND, 0644

using namespace std;
string cleanup(string cmd){
	if(!cmd.empty()){	
		//delete preceeding comments or white space
		while((cmd.at(0) == ';' || cmd.at(0) == ' ' || cmd.at(0) == '\t' 
			|| cmd.at(0) == '|' || cmd.at(0) == '&')){
			cmd.erase(0,1);
			if(cmd.empty())
				break;
		}
	}

	//check not duplicate, may become empty after previous task
	if(!cmd.empty()){
		//delete superceeding comments or white space
		while((cmd.at(cmd.size()-1) == ';' || cmd.at(cmd.size()-1) == ' ' || cmd.at(cmd.size()-1) == '\t'
			|| cmd.at(cmd.size()-1) == '|' || cmd.at(cmd.size()-1) == '&')){
			cmd.resize(cmd.size()-1);
			if(cmd.empty())
				break;
		}
	}
	return cmd;
}
//make a vector of strings into a vector of char *'s
vector<char*> convertvec(vector<string> conv){
	vector<char*> charvec;

	for(unsigned i = 0; i < conv.size(); ++i){
		string temp = conv.at(i);
		char *piece = new char[temp.size() + 1];
		charvec.push_back(strcpy(piece, temp.c_str()));
	}
	charvec.push_back(NULL);
	return charvec;
}

int execredir(string left, string right, int dupval, int ID){
	int var;
	right = cleanup(right);
	left = cleanup(left);
	vector<string> getready;
	left.c_str();
	boost::split(getready, left, boost::is_any_of(" "),	
			  boost::token_compress_on);
	//std::reverse(getready.begin(), getready.end());
	vector<char*> charvec = convertvec(getready);	
	char *argchar[300]; 
	for(unsigned i = 0; i < charvec.size(); ++i){
		argchar[i] = charvec.at(i);
	}
	int fd = -1;
	if(ID == 0)
		fd = open(right.c_str(), inlist); 
	if(ID == 1)
		fd = open(right.c_str(), outlist); 
	if(ID == 2)
		fd = open(right.c_str(), appendlist); 

	if(fd == -1){
		perror("Open Error");
		exit(1);
	}
	int returnstd = dup(dupval);

	int pid = fork();
	if(pid == -1){
		perror("Error with fork()");
		exit(1);
	}
	else if(pid == 0){
		if(-1 == close(dupval)){
			perror("Close Error");
			exit(1);
		}
		if(-1 == dup(fd)){
			perror("Dup Error");
			exit(1);
		}
		execvp(argchar[0], argchar);
		perror("Exec failed");
		exit(1);
	}
	else if(pid != 0){
		while(wait(&var) != pid)
			perror("Error with wait()");
		if(-1 == close(fd)){
			perror("Close Error");
			exit(1);
		}
		if(-1 == dup(returnstd)){
			perror("Dup Error");
			exit(1);
		}
		for(unsigned i = 0; i <  charvec.size(); ++i)
			delete [] charvec.at(i);
		return var;
	}
	for(unsigned i = 0; i <  charvec.size(); ++i)
			delete [] charvec.at(i);

	return -1;
}

int execRedirection(vector<string> cmds){
	bool indetect = false, outdetect = false, appenddetect = false, pipedetect = false;
	for(unsigned i = 0; i < cmds.size(); ++i){
		if(cmds.at(i) == "<")
			indetect = true;
		if(cmds.at(i) == ">")
			outdetect = true;
		if(cmds.at(i) == ">>")
			appenddetect = true;
		if(cmds.at(i) == "|")
			pipedetect = true;
	}	
	if(indetect && (outdetect || appenddetect)){
		cout << "fixme" << endl;
		//FIXME
	}
	else if(indetect || outdetect || appenddetect){
		unsigned inn = 0, outt = 0, appendd = 0;
		cmds.push_back(";");
		string execme = "", lastexecme = "";
		int lastcmd = 0;
		for(unsigned i = 0; i < cmds.size(); ++i){
			if(cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == ";"){
				if(lastcmd == 0){
					if(lastexecme != ""){
						execredir(lastexecme, execme, 0, 0);
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				else if(lastcmd == 1){
					if(lastexecme != ""){
						execredir(lastexecme, execme, 1, 1);
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				else if(lastcmd == 2){
					if(lastexecme != ""){
						execredir(lastexecme, execme, 1, 2);
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				if(cmds.at(i) == "<"){
					lastcmd = 0;
					if(inn == 1){
						cerr << "Error: Only one '<' allowed" << endl;
						break;
					}
					++inn;
				}
				else if(cmds.at(i) == ">"){
					lastcmd = 1;
					if(outt == 1 || appendd == 1){
						cerr << "Error: Only one '>' or '>>' allowed" << endl;
						break;
					}
					++outt;
				}
				else if(cmds.at(i) == ">>"){
					lastcmd = 2;
					if(outt == 1 || appendd == 1){
						cerr << "Error: Only one '>' or '>>' allowed" << endl;
						break;
					}
					++appendd;
				}
				else
					break;
				execme = "";		
			}
			else{
				execme.append(cmds.at(i));
				execme.append(" ");
			}
		}
	}
	return 0;
}

int execute(string commands){
	vector<string> getready;
	commands = cleanup(commands);	
	int var;
	if(commands.size() > 0){
		commands.c_str();
		boost::split(getready, commands, boost::is_any_of(" "),	
			  boost::token_compress_on);
		if(getready.at(0) == "exit" || getready.at(0) == "EXIT"){
			exit(1);
		}
	}
	for(unsigned i = 0; i < getready.size(); ++i){
		if(getready.at(i) == "|" || getready.at(i) == "<" || getready.at(i) == ">"
							|| getready.at(i) == ">>"){
			return execRedirection(getready);
		}
	}
	vector<char*> charvec = convertvec(getready);	
	char *argchar[300]; 
	for(unsigned i = 0; i < charvec.size(); ++i){
		argchar[i] = charvec.at(i);
	}
	int pid = fork();
	if(pid == -1){
		perror("Error with fork()");
		exit(1);
	}
	else if(pid == 0){
		execvp(argchar[0], argchar);
		perror("Exec failed");
		exit(1);
	}
	else if(pid != 0){
		while(wait(&var) != pid)
			perror("Error with wait()");
		for(unsigned i = 0; i <  charvec.size(); ++i)
			delete [] charvec.at(i);
		return var;
	}
	for(unsigned i = 0; i <  charvec.size(); ++i)
			delete [] charvec.at(i);

	return -1;
}

string specialspacing(string fixer){
	for(unsigned i = 1; i < fixer.size(); ++i){
		if(fixer.at(i) == ';'){
			if(i+1 < fixer.size() && fixer.at(i+1) != ' '){
				fixer.insert(i+1, " ");
			}
			if(fixer.at(i-1) != '\\' && fixer.at(i-1) != ' '){
				fixer.insert(i, " ");
			}
		}

		else if(fixer.at(i-1) == '&' && fixer.at(i) == '&'){
			if(i+1 < fixer.size() && fixer.at(i+1) != ' '){
				fixer.insert(i+1, " ");
			}
			if(i-2 >= 0 && fixer.at(i-2) != ' '){
				fixer.insert(i-1, " ");
			}
		}

		else if(fixer.at(i-1) == '|' && fixer.at(i) == '|'){
			if(i+1 < fixer.size() && fixer.at(i+1) != ' '){
				fixer.insert(i+1, " ");
			}
			if(i-2 >= 0 && fixer.at(i-2) != ' '){
				fixer.insert(i-1, " ");
			}
		}
		else if(fixer.at(i) == '<'){
			if(i+1 < fixer.size() && fixer.at(i+1) != ' '){
				fixer.insert(i+1, " ");
			}
			if(fixer.at(i-1) != ' '){
				fixer.insert(i, " ");
			}
		}
		else if(fixer.at(i) == '>'){
			if(i+1 < fixer.size() && fixer.at(i+1) == '>'){
				if(i+2 < fixer.size() && fixer.at(i+2) != ' '){
					fixer.insert(i+2, " ");
				}
				if(fixer.at(i-1) != ' '){
					fixer.insert(i, " ");
				}
			}
			if(i+1 < fixer.size() && fixer.at(i+1) != '>' && fixer.at(i-1) != '>'){
				if(fixer.at(i+1) != ' '){
					fixer.insert(i+1, " ");
				}
				if(fixer.at(i-1) != ' '){
					fixer.insert(i, " ");
				}
			}
			if(fixer.at(i-1) != ' ' && fixer.at(i-1) != '>'){
				fixer.insert(i, " ");
			}
		}
		else if(fixer.at(i) == '|'){
			if(i+1 < fixer.size() && fixer.at(i+1) != '|' && fixer.at(i-1) != '|'){
				if(fixer.at(i+1) != ' '){
					fixer.insert(i+1, " ");
				}
				if(fixer.at(i-1) != ' '){
					fixer.insert(i, " ");
				}
			}
		}
	}
	return fixer;
}

int main(){
	vector<string> cmdline;
	struct passwd *pass = getpwuid(getuid());
	if(pass == NULL){
		perror("Getpwuid Error");
		exit(1);
	}
	char *curuser = pass->pw_name;
	char charhost[100];
	if(-1 == gethostname(charhost, sizeof charhost)){
		perror("Gethostname Error");
		exit(1);
	}
	string curhost = charhost;	
	if(curhost.find('.') != std::string::npos)
		curhost.resize(curhost.find('.'));

	while(true){
		//output prompt and take in command line
		cout << curuser << '@' << curhost << ' ';
		cout << '$' << ' ';
		string command = "";
		getline(cin, command);

		if(cin.fail()){
			cout << "exiting" << endl;
			return 0;
		}

		//if comment, disregard everything following it
		if(command.find('#') != std::string::npos){
			command.resize(command.find('#'));
		}

		command = cleanup(command);	
		//if any commands exist (other than spaces or comments) process commands
		if(command.size() > 0){
			command = specialspacing(command);
			command.c_str();
			//split command line by spaces and tabs and push into cmdline vector
			boost::split(cmdline, command, boost::is_any_of(" , \t"),	
				  boost::token_compress_on);
			cmdline.push_back(";");
			int conditional;
			string execme = "";	
			bool passed = true; 
			//0 = neither, 1 = and, 2 = or
			int lastcmd = 0;
		
			for(unsigned i = 0; i < cmdline.size(); ++i){
				//if first command is && or ||, do nothing
				if(execme == "" && (cmdline.at(i) == "&&" || cmdline.at(i) == "||"));

				//if && or || is found in between commands, act accordingly
				else if(cmdline.at(i) == "&&" || cmdline.at(i) == "||" || cmdline.at(i) == ";"){ 
					//if last was ; (or a brand new command)
					if(lastcmd == 0){
						conditional = execute(execme);
						if(conditional == 0)
							passed = true;
						else
							passed = false;
					}
					//if last was &&
					else if(lastcmd == 1){
						if(passed){
							conditional = execute(execme);
							if(conditional == 0)
								passed = true;
							else
								passed = false;
						}
					}
					//if last was ||
					else if(lastcmd == 2){
						if(!passed){
							conditional = execute(execme);
							if(conditional == 0)
								passed = true;
							else
								passed = false;
						}
					}
					//set what last cmd operator was	
					if(cmdline.at(i) == ";")
						lastcmd = 0;
					else if(cmdline.at(i) == "&&")
						lastcmd = 1;
					else if(cmdline.at(i) == "||")
						lastcmd = 2;

					execme = "";
				}
				//if a normal command, add to execme for later processing
				else{
					execme.append(cmdline.at(i));
					execme.append(" ");
				}
			}
		}
	}	
	return 0;
}	
