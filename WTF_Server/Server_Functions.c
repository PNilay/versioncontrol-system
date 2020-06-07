#include "Server_Functions.h"


void Save_Old_Version(char* Proj_name)
{
  printf("[S]: Saving old version of project %s\n",Proj_name);
  char *Old_Version_Directory = "./Server_Repository/Old_Version";
  if (isDirectoryExists(Old_Version_Directory) != 1)
  {
      int c = mkdir(Old_Version_Directory, 0700);
  }
  //Now  check if project name folder exists in oldversion folder:
  char Old_Version_Directory_Proj_name[strlen(Old_Version_Directory)+strlen(Proj_name)+5];
  sprintf(Old_Version_Directory_Proj_name, "./Server_Repository/Old_Version/%s", Proj_name);

  printf("Old_Version_Directory_Proj_name: %s\n", Old_Version_Directory_Proj_name);
  if (isDirectoryExists(Old_Version_Directory_Proj_name) != 1)
  {
    int h = mkdir(Old_Version_Directory_Proj_name,0700);
  }

  //Get the Version number  of Current Manifest File:
  File_Info *Array;
  char versionNum[10];
  int size1 =0;

  char path_of_Manifest[strlen("./Server_Repository/")+2*strlen(Proj_name)+strlen(".Manifest")+5];
  sprintf(path_of_Manifest, "./Server_Repository/%s/%s.Manifest",Proj_name,Proj_name);

  Array = Get_ManifestFile_Info(path_of_Manifest, &size1,versionNum);

  //Old_Version_path: ./Server_Repository/Old_version/Proj_name/version#.txt
  char Old_Version_Path[strlen(Old_Version_Directory_Proj_name)+15+strlen(versionNum)];
  sprintf(Old_Version_Path, "%s/Version%s.txt", Old_Version_Directory_Proj_name,versionNum);

  // printf("Old_Version_Path %s\n", Old_Version_Path);
  int file = open(Old_Version_Path, O_CREAT|O_RDWR, 00700);
  Send_Project_Protocol(Proj_name, file);
}

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void Project_VersionIncrement(char * Proj_name)
{
	char path[2*strlen(Proj_name)+35];
  sprintf(path, "./Server_Repository/%s/%s.Manifest",Proj_name, Proj_name);

  int fd = open(path, O_RDWR, 00700);

  struct stat st;
  int size =0;
  stat(path, &st);
  size = st.st_size;
  int status = -1;
  char buffer[size];
   do
    {
      status = read(fd, buffer, size);
    }while(status > 0);
  close(fd);
  buffer[size] = '\0';

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
		int len = strlen(Proj_name);
		char path[2*len+35];
    sprintf(path, "./Server_Repository/%s", Proj_name);

    mkdir(path, 0700);

		strcat(path,"/");
		strcat(path, Proj_name);
		strcat(path,".Manifest");
    int fd = open(path, O_CREAT|O_RDWR, 00700);

    int version =0;
    char tmp[10];
    sprintf(tmp,"%d", version);
    write(fd, tmp, strlen(tmp));
    char *newline = "\n";
    write(fd, newline, 1);
    close(fd);


     //Create History file for this Projcet:
    char History[strlen(Proj_name)+ strlen("./Server_Repository/Project_History/.History")+2];
    sprintf(History,"./Server_Repository/Project_History/%s.History",Proj_name );

    if (isDirectoryExists("./Server_Repository/Project_History") != 1)
    {
        int c = mkdir("./Server_Repository/Project_History", 0700);
    }

	fd = open(History, O_CREAT|O_RDWR, 00700);
  close(fd);
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
  	  {//Project exists
  		  return 1;}
	 }
  }
  closedir(dir);
  return 0;
}
void Send_File_Protocol(char *path, int fd, char* FilenamePath) //Writes a file into sendfile.txt
{
	char Filename[strlen(FilenamePath)+25];
  sprintf(Filename,"./Server_Repository/%s", FilenamePath);

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
		 if(strcmp(path1,Filename)==0)
		 {
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
	}
}
  closedir(dir);
}

void Send_File_Protocol_initalize(char * send, char *Project_name)
{

	char path[25+strlen(Project_name)];
  sprintf(path,"./Server_Repository/%s",Project_name);

  int fd = open("./sendfile.txt", O_CREAT|O_RDWR, 00700);
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

	//Send Conformation Message to client:
	char* Success = "Success";
	char* failure = "Failure";

	//Send Conformation Message to client:
	bzero(buffer, sizeof(buffer));

	//Read Length of Project Name:
	read(conn->sock, &len, sizeof(int));
	char *Proj_name = (char *)malloc((len+1)*sizeof(char));
	Proj_name[len] = 0;
	read(conn->sock, Proj_name, len);

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

  sprintf(path, "%s/%s.Manifest:",Proj_name, Proj_name);

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
	write(conn->sock, &size, sizeof(int));
	write(conn->sock, buffer,size);
	remove("./sendfile.txt");

}
	free(Proj_name);
}

void Send_Project_Protocol(char *Project_name, int fd)
{

	char path[strlen(Project_name)+25];
  sprintf(path, "./Server_Repository/%s", Project_name);
	//Number of files inside folder:
	int num_of_Files =0;
	Number_Of_Files(path, &num_of_Files);
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
    sprintf(FilenamePath, "%s/%s",Project_name,dp->d_name);

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
  closedir(dir);
}

void Add_Commit_File_To_Project(char* Proj_name, char* Data, int Data_length)
{

		  char path[strlen("./Server_Repository/")+sizeof(Proj_name)]; //"./clinetrepo/projname
      sprintf(path, "./Server_Repository/%s", Proj_name);

		  char File_Name[strlen(Proj_name)+20];
      sprintf(File_Name, "%s.Commit",Proj_name );

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
      sprintf(commit_path, "%s/%s",path,new_File_Name);

		  int fdT = open(commit_path, O_CREAT|O_RDWR, 00700);
		  write(fdT,Data,Data_length);
		  close(fdT);
}

int File_Exists(char *File_Name, char*path)
{
  char path1[1000] = "";
  struct dirent *dp;
  DIR *dir = opendir(path);

  if (!dir)
  {
    return 0;
  }
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
		  char path[strlen("./Server_Repository/")+sizeof(Proj_name)]; //"./serverrepo/projname
      sprintf(path, "./Server_Repository/%s", Proj_name);

		  char File_Name[strlen(Proj_name)+20];//proj_name.commit
      sprintf(File_Name, "%s.Commit", Proj_name);

		  char new_File_Name[strlen(Proj_name)+20];
		  int No =0;
		  int matched = -1;
		  while(1)
		  {
        sprintf(new_File_Name, "%s%d", File_Name, No);

				if(File_Exists(new_File_Name, path) != 1)
				{
          //If file doen't exists break out of the while loop
					break;
				}

	    //IF commif file verison exists, then compare this commit file with Client_send commit data.
		  char commit_path[strlen(path)+ strlen(new_File_Name)+2];
      sprintf(commit_path, "%s/%s",path,new_File_Name);

		  int fdT = open(commit_path, O_RDWR, 00700);
		  int size;
		  struct stat st;
		  stat(commit_path, &st);
		  size = st.st_size;
		  char Data[size];
		  read(fdT,Data,size);
		  close(fdT);
      Data[size] = '\0';


		  if(size == strlen(Client_Commit_Data))
		  {
			  if(StringCmp_For_Commit(Data,Client_Commit_Data,size) ==0)
			  {
				  matched = No;
				  // printf("[S]: Commit version %i is matched with Client's Data.\n",No);
				  break;
			  }
		  }
		  No= No+1;
		  }

      //Remove all commit files from server's project:
      int newNo =0;
      while(1)
      {

        sprintf(new_File_Name, "%s%d",File_Name, newNo );

      if(File_Exists(new_File_Name, path) != 1)
        {
          break;
        }

        char remove_Path[strlen(path)+2+strlen(new_File_Name)];
        sprintf(remove_Path,"%s/%s", path,new_File_Name );

        remove(remove_Path);
        newNo= newNo+1;
      }

		  if(matched != -1)
		  {
			  //There is match:
			  //add client commit file as version0
		    Add_Commit_File_To_Project(Proj_name, Client_Commit_Data, strlen(Client_Commit_Data)); //<-------------------------------------
        return 1;
		  }
		  return 0;
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
  int status = -1;
  char buffer[size];
  do
  {
    status = read(fd, buffer, size);
  }while(status > 0);
  close(fd);
  buffer[size] = '\0';

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
  char path[strlen("./Server_Repository/")+strlen(Proj_Name)+2];
  sprintf(path, "./Server_Repository/%s",Proj_Name);
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
	  remove(path1);
	}

    }
  closedir(dir);
  return;
}


//This function add file to manifest of that projoect
int Add_File_To_Manifest(char* Proj_name, char* filename)
{
  printf("Add File to manifest: %s\n", filename);
  const char *tab = "\t";
  const char *Version = "0";
  const char *newline = "\n";
  //Check if file exists inside Project Folder:

  char *inital = "./Server_Repository/";
  char *delim = "/";
  char *Manifest = ".Manifest";
  char *u ="";
  char path[40+2*strlen(Proj_name)];
  sprintf(path, "./Server_Repository/%s", Proj_name);
  char Proj_filename[strlen(filename)+strlen(Proj_name)+2];
  sprintf(Proj_filename, "%s/%s", Proj_name, filename);


  int check;
  //check if manifest file exists in project folder:
  char manifestfilename[10+strlen(Proj_name)];
  sprintf(manifestfilename, "%s.Manifest", Proj_name);
  check = File_Exists(manifestfilename, path);
  if(check ==1)
  {
    check = 0;
    check = File_Exists(filename,path);

    if(check != 1){
      printf("File name does not exists in directory!\n");
      return -1;
    }

    strcat(path,delim);
    strcat(path,Proj_name);
    strcat(path,Manifest);

    File_Info *Array;
    int size_of_array =0;
    char versionNum[10];
    Array = Get_ManifestFile_Info(path, &size_of_array, versionNum);

    int i =0;
    int changed = 0;
    while(1 && i<size_of_array)
    {
      if(StringCmp((Array+i)->name,Proj_filename) == 0)
      {
        printf("[C]: warning: file already exists in manifest\n");
        //If file is already in Manifest then just change the hashcode of the file:
        // And Increase the Version number
        char Hash[40];
        Create_Hash(Proj_filename, Hash);

        if(StringCmp((Array+i)->Hashcode,Hash ) != 0)
        {
          (Array+i)->Hashcode =Hash;
          int num = atoi((Array+i)->Version);
          num++;
          sprintf((Array+i)->Version ,"%d",num);
          changed = 1;
          break;
        }
        else
        {
          printf("[S]: manifest is already upto date\n");
          return -1;
        }
      }
      i =i+1;
    }
    if(changed == 1)
    {
      remove(path);
      int fd = open(path,O_CREAT|O_RDWR, 00700);
      write(fd, versionNum, strlen(versionNum));
      write(fd,newline,1);
      for(i =0; i<size_of_array; i++)
      {
        write(fd,(Array+i)->name,strlen((Array+i)->name));
        write(fd,tab,1);
        write(fd,(Array+i)->Version,strlen((Array+i)->Version));
        write(fd,tab,1);
        write(fd,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
        write(fd,newline,1);
      }
      close(fd);

    }
    if(changed == 0)
    {
      char newmanifestfilename[25+10+2*strlen(Proj_name)];
      strcpy(newmanifestfilename,inital);
      strcat(newmanifestfilename, Proj_name);
      u = "/";
      strcat(newmanifestfilename,u);
      strcat(newmanifestfilename, manifestfilename);
      int fd = open(newmanifestfilename,O_APPEND|O_RDWR, 00700);
      write(fd, Proj_filename, strlen(Proj_filename));

      write(fd,tab,1);
      write(fd,Version,1);
      write(fd,tab,1);
      char Hash[40];
      Create_Hash(Proj_filename, Hash);

      write(fd,Hash, strlen(Hash));
      write(fd,newline,1);
      close(fd);
    }

  }
  else
  {
    printf("[S]: Manifest file does not exists in Project\n");
    return -1;
  }
}


//This function removes file from manifest
void Remove_File_From_Manifest(char* Proj_name, char* filename)
{
  const char *tab = "\t";
  const char *newline = "\n";
  char *inital = "./Server_Repository/";
  char *delim = "/";
  char *Manifest = ".Manifest";
  char *u ="";
  char path[40+2*strlen(Proj_name)];
  char Proj_filename[strlen(filename)+strlen(Proj_name)+2];
  strcpy(Proj_filename, u);
  strcat(Proj_filename,Proj_name);
  u = "/";
  strcat(Proj_filename, u);
  strcat(Proj_filename, filename);
  u = "";
  strcpy(path, inital);
  strcat(path,Proj_name);
  int check;

  //check if manifest file exists in project folder:
  char manifestfilename[10+strlen(Proj_name)];
  strcpy(manifestfilename, u);
  strcat(manifestfilename, Proj_name);
  u =".Manifest";
  strcat(manifestfilename, u);
  check = File_Exists(manifestfilename, path);
  if(check ==1)
  {
    strcat(path,delim);
    strcat(path,Proj_name);
    strcat(path,Manifest);

    File_Info *Array;
    int size_of_array =0;
    char versionNum[10];
    Array = Get_ManifestFile_Info(path, &size_of_array, versionNum);
    int i =0;
    int changed = -1;
    while(1 && i<size_of_array)
    {
      if(StringCmp((Array+i)->name,Proj_filename) == 0)
      {
        //printf("file exists in manifest\n");
        changed = i;
        break;
      }
      i =i+1;
    }
    if(changed != -1)
    {
      remove(path);
      int fd = open(path,O_CREAT|O_RDWR, 00700);
      write(fd, versionNum, strlen(versionNum));
      write(fd,newline,1);
      for(i =0; i<size_of_array; i++)
      {
        if(i != changed)
        {
          write(fd,(Array+i)->name,strlen((Array+i)->name));
          write(fd,tab,1);
          write(fd,(Array+i)->Version,strlen((Array+i)->Version));
          write(fd,tab,1);
          write(fd,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
          write(fd,newline,1);
        }
      }

      if(size_of_array <= 2)
      {
        Fix_Manifest(Proj_name);
      }
      close(fd);

    }
    if(changed == -1)
    {
      printf("[S]: file can not be removed because it's not in the manifest\n");
    }
  }
  else
  {
    printf("[S]: Manifest file does not exists in Project\n");
  }
}



void Update_Manifest_Push(char* Proj_name)
{
  printf("Update Manifest push: %s\n", Proj_name);
  const char *tab = "\t";
  const char *newline = "\n";

  char Commit_file[strlen(Proj_name)+strlen("./Server_Repository//.Commit0")+strlen(Proj_name)];
  sprintf(Commit_file, "./Server_Repository/%s/%s.Commit0",Proj_name,Proj_name);
  int size_c =0;
  Commit_File_Info *Array_commit = Get_Commit_File_Info(Commit_file, &size_c);

  int i =0;
  for(i =0; i< size_c; i++)
  {
    if(strcmp((Array_commit+i)->commit,"1")==0 && strlen((Array_commit+i)->Hashcode) == 40)
    {
      printf("add:: %s\n",(Array_commit+i)->name);
      //Add:
      char *filename = &((Array_commit+i)->name[strlen(Proj_name)+1]);
      Add_File_To_Manifest(Proj_name, filename);
    }
    else if(strcmp((Array_commit+i)->commit,"2")==0 && strlen((Array_commit+i)->Hashcode) == 40)
    {
      //Modify:
      char *filename = &((Array_commit+i)->name[strlen(Proj_name)+1]);
      Add_File_To_Manifest(Proj_name, filename);
    }
    else if(strcmp((Array_commit+i)->commit,"0")==0 && strlen((Array_commit+i)->Hashcode) == 40)
    {
      //Remove:
      char *filename = &((Array_commit+i)->name[strlen(Proj_name)+1]);
      Remove_File_From_Manifest(Proj_name,filename);
    }
  }
  remove(Commit_file);
}


void Remove_Old_files_Commit(char* Proj_name)
{
  char Hist_Path[strlen(Proj_name)+ strlen("./Server_Repository/Project_History/.History")+2];
  sprintf(Hist_Path, "./Server_Repository/Project_History/%s.History",Proj_name);
  int hist = open(Hist_Path, O_RDWR, 00700);
  struct stat st_hist;
  int size_hist =0;
  stat(Hist_Path, &st_hist);
  size_hist = st_hist.st_size;
  //Read file and store it into char buffer array.
  int status_hist = -1;
  char buffer_hist[size_hist];
  do
  {
    status_hist = read(hist, buffer_hist, size_hist);
  }while(status_hist > 0);
  buffer_hist[size_hist] = '\0';
  //Write contents back to History:


  const char *tab = "\t";
  const char *newline = "\n";

  // printf("Remove old files from commit %s\n",Proj_name );
  char Commit_file[strlen(Proj_name)+strlen("./Server_Repository//.Commit0")+strlen(Proj_name)];
  sprintf(Commit_file, "./Server_Repository/%s/%s.Commit0",Proj_name,Proj_name);
  int size =0;
  // printf("Remove old files from commit1 %s\n",Proj_name );

  Commit_File_Info *Array = Get_Commit_File_Info(Commit_file, &size);
  // printf("Remove old files from commit2 %s\n",Proj_name );

  int i =0;
  for(i=0; i<size; i++)
  {
    if(strcmp((Array+i)->commit,"0")==0 && strlen((Array+i)->Hashcode) == 40)
    {
      //Remove this file from project:
      // printf("Remove File: %s, %d\n",(Array+i)->name , strlen((Array+i)->name));
      char Remove_file[strlen("./Server_Repository/")+strlen((Array+i)->name)];
      sprintf(Remove_file, "./Server_Repository/%s",(Array+i)->name);
      // printf("Remove File: %s\n",Remove_file );
      remove(Remove_file);

      //Add remove to history file:
      write(hist,"D",strlen("D"));
      write(hist,tab,1);
      write(hist,(Array+i)->name, strlen((Array+i)->name));
      write(hist, tab,1);
      write(hist,(Array+i)->Version,strlen((Array+i)->Version));
      write(hist,tab,1);
      write(hist,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
      write(hist,newline,1);
      // printf("Removed\n" );
    }
    else if(strcmp((Array+i)->commit,"1")==0 && strlen((Array+i)->Hashcode) == 40)
    {
      //Add Add to history file:
      write(hist,"A",strlen("A"));
      write(hist,tab,1);
      write(hist,(Array+i)->name, strlen((Array+i)->name));
      write(hist, tab,1);
      write(hist,(Array+i)->Version,strlen((Array+i)->Version));
      write(hist,tab,1);
      write(hist,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
      write(hist,newline,1);
    }
    else if(strcmp((Array+i)->commit,"2")==0 && strlen((Array+i)->Hashcode) == 40)
    {
      //Add Modify to history file:
      write(hist,"M",strlen("M"));
      write(hist,tab,1);
      write(hist,(Array+i)->name, strlen((Array+i)->name));
      write(hist, tab,1);
      write(hist,(Array+i)->Version,strlen((Array+i)->Version));
      write(hist,tab,1);
      write(hist,(Array+i)->Hashcode, strlen((Array+i)->Hashcode));
      write(hist,newline,1);
    }
  }
  // printf("Remove Manifest:%s\n" );

  //Remove Manifest file also:
  //char Manifest_file[strlen(Proj_name)+strlen("./Server_Repository//.Manifest")+strlen(Proj_name)];
  //sprintf(Manifest_file, "./Server_Repository/%s/%s.Manifest", Proj_name, Proj_name);
  //remove(Manifest_file);

  //Remove commit file as well:
  // remove(Commit_file);
  // printf("Done\n" );
}

Commit_File_Info *Get_Commit_File_Info(char * path, int *size1)
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

    *size1 = r;

    //All the lines of file are now stored in Tokens array of array
    //Now create struct array of size r-1 to store all the individual data.
    Commit_File_Info *array;
    array = malloc(r*sizeof(Commit_File_Info));
    for(i=0;i<r;i++){
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

      //update:
      (array+i)->commit = strdup(&(val[0]));

      //name
      int j =0;
      while(val[j] != '\0')
      {
        j++;
      }
      (array+i)->name = strdup(&(val[j+1]));

      //version
      j = j+1;
      while(val[j] != '\0')
      {
        j++;
      }
      (array+i)->Version = strdup(&val[j+1]);

      //hashcode
      j = j+1;
      while(val[j] != '\0')
      {
        j++;
      }
      (array+i)->Hashcode = strdup(&val[j+1]);
    }
    //printf("DONE!!!!\n");
    return array;
  }
  return NULL;
}

//buffer: data (numoffiles:lengthoffile1:file1name:lenthoffile1data:file1data:......)
void ReCreate_files(char* buffer)
{
	//printf("Recreate FIles\n");
	char *inital = "./Server_Repository/";
	int m =0;
  	int s =0;
  	int last = strlen(buffer);

    char filenamesize[10];
    int num;
    int r =0;

    char numberOfFiles[10];
		while((s<last)&&(buffer[s]!= ':'))
		{
			numberOfFiles[r] = buffer[s];
			r= r+1;
			s = s+1;
		}
		numberOfFiles[r] = buffer[s];

		num = (int)strtol(numberOfFiles, (char**)NULL,10); //File name size;

	  s = s+1;

  for(m =0; m<num; m++)
  	{
		r=0;
		while((s<last)&&(buffer[s]!= ':'))
		{
			filenamesize[r] = buffer[s];
			r= r+1;
			s = s+1;
		}
		filenamesize[r] = buffer[s];

		num = (int)strtol(filenamesize, (char**)NULL,10); //File name size;

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

		//Get Data:
		char fileData[num+1];
		s = s+1;

		y =0;
		for(y = s; y< s+num;y++)
		{
			fileData[y-s] = buffer[y];
		}
		fileData[num] ='\0';
		write(fd, fileData, strlen(fileData));
		close(fd);
		s = s+num+1;
	}
}


void ReGenerate_Server_Manifest(char* Proj_name)
{
	const char *tab = "\t";
	const char *newline = "\n";

	//Check if Client has manifest file or not:
	char * pre = "./Server_Repository/";
	char path[strlen("./Server_Repository/")+sizeof(Proj_name)]; //"./clinetrepo/projname
  sprintf(path, "./Server_Repository/%s", Proj_name);

	char File_name[strlen(Proj_name)+15]; // Proj_name.Manifest
  sprintf(File_name, "%s.Manifest", Proj_name);

	if(File_Exists(File_name,path) == 1)
	{
		//printf("manifest File exists!\n");
		//Get Manifest file info
		File_Info *Array;
		char File_Path[strlen(path)+strlen(File_name)+5];
    sprintf(File_Path, "%s/%s",path, File_name);

		int sizeofArray = 0;
		char versionNum[10];
		Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);

		 int i =0;
		 //Check if File name in Manifest exists,
		 //If it does than create newhash code for it,
		 //and if not set Hashcode to 0, which indicates that the file in manifest
		 //doesn't exists in Project Folder:
		 int need_to_change =0;
  for(i=0; i<sizeofArray; i++)
  {
	  char newname[strlen((Array+i)->name)-strlen(Proj_name)];
	  int k =0;
	  int u =0;
	  for(k = strlen(Proj_name)+1; k<strlen((Array+i)->name); k++)
	  {
		  newname[u] = (Array+i)->name[k];
		  u++;
	  }
	  if(File_Exists(newname,path) == 1)
	  {
		  //Generate new Hash code for it:
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
	char filename_path[strlen(filename)+20];
  sprintf(filename_path,"./Server_Repository/%s", filename);

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
}


int StringCmp_For_Commit(const char *k, const char *ptr, int size)
{
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
    else
    {
		break;
	}
}

  //This part check if one string has more character than other or not
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
  // printf("Fix_Manifest\n" );
	const char *tab = "\t";
	const char *newline = "\n";

	//Check if Server has manifest file or not:
	char path[strlen("./Server_Repository/")+sizeof(Proj_name)]; //"./clinetrepo/projname
  sprintf(path,"./Server_Repository/%s", Proj_name);

	char File_name[strlen(Proj_name)+15]; // Proj_name.Manifest
  sprintf(File_name, "%s.Manifest", Proj_name);

	if(File_Exists(File_name,path) == 1)
	{
		//Get Manifest file info
		File_Info *Array;
		char File_Path[strlen(path)+strlen(File_name)+5];
    sprintf(File_Path, "%s/%s",path, File_name);


		int sizeofArray = 0;
		char versionNum[10];
		Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);

	int i =0;
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
	}
	else
	{
		//printf("Manifest File does not exists\n");
	}
}

//Removes all files form older version folder which has version number gerater or equal to version_num;
void Old_Version_Remove(char* Proj_name, int version_num)
{
    char path[strlen("./Server_Repository/Old_Version/")+sizeof(Proj_name)]; //"./Server_Repository/Old_Version/Proj_name
    sprintf(path,"./Server_Repository/Old_Version/%s", Proj_name);

    char new_File_Name[strlen("Version.txt")+20];
    int No =version_num;
    while(1)
    {
      sprintf(new_File_Name,"Version%d.txt", No);

      if(File_Exists(new_File_Name, path) != 1)
      {
        break;
      }
      char remove_Path[strlen(path)+2+strlen(new_File_Name)];
      sprintf(remove_Path, "%s/%s", path, new_File_Name);
      remove(remove_Path);
      No= No+1;
    }
}



int Commit_Remove(char* Proj_name)
{
		  char *slash = "/";
		  char path[strlen("./Server_Repository/")+sizeof(Proj_name)]; //"./clinetrepo/projname
      sprintf(path, "./Server_Repository/%s", Proj_name );

		  char File_Name[strlen(Proj_name)+20];
      sprintf(File_Name, "%s.Commit", Proj_name);

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
				char remove_Path[strlen(path)+2+strlen(new_File_Name)];
				strcpy(remove_Path, path);
				strcat(remove_Path, slash);
				strcat(remove_Path, new_File_Name);
				remove(remove_Path);
				No= No+1;
		  }

		return 1;

}
