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

const char *Ip = NULL; //IP Address of server
uint32_t Port = NULL; // Port number at which server is listening

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
    printf("[Configure]: Configure!\n");
	int fd = open("./Configure.txt", O_CREAT|O_RDWR, 00700);
	Ip = argv[2];
    Port = strtoul(argv[3],NULL, 10);
	write(fd, Ip, strlen(Ip));
	write(fd, delimiter,1);
	char tmp[10];
	sprintf(tmp,"%d", Port);
	write(fd,tmp,strlen(tmp));
	close(fd);

      printf("[Configure]: IP address: %s\n",Ip);
      printf("[Configure]: Port number: %i\n",Port);

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
	printf("[checkout]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{

			printf("[checkout]: No hostname of port is given!\n");
			exit(0);
		}

		int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}
		int len;
		if(Project_Exists(argv[2], "./Client_Repository") == 1)
		{
		printf("[checkout]: Project already exists on client side\n");
		//Send the Exit message:
		len = strlen(":exit");
		write(sock,&len, sizeof(int));
		write(sock,":exit", len);
		}
		else
		{
		char *exit = ":exit";
		//Send Length of Instruction and Instruction name:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
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
			char Proj_Path[strlen(argv[2])+25];
			sprintf(Proj_Path,"./Client_Repository/%s", argv[2] );
			mkdir(Proj_Path, 0700);
			ReCreate_files(Project);
			printf("[checkout]: checkout command Successful!\n");
		}
		else
		{
			printf("[checkout]: checkout command failed: project does not exists on server \n" );
		}
		//Recive whole project directory from server:
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));
		write(sock,exit, len);

		}
	}
  }

//./WTF create <project name>
  if(StringCmp(argv[1],"create") ==0)
  {
	if(argv[2] == NULL)
	{printf("[Create]:  Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{

			printf("[Create]: No hostname of port is given!\n");
			exit(0);
		}
    	int sock = 0 ;
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
		//printf("[C]: Instruction Conformed by Server?: %s\n",buffer );
		//Send length of Project name to Server:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));

		//Send Project name to Server:
		write(sock , argv[2] , len);

		//Recive sucess message from Server:
		//valread =
		read( sock , buffer, 1024);
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
		ReCreate_files(data);
		printf("[Create]: Create Command Sucessful\n");
		}
		else{
			printf("[Create] Can not project named %s\n", argv[2]);}
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));
		write(sock,exit, len);

		//Close the socket:
		close(sock);
	}
  }

//./WTF destroy <project name>
if(StringCmp(argv[1],"destroy") ==0)
  {
	  if(argv[2] == NULL)
	{printf("[Destroy]: Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{

			printf("[Distroy]: No hostname of port is given!\n");
			exit(0);
		}
    	int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}

		//First send Server "destroy" so that server know this client is
		//asking server to destroy project, if exists
		//Next Send Project name.
		char buffer[1024] = {0};
		char *exit = ":exit";
		//Send Length of Instruction and instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);
		printf("[Destroy]: Instruction message sent\n");

		//Recive Conformation message about instruction from Server:
		//valread =
		read( sock , buffer, 1024);
		// printf("[Destroy]: Instruction Conformed by Server?: %s\n",buffer );
		//Send length of Project name and name to Server for destroy:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);
		// printf("[Destroy]: Project name sent\n");

		//Recive sucess message from Server about existance of Project:
		read( sock , buffer, 1024);
		// printf("[Destroy]: Success Or Failuer?: %s\n",buffer );
		if(StringCmp(buffer, "Failure") == 0)
		{
			printf("[Destroy]: Server failed to destory the given project.\n");
		}
		else
		{
			printf("[Destroy]: Project Removed!\n");
		}
		//Send the Exit message:
		len = strlen(exit);
		write(sock,&len, sizeof(int));
		write(sock,exit, len);

		//Close the socket:
		close(sock);
	}

  }

//./WTF add <projectname> <filename>
if(StringCmp(argv[1],"add") ==0)
{
	if(argv[2] == NULL || argv[3] == NULL)
	{
		printf("[Add]: Invalid Command\n");
	}
	else
	{
		char Proj_path[strlen("./Client_Repository")+strlen(argv[2])];
		sprintf(Proj_path,"./Client_Repository" );

		if(Project_Exists(argv[2], Proj_path) == 1)
		{
			Add_File_To_Manifest(argv[2], argv[3]);
			printf("[Add]: File added to the project\n" );
		}
		else
		{
			printf("[Add]: Project does not exists!\n");
		}
	}
}

//./WTF remove <Projectname> <filename>
if(StringCmp(argv[1],"remove") ==0)
  {
	 if(argv[2] == NULL || argv[3] == NULL)
	{printf("[Remove]: Invalid Command\n");}
	else
	{
	char * clientrepo = "./Client_Repository";
	if(Project_Exists(argv[2], clientrepo) == 1)
	{
			Remove_File_From_Manifest(argv[2], argv[3]);
	}
	else
	{
		printf("[Remove]: Project does not exists!\n");
	}
	}
}

if(StringCmp(argv[1], "rollback") == 0)
{
	if(argv[2] == NULL || argv[3] == NULL)
	{printf("[rollback]: Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{

			printf("[rollback]: No hostname of port is given!\n");
			exit(0);
		}
			int sock = 0;
			sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}

		//Send Length of Instruction and Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);

		//Send length of Project name and project name to Server :
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);

		//Send Project version number:
		int ver_req = atoi(argv[3]);
		write(sock, &ver_req, sizeof(int));

		//Recive Sucess or Failuer:
		int sucess = -1;
		read(sock, &sucess, sizeof(int));
		printf("Sucess:: %d\n", sucess );
		if(sucess == 1)
		{
			printf("[rollback]: Project %s rollbacked to version %s.\n",argv[2], argv[3]);
		}
		else
		{
			printf("[rollback]: Project %s rollback failed \n", argv[2]);
		}
		//Send the Exit message:
		char *exit = ":exit";
		len = strlen(exit);
		write(sock ,&len, sizeof(int));
		write(sock ,exit, len);
}
}
//./WTF currentversion <project name>
if(StringCmp(argv[1],"currentversion") ==0)
{
	if(argv[2] == NULL)
	{printf("[currentversion]: Invalid Command\n");}
	else
	{
		GetIPAndPort();
		if(Ip == NULL || Port == NULL)
		{

			printf("[currentversion]: No hostname of port is given!\n");
			exit(0);
		}
    	int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}

		char *exit = ":exit";
		//Send Length of Instruction and Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);

		//Send length of Project name andd project name to Server :
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);

		int result =-1;
		//Recive sucess message from Server:
		read( sock , &result, sizeof(int));
		if(result == 0)
		{
			printf("[currentversion]: Projcet Does not Exist in Server!\n");
		}
		if(result==1)
		{
		//Recive length of the manifest file from Server
		//Read manifest length and data:
		int rey =0;
		read( sock , &rey, sizeof(int));
		char data[rey];
		read(sock, data, rey);

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
	 {printf("[Update]: Invalid Command\n");}
	 else{
		 GetIPAndPort();
		 if(Ip == NULL || Port == NULL){
			 printf("[Update]: No hostname or port is given!\n");
			 exit(0);
			}


		int sock = 0;
		int len;

    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1){return 0;}

		if(Project_Exists(argv[2], "./Client_Repository") == 1)
		{
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
			//read length of Server's Manifest data
			read(sock, &len, sizeof(int));
			char Project[len+1];
			read(sock, Project,len);
			//Send the Exit message:
			int len2 = strlen(":exit");
			write(sock,&len2, sizeof(int));
			write(sock,":exit", len2);


			Project[len] = '\0';

			//Write Server manifest to Client side:
			char Server_Path[strlen(argv[2])+25];
			sprintf(Server_Path, "./%s_server.Manifest", argv[2]);


			int fdT = open(Server_Path, O_CREAT|O_RDWR, 00700);
			write(fdT,Project,len+1);
			close(fdT);


			//Generate Client's Manifest:
			//Comparision:
			char Client_Path[strlen("./Client_Repository/")+2*strlen(argv[2])+strlen(".Manifest")+5];
			sprintf(Client_Path, "./Client_Repository/%s/%s.Manifest", argv[2], argv[2]);

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
			printf("[Update]: update Sucessful\n");
		}
		else
		{
			printf("[Update]: project does not exists on server side\n");
		}
		}
		else
		{
			printf("[Update]: project does not exists on client side\n");
		}
	}
 }

  if(StringCmp(argv[1],"upgrade") ==0)
 {
	 if(argv[2] == NULL)
	{
	printf("[Upgrade]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{

			printf("[Upgrade]: No hostname of port is given!\n");
			exit(0);
		}

		int sock = 0;
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
		printf("[Upgrade]: Update file exists!\n");

		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));

		//Send Instruction to Server:
		write(sock , argv[1] , len);
		printf("[Upgrade]: Instruction message sent\n");

		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));

		write(sock , argv[2] , len);
		printf("[Upgrade]: Project name sent\n");

		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			int size = 0;
			char update_path[strlen(argv[2])+10];
			sprintf(update_path, "./%s",filename);

			Update_File_Info *array;
			array = Get_Update_File_Info(update_path, &size);
			int t =0;
			int send_len =0;
			for(t =0; t<size; t++)
			{
				send_len = send_len +strlen((array+t)->name);
			}

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

				//Remove ./Proj_name/filename (./argv[2]/FileName)
				char remove_Path[strlen(argv[2])+strlen(FileName)+25];
				char *Dot = "./Client_Repository/";
				char *s = "/";
				strcpy(remove_Path,Dot);
				strcat(remove_Path, argv[2]);
				strcat(remove_Path, s);
				strcat(remove_Path, FileName);
				remove(remove_Path);
				}
				else
				{
					if(StringCmp((array+t)->update,"M")==0 || StringCmp((array+t)->update,"A")==0)
					{
					char *c = ":";
					strcat(send_List, (array+t)->name);
					strcat(send_List,c);
				}}
			}

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

			//ReCreate_files(recived_data);
			ReCreate_files_Change_Manifest(recived_data);

			//Replace client's local manifest with server's manifest file:
			read(sock, &len, sizeof(int));
			char Project[len+1];
			read(sock, Project,len);
			Project[len] = '\0';
			//Update client's Manifest file with Server's Manifest file:
			char Client_manifest_path[2*strlen(argv[2])+strlen("./Client_Repository//.Manifest")+2];
			sprintf(Client_manifest_path, "./Client_Repository/%s/%s.Manifest", argv[2], argv[2]);
			remove(Client_manifest_path);

			int fdT = open(Client_manifest_path, O_CREAT|O_RDWR, 00700);
			write(fdT,Project,len);
			close(fdT);
			remove(update_path);

			//Remove ./Proj_name.Update
			char Remove_Path[strlen(argv[2])+10];
			sprintf(Remove_Path, "./%s.Update", argv[2]);
			remove(Remove_Path);
			printf("[Upgrade]: Sucessfully upgraded\n");
		}
		else
		{
			printf("[Upgrade]: Project does not exists on server side\n");
		}
		}
		else
		{
			printf("[Upgrade]: No Update File Exists, First run Update command\n");
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
	printf("[Commit]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{	printf("[Commit]: No hostname of port is given!\n");
			exit(0);
		}

		//Open  connection with server to check if project exists in server side
		int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}



//Check_For_Update_File(argv[2]);
//First check if client has update file for  this projcet or not
//and if it does, is it empty or contain something
//if file is empty or no update file then continue
//otherwise this command will fail:

if(Check_For_Update_File(argv[2]) == 1 && Project_Exists(argv[2], "./Client_Repository") == 1)
{
		int len;
		//Send Length of Instruction & instruction to server:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);
		printf("[Commit]: Instruction message sent %s\n", argv[1]);

		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);
		printf("[Commit]: Project length and name sent %d, %s\n", len, argv[2]);

		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		printf("[Commit]: recived: Server contains the project %d\n", S_Or_F);
		if(S_Or_F == 1)
		{
			/*
				1. Read server's Manifest data and length
				2. Create a server's temporary manifest file on client side.
				3. Generate Client's manifest from scratch with updated hashcode and version
				4. If version nf both clien's manifest and server's manifest are equal:
						a. Create Commit file using "Finalize_Commit()"
						b. if Commit file sucessfully generated:
								I. Send Commit file to server.
								II. Read Sucess Message.
								III. If Sucess Message is 1:
										1.Commit sucessfully
											Otherwise commit failed
							 Otherwise send fail message
					Otherwise ask user to Update First! (Warning!)
			*/
			//read length of Server's Manifest data and Manifest data:
			read(sock, &len, sizeof(int));
			char Project[len+1];
			read(sock, Project,len);
			Project[len] = '\0';

			//Creating Server Manifest file:
			char Server_Path[strlen(argv[2])+30];
			sprintf(Server_Path, "./%s_server_Commit.Manifest", argv[2]);
			int fdT = open(Server_Path, O_CREAT|O_RDWR, 00700);
			write(fdT,Project,len+1);
			close(fdT);

			//Client Manifest file path:
			char Client_Path[strlen("./Client_Repository/")+2*strlen(argv[2])+strlen(".Manifest")+5];
			sprintf(Client_Path, "./Client_Repository/%s/%s.Manifest", argv[2], argv[2]);

			//Client Manifest Array:
			File_Info *Array1;
			int size1;
			char versionNum1[10];
			Array1 = Get_ManifestFile_Info(Client_Path, &size1,versionNum1);

			//Server Manifest Array:
			File_Info *Array2;
			int size2;
			char versionNum2[10];
			Array2 = Get_ManifestFile_Info(Server_Path, &size2,versionNum2);
			//Generating a Client's new Local manifest from scratch
			//With updated hashcode and version incremented if hase was updated
			//hash in this file is 0 for which file couldn't able to locate
			Generate_Client_Manifest_Local2(argv[2]);

			if(StringCmp(versionNum1, versionNum2) == 0)
			{
				char * pre = "./Client_Repository/";
				char path[strlen(pre)+sizeof(argv[2])]; //"./clinetrepo/projname
				sprintf(path, "./Client_Repository/%s",argv[2]);

				//"./clinetrepo/projname/projname.Commit
				char commit_path[strlen(path)+ strlen(argv[2])+10];
				sprintf(commit_path, "%s/%s.Commit",path, argv[2]);

				int cmt = Finalize_Commit( Array1, size1, Array2, size2, argv[2], commit_path);

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

					write(sock,&size1, sizeof(int));
					write(sock, buf2, size1);
					int rey;
					read(sock, &rey, sizeof(int));
					if(rey == 1)
					{
						printf("[Commit]: Commit Successful\n");
					}
			}
			else
			{	int nike = 0;
				write(sock, &nike, sizeof(int)); }
			}
			else
			{printf("[Commit]: Please use update command before commit! project version mismatch\n");
				int fail = 0;
				write(sock, &fail, sizeof(int));}
		}
		else
		{
			printf("[Commit]: project does not exists on server side\n");
		}
	}
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
	printf("[Push]: Invalid Command\n");
	}
	else
	{
	GetIPAndPort();
			if(Ip == NULL || Port == NULL)
		{	printf("[Push]: hostname of port is given!\n");
			exit(0);
		}

		//Open  connection with server to check if project exists in server side
		int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}

		char path[strlen("./Client_Repository/")+sizeof(argv[2])]; //"./clinetrepo/projname
		sprintf(path, "./Client_Repository/%s", argv[2]);

		char commit_path[strlen(path)+ strlen(argv[2])+10];
		sprintf(commit_path, "%s/%s.Commit", path, argv[2]);


			int fdT = open(commit_path, O_RDWR, 00700);
			struct stat st1;
			int size1 =0;
			stat(commit_path, &st1);
			size1 = st1.st_size;
			char buf2[size1];
			read(fdT,buf2,size1);
			close(fdT);

	if(Project_Exists(argv[2], "./Client_Repository") == 1 && Update_File_For_Push(argv[2]) == 1 && size1 > 0 )
		{
		int len;

		//Send Length of Instruction & instruction to server:
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);
		printf("[Push]: Instruction message sent\n");

		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);
		printf("[Push]: Project name sent\n");

		//Read Sucess or Failuer:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			S_Or_F = -1;
			read(sock, &S_Or_F, sizeof(int)); //Indicates succesfully locking repository
			if(S_Or_F == 1)
			{
				//Sending length and commit file
				write(sock,&size1, sizeof(int));
				write(sock, buf2, size1);

				read(sock, &S_Or_F, sizeof(int));

				//Prepare data to be sent to the server:
				char *Path_send = "./Push_Send_Data.txt";
				remove(Path_send);
				int Send_fd = open(Path_send, O_CREAT|O_RDWR, 00700);

				int po = Send_Project_Protocol2(argv[2], Send_fd, commit_path);
				close(Send_fd);

				if(po != -1){
				fdT = open(Path_send, O_RDWR, 00700);
				struct stat st2;
				int size2 =0;
				stat("./Push_Send_Data.txt", &st2);
				size2 = st2.st_size;
				char buf3[size2+1];

				read(fdT,buf3,size2);
				close(fdT);
				buf3[size2] = '\0';
				write(sock,&size2, sizeof(int));
				write(sock, buf3, size2);

				remove("./Push_Send_Data.txt");
				read(sock, &S_Or_F, sizeof(int));// <=== Sucess of failuer for sending all files
				if(S_Or_F == 1)
				{
					//Recive server's Updated manifest and replace it with client's Manifest_file:
					//Read length of new manifest file:
					int len_man = 0;
					read(sock, &len_man, sizeof(int));
					//Read manifest data:
					char Manifest_Data[len_man];
					read(sock, Manifest_Data, len_man);
					Manifest_Data[len_man] = '0';
					// printf("Manifest Data recived: %s\n",Manifest_Data );

					char Manifest_file[strlen(argv[2])+strlen("./Client_Repository//.Manifest")+strlen(argv[2])];
					sprintf(Manifest_file, "./Client_Repository/%s/%s.Manifest", argv[2], argv[2]);
					//Flush older manifest file
					remove(Manifest_file);
					//create new manifest file:
					int fd = open(Manifest_file,O_CREAT|O_RDWR, 00700);
					write(fd,Manifest_Data,len_man);

					// Project_VersionIncrement(argv[2]);
					printf("[Push]: Push sucessfull\n");
				}

				remove(commit_path);
				char Local_manifest_rm[strlen("./Client_Repository//Local.Manifest")+strlen(argv[2])];
				sprintf(Local_manifest_rm, "./Client_Repository/%s/Local.Manifest", argv[2]);
				remove(Local_manifest_rm);
			}
			else
			{
				printf("[Push]: Problems with send project Send_Project_Protocol" );
			}
			}
			else
			{printf("[Push]: Project Does not exists in Server\n");}
		}
		else
		{
			printf("[Push]: repository lock unsucessful!! \n" );
		}
		}
		else
		{
			printf("[Push]: Project does not exists on Client side or side\n");
		}
				//Send the Exit message:
		int len2 = strlen(":exit");
		write(sock,&len2, sizeof(int));
		write(sock,":exit", len2);
	}
	}


	if(StringCmp(argv[1],"history") ==0)
	 {
		 if(argv[2] == NULL)
		{
		printf("[history]: Invalid Command\n");
		}
		else
		{
		GetIPAndPort();
				if(Ip == NULL || Port == NULL)
			{	printf("[history]: hostname of port is given!\n");
				exit(0);
			}

		//Open  connection with server to check if project exists in server side
		int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}

		//Send Length of Instruction & instruction to server:
		int len = strlen(argv[1]);
		write(sock,&len, sizeof(int));
		write(sock , argv[1] , len);

		//send lenght of Project name and Project name:
		len = strlen(argv[2]);
		write(sock,&len, sizeof(int));
		write(sock , argv[2] , len);

		//Read if project exist on server side or not:
		int S_Or_F = -1;
		read(sock, &S_Or_F, sizeof(int));
		if(S_Or_F == 1)
		{
			//Read size of history datd and history data:
			int hist_len =0;
			read(sock, &hist_len, sizeof(int));
			printf("Sucess of failuer: %d\n",hist_len);

			char Hist_data[hist_len];
			read(sock,Hist_data, hist_len);
			Hist_data[hist_len] = '\0';
			printf("[History]: History for project: \n");
			printf("%s\n",Hist_data);
		}

		//Send the Exit message:
		int len2 = strlen(":exit");
		write(sock,&len2, sizeof(int));
		write(sock,":exit", len2);
	}}


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

    	int sock = 0;
    	sock = CreateSocket(Port, Ip); // Opeaning a connection between client and Server
		if(sock== -1)
		{return 0;}

		char *exit = ":exit";
		//Send Length of Instruction:
		int len;
		len = strlen(argv[1]);
		write(sock,&len, sizeof(int));

		//Send Instruction to Server:
		write(sock , argv[1] , len);
		printf("[C]: Instruction message sent\n");

		int rey =0;
		read( sock , &rey, sizeof(int));
		char data[rey];
		read(sock, data, rey);
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
