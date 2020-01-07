#ifndef FUNCTIONS_H
#define FUNCTIONS_H

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
	char *name;
	char *Version;
	char *Hashcode;
} File_Info;

typedef struct
{
	char *update;
	char *name;
	char *Version;
	char *Hashcode;
}Update_File_Info;

typedef struct 
{
	char *commit;
	char *name;
	char *Version;
	char *Hashcode;
}Commit_File_Info;

//FUNCTION:
int isDirectory(const char *path);
int StringCmp(const char *k, const char *ptr);
int CreateSocket(uint32_t Port, const char *Ip);
void GetIPAndPort();
int Project_Exists(char *Project_Name, char *path);// To check if project exists in client side,
int File_Exists(char *File_Name, char*path);// check if file exists in project folder
void CreateAndInitalize_Manifest(char* Proj_name);
void Project_VersionIncrement(char * Proj_name);
void ReCreate_files(char* data);
void Add_File_To_Manifest(char* Proj_name, char* filename);
File_Info *Get_ManifestFile_Info(char * path, int *size1,char* versionNum);
void Remove_File_From_Manifest(char* Proj_name, char* filename);
//filename_path: proj_name/filename, Hash is return value.
void Create_Hash(char* filename_path, char *Hash);
void ReGenerate_Client_Manifest(char* Proj_name);
void Fix_Manifest(char *Proj_name);
void Compare_Manifest(File_Info *array1, int size1, char* versionnum1,File_Info *array2, int size2, char*versionnum2, char *Proj_Name);
Update_File_Info *Get_Update_File_Info(char * path, int *size1);
void Fix_Update(char *Proj_name);
int Check_For_Update_File(char * Proj_name); // This function will return 1 if client has no
											//update file or update file is empty.
void Generate_Client_Manifest_Commit(char *Proj_name);
void Fix_Server_Manifest(char *File_path);
Commit_File_Info *Get_Commit_File_Info(char * path, int *size1);
int Finalize_Commit( Commit_File_Info *array3, int size3, File_Info *array1, int size1, File_Info *array2, int size2, char*Proj_Name, char*path);
int Update_File_For_Push(char *Proj_name);
void Send_File_Protocol(char *path, int fd, char* Filename);
void Send_File_Protocol_initalize(char *send, char *Project_name);
void Send_Project_Protocol(char *Project_name, int fd);
void Number_Of_Files(char *dir_Path, int *num_of_Files);
void Generate_Live_Client_Manifest(char* Proj_name);
void Compare_Manifest_Files(File_Info *array1, int size1, char* versionnum1,File_Info *array2, int size2, char*versionnum2, char *Proj_Name, File_Info *array3);
void ReCreate_files_Change_Manifest(char* buffer);
void Generate_Client_Manifest_Local(char *Proj_name);
#endif
