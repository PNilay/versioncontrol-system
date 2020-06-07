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
				create((void*)conn, buffer,len);
			}

			else if(strcmp(buffer, "destroy") == 0)
			{
				//Lock:
				pthread_mutex_lock(&thread);

				//Send Conformation Message to client:
				char* conformation = "OK";
				char* Success = "Success";
				char* failure = "Failure";

				write(conn->sock, conformation, strlen(conformation));
				bzero(buffer, sizeof(buffer));

				//Read Length and Project Name:
				read(conn->sock, &len, sizeof(int));
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);

				int temp =0;
				temp = Project_Exists(Proj_name, "./Server_Repository");
				if(temp ==1 ){
					//printf("Project Exists\n");
					write(conn->sock, Success, strlen(Success));
					//Expire any painding commits:
					//Delete all the files under project and send back a sucess message:
					Commit_Remove(Proj_name);
					Remove_Files_From_Proj(Proj_name);
					char Proj_name_path[strlen(Proj_name)+strlen("./Server_Repository/")];
					sprintf(Proj_name_path,"./Server_Repository/%s", Proj_name);
					char Proj_hist[strlen(Proj_name)+strlen("./Server_Repository/Project_History/.History")];
					sprintf(Proj_hist,"./Server_Repository/Project_History/%s.History" ,Proj_name );
					remove(Proj_hist);
					remove(Proj_name_path);
				}
				else{//printf("Project does not Exists\n");
				write(conn->sock, failure, strlen(failure));}


				free(Proj_name);

				//UNLOCK:
				pthread_mutex_unlock(&thread);
			}
			else if(strcmp(buffer, "checkout") == 0)
			{
				//Read length of project name and Project name:
				read(conn->sock, &len, sizeof(int));
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);

				int m = Project_Exists(Proj_name, "./Server_Repository");
				if(m ==1)
				{	int Sucess = 1;
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
				{	int Sucess = 1;
					char send[2*strlen(Proj_name)+40];// = "Test/filename1.txt:Test/filename2.txt:";
					sprintf(send, "./Server_Repository/%s/%s.Manifest",Proj_name, Proj_name);

					char Filename[strlen(Proj_name)+10];
					sprintf(Filename, "%s.Manifest",Proj_name);

					char PATH[strlen("./Server_Repository/")+strlen(Proj_name)];
					sprintf(PATH, "./Server_Repository/%s", Proj_name);

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
					sprintf(send, "./Server_Repository/%s/%s.Manifest",Proj_name, Proj_name );

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

					Send_File_Protocol_initalize(send, Proj_name);

					int fdT = open("./sendfile.txt", O_RDWR, 00700);
					int status = -1;
					int size;
					struct stat st;
					stat("./sendfile.txt", &st);
					size = st.st_size;
					char buffer[size+1];
					status = read(fdT, buffer, size);
					close(fdT);
					buffer[size] = '\0';
					size = size+1;

					write(conn->sock, &size, sizeof(int));
					write(conn->sock, buffer,size);
					remove("./sendfile.txt");

					//Seneding Server's Manifest file:
					//send manifest file:
					char send2[2*strlen(Proj_name)+40];// = "Test/filename1.txt:Test/filename2.txt:";
					sprintf(send2,"./Server_Repository/%s/%s.Manifest", Proj_name, Proj_name );

					int fdT2 = open(send2, O_RDWR, 00700);//Server manifest
					int status2 = -1;
					int size2;
					struct stat st2;
					stat(send2, &st2);
					size2 = st2.st_size;
					char buffer2[size2+1];
					do
					{
						status2 = read(fdT2, buffer2, size2);
					}while(status2 > 0);
					close(fdT2);

					buffer2[size2] = '\0';
					size = size2+1;

					write(conn->sock, &size2, sizeof(int)); //size of server's manifest
					write(conn->sock, buffer2,size2); //Server's manifest contents
				}
			}
			else if(strcmp(buffer, "rollback")==0)
			{
				//Read length of project name & Project_name:
				read(conn->sock, &len, sizeof(int));
				Proj_name = (char *)malloc((len+1)*sizeof(char));
				Proj_name[len] = 0;
				read(conn->sock, Proj_name, len);

				printf("[rollback]: Project_name length and Project_name recived: %d,  %s\n",len, Proj_name );

				//Read project version for rollback:
				int version_number = -1;
				read(conn->sock, &version_number, sizeof(int));
				// printf("Current version rollback :%d\n",version_number );
				int Sucessful = 0;
				if(version_number > -1)
				{
					int m = Project_Exists(Proj_name, "./Server_Repository");
					if(m == 1)
					{ //Projcet exists in server's repository:
					//Check for rollback version old file in Project old version Folder:
					char Rollback_File_Path[strlen("./Server_Repository/Old_Version//Version.txt")+strlen(Proj_name)+50];
					sprintf(Rollback_File_Path,"./Server_Repository/Old_Version/%s/Version%d.txt",Proj_name,version_number);
					struct stat buffer;
					int exists = stat(Rollback_File_Path, &buffer);
					if(exists == 0)
					{
						//rollback old version file exists:
						//If rollback file exists read contetents of version File
						int fdT_rollback = open(Rollback_File_Path, O_RDWR, 00700);//Server manifest
						int status_rollback = -1;
						int size_rollback;
						struct stat st_rollback;
						stat(Rollback_File_Path, &st_rollback);
						size_rollback = st_rollback.st_size;
						char buffer_rollback[size_rollback+1];
						do
						{
							status_rollback = read(fdT_rollback, buffer_rollback, size_rollback);
						}while(status_rollback > 0);
						close(fdT_rollback);

						buffer_rollback[size_rollback] = '\0';

						//--> Delete all most recent version saved on server side
						Old_Version_Remove(Proj_name, version_number);
						//Clean out all the contents form Project_name folder:
						Remove_Files_From_Proj(Proj_name);
						//--> Recreate all files in rollback version txt file
						ReCreate_files(buffer_rollback);
						Sucessful = 1;
						write(conn->sock, &Sucessful, sizeof(int));//Sending sucess
					}
					else
					{
						//rollback old version file doesn't exists:
						printf("[rollback]: Rollback file does not exists:\n" );
						Sucessful = 0;
						write(conn->sock, &Sucessful, sizeof(int));//Sending sucess
					}
				}
				else
				{
					//Project doesn't exists in server's repository:
					Sucessful = 0;
					write(conn->sock, &Sucessful, sizeof(int));//Sending sucess
				}
			}
			else
			{
				Sucessful = 0;
				write(conn->sock, &Sucessful, sizeof(int)); // sending '0' -> unsucessful
			}
		}


		else if(strcmp(buffer, "commit") == 0)
		{
		//Read length of project name & Project_name:
		read(conn->sock, &len, sizeof(int));
		Proj_name = (char *)malloc((len+1)*sizeof(char));
		Proj_name[len] = 0;
		read(conn->sock, Proj_name, len);

		printf("[Commit]: Project_name length and Project_name recived: %d,  %s\n",len, Proj_name );

		int m = Project_Exists(Proj_name, "./Server_Repository");
		if(m ==1)
		{	  int Sucess = 1;
			write(conn->sock, &Sucess, sizeof(int));//Sending sucess

			//send manifest file:
			char send[2*strlen(Proj_name)+40];// = "Test/filename1.txt:Test/filename2.txt:";
			sprintf(send,"./Server_Repository/%s/%s.Manifest", Proj_name, Proj_name );


			int fdT = open(send, O_RDWR, 00700);//Server manifest
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

			write(conn->sock, &size, sizeof(int)); //size of server's manifest
			write(conn->sock, buffer,size); //Server's manifest contents

			int nike = -1;
			read(conn->sock, &nike, sizeof(int));

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
			}

		}
	}

	else if(strcmp(buffer, "history")==0)
	{
		//Read length of project name:
		read(conn->sock, &len, sizeof(int));
		Proj_name = (char *)malloc((len+1)*sizeof(char));
		Proj_name[len] = 0;
		read(conn->sock, Proj_name, len);

		int m = Project_Exists(Proj_name, "./Server_Repository");
		if(m ==1)
		{	int Sucess = 1;
			write(conn->sock, &Sucess, sizeof(int));
			//Send History file for project:
			char Hist_path[strlen(Proj_name)+strlen("./Server_Repository/Project_History/.History")+2];
			sprintf(Hist_path, "./Server_Repository/Project_History/%s.History",Proj_name);
			int fdT = open(Hist_path, O_RDWR, 00700);
			struct stat st1;
			int size1 =0;
			stat(Hist_path, &st1);
			size1 = st1.st_size;
			char buf2[size1];
			read(fdT,buf2,size1);
			close(fdT);
			buf2[size1] = '\0';

			//Write length and data of history file to client:
			write(conn->sock,&size1, sizeof(int));
			write(conn->sock,buf2, size1 );

		}
	}

	else if(strcmp(buffer, "push") == 0)
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

			//Lock the respository so no other client can access this part:
			//LOCK:
			pthread_mutex_lock(&thread);
			printf("[push]: Locked\n");
			write(conn->sock, &Sucess, sizeof(int));//Sucessfully locked a respository

			//Recive Length of commit file and Commit file:
			int size =0;
			read(conn->sock, &size, sizeof(int));
			char Commit_Data[size];
			read(conn->sock, Commit_Data,size);
			Commit_Data[size] = '\0';

			char copy_commit_data[strlen(Commit_Data)];
			strcpy(copy_commit_data, Commit_Data);


			write(conn->sock, &Sucess, sizeof(int));

			int newsize =0;
			//Recive Client side Project Data and Lenght:
			read(conn->sock, &newsize, sizeof(int));
			char Important_Client_Data[newsize];
			read(conn->sock, Important_Client_Data,newsize);
			Important_Client_Data[newsize] = '\0';

			write(conn->sock, &Sucess, sizeof(int)); //<=== Sucess of Failuer for reciving all files

			//Compare current commit file with all other commit files inside  Project:
			int result = -1;
			result = Compare_Commit_Remove(Commit_Data, Proj_name);

			if(result == 1)
			{
				// printf("Result eual 1\n");
				Save_Old_Version(Proj_name);
				// printf("Save old version of project: \n");
				//Remove Files in commit which are marked with Delete:
				Remove_Old_files_Commit(Proj_name);
				//Write all the files sent by Client to Project Folder:
				ReCreate_files(Important_Client_Data);
				//Update Server's Manifest File:
				Update_Manifest_Push(Proj_name);
				//Update and Increment Version number for manifest File:
				Project_VersionIncrement(Proj_name);
				//Send new server's Manifest file to client:

				char Manifest_file[strlen(Proj_name)+strlen("./Server_Repository//.Manifest")+2*strlen(Proj_name)];
				sprintf(Manifest_file, "./Server_Repository/%s/%s.Manifest", Proj_name, Proj_name);
				int fdT = open(Manifest_file, O_RDWR, 00700);
				struct stat st1;
				int size1 =0;
				stat(Manifest_file, &st1);
				size1 = st1.st_size;
				char buf2[size1];
				read(fdT,buf2,size1);
				buf2[size1] ='\0';
				close(fdT);

				write(conn->sock,&size1, sizeof(int));
				write(conn->sock, buf2, size1);
				printf("[push]: Push Sucess\n" );
			}
			//Unlock:
			pthread_mutex_unlock(&thread);
			printf("[push]: Unlocked\n");

		}
	}


	else{
		// printf("[S]: Message from Client: %s\n", buffer);
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
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock <= 0){
		printf("[S]: Error in connection.\n");
		exit(1);
	}
	printf("[S]: Server Socket is created.\n");

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	int ret;
	ret = bind(sock, (struct sockaddr*)&address, sizeof(struct sockaddr_in));
	if(ret < 0){
		printf("[S]: Error in binding.\n");
		exit(1);
	}
	printf("[S]: Bind to port %i\n",port);

	//listen on port
	if(listen(sock, 20) < 0){
		printf("[S]: Can not listen!\n");
	}
	printf("[S]: Listening.......\n");


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
