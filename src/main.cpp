#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/types.h>

using namespace std;
string cleanup(string cmd){
	if(!cmd.empty()){	
			//delete preceeding comments or white space
		while((cmd.at(0) == ';' || cmd.at(0) == ' ' || cmd.at(0) == '\t')){
			cmd.erase(0,1);
			if(cmd.empty())
				break;
		}
	}

		//check not duplicate, may become empty after previous task
	if(!cmd.empty()){
		//delete superceeding comments or white space
		while((cmd.at(cmd.size() - 1) == ';' || cmd.at(cmd.size() - 1) == ' ' || cmd.at(cmd.size() -1) == '\t')){
			cmd.resize(cmd.size() - 1);
			if(cmd.empty())
				break;
		}
	}
	return cmd;
}
//make a vector of strings into a vector of char *'s
vector<char*> convertvec(vector<string> conv){
	vector<char*> charvec;

	for(int i = 0; i < conv.size(); ++i){
		string temp = conv.at(i);
		char *piece = new char[conv.size() + 1];
		charvec.push_back(strcpy(piece, temp.c_str()));
	}
	charvec.push_back(NULL);
	return charvec;
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
/*		for(int i = 0; i < getready.size(); ++i){
			cout << getready.at(i) << endl;
		} */
	}
	vector<char*> charvec = convertvec(getready);	
	char *argchar[getready.size()]; 
	for(int i = 0; i < charvec.size(); ++i){
		argchar[i] = charvec.at(i);
	}
/*	for(int i = 0; argchar[i] != '\0'; ++i){
		cout << "char array: " << argchar[i] << endl;
	} */
	int pid = fork();
//	if(pid == -1){
//		perror("Error with fork()");
//		exit(1);
//	}
	if(pid == 0){
		int r = execvp(argchar[0], argchar);
		perror("Exec failed");
		exit(1);
	}
	else if(pid != 0){
		if(-1 == wait(0)){

		if(wait(&var) != pid)
			perror("Error with wait()");
			return -1;
		}

		return var;
	}
}

string specialspacing(string fixer){
	for(int i = 1; i < fixer.size(); ++i){
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
	}
	return fixer;
}

void runterminal(){
	vector<string> cmdline;
	string command = "";
	struct passwd *pass = getpwuid(getuid());
	char *curuser = pass->pw_name;
	char charhost[100];
	gethostname(charhost, sizeof charhost);
	string curhost = charhost;	
	if(curhost.find('.') != std::string::npos)
		curhost.resize(curhost.find('.'));
	while(true){
		//output prompt and take in command line
		cout << curuser << '@' << curhost << ' ';
		cout << '$' << ':';
		getline(cin, command);

		if(cin.fail()){
			cout << "exiting" << endl;
			return;
		}

		//if comment, disregard everything following it
		if(command.find('#') != std::string::npos){
			command.resize(command.find('#'));
		}

		command = cleanup(command);	
		//if any commands exist (other than spaces or comments) process commands
		if(command.size() > 0){
			command = specialspacing(command);
		//	cout << "command: " << command << endl;
			command.c_str();
			//split command line by spaces and tabs and push into cmdline vector
			boost::split(cmdline, command, boost::is_any_of(" , \t"),	
				  boost::token_compress_on);

			int conditional;
			string execme = "";	
			for(int i = 0; i < cmdline.size(); ++i){
				//if first command is && or ||, do nothing
				if(execme == "" && (cmdline.at(i) == "&&" || cmdline.at(i) == "||"));

				//if && or || is found in between commands, act accordingly
				else if(cmdline.at(i) == "&&" || cmdline.at(i) == "||" || cmdline.at(i) == ";"){ 
					//if its &&
					if(cmdline.at(i) == "&&"){
						conditional = execute(execme);
						//if first command failed with &&, skip the rest for that command
						if(conditional == 0){
							if(i+1 < cmdline.size()){
								while(cmdline.at(i+1) != ";" && i+1 < cmdline.size()) 
									++i;
							}
						}
						execme = "";
					}
					//if its ||
					else if(cmdline.at(i) == "||"){
						conditional = execute(execme);
						//if first command succeeds with ||, skip the rest for that command
						if(conditional != 0){
							if(i+1 < cmdline.size()){
								while(cmdline.at(i+1) != ";" && i+1 < cmdline.size()) 
									++i;
							}
						}
						execme = "";
					}
					//if its a ;, just execute normally
					else{
						execute(execme);
						execme = "";
					}
				}
				//if a normal command, add to execme for later processing
				else{
					execme.append(cmdline.at(i));
					execme.append(" ");
				}
			}
			//if there are unexecuted commands after loop ends, execute them
			if(execme != ""){
				bool conditional = true;
				execute(execme);
				execme = "";
			}
		}
	}	
}	

int main(){
	runterminal();	
	return 1;
}
