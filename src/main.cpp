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
#include <signal.h>

#define outlist O_RDWR|O_CREAT|O_TRUNC, 00744
#define inlist O_RDONLY
#define appendlist O_RDWR|O_CREAT|O_APPEND, 00744
#define num0appendlist O_RDONLY|O_CREAT|O_APPEND, 00744
#define num0outlist O_RDONLY|O_TRUNC

using namespace std;

void handler(int i){
	if(i == SIGINT){
		return;
	}

}

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

//is_number function taken from stackoverflow.com
bool is_number(const string& s){
	string::const_iterator it = s.begin();
	while(it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

void deletevec(vector<char*> rmvec){
	for(unsigned i = 0; i <  rmvec.size(); ++i)
		delete [] rmvec.at(i);
}

int changeDir(string newdir){

	return -1;
}

void findCommands(char **argchar){
	vector<string> getready;
	string location;
	char *path = getenv("PATH");
	if(path == NULL){
		perror("Getenv Error");
		exit(1);
	}
	boost::split(getready, path, boost::is_any_of(":"),	
			  boost::token_compress_on);
	for(unsigned i = 0; i < getready.size(); ++i){
		location = getready.at(i) + "/" + argchar[0];
		execv(location.c_str(), argchar);
	}
	perror("Execv Error");
	exit(1);
}

int execute(string commands);

int execredir(string left, string right, int dupval, int ID){
	int var;
	right = cleanup(right);
	left = cleanup(left);
	vector<string> getready;
	left.c_str();
	boost::split(getready, left, boost::is_any_of(" "),	
			  boost::token_compress_on);
	vector<char*> charvec = convertvec(getready);	
	char *argchar[300]; 
	for(unsigned i = 0; i < charvec.size(); ++i){
		argchar[i] = charvec.at(i);
	}
	int fd = -1;
	if(ID == 0){
		fd = open(right.c_str(), inlist); 
		if(fd == -1){
		perror("Open Error");
		deletevec(charvec);
		return(-1);
		}
	}
	if(ID == 1 || ID == 4){
		fd = open(right.c_str(), outlist); 
		if(fd == -1){
		perror("Open Error");
		deletevec(charvec);
		return(-1);
		}
	}
	if(ID == 2 || ID == 6){
		fd = open(right.c_str(), appendlist); 
		if(fd == -1){
		perror("Open Error");
		deletevec(charvec);
		return(-1);
		}
	}
	if(ID == 3){
		fd = open(right.c_str(), num0outlist);
		if(fd == -1){
		perror("Open Error");
		deletevec(charvec);
		return(-1);
		}
	}
	if(ID == 5){
		fd = open(right.c_str(), num0appendlist);
		if(fd == -1){
			perror("Open Error");
			deletevec(charvec);
			return(-1);
		}
	}
	int returnstd = dup(dupval);
	if(returnstd == -1){
		perror("Dup Error");
		deletevec(charvec);
		return(-1);
	}

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
		findCommands(argchar);
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
		deletevec(charvec);
		return var;
	}
		deletevec(charvec);
	return -1;
}

int execredir2(string left, string middle, string right, int dupval){
	int var;
	right = cleanup(right);
	middle = cleanup(middle);
	left = cleanup(left);
	vector<string> getready;
	left.c_str();
	boost::split(getready, left, boost::is_any_of(" "),	
			  boost::token_compress_on);
	vector<char*> charvec = convertvec(getready);	
	char *argchar[300]; 
	for(unsigned i = 0; i < charvec.size(); ++i){
		argchar[i] = charvec.at(i);
	}
	int fd1;
	if(dupval == 0){
		fd1 = open(right.c_str(), outlist); 
		if(fd1 == -1){
			perror("Open Error");
			deletevec(charvec);
			return(-1);
		}
	}
	if(dupval == 1){
		fd1 = open(right.c_str(), appendlist);
		if(fd1 == -1){
			perror("Open Error");
			deletevec(charvec);
			return(-1);
		}	
	}
	int fd2 = open(middle.c_str(), O_RDONLY); 
	if(fd2 == -1){
		perror("Open Error");
		deletevec(charvec);
		return(-1);
	}

	int returnstdin = dup(0);
	if(returnstdin == -1){
		perror("Dup Error");
		deletevec(charvec);
		return(-1);
	}

	int returnstdout = dup(1);
	if(returnstdout == -1){
		perror("Dup Error");
		deletevec(charvec);
		return(-1);
	}

	int pid = fork();
	if(pid == -1){
		perror("Error with fork()");
		exit(1);
	}
	else if(pid == 0){
		if(-1 == close(1)){
			perror("Close Error");
			exit(1);
		}
		if(-1 == dup(fd1)){
			perror("Dup Error");
			exit(1);
		}
		if(-1 == close(0)){
			perror("Close Error");
			exit(1);
		}
		if(-1 == dup(fd2)){
			perror("Dup Error");
			exit(1);
		}
		findCommands(argchar);
	}
	else if(pid != 0){
		while(wait(&var) != pid)
			perror("Error with wait()");
		if(-1 == close(fd1)){
			perror("Close Error");
			exit(1);
		}
		if(-1 == dup(returnstdout)){
			perror("Dup Error");
			exit(1);
		}
		if(-1 == close(fd2)){
			perror("Close Error");
			exit(1);
		}
		if(-1 == dup(returnstdin)){
			perror("Dup Error");
			exit(1);
		}
		deletevec(charvec);
		return var;
	}
	deletevec(charvec);
	return -1;
}

int execRedirection(vector<string> cmds){
	int rval = -1;
	bool indetect = false, in3detect = false, outdetect = false, appenddetect = false, 
		numoutdetect = false, numappenddetect = false;
	int numout, numappend;
	for(unsigned i = 0; i < cmds.size(); ++i){
		if(cmds.at(i) == "<")
			indetect = true;
		if(cmds.at(i) == "<<<")
			in3detect = true;
		if(cmds.at(i) == ">")
			outdetect = true;
		if(cmds.at(i) == ">>")
			appenddetect = true;
		if(is_number(cmds.at(i)) && (i+1 < cmds.size() && cmds.at(i+1) == ">" && !numoutdetect)){
			numoutdetect = true;
			numout = atoi(cmds.at(i).c_str());
		}
		if(is_number(cmds.at(i)) && (i+1 < cmds.size() && cmds.at(i+1) == ">>" && !numappenddetect)){
			numappenddetect = true;
			numappend = atoi(cmds.at(i).c_str());
		}
	}	
	if((indetect || in3detect) && (outdetect || appenddetect)){
		unsigned inn = 0, inn2 = 0, outt = 0, appendd = 0;
		cmds.push_back(";");
		string execme = "", lastexecme = "", midexecme = "";
		int lastcmd = 0;
		int ID = 1;
		bool infirst;
		for(unsigned i = 0; i < cmds.size(); ++i){
			if(is_number(cmds.at(i)) && i+1 < cmds.size() && (cmds.at(i+1) == ">>" || cmds.at(i+1) == ">"));
			else if(cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == "<<<" || cmds.at(i) == ";"){
				if(lastexecme != ""){
					if(infirst){
						if(inn2 == 1){
							if(midexecme.at(0) == '"' && midexecme.at(midexecme.size()-2) == '"')
								midexecme = midexecme.substr(1, midexecme.size()-3);
							lastexecme = "echo " + midexecme;
							rval = execredir(lastexecme, execme, 1, ID);
						}
						else
							rval = execredir2(lastexecme, midexecme, execme, lastcmd);
					}
					else{
						rval = execredir2(lastexecme, execme, midexecme, lastcmd);
					}
					lastexecme = "";
				}
				else if(midexecme != ""){
					lastexecme = midexecme;
					midexecme = execme;
				}
				else
					midexecme = execme;
				if((inn == 1 || inn2 == 1) &&(outt == 0 && appendd == 0))
					infirst = true;
				else if ((outt == 1 || appendd == 1) && (inn == 0 && inn2 == 0))
					infirst = false;
				if(cmds.at(i) == "<"){
					if(inn == 1 || inn2 == 1){
						cerr << "Error: Only one input allowed. Only first was executed." << endl;
						break;
					}
					++inn;
				}
				else if(cmds.at(i) == "<<<"){
					if(inn == 1 || inn2 == 1){
						cerr << "Error: Only one input allowed. Only first was executed." << endl;
						break;
					}
					++inn2;
				}
				else if(cmds.at(i) == ">"){
					if(outt == 1 || appendd == 1){
						cerr << "Error: Only one ouput allowed. Only first was executed." << endl;
						break;
					}
					++outt;
				}
				else if(cmds.at(i) == ">>"){
					ID = 2;
					lastcmd = 1;
					if(outt == 1 || appendd == 1){
						cerr << "Error: Only one output allowed. Only first was executed." << endl;
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
	else if(indetect || in3detect || outdetect || appenddetect){
		unsigned inn = 0, inn2 = 0, outt = 0, appendd = 0;
		cmds.push_back(";");
		string execme = "", lastexecme = "";
		int lastcmd = 0;
		for(unsigned i = 0; i < cmds.size(); ++i){
			if(is_number(cmds.at(i)) && i+1 < cmds.size() && (cmds.at(i+1) == ">>" || cmds.at(i+1) == ">"));
			else if(cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == "<<<" || cmds.at(i) == ";"){
				if(lastcmd == 0){
					if(lastexecme != ""){
						rval = execredir(lastexecme, execme, 0, 0);
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				else if(lastcmd == 1){
					if(lastexecme != ""){
						if(numoutdetect){
							if(numout == 0)
								rval = execredir(lastexecme, execme, numout, 3);
							else
								rval = execredir(lastexecme, execme, numout, 4);
						}
						else
							rval = execredir(lastexecme, execme, 1, 1);
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				else if(lastcmd == 2){
					if(lastexecme != ""){
						if(numappenddetect){
							if(numappend == 0)
								rval = execredir(lastexecme, execme, numappend, 5);
							else
								rval = execredir(lastexecme, execme, numappend, 6);
						}
						else
							rval = execredir(lastexecme, execme, 1, 2);
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				else if (lastcmd == 3){
					if(lastexecme != ""){
						if(execme.at(0) == '"' && execme.at(execme.size()-2) == '"')
							cout << execme.substr(1, execme.size()-3) << endl;
						else
							cout << execme << endl;
						rval = 0;
						lastexecme = "";
					}
					else
						lastexecme = execme;
				}
				if(cmds.at(i) == "<"){
					lastcmd = 0;
					if(inn == 1 || inn2 == 1){
						cerr << "Error: Only one input allowed. Only first was executed." << endl;
						break;
					}
					++inn;
				}
				else if(cmds.at(i) == ">"){
					lastcmd = 1;
					if(outt == 1 || appendd == 1){
						cerr << "Error: Only one output allowed. Only first was executed." << endl;
						break;
					}
					++outt;
				}
				else if(cmds.at(i) == ">>"){
					lastcmd = 2;
					if(outt == 1 || appendd == 1){
						cerr << "Error: Only one output allowed. Only first was executed." << endl;
						break;
					}
					++appendd;
				}
				else if(cmds.at(i) == "<<<"){
					lastcmd = 3;
					if(inn == 1 || inn2 == 1){
						cerr << "Error: Only one input allowed. Only first was executed." << endl;
						break;
					}
					++inn2;
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
	return rval;
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
		if(getready.at(0) == "cd"){
			if(getready.size() < 2){
				cerr << "Error: no parameters. Directory not changed." << endl;
				return -1;
			}
			else
				return changeDir(getready.at(1));
		}
	}
	/*for(unsigned i = 0; i < getready.size(); ++i){
		if(getready.at(i) == "|")
			++pipedetect;
	}
	if(pipedetect > 0){
		execPiping(getready);
	}
	*/
	for(unsigned i = 0; i < getready.size(); ++i){
		if(getready.at(i) == "<" || getready.at(i) == ">"	|| getready.at(i) == ">>" || getready.at(i) == "<<<"){
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
		findCommands(argchar);
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
		else if(fixer.at(i) == '<' && (i+2 < fixer.size() && fixer.at(i+2) == '<' && fixer.at(i+1) == '<')){
			if(i+3 < fixer.size() && fixer.at(i+3) != ' ')
				fixer.insert(i+3, " ");
			if(fixer.at(i-1) != ' '){
				fixer.insert(i, " ");
				++i;
			}
			i += 2;
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
	signal(SIGINT, handler);
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
