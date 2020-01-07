#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H


#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/evp.h>

typedef struct 
{
	char *commit;
	char *name;
	char *Version;
	char *Hashcode;
}Commit_File_Info;

typedef struct
{
	char *name;
	char *Version;
	char *Hashcode;
} File_Info;

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t2;

//FUNCTION:
int isDirectory(const char *path);
int StringCmp(const char *k, const char *ptr);
int Project_Exists(char *Project_Name, char *path);// To check if project exists in client side,
void create(void * ptr, char *buffer, int len);
void CreateAndInitalize_Manifest(char* Proj_name);
void Project_VersionIncrement(char * Proj_name);
void Send_File_Protocol(char *path, int fd, char* Filename);
void Send_File_Protocol_initalize(char *send, char *Project_name);
void Send_Project_Protocol(char *Project_name, int fd);
void Number_Of_Files(char *dir_Path, int *num_of_Files);
//void ReCreate_files(char* buffer);
void Add_Commit_File_To_Project(char* Proj_name, char* Data, int Data_length);
int File_Exists(char *File_Name, char*path);
int Compare_Commit_Remove(char *Client_Commit_Data, char* Proj_Name);
int isDirectoryExists(char *path);
File_Info *Get_ManifestFile_Info(char * path, int *size1,char* versionNum);
void Remove_Files_From_Proj(char* Proj_Name);
void ReCreate_files(char* data);
void ReGenerate_Server_Manifest(char* Proj_name);
void Create_Hash(char* filename_path, char *Hash);
int StringCmp_For_Commit(const char *k, const char *ptr, int size);
void Fix_Manifest(char *Proj_name);
int Commit_Remove(char* Proj_name);
void Write_Commit_tO_History();
Commit_File_Info *Get_Commit_File_Info(char * path, int *size1);
#endif
