#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include "Server_Functions.h"

pthread_mutex_t thread = PTHREAD_MUTEX_INITIALIZER; 
typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;

void * process(void * ptr)
{
	char * buffer, *Proj_name;
	int len;
	connection_t * conn;
	long addr = 0;

	if (!ptr) pthread_exit(0); 
	conn = (connection_t *)ptr;
	
	while(1){
			//read length of message 
			read(conn->sock, &len, sizeof(int));
		if (len > 0)
		{
			buffer = (char *)malloc((len+1)*sizeof(char));
			buffer[len] = 0;
			 //read message 
			read(conn->sock, buffer, len);
			
			if(strcmp(buffer, ":exit") == 0){
				printf("[S]: Disconnected from Client\n");
				break;
				}
			else if(strcmp(buffer, "create") == 0)
			{
				//printf("Create\n");
				create((void*)conn, buffer,len);
			}
			
			else if(strcmp(buffer, "destroy") == 0)
			{
				//LOCK:
				pthread_mutex_lock(&thread);
				//printf("Locked\n");
				
				//printf("destroy Function called\n");
				//Send Conformation Message to client:
				char* conformation = "OK";
				char* Success = "Success";
				char* failure = "Failure";
				//printf("[S]: Message from Client: %s\n", buffer);
				
				write(conn->sock, conformation, strlen(conformation));	
				bzero(buffer, sizeof(buffer));
				
				//Read Length of Project Name:
				read(conn->sock, &len, sizeof(int));
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);
				//Print name of Project:
				//printf("Project name :%s\n", Proj_name);
				int temp =0;
				temp = Project_Exists(Proj_name, "./Server_Repository");
				if(temp ==1 ){//printf("Project Exists\n");
					write(conn->sock, Success, strlen(Success));
					//Expire any painding commits:
					//Delete all the files uder project and send back a sucess message:
					Commit_Remove(Proj_name);
					Remove_Files_From_Proj(Proj_name);
					}
				else{//printf("Project does not Exists\n");
					write(conn->sock, failure, strlen(failure));}
					
					
				free(Proj_name);
				//Send Success Message to Client:
				//write(conn->sock, Success, strlen(Success));
				
				//UNLOCK:
				pthread_mutex_unlock(&thread);
				//printf("Unlocked\n");
			}
			else if(strcmp(buffer, "checkout") == 0)
			{
				//Read length of project name:
				read(conn->sock, &len, sizeof(int));
				//Read Project name:
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);				
			  //char *Project_Name = "Test";
			  char* Sucess = "Success";
			 
			  int m = Project_Exists(Proj_name, "./Server_Repository");
			  if(m ==1)
			{	  int Sucess = 1;
				write(conn->sock, &Sucess, sizeof(int));
			
			  int fd = open("./SendProject.txt", O_CREAT|O_RDWR, 00700);
			  Send_Project_Protocol(Proj_name,fd);
			  close(fd);
			  	
			  	int fdT = open("./SendProject.txt", O_RDWR, 00700);
				int size;
				struct stat st;
				stat("./SendProject.txt", &st);
				size = st.st_size;
				
				int status = -1;
				char buffer[size];
				do
				{
				status = read(fdT, buffer, size);
				}while(status > 0);
				close(fdT);
				
				//Write all the data to Client:
				//Send length of data:
				len = strlen(buffer);
				write(conn->sock, &len, sizeof(int));
				//Send data:
				write(conn->sock, buffer, len);
				free(Proj_name);
				//printf("BUFFER: %s\n",buffer);
				remove("./SendProject.txt");
				}
				else
				{
						//printf("Project does not exists on Server side\n");
						int fail = -1;
						write(conn->sock, &fail, sizeof(int));
				}
				
			}
			else if(strcmp(buffer, "currentversion") == 0)
			{
			//Read length of project name:
				read(conn->sock, &len, sizeof(int));
				//Read Project name:
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);				
			  			 
			  int m = Project_Exists(Proj_name, "./Server_Repository");
			  if(m ==1)
			{	  int Sucess = 1;

				
				
				char send[2*strlen(Proj_name)+40];// = "Test/filename1.txt:Test/filename2.txt:";
				char *repo = "./Server_Repository/";
				strcpy(send, repo);
				strcat(send, Proj_name);
				strcat(send, "/");
				strcat(send,Proj_name);
				strcat(send, ".Manifest");
				
				char Filename[strlen(Proj_name)+10];
				char *mn = ".Manifest";
				strcpy(Filename, Proj_name);
				strcat(Filename, mn);
				
				char PATH[strlen(repo)+strlen(Proj_name)];
				strcpy(PATH, repo);
				strcat(PATH, Proj_name);
				
				if(File_Exists(Filename, PATH) == 1)
				{
				write(conn->sock, &Sucess, sizeof(int));				
				int fdT = open(send, O_RDWR, 00700);
				int status = -1;				
				int size;
				struct stat st;
				stat(send, &st);
				size = st.st_size;
				char buffer[size];
				do
				{
				status = read(fdT, buffer, size);
				}while(status > 0);
				close(fdT);
				write(conn->sock, &size, sizeof(int));
				write(conn->sock, buffer,size);
			}
			else
			{
				int fail = 0;
				write(conn->sock, &fail, sizeof(int));
			}
			}
			else
			{
				int fail = 0;
				write(conn->sock, &fail, sizeof(int));
			}	
			}
			
			else if(strcmp(buffer, "update") == 0)
			{
				//Read length of project name:
				read(conn->sock, &len, sizeof(int));
				//Read Project name:
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);				
			  			 
			  int m = Project_Exists(Proj_name, "./Server_Repository");
			  if(m ==1)
			{	  int Sucess = 1;
				write(conn->sock, &Sucess, sizeof(int));
				
				
				char send[2*strlen(Proj_name)+40];// = "Test/filename1.txt:Test/filename2.txt:";
				char *repo = "./Server_Repository/";
				strcpy(send, repo);
				strcat(send, Proj_name);
				strcat(send, "/");
				strcat(send,Proj_name);
				strcat(send, ".Manifest");
				
				int fdT = open(send, O_RDWR, 00700);
				int status = -1;				
				int size;
				struct stat st;
				stat(send, &st);
				size = st.st_size;
				char buffer[size];
				do
				{
				status = read(fdT, buffer, size);
				}while(status > 0);
				close(fdT);
				//printf("BUFFER:--------------- %s\n",buffer);
				write(conn->sock, &size, sizeof(int));
				write(conn->sock, buffer,size);
				//printf("TEST DONE!\n" );
			}
		}
		
		else if(strcmp(buffer, "upgrade") == 0)
			{
				//Read length of project name:
				read(conn->sock, &len, sizeof(int));
				//Read Project name:
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);				
			  			 
			  int m = Project_Exists(Proj_name, "./Server_Repository");
			  if(m ==1)
			{	  int Sucess = 1;
				write(conn->sock, &Sucess, sizeof(int));
				
				//Read length of filenames:
				read(conn->sock, &len, sizeof(int));
				char send[len];
				//Read sendfiles name:
				read(conn->sock, send, len);
				//printf("send file list ----------------->%s\n",send);
				Send_File_Protocol_initalize(send, Proj_name);
				
				int fdT = open("./sendfile.txt", O_RDWR, 00700);
				int status = -1;				
				int size;
				struct stat st;
				stat("./sendfile.txt", &st);
				size = st.st_size;
				//printf("size:----------%i---- %i\n",counter, size);
				char buffer[size+1];
			//	do
			//	{
				status = read(fdT, buffer, size);
				//}while(status > 0);
				close(fdT);
				buffer[size] = '\0';
				size = size+1;
				//printf("BUFFER:-------%i--------  %s\n",size,buffer);
				int r =0;
				write(conn->sock, &size, sizeof(int));
				write(conn->sock, buffer,size);
			    remove("./sendfile.txt");
				//printf("TEST DONE!\n" );
			}
		}
		
		
		else if(strcmp(buffer, "commit") == 0)
			{//printf("commit-------------------------------------------\n");
				//Read length of project name:
				read(conn->sock, &len, sizeof(int));
				//Read Project name:
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);				
			  			 
			  int m = Project_Exists(Proj_name, "./Server_Repository");
			  if(m ==1)
			{	  int Sucess = 1;
				write(conn->sock, &Sucess, sizeof(int));
				
				//send manifest file:				
				char send[2*strlen(Proj_name)+40];// = "Test/filename1.txt:Test/filename2.txt:";
				char *repo = "./Server_Repository/";
				strcpy(send, repo);
				strcat(send, Proj_name);
				strcat(send, "/");
				strcat(send,Proj_name);
				strcat(send, ".Manifest");
				Fix_Manifest(Proj_name);
				int fdT = open(send, O_RDWR, 00700);
				int status = -1;				
				int size;
				struct stat st;
				stat(send, &st);
				size = st.st_size;
				char buffer[size+1];
				do
				{
				status = read(fdT, buffer, size);
				}while(status > 0);
				close(fdT);
				buffer[size] = '\0';
				size = size+1;
				//printf("BUFFER:--------------- %s\n",buffer);
				
				write(conn->sock, &size, sizeof(int));
				write(conn->sock, buffer,size);
				int nike = -1;
				read(conn->sock, &nike, sizeof(int));
				//printf("nike: %i\n",nike);
				if(nike == 1)
				{
				//Recive Length of commit file and Commit file:
				read(conn->sock, &size, sizeof(int));
				//printf("Length of Commit File: %i\n", size);
				char Commit_Data[size];
				read(conn->sock, Commit_Data,size);
				//printf("Commit Data: %s\n", Commit_Data);
				Add_Commit_File_To_Project(Proj_name, Commit_Data, size);
				
				Sucess = 1;
				write(conn->sock, &Sucess, sizeof(int));
				//printf("TEST DONE!\n" );
			}
				
			}
		}
		
		else if(strcmp(buffer, "push") == 0)
			{//printf("push-------------------------------------------\n");
				//Read length of project name:
				read(conn->sock, &len, sizeof(int));
				//Read Project name:
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);				
			  			 
			  int m = Project_Exists(Proj_name, "./Server_Repository");
			  if(m ==1)
			{	  int Sucess = 1;
				write(conn->sock, &Sucess, sizeof(int));
				
				//Lock the respository so no other client can access this part:
				
				//LOCK:
				pthread_mutex_lock(&thread);
				printf("Locked\n");
				write(conn->sock, &Sucess, sizeof(int));
				
				//Recive Length of commit file and Commit file:
				int size =0;
				read(conn->sock, &size, sizeof(int));
				char Commit_Data[size];
				read(conn->sock, Commit_Data,size);
				//printf("Commit Data: %s\n", Commit_Data);
				
				//printf("___________________________________________________\n");
				
				write(conn->sock, &Sucess, sizeof(int));
				
				int newsize =0;
				//Recive Client side Project Data and Lenght:
				read(conn->sock, &newsize, sizeof(int));
				//printf("Recived data length: %i\n",newsize);
				char Important_Client_Data[newsize];
				read(conn->sock, Important_Client_Data,newsize);
				//printf("Recived Data: \n%s\n", Important_Client_Data);
				
				write(conn->sock, &Sucess, sizeof(int));
				
				//Compare current commit file with all other commit files inside  Project:
				int result = Compare_Commit_Remove(Commit_Data, Proj_name);
				
				//printf("Compare COMMIT--------------------------------\n");
				if(result == 1)
				{
					//printf("Commit Found!\n");
					//Write Commit File to History file accordingly:
					//Write_Commit_tO_History();
					
					//Using Send_Project_Protocl(), create a single file which contains
					// whole Project Information and save that file inside 
					//Oldversion/Project_Name/Projetname(Version#).txt
					
					
					//check if old version compressed direcotory exists:
			//printf("compressed directory or uncompressed\n");
			/*char *Old_Compressed = "./Server_Repository/Compressed.tar.gz";
			if (isDirectoryExists(Old_Compressed) == 1)
			{
				//Unzip this compressed directory:
				system("tar -zxvf Compressed");
			}
			else
			{printf("No Compressed directory !\n");}*/
	//Oldversion Directory Path: ./Server_Repository/Old_Version/Proj_Name/Version#.txt
		char *Old_Version_Directory = "./Server_Repository/Old_Version";
		//printf("OldVersion Drectory: %s\n", Old_Version_Directory);
		if (isDirectoryExists(Old_Version_Directory) != 1)
		{
			//printf("Creating Direcorty!\n");
			int c = mkdir(Old_Version_Directory, 0700);
			//printf("c = %i\n",c);
		}
		//Now  check if project name folder exists in oldversion folder:
		char Old_Version_Directory_Proj_name[strlen(Old_Version_Directory)+strlen(Proj_name)+5];
		strcpy(Old_Version_Directory_Proj_name,Old_Version_Directory);
		char *y = "/";
		strcat(Old_Version_Directory_Proj_name,y);
		strcat(Old_Version_Directory_Proj_name,Proj_name);
		//printf("Old_Version_Directory_Proj_name: %s\n", Old_Version_Directory_Proj_name);
		if (isDirectoryExists(Old_Version_Directory_Proj_name) != 1)
		{
			//printf("Creating Direcorty!\n");
			int h = mkdir(Old_Version_Directory_Proj_name,0700);
//printf("h = %i\n",h);
		}
		
		//Get the Version number  of Current Manifest File:
		File_Info *Array;
		char versionNum[10];
		int size1 =0;
		
		char path_of_Manifest[strlen("./Server_Repository/")+2*strlen(Proj_name)+strlen(".Manifest")+5];
		char *inital = "./Server_Repository/";
		char *t ="/";
		char *Manifest = ".Manifest";
		strcpy(path_of_Manifest, inital);
		strcat(path_of_Manifest, Proj_name);
		strcat(path_of_Manifest, t);
		strcat(path_of_Manifest,Proj_name);
		strcat(path_of_Manifest, Manifest);
		Array = Get_ManifestFile_Info(path_of_Manifest, &size1,versionNum);
		
		//Old_Version_path: ./Server_Repository/Old_version/Proj_name/version#.txt
		char *version = "Version";
		char *end = ".txt";
		char Old_Version_Path[strlen(Old_Version_Directory_Proj_name)+15+strlen(versionNum)];
		strcpy(Old_Version_Path, Old_Version_Directory_Proj_name);
		strcat(Old_Version_Path,t);
		strcat(Old_Version_Path,version);
		strcat(Old_Version_Path, versionNum);
		strcat(Old_Version_Path, end);
		//printf("Old_Version_Path: %s\n",Old_Version_Path);
		
		int file = open(Old_Version_Path, O_CREAT|O_RDWR, 00700);
		Send_Project_Protocol(Proj_name, file);
		Remove_Files_From_Proj(Proj_name);
		//printf("File Removed: \n");
		//Write all the files sent by Client to Project Folder:
		ReCreate_files(Important_Client_Data);
		
		//Update and Increment Version number for manifest File:
		Project_VersionIncrement(Proj_name);
		
		
	}
				//Unlock:
				pthread_mutex_unlock(&thread);
				//printf("Unlocked\n");

			}
		}
		
		
		else{
				printf("[S]: Message from Client: %s\n", buffer);
				write(conn->sock, buffer, strlen(buffer));
				bzero(buffer, sizeof(buffer));
				}
		
	}
}	
	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}


void handler(int num)
{
printf("\n[S]: Closing Server Socket...\n");
exit(0);
}


int main(int argc, char const *argv[]){
	signal(SIGINT,handler);// This function carefully closes this program. 
	
	//Variables:
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t * connection;
	pthread_t thread;
//printf("Port number: %i, %d\n",port, port);
	if (argc != 2)
	{
		fprintf(stderr, "Invalid Input\n");
		return -1;
	}
	/* obtain port number */
	if (sscanf(argv[1], "%d", &port) <= 0)
	{
		fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
		return -2;
	}
			//printf("Port number: %i, %d\n",port, port);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock <= 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	int ret;
	ret = bind(sock, (struct sockaddr*)&address, sizeof(struct sockaddr_in));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %i\n",port);

   //listen on port 
	if(listen(sock, 20) < 0){
		printf("[-] Can not listen!\n");
	}
	printf("Listening.......\n");
	
	
	while(1){	
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, (socklen_t*)(&connection->addr_len));
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
	}
	return 0;
}

