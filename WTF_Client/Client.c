
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "Functions.h"
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>


//int StringCmp(const char *k, const char *ptr);

const char *Ip = NULL;
uint32_t Port = NULL;

int main(int argc, char const *argv[])
{
	//Check if enough argument is given, if not close this project.
	if (argc < 3)
	{
		printf("[C]: Invalid Input\n");
		return 0;
	}
	const char *delimiter = ":";
	 
  //./WTF configure <IP> <port>
  if(StringCmp(argv[1],"configure") ==0 && argc ==4)
  {
    printf("[C]: Configure!\n");
	int fd = open("./Configure.txt", O_CREAT|O_RDWR, 00700);
	Ip = argv[2];
    Port = strtoul(argv[3],NULL, 10);
	write(fd, Ip, strlen(Ip));
	write(fd, delimiter,1);
	char tmp[10];
	sprintf(tmp,"%d", Port);
	write(fd,tmp,strlen(tmp));
	close(fd);

      //printf("IP address: %s\n",Ip);
      //printf("Port number: %i\n",Port);

  }

  //./WTF checkout <project name>
  /*
  1. check if configure has ran on client side or not, and also check if project already exists on client side
    -If not then client can't communicate with Server
    -If project exists on client side then checkout command will fail
  2. Check if name exists on server side, if not checkout will fail
  3. If above both condition satisfyies request entire project from server
  */
  if(StringCmp(argv[1],"checkout") ==0)
  {
	if(argv[2] == NULL)
	{
	printf("[C]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: No hostname of port is given!\n");
			exit(0);
		}

		int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		int len;
		if(Project_Exists(argv[2], "./Client_Repository") == 1)
		{
		printf("[C]: Project already exists on client side\n");
		//Send the Exit message:
		len = strlen(":exit");
		write(sock,&len, sizeof(int));    
		write(sock,":exit", len);
		}
		else
		{		
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		
		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		
		write(sock , argv[2] , len);
		
		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			//read length of data
			read(sock, &len, sizeof(int));
			char Project[len+1];
			read(sock, Project,len);
			Project[len] = '\0';
			//Create a directory named "argv[2]":
			char Proj_Path[strlen(argv[2]+25)];
			char *in = "./Client_Repository/";
			strcpy(Proj_Path, in);
			strcat(Proj_Path,argv[2]);
			mkdir(Proj_Path, 0700);
			ReCreate_files(Project);
			printf("[C]: checkout command Successful!\n");
		}
		//Recive whole project directory from server:
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));    
		write(sock,exit, len);
		
		}
	}
  }

//./WTF create <project name> ===================================================================
  if(StringCmp(argv[1],"create") ==0)
  {
	if(argv[2] == NULL)
	{printf("[C]:  Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: No hostname of port is given!\n");
			exit(0);
		}	
    	int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		
		//First send Server "create" so that server know this client is 
		//asking for creating new project
		//Next Send Project name.
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		//printf("[C]: Instruction message sent\n");
		
		//Recive Conformation message about instruction from Server:
		//valread = read( sock , buffer, 1024);
		//printf("[C]: Instruction Conformed by Server?: %s\n",buffer );
		
		//Send length of Project name to Server:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		
		//Send Project name to Server:
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		//Recive sucess message from Server:
		valread = read( sock , buffer, 1024);
		//printf("[C]: Success Or Failuer?: %s\n",buffer );		
		
		if(StringCmp(buffer,"Success")==0)
		{
		//Create Directory named project inside Client Respotory:
		char* inti = "./Client_Repository/";
		int len = strlen(argv[2]);
		char path[2*len+35];

	    strcpy(path, inti);
		strcat(path,argv[2]);
		mkdir(path, 0700);	
		//Recive length of the manifest file from Server
		int rey =0;
		read( sock , &rey, sizeof(int));
		char data[rey];
		read(sock, data, rey);
		//printf("data: %s\n",data);
		ReCreate_files(data);
		printf("[C]: Create Command Sucessful\n");
		}
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));    
		write(sock,exit, len);
		
		//Close the socket:
		close(sock);
	}
  }
  
//./WTF destroy <project name> ===================================================================
if(StringCmp(argv[1],"destroy") ==0)
  {
	  if(argv[2] == NULL)
	{printf("[C]: Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: No hostname of port is given!\n");
			exit(0);
		}
    	int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		
		//First send Server "destroy" so that server know this client is 
		//asking server to destroy project, if exists
		//Next Send Project name.
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		//printf("[C]: Instruction message sent\n");
		
		//Recive Conformation message about instruction from Server:
		valread = read( sock , buffer, 1024);
		//printf("[C]: Instruction Conformed by Server?: %s\n",buffer );
		
		//Send length of Project name to Server for destroy:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		
		//Send Project name to Server:
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		//Recive sucess message from Server about existance of Project:
		valread = read( sock , buffer, 1024);
		//printf("[C]: Success Or Failuer?: %s\n",buffer );		
		
		if(StringCmp(buffer, "Failure") == 0)
		{
			printf("[C]: Server failed to destory the given project.\n");
		}
		else
		{
			printf("[C]: Project Removed!\n");
		}
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));    
		write(sock,exit, len);
		
		//Close the socket:
		close(sock);
	}
	
  }

//./WTF add <projectname> <filename> ============================================================
if(StringCmp(argv[1],"add") ==0)
  {
	 if(argv[2] == NULL || argv[3] == NULL)
	{printf("Invalid Command\n");}
	else
	{
	char Proj_path[strlen("./Client_Repository")+strlen(argv[2])];
	char * clientrepo = "./Client_Repository";
	strcpy(Proj_path,clientrepo);
	//strcat(Proj_path, argv[2]);	
	if(Project_Exists(argv[2], Proj_path) == 1)
	{
			//printf("Project Exists!\n");
			Add_File_To_Manifest(argv[2], argv[3]);
	}
	else
	{
		printf("[C]: Project does not exists!\n");
	}
	}
}

//./WTF remove <Projectname> <filename>=================================================
if(StringCmp(argv[1],"remove") ==0)
  {
	 if(argv[2] == NULL || argv[3] == NULL)
	{printf("Invalid Command\n");}
	else
	{
	char * clientrepo = "./Client_Repository";	
	if(Project_Exists(argv[2], clientrepo) == 1)
	{
			//printf("Project Exists!\n");
			Remove_File_From_Manifest(argv[2], argv[3]);
	}
	else
	{
		printf("[C]: Project does not exists!\n");
	}
	}
}
//./WTF currentversion <project name>==========================================================
if(StringCmp(argv[1],"currentversion") ==0)
{
	if(argv[2] == NULL)
	{printf("[C]: Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: NO hostname of port is given!\n");
			exit(0);
		}	
    	int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		//printf("[C]: Instruction message sent\n");
		
		//Send length of Project name to Server:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		
		//Send Project name to Server:
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		int result =-1;
		//Recive sucess message from Server:
		valread = read( sock , &result, sizeof(int));
		//printf("[C]: Success Or Failuer?: %i\n",result );
		if(result == 0)
		{
			printf("[C]: Projcet Does not Exist in Server!\n");
		}		
		
		if(result==1)
		{
		//Recive length of the manifest file from Server
		int rey =0;
		read( sock , &rey, sizeof(int));
		char data[rey];
		//Read Manifest file Data:
		read(sock, data, rey);
		//printf("data: %s\n",data);
		
		int fdT = open("./currentversion.txt", O_CREAT|O_RDWR, 00700);
		write(fdT,data,rey);
		close(fdT);
		File_Info *Array;
		int size1 =0;
		char versionNum[10];
		Array = Get_ManifestFile_Info("./currentversion.txt", &size1,versionNum);
		int i =0;
		printf("Version\t\tFile Path\n");
		for(i =0; i<size1; i++)
		{
			printf("%s\t\t%s\n",(Array+i)->Version,(Array+i)->name);
		}
		remove("./currentversion.txt");
		}
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));    
		write(sock,exit, len);
		
		//Close the socket:
		close(sock);
	}
}
 
 if(StringCmp(argv[1],"update") ==0)
 {
	 if(argv[2] == NULL)
	{
	printf("[C]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
//printf("IP:::::::::::::::: %s\n",Ip);
			if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: No hostname of port is given!\n");
			exit(0);
		}

//		printf("project does not exist on Client side\n");
		int sock = 0, valread;
	//	printf("IP:::::::::::::::: %s\n",Ip);
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		int len;
		if(Project_Exists(argv[2], "./Client_Repository") == 1)
		{
		//printf("project exists on client side\n");
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		//printf("[C]: Instruction message sent\n");
		
		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			//read length of Server's Manifest data
			read(sock, &len, sizeof(int));
			char Project[len+1];
			read(sock, Project,len);
			//Send the Exit message:
			int len2 = strlen(":exit");
			write(sock,&len2, sizeof(int));    
			write(sock,":exit", len2);
		
		
			Project[len] = '\0';
			//printf("Server's Manifest: %s\n",Project);
			
			//Write Server manifest to Client side:
			char *sinital = "./";
			char *sfinal = "_server.Manifest";
			char Server_Path[strlen(argv[2])+25];
			strcpy(Server_Path, sinital);
			strcat(Server_Path,argv[2]);
			strcat(Server_Path,sfinal);
			
			int fdT = open(Server_Path, O_CREAT|O_RDWR, 00700);
			write(fdT,Project,len+1);
			close(fdT);
			
			
			//Generate Client's Manifest:
			//ReGenerate_Client_Manifest(argv[2]); // This Function scan 
			//through all files in client side and generate Hash for every 
			//file in manifest 	
			
			//Comparision:
			char* intial = "./Client_Repository/";
			char Client_Path[strlen("./Client_Repository/")+2*strlen(argv[2])+strlen(".Manifest")+5];
			strcpy(Client_Path,intial);
			strcat(Client_Path,argv[2]);
			char *y = "/";
			strcat(Client_Path, y);
			strcat(Client_Path,argv[2]);
			char * final = ".Manifest";
			strcat(Client_Path, final);
			//printf("Client_Path: %s\n",Client_Path);
			
			File_Info *Array1;
			int size1;
			char versionNum1[10];
			
			Array1 = Get_ManifestFile_Info(Client_Path, &size1,versionNum1);
			
			
			File_Info *Array2;
			int size2;
			char versionNum2[10];
			
			Array2 = Get_ManifestFile_Info(Server_Path, &size2,versionNum2);	

			Compare_Manifest(Array1,size1,versionNum1, Array2,size2,versionNum2, argv[2]);
			remove(Server_Path);
			Fix_Update(argv[2]);

		}
		else
		{
			printf("[C]: project does not exists on server side\n");
		}
		}

	}
 }
 
  if(StringCmp(argv[1],"upgrade") ==0)
 {
	 if(argv[2] == NULL)
	{
	printf("[C]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
	//		printf("IP:::::::::::::::: %s\n",Ip);
			if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: No hostname of port is given!\n");
			exit(0);
		}

		int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		int len;
		
		char *update = ".Update";
		char filename[strlen(argv[2])+10];
		strcpy(filename, argv[2]);
		strcat(filename, update);
		
		if(File_Exists(filename, ".") == 1)
		{
		//printf("Update file exists!\n");
		
		//Check if project exists on Server side:
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		//printf("[C]: Instruction message sent\n");
		
		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			//printf("SUCESS!\n");
			int size = 0;
			char *init = "./";
			char update_path[strlen(argv[2])+10];
			strcpy(update_path, init);
			strcat(update_path,filename);
			
			Update_File_Info *array;
			array = Get_Update_File_Info(update_path, &size);
			int t =0;
			int send_len =0;
			//printf("send length: %i\n",send_len);
			for(t =0; t<size; t++)
			{
				//printf("(array+t)->name: %s\n",(array+t)->name);
				send_len = send_len +strlen((array+t)->name);
			}
			
			//printf("send length: %i\n",send_len);
			char *starting = "";
			char send_List[send_len+size];
			strcpy(send_List, starting);
			for(t =0; t<size; t++)
			{
				if(StringCmp((array+t)->update,"D")==0)
				{
				char FileName[strlen((array+t)->name)-strlen(argv[2])];
				int q =0;
				for(q =0; q<strlen((array+t)->name); q++)
				{
					FileName[q] = (array+t)->name[strlen(argv[2])+1+q];
				}
				Remove_File_From_Manifest(argv[2], FileName);
				}
				else
				{
					if(StringCmp((array+t)->update,"M")==0 || StringCmp((array+t)->update,"A")==0)
					{int u =0;
					char *c = ":";
					strcat(send_List, (array+t)->name);
					strcat(send_List,c);
				}
				}
			}
			//printf("Send_List: %s\n",send_List);
			
			//Send length of Send_List to Server:
			int length = strlen(send_List);
			write(sock, &length, sizeof(int));
			//Send Send_List to server:
			write(sock, send_List, length);
			//Recive length of files data:
			read(sock, &length, sizeof(int));
			char recived_data[length];
			//recive data:
			read(sock, recived_data, length);
			
			//printf("Recived Data: %s\n",recived_data);
			//ReCreate_files(recived_data);
			ReCreate_files_Change_Manifest(recived_data);
			//printf("update path: %s\n",update_path);
			remove(update_path);
			
			
			//Remove ./Proj_name.Update
			char Remove_Path[strlen(argv[2])+10];
			char *dot = "./"; 
			strcpy(Remove_Path,dot);
			strcat(Remove_Path, argv[2]);
			strcat(Remove_Path, update);
			remove(Remove_Path);
		}
		else
		{
			printf("[C]: Project does not exists on server side\n");
		}
		}
		else
		{
			printf("[C]: No Update File Exists, First run Update command\n");
		}
		
		//Send the Exit message:
		int len2 = strlen(":exit");
		write(sock,&len2, sizeof(int));    
		write(sock,":exit", len2);
		}
 }
 
 
  if(StringCmp(argv[1],"commit") ==0)
 {
	 if(argv[2] == NULL)
	{
	printf("[C]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{	printf("[C]: No hostname of port is given!\n");
			exit(0);
		}
		
			//Open  connection with server to check if project exists in server side
		int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
//Check_For_Update_File(argv[2]);
//First check if client has update file for  this projcet or not
//and if it does, is it empty or contain something
//if file is empty or no update file then continue 
//otherwise this command will fail:

//printf("Commit COndition\n");
if(Check_For_Update_File(argv[2]) == 1 && Project_Exists(argv[2], "./Client_Repository") == 1)
{
		int len;
	
		char buffer[1024] = {0};
		char *exit = ":exit";
		
		//Send Length of Instruction & instruction to server:
		
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);
		//printf("[C]: Instruction message sent\n");
		
		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			//read length of Server's Manifest data
			read(sock, &len, sizeof(int));
			char Project[len+1];
			read(sock, Project,len);
		
			Project[len] = '\0';
			//printf("Server's Manifest: %s\n",Project);
			
			//Write Server manifest to Client side:
			char *sinital = "./";
			char *sfinal = "_server_Commit.Manifest";
			char Server_Path[strlen(argv[2])+30];
			strcpy(Server_Path, sinital);
			strcat(Server_Path,argv[2]);
			strcat(Server_Path,sfinal);
			
			int fdT = open(Server_Path, O_CREAT|O_RDWR, 00700);
			write(fdT,Project,len+1);
			close(fdT);	
			
			//Fix_Server_Manifest(Server_Path);
			Generate_Client_Manifest_Local(argv[2]);

			//ReGenerate_Client_Manifest(argv[2]);
			//Comparision:
			char* intial = "./Client_Repository/";
			char Client_Path[strlen("./Client_Repository/")+2*strlen(argv[2])+strlen(".Manifest")+5];
			strcpy(Client_Path,intial);
			strcat(Client_Path,argv[2]);
			char *y = "/";
			strcat(Client_Path, y);
			strcat(Client_Path,argv[2]);
			char * final = ".Manifest";
			strcat(Client_Path, final);
			//printf("Client_Path: %s\n",Client_Path);
			
			File_Info *Array1;
			int size1;
			char versionNum1[10];
			
			Array1 = Get_ManifestFile_Info(Client_Path, &size1,versionNum1);
			
			File_Info *Array2;
			int size2;
			char versionNum2[10];
			
			Array2 = Get_ManifestFile_Info(Server_Path, &size2,versionNum2);	
			
			if(StringCmp(versionNum1, versionNum2) == 0)
			{
				//Generate_Client_Manifest_Commit(argv[2]);
				
				char * pre = "./Client_Repository/";
				char *slash = "/";
				char path[strlen(pre)+sizeof(argv[2])]; //"./clinetrepo/projname
				strcpy(path, pre);
				strcat(path, argv[2]);
				char commit_path[strlen(path)+ strlen(argv[2])+10];
				char* commit = ".Commit";
				strcpy(commit_path, path);
				strcat(commit_path, slash);
				strcat(commit_path, argv[2]);
				strcat(commit_path, commit);
		  
				int fdT = open(commit_path, O_RDWR, 00700);
				struct stat st;
				int size =0;
				stat(commit_path, &st);
				size = st.st_size;
				char buf[size];
				read(fdT,buf,size);
				close(fdT);
				//printf("COMMIT: %s\n",buf);
				
				int size3 =0;
				Commit_File_Info *Array3; 
				Array3 = Get_Commit_File_Info(commit_path, &size3);
				
				int cmt = Finalize_Commit(Array3,size3, Array1, size1, Array2, size2, argv[2], commit_path);
				if(cmt == 1)
				{
				int nike = 1;
				write(sock, &nike, sizeof(int));
				//Write Commit length and data to server:
				fdT = open(commit_path, O_RDWR, 00700);
				struct stat st1;
				int size1 =0;
				stat(commit_path, &st1);
				size1 = st1.st_size;
				char buf2[size1];
				read(fdT,buf2,size1);
				close(fdT);
				//printf("Commit Data: %s\n", buf2);
				
				write(sock,&size1, sizeof(int));
				write(sock, buf2, size1);
				int rey;
				read(sock, &rey, sizeof(int));
				if(rey == 1)
				{
					printf("[C]: Commit Successful\n");
				}
			}
			else
			{	int nike = 0;
				write(sock, &nike, sizeof(int)); }	
			}
			else
			{printf("[C]: Please use update command before commit!\n");
				int fail = 0;
				write(sock, &fail, sizeof(int));}
		}
		else
		{
			printf("[C]: project does not exists on server side\n");
		}
	}
		//int sucess = 0;
		//write(sock,&sucess, sizeof(int));
		//Send the Exit message:
		int len2 = strlen(":exit");
		write(sock,&len2, sizeof(int));    
		write(sock,":exit", len2);
	}
	}
 
  if(StringCmp(argv[1],"push") ==0)
 {
	 if(argv[2] == NULL)
	{
	printf("[C]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{	printf("[C]: hostname of port is given!\n");
			exit(0);
		}
		
			//Open  connection with server to check if project exists in server side
		int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		
		
//printf("push COndition\n");
if(Project_Exists(argv[2], "./Client_Repository") == 1 && Update_File_For_Push(argv[2]) == 1)
{
	
	//Update_File_For_Push(char *Proj_name) == 1
	
		int len;	
		char buffer[1024] = {0};
		char *exit = ":exit";
		
		//Send Length of Instruction & instruction to server:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);
	//	printf("[C]: Instruction message sent\n");
		
		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);
		//printf("[C]: Project name sent\n");
		
		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			read(sock, &S_Or_F, sizeof(int));
			//send commit and whole project to Server:
			//Write Commit length and data to server:
			
		  char * pre = "./Client_Repository/";
		  char *slash = "/";
		  char path[strlen(pre)+sizeof(argv[2])]; //"./clinetrepo/projname
		  strcpy(path, pre);
		  strcat(path, argv[2]);
		  char commit_path[strlen(path)+ strlen(argv[2])+10];
		  char* commit = ".Commit";
		  strcpy(commit_path, path);
		  strcat(commit_path, slash);
		  strcat(commit_path, argv[2]);
		  strcat(commit_path, commit);
		  
		  
				int fdT = open(commit_path, O_RDWR, 00700);
				struct stat st1;
				int size1 =0;
				stat(commit_path, &st1);
				size1 = st1.st_size;
				char buf2[size1];
				read(fdT,buf2,size1);
				close(fdT);
				//printf("Commit Data: %s\n", buf2);
				
				write(sock,&size1, sizeof(int));
				write(sock, buf2, size1);
				
				read(sock, &S_Or_F, sizeof(int));
				
				char *Path_send = "./Push_Send_Data.txt";
				remove(Path_send);
				int Send_fd = open(Path_send, O_CREAT|O_RDWR, 00700);
				Send_Project_Protocol(argv[2], Send_fd);
				close(Send_fd);
				
				fdT = open(Path_send, O_RDWR, 00700);
				struct stat st2;
				int size2 =0;
				stat("./Push_Send_Data.txt", &st2);
				size2 = st2.st_size;
				//printf("send Data length: %i\n",size2);
				char buf3[size2+1];
				
				read(fdT,buf3,size2);
				close(fdT);
				buf3[size2] = '\0';
				//printf("Send Data: %s\n", buf3);
				write(sock,&size2, sizeof(int));
				write(sock, buf3, size2);
				
				remove("./Push_Send_Data.txt");
				read(sock, &S_Or_F, sizeof(int));
				if(S_Or_F == 1)
				{
					Project_VersionIncrement(argv[2]);
				}
				
			}
			else
			{printf("[C]: Project Does not exists in Server\n");}
		}
		else
		{
			printf("[C]: Project does not exists on Client side or side\n");
		}
				//Send the Exit message:
		int len2 = strlen(":exit");
		write(sock,&len2, sizeof(int));    
		write(sock,":exit", len2);
	}
	}
 
 if(StringCmp(argv[1],"TEST") ==0)
 {
	 if(argv[2] == NULL)
	{printf("[C]: Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{
			
			printf("[C]: No hostname of port is given!\n");
			exit(0);
		}	
		
    	int sock = 0, valread;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		
		//Send Instruction to Server:
		write(sock , argv[1] , len);
		printf("[C]: Instruction message sent\n");
		
		int rey =0;
		valread = read( sock , &rey, sizeof(int));
		char data[rey];
		read(sock, data, rey);
		//printf("data: %s\n",data);
		ReCreate_files(data);

		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));    
		write(sock,exit, len);
		
		//Close the socket:
		close(sock);
	}
 }
    return 0;
}



