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

	for(unsigned i = 0; i < conv.size(); ++i){
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
		/*for(int i = 0; i < getready.size(); ++i){
			cout << getready.at(i) << endl;
		} */
	}
	vector<char*> charvec = convertvec(getready);	
	char *argchar[300]; 
	for(unsigned i = 0; i < charvec.size(); ++i){
		argchar[i] = charvec.at(i);
	}
	/*for(int i = 0; argchar[i] != '\0'; ++i){
		cout << "char array: " << argchar[i] << endl;
	} */
	int pid = fork();
	//if(pid == -1){
	//	perror("Error with fork()");
	//	exit(1);
	//}
	if(pid == 0){
		execvp(argchar[0], argchar);
		perror("Exec failed");
		exit(1);
	}
	else if(pid != 0){
		while(wait(&var) != pid)
			perror("Error with wait()");

		return var;
	}
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
	}
	return fixer;
}

int determinest(vector<string> cline, int start){
	int num = 0;
	for(unsigned i = start; i < cline.size(); ++i){
		if(cline.at(i) == "||"){
			num = 2;
			break;
		}
	}
	return num;
}

void runterminal(){
	vector<string> cmdline;
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
		cout << '$' << ' ';
		string command = "";
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
			//cout << "command: " << command << endl;
			command.c_str();
			//split command line by spaces and tabs and push into cmdline vector
			boost::split(cmdline, command, boost::is_any_of(" , \t"),	
				  boost::token_compress_on);

			int conditional;
			string execme = "";	
			bool passed = true; 
			int det = 0;
			//0 = neither, 1 = and, 2 = or
			int lastcmd = 0;
			det = determinest(cmdline, 0);
		
			for(unsigned i = 0; i < cmdline.size(); ++i){
				//cout << "execme: " << execme << endl;
				//if first command is && or ||, do nothing
				if(execme == "" && (cmdline.at(i) == "&&" || cmdline.at(i) == "||"));

				//if && or || is found in between commands, act accordingly
				else if(cmdline.at(i) == "&&" || cmdline.at(i) == "||" || cmdline.at(i) == ";"){ 
					//if its &&
					if(cmdline.at(i) == "&&"){
						if(passed){
							conditional = execute(execme);
							if(conditional != 0){
								passed = false;
							}
							else
								passed = true;
						}
						execme = "";
						lastcmd = 1;
					}
					//if its ||
					else if(cmdline.at(i) == "||"){
						if(det == 2){	
							passed = false;
							det = 0;
						}
						if(!passed){
							conditional = execute(execme);
							if(conditional == 0)
								passed = true;
							else
								passed = false;
						}
						execme = "";
						lastcmd = 2;
					}
					//if its a ;, just execute normally
					
					else{
						if(passed){
							conditional = execute(execme);
							if(conditional == 0)
								passed = true;
							else
								passed = false;
						}
						//passed = true;
						determinest(cmdline, i);
						execme = "";
						lastcmd = 0;
					}
					//execute(execme);
					//execme = "";
				}
				//if a normal command, add to execme for later processing
				else{
					execme.append(cmdline.at(i));
					execme.append(" ");
				}
			}
			//if there are unexecuted commands after loop ends, execute them
			if(execme != ""){
				if(passed && lastcmd != 2){
					execute(execme);
				}
				else if(!passed && lastcmd == 2){
					execute(execme);
				}
				execme = "";
			}
		}
	}	
}	

int main(){
	runterminal();	
	return 1;
}
