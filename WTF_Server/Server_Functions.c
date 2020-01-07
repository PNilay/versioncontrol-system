#include "Server_Functions.h"

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void Project_VersionIncrement(char * Proj_name)
{
	char *init = "./Server_Repository/";
	char path[2*strlen(Proj_name)+35];
    strcpy(path, init);
	strcat(path,Proj_name);
	strcat(path,"/");
	strcat(path, Proj_name);
	strcat(path,".Manifest"); 
	
int fd = open(path, O_RDWR, 00700);
      
struct stat st;
int size =0;
stat(path, &st);
size = st.st_size;
//printf("size: %i\n",size);
int status = -1;
char buffer[size];
 do
  {
    status = read(fd, buffer, size);
  }while(status > 0);
close(fd);  
  if(buffer != NULL){
	int len = strlen(buffer);
    int c =0;
    int numberofToken =0;
    while(c<len)
      {
	if(buffer[c] =='\n')
	  {
	    buffer[c] = '\0';
	    numberofToken++;
	  }
	c++;
      }

char **Tokens = (char**)malloc(numberofToken*sizeof(char*));
    int r=0;
    if(buffer[0] != '\0')
      {
	Tokens[0] = &buffer[0];
	r++;
      }
    else{
      numberofToken--;
    }

    int i =0;
    while(i<len)
      {
	if(buffer[i] == '\0' && buffer[i+1] != '\0' && r<=numberofToken)
	  {
	    Tokens[r] = &(buffer[i+1]);
	    r++;
	  }
	if(buffer[i] == '\0' && buffer[i+1] == '\0')
	  {
	    numberofToken--;
	  }
	i++;
      }
     
	  int num = atoi(Tokens[0]);
	  num++;
	  sprintf(Tokens[0] ,"%d",num);
	  remove(path);
	int fd = open(path, O_CREAT|O_RDWR, 00700);
	const char *newline = "\n";
	  for(i =0; i<r; i++)
      {
		write(fd, Tokens[i], strlen(Tokens[i]));
		write(fd, newline, 1);
	  }
  }
}

void CreateAndInitalize_Manifest(char* Proj_name)
{
		int check; 
		char* inti = "./Server_Repository/";
		int len = strlen(Proj_name);
		char path[2*len+35];

	    strcpy(path, inti);
		strcat(path,Proj_name);
			
	check = mkdir(path, 0700); 
		//printf("Directory created\n");
		strcat(path,"/");
		strcat(path, Proj_name);
		strcat(path,".Manifest");  
		//printf("path:::: %s\n",path);
      int fd = open(path, O_CREAT|O_RDWR, 00700);
      
      int version =0;
      char tmp[10];
      sprintf(tmp,"%d", version);
	  write(fd, tmp, strlen(tmp));
      char *newline = "\n";
      write(fd, newline, 1);
      close(fd);
     
     
     //Create History file for this Projcet:
	 char *start = "./";
     char *Hist = ".History";
     char History[strlen(Proj_name)+ strlen(Hist)+5];
     strcpy(History, start);
     strcat(History, Proj_name);
     strcat(History, Hist);
	fd = open(History, O_CREAT|O_RDWR, 00700);
	char * Add = "A";
	const char* tab = "\t";
	write(fd, Add, strlen(Add));
	write(fd, tab,1);
	
	char ManName[2*strlen(Proj_name)+10];
	strcpy(ManName, Proj_name);
	strcat(ManName, "/");
	strcat(ManName,Proj_name);
	strcat(ManName, ".Manifest");
	write(fd, ManName, strlen(ManName));
	write(fd, tab,1);
	char *Zero = "0";
	write(fd,Zero, strlen(Zero));
	write(fd,newline,1);

     // printf("Initialized\n");
}
// STRING COMPARE:
/*
StringCmp function:
- StringCmp function takes two variables, both are address of char string.
- Than it compares string character by character and tells which input string is grater.
- This function returns 0 if both strings are equal, 1 when 1st input string is grater
  and -1 when second input string is greater.
- This function works very similar to build in strcmp() function.
 */
int StringCmp(const char *k, const char *ptr)
{
  int j =0;
  int m;
  while(ptr[j] != '\0' || k[j] != '\0') // compare until either one or both of input string reaches end of the string
    {

      // if string 2 has upper latter character and string 1 has lower latter
      // Than string 2 is greater than string 1 therefore returns -1.
      if(isupper(ptr[j])>0 && isupper(k[j])==0)
        {
          m = -1;
          break;
        }

      // if string 1 has upper latter character and string 2 has lower latter
      // Than string 1 is greater than string 2 therefore returns 1.
      else if(isupper(ptr[j])==0 && isupper(k[j])>0)
        {
          m =1;
          break;
        }
      else{
	// if both strings has lower alphabatic charactr than compare whicg character
	// is greater and return value according to that.
	if(ptr[j]>k[j])
	  {
	    m = -1;
	    break;
	  }
	else if(ptr[j]<k[j])
	  {
	    m= 1;
	    break;
	  }
	else
	  {
	    m=0;
	    j++;
	  }
      }
    }

  //This part check if one string has more character than other or not
  if(m ==0)
    {
      if(ptr[j] == '\0' && k[j] != '\0')
        {
          m = 1;
        }
      else if(ptr[j] != '\0' && k[j] == '\0')
        {
          m = -1;
        }
	}
  return m;
}

int Project_Exists(char *Project_Name, char *path)
{
  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return 0;
  }
int m =0;
  while ((dp = readdir(dir)) != NULL)
    {
      if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
	{
	  strcpy(path1, path);
	  strcat(path1,"/");
	  strcat(path1, dp->d_name);
	  if(StringCmp(dp->d_name, Project_Name)==0)
	  {//printf("Project Exists!\n");
		  return 1;}
	}

    }
  closedir(dir);
  return 0;
}

void Send_File_Protocol(char *path, int fd, char* FilenamePath) //Writes a file into sendfile.txt
{
	//printf("Send_file_protocol path: %s\n",path);
	//printf("Send_file_protocol Filenamepath: %s\n",FilenamePath);

	char Filename[strlen(FilenamePath)+25];
	char *initial = "./Server_Repository/";
	  strcpy(Filename, initial);
	  strcat(Filename, FilenamePath);
	 //printf("newfilename: %s\n",Filename);
	
	
  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return;
  }
  
int m =0;
  while ((dp = readdir(dir)) != NULL)
    {
      if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
	{
	  strcpy(path1, path);
	  strcat(path1,"/");
	  strcat(path1, dp->d_name);
	  if(!isDirectory(path1))
	  {	
		  //printf("path1: %s == filename: %s\n",path1, Filename);
		 if(strcmp(path1,Filename)==0)
		  {
			  //printf("----------->\n");
		int size;
		struct stat st;
		stat(path1, &st);
		size = st.st_size;
		
		int fdT = open(path1, O_RDWR, 00700);
		
		int status = -1;
		char buffer[size];
		do
		{
			status = read(fdT, buffer, size);
		}while(status > 0);
		close(fdT);
			char tmp[10];
			const char *del = ":";
			sprintf(tmp,"%d", strlen(FilenamePath));
			  write(fd,tmp, strlen(tmp));
			  write(fd,del,1);
			  write(fd,FilenamePath, strlen(FilenamePath));
			  write(fd,del,1);
			  sprintf(tmp,"%d", size);
			  write(fd, tmp, strlen(tmp));
			  write(fd,del,1);
			  write(fd,buffer,size);
			  write(fd,del,1);
			    closedir(dir);
			  return;
		  }
	  }
	}}
  closedir(dir);
}

void Send_File_Protocol_initalize(char * send, char *Project_name)
{
	
	char path[25+strlen(Project_name)];
	char *initial = "./Server_Repository";
	  strcpy(path, initial);
	  strcat(path,"/");
	  strcat(path, Project_name);
	 //printf("newpath: %s\n",path);
  int fd = open("./sendfile.txt", O_CREAT|O_RDWR, 00700);
  //const char *path = ".";
  int length = strlen(send);
  int m =0;
  int numberoffiles =0;
  while(m<length)
  {
	  if(send[m] == ':')
	  {
		  numberoffiles = numberoffiles+1;
	  }
	  m = m+1;
  } 
  
      char tmp[10];
      sprintf(tmp,"%d", numberoffiles);
	  write(fd, tmp, strlen(tmp));
	  const char *delim = ":";
	  write(fd,delim,1);
	  
  //printf("Total Number of Files: %i\n", numberoffiles);
  char * pch;
  pch = strtok (send,":");
  while (pch != NULL)
  {
    Send_File_Protocol(path, fd,pch);
    pch = strtok (NULL, ":");
  }
  close(fd);
}

void create(void* ptr,char *buffer, int len)
{
	connection_t2 *conn;
	conn = (connection_t2 *)ptr;
	//printf("Create Function called\n");
	
	//Send Conformation Message to client:
	char* conformation = "OK";
	char* Success = "Success";
	char* failure = "Failure";
	
	//printf("[S]: Message from Client: %s\n", buffer);
	//Send Conformation Message to client:
	//write(conn->sock, conformation, strlen(conformation));	
	bzero(buffer, sizeof(buffer));
				
	//Read Length of Project Name:
	read(conn->sock, &len, sizeof(int));
	char *Proj_name = (char *)malloc((len+1)*sizeof(char));
	Proj_name[len] = 0;
	read(conn->sock, Proj_name, len);
				
	//Print name of Project:
	//printf("Project name :%s\n", Proj_name);
	
	//Check if project already exists or not
	//If Project Does exist then send failuer message to Client.
	int temp =0;
	temp = Project_Exists(Proj_name, "./Server_Repository");
	if(temp ==1 )
	{
		//printf("Project Exists\n");
	write(conn->sock, failure, strlen(failure));
	}
	//If Project Exists send Sucess Message & then send Manifest file to Client.
	
	else{//printf("Project does not Exists\n");
	CreateAndInitalize_Manifest(Proj_name);
	write(conn->sock, Success, strlen(Success));
	char path[2*strlen(Proj_name)+10];
	char *slash = "/";
	char *Manifest = ".Manifest:";
	int r =0;
	char newproj_name[strlen(Proj_name)+1];
	
	
	strcpy(path, Proj_name);
	strcat(path,slash);
	strcat(path,Proj_name);
	strcat(path,Manifest);
	//printf("PAth of maifest file: %s\n",path);
	Send_File_Protocol_initalize(path,Proj_name);
	int fdT = open("./sendfile.txt", O_RDWR, 00700);
	int size;
	struct stat st;
	stat("./sendfile.txt", &st);
	size = st.st_size;
				
	int status = -1;
	char buffer[size];
	do
	{
		status = read(fdT, buffer, size);
		}while(status > 0);
		close(fdT);
		//printf("BUFFER: %s\n",buffer);
	write(conn->sock, &size, sizeof(int));
	write(conn->sock, buffer,size);
	remove("./sendfile.txt");
	//printf("Create DONE!\n" );
				
}
	free(Proj_name);
}

void Send_Project_Protocol(char *Project_name, int fd)
{
	
	char path[strlen(Project_name)+25];
	char *initial = "./Server_Repository/";
	strcpy(path, initial);
	strcat(path, Project_name);
	//printf("path: %s\n",path);
	//Number of files inside folder:
	int num_of_Files =0;
	Number_Of_Files(path, &num_of_Files);
	//printf("Number of files:  ---------->%i\n",num_of_Files);
	char tmp[10];
	const char *del = ":";
	sprintf(tmp,"%d",num_of_Files);
	write(fd,tmp, strlen(tmp));
	write(fd,del,1);
	
  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return;
  }
  
  
int m =0;
  while ((dp = readdir(dir)) != NULL)
    {
      if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
	{
	  strcpy(path1, path);
	  strcat(path1,"/");
	  strcat(path1, dp->d_name);
	  
	  if(!isDirectory(path1))
	  {
		char FilenamePath[strlen(Project_name)+strlen(dp->d_name)+10];
		strcpy(FilenamePath,Project_name);
		strcat(FilenamePath, "/");
		strcat(FilenamePath,dp->d_name);   	
		int size;
		struct stat st;
		stat(path1, &st);
		size = st.st_size;
		
		int fdT = open(path1, O_RDWR, 00700);
		
		int status = -1;
		char buffer[size];
		do
		{
			status = read(fdT, buffer, size);
		}while(status > 0);
		close(fdT);
		
		//char tmp[10];
		//const char *del = ":";
		sprintf(tmp,"%d", strlen(FilenamePath));
		write(fd,tmp, strlen(tmp));
			  write(fd,del,1);
			  write(fd,FilenamePath, strlen(FilenamePath));
			  write(fd,del,1);
			  sprintf(tmp,"%d", size);
			  write(fd, tmp, strlen(tmp));
			  write(fd,del,1);
			  write(fd,buffer,size);
			  write(fd,del,1);
	  }
	}}
  closedir(dir);
	
}

void Number_Of_Files(char *path, int *num_of_Files1)
{
	int num_of_Files =0;
	  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return;
  }
  

  while ((dp = readdir(dir)) != NULL)
    {
      if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
	{
	  strcpy(path1, path);
	  strcat(path1,"/");
	  strcat(path1, dp->d_name);
	  if(!isDirectory(path1))
	  {	
		num_of_Files = num_of_Files+1;
	  }
	}}
	*num_of_Files1 = num_of_Files;
		//printf("Number of Files %i\n", *num_of_Files1);
  closedir(dir);
}

void Add_Commit_File_To_Project(char* Proj_name, char* Data, int Data_length)
{
		  char * pre = "./Server_Repository/";
		  char *slash = "/";
		  char path[strlen(pre)+sizeof(Proj_name)]; //"./clinetrepo/projname
		  strcpy(path, pre);
		  strcat(path, Proj_name);
		  
		  char File_Name[strlen(Proj_name)+20];
		  char* commit = ".Commit";
		  strcpy(File_Name, Proj_name);
		  strcat(File_Name,commit);
		
		  char new_File_Name[strlen(Proj_name)+20];
		  int No =0; 
		  while(1)
		  {		
			  	char str[10];
				sprintf(str, "%d", No);
				strcpy(new_File_Name, File_Name);
				strcat(new_File_Name,str);
				
				if(File_Exists(new_File_Name, path) != 1)
				{
					break;
				}
				No= No+1;				
		  }
		  char commit_path[strlen(path)+ strlen(new_File_Name)+2];
		  strcpy(commit_path, path);
		  strcat(commit_path, slash);
		  strcat(commit_path, new_File_Name);
		  
		  int fdT = open(commit_path, O_CREAT|O_RDWR, 00700);
		  write(fdT,Data,Data_length);
		  close(fdT);
		  //printf("done commit!\n");
		  
		  
}

int File_Exists(char *File_Name, char*path)
{
  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return;
  }
int m =0;
  while ((dp = readdir(dir)) != NULL)
    {
      if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
	{
	  if(!isDirectory(path1))
	  {		
		 if(strcmp(dp->d_name,File_Name)==0)
		  {
			return 1;
			}
		}
		}
	}
  closedir(dir);
  return 0;
}


int Compare_Commit_Remove(char *Client_Commit_Data, char* Proj_name)
{
	//printf("Compare_Commit_Remove ____________________________________________________\n");
		  char * pre = "./Server_Repository/";
		  char *slash = "/";
		  char path[strlen(pre)+sizeof(Proj_name)]; //"./clinetrepo/projname
		  strcpy(path, pre);
		  strcat(path, Proj_name);
		  
		  char File_Name[strlen(Proj_name)+20];
		  char* commit = ".Commit";
		  strcpy(File_Name, Proj_name);
		  strcat(File_Name,commit);
		
		  char new_File_Name[strlen(Proj_name)+20];
		  int No =0;
		  int matched = -1; 
		  while(1)
		  {		
			  	char str[10];
				sprintf(str, "%d", No);
				strcpy(new_File_Name, File_Name);
				strcat(new_File_Name,str);
				
				if(File_Exists(new_File_Name, path) != 1)
				{
					break;
				}
				
	//IF commif file verison exists, then compare this commit file with Client_send commit data.				
		  char commit_path[strlen(path)+ strlen(new_File_Name)+2];
		  strcpy(commit_path, path);
		  strcat(commit_path, slash);
		  strcat(commit_path, new_File_Name);
		  
		  int fdT = open(commit_path, O_RDWR, 00700);
		  int size;
		  struct stat st;
		  stat(commit_path, &st);
		  size = st.st_size;
		  //printf("SIZE:::::::::: %i\n", size);
		  char Data[size];
		  read(fdT,Data,size);
		  close(fdT);
		  
		//  printf("DATA%i: %s\n",No, Data);
		 // printf(" strlen(Data) == strlen(Client_Commit_Data)  ::: %i == %i\n",size ,strlen(Client_Commit_Data));
		  //printf("Data: \n%s\n---------------------------", Data);
		  //printf("Client_Commit_Data: \n%s\n----------------",Client_Commit_Data);

		  if(size == strlen(Client_Commit_Data))
		  {
			  //printf("Stringcmp(Client_Commit_Data, Data) ==== %i\n",StringCmp_For_Commit(Data,Client_Commit_Data, size) );
			  if(StringCmp_For_Commit(Data,Client_Commit_Data,size) ==0)
			  {
				  matched = No;
				  //printf("Commit version %i is matched with Client's Data.\n",No);
				  break;
			  }
		  }
		  No= No+1;
		  }
		  
		  if(matched != -1)
		  {
			  //printf("Matched != -1\n");
			  //There is match:
			  //Remove all other Commit files and add client commit file as version0
			int newNo =0;
			  while(1)
		  {		
			  	char str1[10];
				sprintf(str1, "%d", newNo);
				strcpy(new_File_Name, File_Name);
				strcat(new_File_Name,str1);
				
				if(File_Exists(new_File_Name, path) != 1)
				{
					break;
				}
				
				
				//printf("remove(%s)\n",new_File_Name);
				char remove_Path[strlen(path)+2+strlen(new_File_Name)];
				strcpy(remove_Path, path);
				strcat(remove_Path, slash);
				strcat(remove_Path, new_File_Name);
				//printf("remove(%s)\n",remove_Path);
				remove(remove_Path);
				newNo= newNo+1;				
		  }
  
		//add client commit as version0				
		Add_Commit_File_To_Project(Proj_name, Client_Commit_Data, strlen(Client_Commit_Data));
		return 1;
		
		  }
		  return 0;
		  //printf("done commit!\n");
}

/*
 * Function to check whether a directory exists or not.
 * It returns 1 if given path is directory and  exists 
 * otherwise returns 0.
 */
int isDirectoryExists(char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

File_Info *Get_ManifestFile_Info(char * path, int *size1,char* versionNum)
{
	int fd = open(path, O_RDWR, 00700);
    
    // get the size of path, so we can store file into char array.
	struct stat st;
	int size =0;
	stat(path, &st);
	size = st.st_size;
	//Read file and store it into char buffer array.
	int status = -1;
	char buffer[size];

	do
	{
    status = read(fd, buffer, size);
	}while(status > 0);
	close(fd);  

  if(buffer != NULL){
	int len = strlen(buffer);
    int c =0;
    int numberofToken =0;
    while(c<len)
      {
	if(buffer[c] =='\n')
	  {
	    buffer[c] = '\0';
	    numberofToken++;
	  }
	c++;
      }

	char **Tokens = (char**)malloc(numberofToken*sizeof(char*));
    int r=0;
    if(buffer[0] != '\0')
      {
	Tokens[0] = &buffer[0];
	r++;
      }
    else{
      numberofToken--;
    }

    int i =0;
    while(i<len)
      {
	if(buffer[i] == '\0' && buffer[i+1] != '\0' && r<=numberofToken)
	  {
	    Tokens[r] = &(buffer[i+1]);
	    r++;
	  }
	if(buffer[i] == '\0' && buffer[i+1] == '\0')
	  {
	    numberofToken--;
	  }
	i++;
      }
  strcpy(versionNum,Tokens[0]);
  
  *size1 = (r-1);

  File_Info *array;
  array = malloc((r-1)*sizeof(File_Info));
  for(i=1;i<r;i++){
  char val[strlen(Tokens[i])];
  	char *init = "";
    strcpy(val, init);
  	strcat(val,Tokens[i]);

  c =0;
  int u =0;
  int length = strlen(val);
  for(c =0; c<length; c++)
      {
		if(val[c]== '\t')
		{
			val[c] = '\0';
			u++;
		}
      }
      
      (array+(i-1))->name = strdup(&(val[0]));
      //printf("__________________%s\n",(array+(i-1))->name);
      int j =0;
      while(val[j] != '\0')
      {
		  j++;
	  }
	  	  
      (array+(i-1))->Version = strdup(&val[j+1]);
      j = j+1;     
      while(val[j] != '\0')
      {
		  j++;
	  }
      (array+(i-1))->Hashcode = strdup(&val[j+1]);
}
  return array;
  }	
}

void Remove_Files_From_Proj(char* Proj_Name)
{
  char * in = "./Server_Repository/";
  char path[strlen(in)+strlen(Proj_Name)+2];
  strcpy(path, in);
  strcat(path, Proj_Name);
  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return 0;
  }
int m =0;
  while ((dp = readdir(dir)) != NULL)
    {
      if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
	{
	  strcpy(path1, path);
	  strcat(path1,"/");
	  strcat(path1, dp->d_name);
	  remove(path1);
	}

    }
  closedir(dir);
  return 0;
}

//buffer: data (numoffiles:lengthoffile1:file1name:lenthoffile1data:file1data:......)
void ReCreate_files(char* buffer)
{
	//printf("Recreate FIles\n");
	char *inital = "./Server_Repository/";
	//int sizeofinital = strlen(inital);
	int m =0;
  	int s =0;
  	//char *re;
  	const char *tk = ":";
  	int start;
  	int last = strlen(buffer);
  	//printf("[S->C]: Data length: %i\n",last);
  	 char delm[] =":";
  	 int len = last;
      
      char filenamesize[10];
      char datasize[10];
      int num;
      int r =0;

char numberOfFiles[10];
  	//printf("[S->C]: numberOfFiles: %s\n",numberOfFiles);
		while((s<last)&&(buffer[s]!= ':'))
		{
			numberOfFiles[r] = buffer[s];
			r= r+1;
			s = s+1;
		}
		numberOfFiles[r] = buffer[s];
		
		num = (int)strtol(numberOfFiles, (char**)NULL,10); //File name size;
		//printf("number of files: %i\n",num);
  	
	s = s+1;
      
  for(m =0; m<num; m++)
  	{
		r=0;
		//printf("FOR \n");
		//printf("s = %i\n", s);
		while((s<last)&&(buffer[s]!= ':'))
		{
			filenamesize[r] = buffer[s];
			r= r+1;
			s = s+1;
		}
		filenamesize[r] = buffer[s];
		
		num = (int)strtol(filenamesize, (char**)NULL,10); //File name size;
		//printf("file name size: %i\n",num);
		
	 //Get filename size: 
		char filename[num+1];
		s = s+1;
		//Get File:
		int y =0;
		for(y = s; y< s+num;y++)
		{
			filename[y-s] = buffer[y];
		}
		filename[num] = '\0';
		char newfilename[num+25];
		strcpy(newfilename, inital);
		strcat(newfilename, filename);
	
		//printf("newfilename : %s\n", newfilename);
		s = s+num+1;
		remove(newfilename);
		int fd = open(newfilename, O_CREAT|O_RDWR, 00700);
		//Get Data size:
		r =0;
		while((s<last)&&(buffer[s]!= ':'))
		{
			filenamesize[r] = buffer[s];
			r= r+1;
			s = s+1;
		}
		filenamesize[r] = buffer[s];
		
		num = (int)strtol(filenamesize, (char**)NULL,10); //File name size;
		//printf("Data size: %i\n",num);
		
		//Get Data:
		char fileData[num+1];
		s = s+1;
		
		y =0;
		for(y = s; y< s+num;y++)
		{
			fileData[y-s] = buffer[y];
		}
		fileData[num] ='\0';
		//printf("Data : %s\n", fileData);
		write(fd, fileData, strlen(fileData));
		close(fd);
		s = s+num+1;
	} 
}


void ReGenerate_Server_Manifest(char* Proj_name)
{
	//Fix_Manifest(Proj_name);
	const char *tab = "\t";
	const char *Version = "0";
	const char *newline = "\n";
		
	//Check if Client has manifest file or not:
	char * pre = "./Server_Repository/";
	char path[strlen(pre)+sizeof(Proj_name)]; //"./clinetrepo/projname
	strcpy(path, pre);
	strcat(path, Proj_name);
	
	char *Manifest = ".Manifest";
	char File_name[strlen(Proj_name)+15]; // Proj_name.Manifest
	strcpy(File_name, Proj_name);
	strcat(File_name, Manifest);
	if(File_Exists(File_name,path) == 1)
	{
		//printf("manifest File exists!\n");
		//Get Manifest file info
		File_Info *Array;
		char File_Path[strlen(path)+strlen(File_name)+5];
		strcpy(File_Path, path);
		char *slash = "/"; 
		strcat(File_Path,slash);
		strcat(File_Path, File_name);
		//printf("File_Path: %s\n",File_Path);
		
		int sizeofArray = 0;
		char versionNum[10];
		Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);
		//printf("size of array: %i",sizeofArray);
		
		 int i =0;
		 //Check if File name in Manifest exists,
		 //If it does than create newhash code for it,
		 //and if not set Hashcode to 0, which indicates that the file in manifest 
		 //doesn't exists in Project Folder:
		 int need_to_change =0;
  for(i=0; i<sizeofArray; i++)
  {		//printf("(Array+i)->name: %s ,path: %s\n",(Array+i)->name,path);
	  char newname[strlen((Array+i)->name)-strlen(Proj_name)];
	  int k =0;
	  int u =0;
	  for(k = strlen(Proj_name)+1; k<strlen((Array+i)->name); k++)
	  {
		  newname[u] = (Array+i)->name[k];
		  u++;
	  }
	  //printf("newname: %s\n",newname);
	  if(File_Exists(newname,path) == 1)
	  {
		  //Generate new Hash code for it:
		  //printf("Generating New Hashcode\n");
		  char newHashCode[40];
		  Create_Hash((Array+i)->name,newHashCode);
		  
		  if(strcmp((Array+i)->Hashcode,newHashCode) != 1)
		  {
			  strcpy((Array+i)->Hashcode, newHashCode);
			  //Also Increment File Version Number:
			  int num = atoi((Array+i)->Version);
			  num = num +1;
			  sprintf((Array+i)->Version,"%d",num);
			  need_to_change = need_to_change+1;
		  }
	  }
	  else
	  {
		//printf("File Does not exists\n");
		  (Array+i)->Hashcode = "0";
		  need_to_change = need_to_change+1;
	  }
  }
  
  if(need_to_change>0)
  {
  remove(File_Path);//Remove Old Manifest and add New Manifest:
  int fdN = open(File_Path,O_CREAT|O_RDWR, 00700);
	write(fdN, versionNum, strlen(versionNum));
	write(fdN,newline,1);
	for(i=0; i<sizeofArray; i++)
	{
		if(strlen((Array+i)->Hashcode) == 40)
		{
			write(fdN,(Array+i)->name,strlen((Array+i)->name));
			write(fdN,tab,1);
			write(fdN,(Array+i)->Version,strlen((Array+i)->Version));
			write(fdN,tab,1);
			write(fdN,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
			write(fdN,newline,1);
		}
	}
		close(fdN);
  }
		//printf("Done!\n");
	}
	else
	{
		//printf("Manifest File does not exists\n");
	}
}

//filename: projname/filename
//Hash: return string
void Create_Hash(char* filename, char *Hash)
{
	//printf("filename : %s\n",filename);
	//printf("Hash: %s\n",Hash);
	char *init = "./Server_Repository/";
	char filename_path[strlen(filename)+20];
	strcpy(filename_path, init);
	strcat(filename_path, filename);
	int fdT = open(filename_path, O_RDWR, 00700);
	int size;
	struct stat st;
	stat(filename_path, &st);
	size = st.st_size;
		
	int status = -1;
	char data[size+1];
	do
	{
		status = read(fdT,data, size);
	}while(status > 0);
	close(fdT);
	data[size] = '\0';
	//printf("Buffer in HSA:::: %s\n",data);
	//printf("\n");	
	//char data[100] = "Hello World";
	int i = 0;
    int SHA_DIGEST_LENGTH =20;
    unsigned char temp[SHA_DIGEST_LENGTH];
    char buf[SHA_DIGEST_LENGTH*2];
    
    memset(buf, 0x0, SHA_DIGEST_LENGTH*2);
    memset(temp, 0x0, SHA_DIGEST_LENGTH);
 
    SHA1((unsigned char *)data, strlen(data), temp);
 
    for (i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buf[i*2]), "%02x", temp[i]);
    }
    
	strcpy(Hash,buf);
    //printf("SHA1 of %s is %s\n", data, buf);
	
}


int StringCmp_For_Commit(const char *k, const char *ptr, int size)
{
	//printf("size:   %i\n",size);
  int j =0;
  int m;
  while(ptr[j] != '\0' || k[j] != '\0') // compare until either one or both of input string reaches end of the string
    {
		if(j < size)
		{
      // if string 2 has upper latter character and string 1 has lower latter
      // Than string 2 is greater than string 1 therefore returns -1.
      if(isupper(ptr[j])>0 && isupper(k[j])==0)
        {
          m = -1;
          break;
        }

      // if string 1 has upper latter character and string 2 has lower latter
      // Than string 1 is greater than string 2 therefore returns 1.
      else if(isupper(ptr[j])==0 && isupper(k[j])>0)
        {
			//printf("ptr[%i]: %c, k[%i]: %c\n",j, ptr[j], j, k[j]);
          m =1;
          break;
        }
      else{
	// if both strings has lower alphabatic charactr than compare whicg character
	// is greater and return value according to that.
	if(ptr[j]>k[j])
	  {
	    m = -1;
	    break;
	  }
	else if(ptr[j]<k[j])
	  {
		 //printf("ptr[%i]: %c, k[%i]: %c\n",j, ptr[j], j, k[j]);
	    m= 1;
	    break;
	  }
	else
	  {
	    m=0;
	    j++;
	  }
      }
    }
    else
    {
		break;
	}
}

  //This part check if one string has more character than other or not
 // printf("j = %i, size = %i, m == %i\n", j, size, m);
  if(m ==0)
    {
		if(j<size)
		{
      if(ptr[j] == '\0' && k[j] != '\0')
        {
          m = 1;
        }
      else if(ptr[j] != '\0' && k[j] == '\0')
        {
          m = -1;
        }
	}
	}
  return m;
}


//Proj_name: project name
void Fix_Manifest(char *Proj_name)
{
		const char *tab = "\t";
		const char *Version = "0";
		const char *newline = "\n";
		
	//Check if Client has manifest file or not:
	char * pre = "./Server_Repository/";
	char path[strlen(pre)+sizeof(Proj_name)]; //"./clinetrepo/projname
	strcpy(path, pre);
	strcat(path, Proj_name);
	
	char *Manifest = ".Manifest";
	char File_name[strlen(Proj_name)+15]; // Proj_name.Manifest
	strcpy(File_name, Proj_name);
	strcat(File_name, Manifest);
	if(File_Exists(File_name,path) == 1)
	{
		//printf("manifest File exists!\n");
		//Get Manifest file info
		File_Info *Array;
		char File_Path[strlen(path)+strlen(File_name)+5];
		strcpy(File_Path, path);
		char *slash = "/"; 
		strcat(File_Path,slash);
		strcat(File_Path, File_name);
		//printf("File_Path: %s\n",File_Path);
		
		int sizeofArray = 0;
		char versionNum[10];
		Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);
		//printf("size of array: %i",sizeofArray);
		
		 int i =0;
  /*for(i=0; i<sizeofArray; i++)
  {
	  printf("(Array+i)->name: %s\n",(Array+i)->name);
	  printf("(Array+i)->Version: %s\n",(Array+i)->Version);
	  printf("(Array+i)->Hashcode: %i\n",strlen((Array+i)->Hashcode));
	  
  }*/
	int error = -1;
	for(i=0; i<sizeofArray; i++)
	{
		if(strlen((Array+i)->Hashcode) != 40)
		{
			error = i;
			break;
		}
	}
	
	if(error != -1)
	{
	remove(File_Path);
	int fdN = open(File_Path,O_CREAT|O_RDWR, 00700);
	write(fdN, versionNum, strlen(versionNum));
	write(fdN,newline,1);
	for(i=0; i<sizeofArray; i++)
	{
		if(i != error)
		{
			write(fdN,(Array+i)->name,strlen((Array+i)->name));
			write(fdN,tab,1);
			write(fdN,(Array+i)->Version,strlen((Array+i)->Version));
			write(fdN,tab,1);
			write(fdN,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
			write(fdN,newline,1);
		}
	}
	close(fdN);
}
	//printf("Error Index: %i\n",error);
		
		//printf("Done!\n");
	}
	else
	{
		//printf("Manifest File does not exists\n");
	}
}



int Commit_Remove(char* Proj_name)
{
	
		  char * pre = "./Server_Repository/";
		  char *slash = "/";
		  char path[strlen(pre)+sizeof(Proj_name)]; //"./clinetrepo/projname
		  strcpy(path, pre);
		  strcat(path, Proj_name);
		  
		  char File_Name[strlen(Proj_name)+20];
		  char* commit = ".Commit";
		  strcpy(File_Name, Proj_name);
		  strcat(File_Name,commit);
		
		  char new_File_Name[strlen(Proj_name)+20];
		  int No =0;
		  int matched = -1; 
		  while(1)
		  {		
			  	char str[10];
				sprintf(str, "%d", No);
				strcpy(new_File_Name, File_Name);
				strcat(new_File_Name,str);
				
				if(File_Exists(new_File_Name, path) != 1)
				{
					break;
				}
				char remove_Path[strlen(path)+2+strlen(new_File_Name)];
				strcpy(remove_Path, path);
				strcat(remove_Path, slash);
				strcat(remove_Path, new_File_Name);
				remove(remove_Path);
				No= No+1;				
		  }
		 
		return 1;
		
}


 void Write_Commit_tO_History()
 {
	 
 }
