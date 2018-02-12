//g++ -g -Wall -std=c++0x -pthread server.cpp -o server

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sstream> 		//stringstream
#include <string>
#include <thread>
#include <vector>
#include <map> 			//element map
#include <fstream>		//ecriture fichier

#define PORT 666

using namespace std; //On utilise un espace de noms ici

int numberOfRoom=5;
int player=0;
map <int, vector <string>> chanList;
map <string, string> pseudoList;

//Request recovery and transformation into string
string readToString(int sock){
    char buffer[1024] = {0};
    int valRead;
	string transForm;
	while(true){
		valRead = read( sock , buffer,1024);
		if(valRead!=1024){
			buffer[valRead]='\0';
		}
		stringstream ss;
		ss << transForm << buffer;
		transForm = ss.str();
		if(valRead!=1024){
			return transForm;
		}
	}
}
void saveRoomText(string message, int room){
	string name;
	stringstream ssName;
	FILE * file;
	message+="\n";
	ssName << "chat" << room << ".txt";
	name = ssName.str();
	file = fopen (name.c_str(),"a+");
	if (file!=NULL){
		fputs(message.c_str(),file);			
	}
	fclose (file);
}
//Management of different requests of a user
void newSocket(int socket){


	int idP=player++;

    string Pseudo="pseudoSckt";
    string Info="infoSckt";
    string Message="messSckt";
    string Send="sendSckt";

   	while(true){
	    string msgRead = readToString(socket);
		vector<string> split;
		char *str;
		str = const_cast<char *>(msgRead.c_str());
		char * pointerOfChar;
		pointerOfChar = strtok (str,"&");
		while (pointerOfChar != NULL){
			string tmp = pointerOfChar;
			split.push_back(tmp);
			pointerOfChar = strtok (NULL, "&");
		}
	    if(msgRead.length()==0){
	    	return;
	    }
		if(split[0].compare(Pseudo)==0){
			string msgReturn ="1";
			msgRead = msgRead.substr(11,msgRead.length()-4);
			pseudoList[msgRead]=msgRead;
			send(socket , msgReturn.c_str() , msgReturn.length() , 0 );
			printf("new user %s%d\n",(msgRead.c_str()),idP );
		}
		if(split[0].compare(Info)==0){
			string msgInfo ="0&";
			stringstream ss;
			ss << msgInfo << numberOfRoom << "&";
			msgInfo = ss.str();
			send(socket , msgInfo.c_str() , msgInfo.length() , 0 );
		}
		if(split[0].compare(Message)==0){
			stringstream ss;
			for (unsigned int i = 0; i != chanList[atoi(split[1].c_str())].size(); i++){
				ss << chanList[atoi(split[1].c_str())][i] << "\n";
			}
			string msgInfo=ss.str();
			send(socket , msgInfo.c_str() , msgInfo.length() , 0 );
			
		}
		if(split[0].compare(Send)==0){
			stringstream ss;
			ss << split[2] << "#" << idP << " : " << split[3];
			chanList[atoi(split[1].c_str())].push_back(ss.str());
			//append words in specific room text file
			saveRoomText(ss.str(),atoi(split[1].c_str()) );
		}
	}
}
//Backup of different servers on txt files
int main(int argc, char const *argv[]){
	int addrlen;
	int threadCpt=0;
	struct sockaddr_in address;
	int server_fd;
	for (int i = 0; i < numberOfRoom; ++i){
		if(chanList[i].size()==0){
			stringstream ss;
			ss << "Server : Hello room " << i << ".";
			chanList[i].push_back(ss.str());
		}
    }
    string text="";
    int opt = 1;
    addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("server run\n");
    vector<thread> threads;
	while(true){
		while (player+5>=threadCpt){
			int clientSocket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);
			threads.emplace_back(newSocket,clientSocket);
			threadCpt++;
			for (thread & t : threads) {
				if(t.joinable()==false)
					t.join();
			}
		}
	}
    return 0;
}

