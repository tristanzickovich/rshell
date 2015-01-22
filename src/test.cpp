#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

using namespace std;
/*
int executecommands(vector<string> commands){
	int pid = fork();
	if(pid == -1){
		perror("Error with fork().");
		exit(1);
	}
	else if(pid == 0){
		cout << "in the child process" << endl;
	}
	else if(pid > 0){
	}
}
*/
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

		if(!command.empty()){	
			//delete preceeding comments or white space
			while((command.at(0) == ';' || command.at(0) == ' ')){
				command.erase(0,1);
				if(command.empty())
					break;
			}
		}

		//check not duplicate, may become empty after previous task
		if(!command.empty()){
			//delete superceeding comments or white space
			while((command.at(command.size() - 1) == ';' || command.at(command.size() - 1) == ' ')){
				command.resize(command.size() - 1);
				if(command.empty())
					break;
			}
		}
		
		//if any commands exist (other than spaces, semi-colons, or comments) process commands
		if(command.size() > 0){
			command.c_str();
			//split command line by ';' or ' ', and push into cmdline vector
			boost::split(cmdline, command, boost::is_any_of(" ,;"),	
				  boost::token_compress_on);
			for(int i = 0; i < cmdline.size(); ++i){
				if(cmdline.at(i) == "exit")
					exit(1);
				else
					cout << cmdline.at(i) << endl;
			}
		}
	}	
}	

int main(){
	runterminal();	
	return 1;
}
