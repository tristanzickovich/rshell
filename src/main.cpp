#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
string cleanup(string cmd){
	if(!cmd.empty()){	
			//delete preceeding comments or white space
		while((cmd.at(0) == ';' || cmd.at(0) == ' ')){
			cmd.erase(0,1);
			if(cmd.empty())
				break;
		}
	}

		//check not duplicate, may become empty after previous task
	if(!cmd.empty()){
		//delete superceeding comments or white space
		while((cmd.at(cmd.size() - 1) == ';' || cmd.at(cmd.size() - 1) == ' ')){
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

void execute(string commands){
	vector<string> getready;
	commands = cleanup(commands);	
	if(commands.size() > 0){
		
		commands.c_str();
		boost::split(getready, commands, boost::is_any_of(" "),	
			  boost::token_compress_on);
	//	cout << "phrase to exec: " <<  commands << endl;
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
	if(pid == -1){
		perror("Error with fork().");
		exit(1);
	}
	else if(pid == 0){
		int r = execvp(argchar[0], argchar);
		perror("exec failed");
		exit(1);
	}
	else if(pid > 0){
		if(-1 == wait(0))
			perror("Error with wait().");
	}
}

void runterminal(){
	vector<string> cmdline;
	string command = "";
	while(true){
		//output prompt and take in command line
		cout << '$' << ' ';
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

		//if any commands exist (other than spaces, semi-colons, or comments) process commands
		if(command.size() > 0){
			command.c_str();
			//split command line by spaces and push into cmdline vector
			boost::split(cmdline, command, boost::is_any_of(" "),	
				  boost::token_compress_on);

			string execme = "";	

			for(int i = 0; i < cmdline.size(); ++i){
				if(cmdline.at(i) == "&&" || cmdline.at(i) == "||" || cmdline.at(i) == ";"){ 
					execute(execme);
					//cout << execme <<" fail ";
					execme = "";
				}
				else
					execme.append(cmdline.at(i));
					execme.append(" ");
			}
			if(execme != ""){
				execute(execme);
				execme = "";
			}
	//		cout << execme << endl;
		
		}
	}	
}	

int main(){
	runterminal();	
	return 1;
}
