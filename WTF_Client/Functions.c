#include "Functions.h"

//Check if given a directory name directroy exists or not
//path: (./Client_Respotory/Projname)
int isDirectory(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
  return 0;
  return S_ISDIR(statbuf.st_mode);
}

//This function takes data  and recreate them into file in which form they were orignally in
//buffer: data (numoffiles:lengthoffile1:file1name:lenthoffile1data:file1data:......)
void ReCreate_files(char* buffer)
{
  char *inital = "./Client_Repository/";
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

//This function increment version number of manifest file
//Proj_name: projectname
void Project_VersionIncrement(char * Proj_name)
{
  char path[2*strlen(Proj_name)+15];
  char* inti = "./Client_Repository/";
  strcpy(path, inti);
  strcat(path, Proj_name);
  strcat(path,"/");
  strcat(path, Proj_name);
  strcat(path,".Manifest");

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

//This function create and set it's version number to 0
//Proj_name: ./Client_repo/Projectname  ??
void CreateAndInitalize_Manifest(char* Proj_name)
{
  mkdir(Proj_name, 0700);
  int len = strlen(Proj_name);
  char path[len+20];
  strcpy(path, Proj_name);
  strcat(path,"/test.Manifest");
  int fd = open(path, O_CREAT|O_RDWR, 00700);
  int version =0;
  char tmp[10];
  sprintf(tmp,"%d", version);
  write(fd, tmp, strlen(tmp));
  char *newline = "\n";
  write(fd, newline, 1);
  write(fd, tmp, strlen(tmp));

}
//CREATE SOCKET:
//This function opens up connection between Client and Server.
//This function returns integer variable that can be use to send and get information from server.
int CreateSocket(uint32_t Port, const char *Ip)
{
  // struct sockaddr_in address;
  int sock = 0;
  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
  {
    printf("[C]: Socket creation error: can not create socket\n");
    return -1;
  }
  printf("[C]: Client Socket Created \n");

  //Connect to Server:
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(Port);

  //IP = 127.0.0.1
  if(inet_pton(AF_INET, Ip, &serv_addr.sin_addr)<=0)
  {
    printf("[C]: Invalid address/ Address not supported \n");
    return -1;
  }

  int checker = 0;
  while(checker != 1)
  {
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      printf("[C]: Trying to connect to Server \n");
      sleep(3);
    }
    else
    {checker = 1;}
  }
  return sock;
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


// This function return port and ip address by reading from configure file.
void GetIPAndPort()
{
  extern const char *Ip;
  extern uint32_t Port;

  char **Tokens = (char**)malloc(2*sizeof(char*));
  struct stat st;
  int fd = open("./Configure.txt", O_RDWR);
  if(fd > 0)
  {
    int size =0;
    stat("./Configure.txt", &st);
    size = st.st_size;
    int status = -1;
    char newchar[size];
    do
    {
      status = read(fd, newchar, size);
    }while(status > 0);


    if(newchar != NULL){
      int len = strlen(newchar);// Length of the Input string

      int c =0;
      int numberofToken =0;
      while(c<len)
      {
        if(newchar[c] == ':')
        {
          newchar[c] = '\0';
          numberofToken++;
        }
        c++;
      }
      int r=0;
      if(newchar[0] != '\0')
      {
        printf("\n");
        Tokens[0] = &newchar[0];
        r++;
      }
      else{
        printf("\n");
        numberofToken--;
      }

      int i =0;
      while(i<len)
      {// if current character is '\0' and next charcter not '\0' and number of token is
      // less than r than assign address of next character in array.
      if(newchar[i] == '\0' && newchar[i+1] != '\0' && r<=numberofToken)
      {
        Tokens[r] = &(newchar[i+1]);
        r++;
      }
      // if current character is '\0' and also next charcter is '\0' than decrement the number of Tokens.
      if(newchar[i] == '\0' && newchar[i+1] == '\0')
      {
        numberofToken--;
      }
      i++;// increment i to check next character in argv[1].
    }
}

Ip = Tokens[0];
Port = strtoul(Tokens[1],NULL, 10);

}
}

//This function checks if project exists
int Project_Exists(char *Project_Name, char *path)
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

//This function add file to manifest of that projoect
int Add_File_To_Manifest(char* Proj_name, char* filename)
{
  const char *tab = "\t";
  const char *Version = "0";
  const char *newline = "\n";
  //Check if file exists inside Project Folder:


  char *inital = "./Client_Repository/";
  char *delim = "/";
  char *Manifest = ".Manifest";
  char *u ="";
  char path[40+2*strlen(Proj_name)];
  sprintf(path, "./Client_Repository/%s", Proj_name);
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
          printf("[c]: manifest is already upto date\n");
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
    printf("[C]: Manifest file does not exists in Project\n");
    return -1;
  }
}

//This Function  return Array of struct where each struct location contain information about
// Project files
File_Info *Get_ManifestFile_Info(char * path, int *size1, char* versionNum) // takes pathname of the file.
{
  // Open manfest file and geather all the information from it
  // and save all the information into structure:
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


    char val45[strlen(Tokens[r-1])];
    char *init45 = "";
    strcpy(val45, init45);
    strcat(val45,Tokens[r-1]);

    if(r>0)
    {if(strlen(val45) < 40)
      {
        r = r-1;
      }
      }
    *size1 = (r-1);



  //All the lines of file are now stored in Tokens array of array
  //Now create struct array of size r-1 to store all the individual data.
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
else
{
  return NULL;
}
}

// Check if file exists
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
      sprintf(path1, "%s",dp->d_name);
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

//This function removes file from manifest
void Remove_File_From_Manifest(char* Proj_name, char* filename)
{
  const char *tab = "\t";
  const char *newline = "\n";
  char *inital = "./Client_Repository/";
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
      printf("[C]: file can not be removed because it's not in the manifest\n");
    }
  }
  else
  {
    printf("[C]: Manifest file does not exists in Project\n");
  }
}

//filename: projname/filename
//Hash: return string
void Create_Hash(char* filename, char *Hash)
{
  char *init = "./Client_Repository/";
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
  printf("\n");

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

//Proj_name: project name
void Fix_Manifest(char *Proj_name)
{
  const char *tab = "\t";
  const char *newline = "\n";

  //Check if Client has manifest file or not:
  char * pre = "./Client_Repository/";
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
  //printf("Done!\n");
}
else
{
  printf("[C]: Manifest File does not exists\n");
}
}
void ReGenerate_Client_Manifest(char* Proj_name)
{
  //Fix_Manifest(Proj_name);
  const char *tab = "\t";
  const char *newline = "\n";

  //Check if Client has manifest file or not:
  char * pre = "./Client_Repository/";
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
      //printf("Generating New Hashcode\n");
      char newHashCode[40];
      Create_Hash((Array+i)->name,newHashCode);

      if(strcmp((Array+i)->Hashcode,newHashCode) != 1)
      {
        strcpy((Array+i)->Hashcode, newHashCode);
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
  printf("[C]: Manifest File does not exists\n");
}
}


//array1: Client's array
//array2: Server's array
void Compare_Manifest(File_Info *array1, int size1, char* versionnum1,File_Info *array2, int size2, char*versionnum2, char *Proj_Name)
{
  //create "Projectname.update" (./Projectname.update) to store all stdout:
  printf("Code\tName\t\tVersion\t\tHashCode\n");
  char *start = "./";
  char *update = ".Update";
  char path[strlen(Proj_Name)+10];
  strcpy(path, start);
  strcat(path,Proj_Name);
  strcat(path,update);

  remove(path);
  int fd = open(path, O_CREAT|O_RDWR, 00700);
  //Create int array to keep track which line is already passed (or visited):
  const char *M = "M";
  const char *A = "A";
  const char *D = "D";
  const char *tab = "\t";
  const char *newline = "\n";

  int i =0;
  int tracker2[size2];
  for(i=0; i<size2; i++)
  {
    tracker2[i] = 0;
  }

  int tracker1[size1];
  for(i=0; i<size1; i++)
  {
    tracker1[i] = 0;
  }

  int c =0;
  for( i =0; i<size1; i++)
  {
    c =0;
    for(c =0; c<size2; c++)
    {
      if(tracker2[c] != 1)
      {
        if(StringCmp(array2[c].name, array1[i].name) == 0 && strlen(array2[c].name)== strlen(array1[i].name))
        {
          tracker1[i] = 1;
          tracker2[c] = 1;
          // printf("Names are equal!\n");
          //Client and server both contains file
          // Manifest version of both client and server file is different
          // File version is different in both Client and Server
          //Client's live hash of the file is the same as in its .Manifest.
          //Get liveHash for filename array1[i].name
          char liveHash[40];
          Create_Hash(array1[i].name, liveHash);

          if(StringCmp(liveHash, array1[i].Hashcode) == 0  &&
          StringCmp(array2[c].Version, array1[i].Version) != 0 &&
          StringCmp(versionnum1, versionnum2) != 0)
          {
            // printf("MODIFY: %s\n",array1[i].name);
            write(fd,M,1);
            write(fd,tab,1);
            write(fd,array1[i].name, strlen(array1[i].name));
            write(fd, tab,1);
            write(fd,array1[i].Version,strlen(array1[i].Version));
            write(fd,tab,1);
            write(fd,array1[i].Hashcode, strlen(array1[i].Hashcode));
            write(fd,newline,1);
            printf("%s\t%s\t%s\t%s\n", "M", array1[i].name, array1[i].Version, array1[i].Hashcode);
            break;
          }
          else if(StringCmp(array2[c].Hashcode, liveHash) != 0  &&
          StringCmp(versionnum1, versionnum2) == 0)
          {
            //printf("Update: %s\n",array1[i].name);
            printf("%s\t%s\t%s\t%s\n", "U", array1[i].name, array1[i].Version, array1[i].Hashcode);
            break;
          }
        }
      }
    }
  }

  if(StringCmp(versionnum1, versionnum2) == 0)
  {
    for(i =0; i<size1; i++)
    {
      if(tracker1[i] == 0)
      {
        /*//printf("Update: %s\n",array1[i].name);*/
        printf("%s\t%s\t%s\t%s\n", "U", array1[i].name, array1[i].Version, array1[i].Hashcode);
      }
    }
  }
  else
  {
    for(i =0; i<size1; i++)
    {
      if(tracker1[i] == 0)
      {
        //printf("Deleated: %s\n",array1[i].name);
        write(fd,D,1);
        write(fd,tab,1);
        write(fd,array1[i].name, strlen(array1[i].name));
        write(fd, tab,1);
        write(fd,array1[i].Version,strlen(array1[i].Version));
        write(fd,tab,1);
        write(fd,array1[i].Hashcode, strlen(array1[i].Hashcode));
        write(fd,newline,1);
        printf("%s\t%s\t%s\t%s\n", "D", array1[i].name, array1[i].Version, array1[i].Hashcode);
      }
    }
    for(i = 0; i<size2; i++)
    {
      if(tracker2[i] == 0)
      {
        //printf("Added: %s\n",array2[i].name);
        write(fd,A,1);
        write(fd,tab,1);
        write(fd,array2[i].name, strlen(array2[i].name));
        write(fd, tab,1);
        write(fd,array2[i].Version,strlen(array2[i].Version));
        write(fd,tab,1);
        write(fd,array2[i].Hashcode, strlen(array2[i].Hashcode));
        write(fd,newline,1);
        printf("%s\t%s\t%s\t%s\n", "A", array2[i].name, array2[i].Version, array2[i].Hashcode);
      }
    }
  }
}

Update_File_Info *Get_Update_File_Info(char * path, int *size1)
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
    Update_File_Info *array;
    array = malloc(r*sizeof(Update_File_Info));
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
      (array+i)->update = strdup(&(val[0]));

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
    return array;
  }
}

//Proj_name: project name
void Fix_Update(char *Proj_name)
{
  const char *tab = "\t";
  const char *newline = "\n";

  //Check if Client has manifest file or not:
  char * path = ".";
  char *Post = ".Update";

  char File_name[strlen(Proj_name)+15]; // Proj_name.Manifest
  strcpy(File_name, Proj_name);
  strcat(File_name, Post);
  if(File_Exists(File_name,path) == 1)
  {
    //printf("Update File exists!\n");
    //Get Update file info
    Update_File_Info *Array;
    char File_Path[strlen(path)+strlen(File_name)+5];
    strcpy(File_Path, path);
    char *slash = "/";
    strcat(File_Path,slash);
    strcat(File_Path, File_name);
    //printf("File_Path: %s\n",File_Path);

    int sizeofArray = 0;
    Array = Get_Update_File_Info(File_Path, &sizeofArray);
    //printf("size of array: %i",sizeofArray);

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

    for(i=0; i<sizeofArray; i++)
    {
      if(i != error)
      {
        write(fdN,(Array+i)->update,strlen((Array+i)->update));
        write(fdN,tab,1);
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

  //	printf("Done!\n");
}
else
{
  //printf("Update File does not exists\n");
}
}

int Check_For_Update_File(char * Proj_name)
{
  char *update = ".Update";
  char * start = "./";
  char File_Name[strlen(Proj_name)+strlen(update)+2];
  strcpy(File_Name, Proj_name);
  strcat(File_Name, update);

  if(File_Exists(File_Name,".") == 1)
  {
    //printf("Update File exists!\n");
    char path[strlen(File_Name)+5];
    strcpy(path, start);
    strcat(path,File_Name);
    struct stat st;
    int size =0;
    stat(path, &st);
    size = st.st_size;
    if(size>0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }
}

void Generate_Client_Manifest_Commit(char *Proj_name)
{
  const char *tab = "\t";
  const char *Version = "0";
  const char *newline = "\n";

  //Check if Client has manifest file or not:
  char * pre = "./Client_Repository/";
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

    int sizeofArray = 0;
    char versionNum[10];
    Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);


    //Create New File called .Commit to store all the files which have updated hashcode.
    //Create Commit File:
    //./Client_Repository/ProjectName/ProjectName.Commit
    // path = "./Client_repository/Projectname/
    char commit_path[strlen(path)+ strlen(Proj_name)+10];
    char* commit = ".Commit";
    strcpy(commit_path, path);
    strcat(commit_path, slash);
    strcat(commit_path, Proj_name);
    strcat(commit_path, commit);

    int fdN = open(commit_path,O_CREAT|O_RDWR, 00700);


    int i =0;
    //Check if File name in Manifest exists,
    //If it does than create newhash code for it,
    //and if not set Hashcode to 0, which indicates that the file in manifest
    //doesn't exists in Project Folder:
    int need_to_change =0;
    for(i=0; i<sizeofArray-1; i++)
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
      //printf("Generating New Hashcode\n");
      char newHashCode[40];
      Create_Hash((Array+i)->name,newHashCode);

      if(strcmp((Array+i)->Hashcode,newHashCode) != 1)
      {	 char *Initial_commit = "-1";
      write(fdN,Initial_commit,strlen(Initial_commit));
      write(fdN, tab,1);
      write(fdN,(Array+i)->name, strlen((Array+i)->name));
      write(fdN, tab, 1);

      int num = atoi((Array+i)->Version);
      num++;
      sprintf((Array+i)->Version ,"%d",num);

      write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
      write(fdN, tab, 1);

      write(fdN,newHashCode,strlen(newHashCode));
      write(fdN,newline,1);

      need_to_change = need_to_change+1;
    }
  }
  else
  {
    //printf("File Does not exists\n");
    if(strlen((Array+i)->Version)==40)
    {
      char *Initial_commit = "-1";
      write(fdN,Initial_commit,strlen(Initial_commit));
      write(fdN, tab,1);
      write(fdN,(Array+i)->name, strlen((Array+i)->name));
      write(fdN, tab, 1);

      int num = atoi((Array+i)->Version);
      num++;
      sprintf((Array+i)->Version ,"%d",num);

      write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
      write(fdN, tab, 1);
      char *newHashCode = "0";
      write(fdN,newHashCode,strlen(newHashCode));
      write(fdN,newline,1);
      need_to_change = need_to_change+1;
    }
  }
}
close(fdN);
//printf("Done!\n");
}
else
{
  printf("[C]: Manifest File does not exists\n");
}

}

void Fix_Server_Manifest(char *File_Path)
{
  const char *tab = "\t";
  const char *newline = "\n";

  File_Info *Array;
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

//printf("Done!\n");
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

//File_Info *array1, int size1, File_Info *array2, int size2, char*Proj_Name,
//int Finalize_Commit(Commit_File_Info *array3,int size3,File_Info *array1, int size1, File_Info *array2, int size2, char*Proj_Name, char*path)
int Finalize_Commit(File_Info *array1, int size1, File_Info *array2, int size2, char*Proj_Name, char*path)
{
  remove(path);
  //Open Local updated manifest:
  char * L = "./Client_Repository/";
  char *Lo = "Local";
  char *Me = ".Manifest";
  char * e = "/";
  const char *Zero = "0";
  const char *One = "1";
  const char *Two = "2";
  const char *tab = "\t";
  // const char *Version = "0";
  const char *newline = "\n";

  char Local_Path[strlen(Proj_Name)+strlen(L)+strlen(Lo)+strlen(Me)+strlen(e)];
  sprintf(Local_Path, "./Client_Repository/%s/Local.Manifest",Proj_Name);

  //Local Manifest
  File_Info *Local;
  int Local_size;
  char Local_Version[10];
  Local = Get_ManifestFile_Info(Local_Path,&Local_size,Local_Version);

  int fd = open(path, O_CREAT|O_RDWR, 00700); //Commit file
  //Create int array to keep track which line is already passed (or visited):

    int i =0;
    int tracker2[size2]; //server's manfest
    for(i=0; i<size2; i++)
    {
      tracker2[i] = 0;
    }

    int tracker1[size1];// client's manfest
    for(i=0; i<size1; i++)
    {
      tracker1[i] = 0;
    }

    printf("Code\tName\t\tVersion\t\tHashCode\n");
    //Update:
    int checker = 0;
    int c =0;
    for( i =0; i<size1; i++)
    {
      // printf("array1[i].name : %s\n",array1[i].name);
      c =0;
      for(c =0; c<size2; c++)
      {
        if(tracker2[c] != 1)
        {
          if(StringCmp(array2[c].name, array1[i].name) == 0)
          {
            tracker1[i] = 1;
            tracker2[c] = 1;
            // printf("Names are equal!, %d\n", Local_size);

            int a =0;
            for(a =0; a<Local_size; a++)
            {
              // printf("a == %d\n", Local_size);
              if(StringCmp(array1[i].name, Local[a].name) == 0)
              {
                // printf("Entry found in local %s\n", Local[a].name);
                // printf("array1[i].Version == %s, Local[a].Version == %s\n", array1[i].Version, Local[a].Version);
                // printf("array1[i].Hashcode == %s, Local[a].Hashcode == %s \n", array1[i].Hashcode,Local[a].Hashcode);
                if(StringCmp(array1[i].Version, Local[a].Version) == -1 && StringCmp(array1[i].Hashcode, Local[a].Hashcode) != 0)
                {
                  write(fd,Two,1);
                  write(fd,tab,1);
                  write(fd,array1[i].name, strlen(array1[i].name));
                  write(fd, tab,1);
                  write(fd,array1[i].Version,strlen(array1[i].Version));
                  write(fd,tab,1);
                  write(fd,array1[i].Hashcode, strlen(array1[i].Hashcode));
                  write(fd,newline,1);
                  printf("%s\t%s\t\t%s\t\t%s\n", Two, array1[i].name,array1[i].Version,array1[i].Hashcode);
              }
                checker = 1;
                break;
              }
            }

            if(checker == 0)
            {
              printf("[C]: Conditions doesn't match!\n");
              remove(path);
              return 0;
            }
          }

        }
      }
    }

    //Files that are in client's manifest that are not in server's manfest:
    //Addition: '1'
    for(i =0; i<size1; i++)
    {
      if(tracker1[i] == 0)
      {
        if(strlen(array1[i].Hashcode) == 40)
        {
          write(fd,One,1);
          write(fd,tab,1);
          write(fd,array1[i].name, strlen(array1[i].name));
          write(fd, tab,1);
          write(fd,array1[i].Version,strlen(array1[i].Version));
          write(fd,tab,1);
          write(fd,array1[i].Hashcode, strlen(array1[i].Hashcode));
          write(fd,newline,1);
          printf("%s\t%s\t\t%s\t\t%s\n", One, array1[i].name,array1[i].Version,array1[i].Hashcode);
        }
      }
    }

    //Delete
    for(i = 0; i<size2; i++)
    {
      if(tracker2[i] == 0)
      {
        if(strlen(array2[i].Hashcode) == 40)
        {
          write(fd,Zero,1);
          write(fd,tab,1);
          write(fd,array2[i].name, strlen(array2[i].name));
          write(fd, tab,1);
          write(fd,array2[i].Version,strlen(array2[i].Version));
          write(fd,tab,1);
          write(fd,array2[i].Hashcode, strlen(array2[i].Hashcode));
          write(fd,newline,1);
          printf("%s\t%s\t\t%s\t\t%s\n",Zero,array2[i].name,array2[i].Version,array2[i].Hashcode);
        }}
      }
  close(fd);
  return 1;
}

int Update_File_For_Push(char *Proj_name)
{
  char *update = ".Update";
  char * start = "./";
  char File_Name[strlen(Proj_name)+strlen(update)+2];
  strcpy(File_Name, Proj_name);
  strcat(File_Name, update);

  if(File_Exists(File_Name,".") == 1)
  {
    //printf("Update File exists!\n");
    char path[strlen(File_Name)+5];
    strcpy(path, start);
    strcat(path,File_Name);
    struct stat st;
    int size =0;
    stat(path, &st);
    size = st.st_size;
    if(size>0)
    {
      char buffer[size];
      int fd = open(path,O_RDWR, 00700);
      read(fd, buffer, size);
      close(fd);

      Update_File_Info *Array;
      int size1 =0;
      Array = Get_Update_File_Info(path, &size1);

      int i =0;
      for(i =0; i<size1; i++)
      {
        if(StringCmp((Array[i].update),"M") == 0 && strlen(Array[i].Hashcode) ==40)
        {
          return 0;
        }
      }

      return 1;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }
}

void Send_File_Protocol(char *path, int fd, char* FilenamePath) //Writes a file into sendfile.txt
{
  char Filename[strlen(FilenamePath)+25];
  char *initial = "./Client_Repository/";
  strcpy(Filename, initial);
  strcat(Filename, FilenamePath);

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
    }}
    closedir(dir);
  }

  void Send_File_Protocol_initalize(char * send, char *Project_name)
  {

    char path[25+strlen(Project_name)];
    char *initial = "./Client_Repository";
    strcpy(path, initial);
    strcat(path,"/");
    strcat(path, Project_name);
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



void Write_File_Protocol(int fd, char* path)
{
  const char *del = ":";
  char tmp[10];
  //Read file at path given and write into a file "id" with sending protocols:
  char complete_path[strlen(path)+strlen("./Client_Repository/")+2];
  sprintf(complete_path, "./Client_Repository/%s", path);

      int size;
      struct stat st;
      stat(complete_path, &st);
      size = st.st_size;

      int fdT = open(complete_path, O_RDWR, 00700);

      int status = -1;
      char buffer[size];
      do
      {
        status = read(fdT, buffer, size);
      }while(status > 0);
      close(fdT);

    sprintf(tmp,"%d", strlen(path));
    write(fd,tmp, strlen(tmp));
    write(fd,del,1);
    write(fd,path, strlen(path));
    write(fd,del,1);
    sprintf(tmp,"%d", size);
    write(fd, tmp, strlen(tmp));
    write(fd,del,1);
    write(fd,buffer,size);
    write(fd,del,1);
}



  int Send_Project_Protocol2(char *Project_name, int fd, char* commit_path)
  {
    int Number_of_files_sent =0;
    const char *del = ":";
    int size_commit =0;
    Commit_File_Info *Client_Commit = Get_Commit_File_Info(commit_path, &size_commit);
    int i =0;
    char temp_name[strlen(Project_name)+ strlen("./ Temp_send.send")];
    sprintf(temp_name, "./%s_Temp_send.send",Project_name);
    int temp_send = open(temp_name, O_CREAT|O_RDWR, 00700);
    for(i =0; i < size_commit; i++)
    {
      if(StringCmp((Client_Commit+i)->commit, "0") != 0 && strlen((Client_Commit+i)->Hashcode)==40)
      {
        Write_File_Protocol(temp_send, (Client_Commit+i)->name);
        Number_of_files_sent++;
      }
    }

    //Also needs to add project's Manifest file:
    // char Client_manifest[strlen(Project_name)+strlen(Project_name)+strlen(".Manifest")+2];
    // sprintf(Client_manifest, "%s/%s.Manifest", Project_name, Project_name);
    // Write_File_Protocol(temp_send, Client_manifest);
    // Number_of_files_sent++;
    close(temp_send);

    char tmp[10];
    sprintf(tmp,"%d",Number_of_files_sent);
    write(fd,tmp, strlen(tmp));
    write(fd,del,1);

    int size;
    struct stat st;
    stat(temp_name, &st);
    size = st.st_size;
    int fdT = open(temp_name, O_RDWR, 00700);

    int status = -1;
    char buffer[size];
    do
    {
      status = read(fdT, buffer, size);
    }while(status > 0);
    close(fdT);
    write(fd,buffer, strlen(buffer));
    remove(temp_name);
    return 1;
  }

  void Send_Project_Protocol(char *Project_name, int fd)
  {

    char path[strlen(Project_name)+25];
    char *initial = "./Client_Repository/";
    strcpy(path, initial);
    strcat(path, Project_name);
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


      void Generate_Live_Client_Manifest(char* Proj_name)
      {
        const char *tab = "\t";
        const char *newline = "\n";

        //Check if Client has manifest file or not:
        char * pre = "./Client_Repository/";
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
          char File_Path[strlen(path)+strlen(File_name)+15];
          strcpy(File_Path, path);
          char *slash = "/";
          strcat(File_Path,slash);
          strcat(File_Path, File_name);

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
          // remove(File_Path);//Remove Old Manifest and add New Manifest:
          char * Live = "Live";
          strcat(File_Path, Live);
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
        printf("[C]: Manifest File does not exists\n");
      }
    }


    void ReCreate_files_Change_Manifest(char* buffer)
    {

      char *inital = "./Client_Repository/";
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

        if(strlen(filenamesize) != 0)
        {
        num = (int)strtol(filenamesize, (char**)NULL,10); //File name size;

        //Get filename size:
        char filename[num+1];
        s = s+1;
        int y =0;
        for(y = s; y< s+num;y++)
        {
          filename[y-s] = buffer[y];
        }
        filename[num] = '\0';
        char newfilename[num+25];
        strcpy(newfilename, inital);
        strcat(newfilename, filename);

        char Projname[strlen(filename)];
        char Filename[strlen(filename)];
        int l =0;
        while(filename[l] != '/')
        {
          Projname[l] = filename[l];
          l = l+1;
        }
        Projname[l] = '\0';
        l = l+1;
        while(l <strlen(filename))
        {
          Filename[l-strlen(Projname)-1] = filename[l];
          l = l+1;
        }
        Filename[l-strlen(Projname)-1] = '\0';

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
    }



    void Generate_Client_Manifest_Local(char *Proj_name)
    {
      const char *tab = "\t";
      const char *newline = "\n";

      //Check if Client has manifest file or not:
      char * pre = "./Client_Repository/";
      char path[strlen(pre)+sizeof(Proj_name)]; //"./clinetrepo/projname
      sprintf(path, "./Client_Repository/%s", Proj_name);

      char File_name[strlen(Proj_name)+15]; // Proj_name.Manifest
      sprintf(File_name, "%s.Manifest", Proj_name);


      if(File_Exists(File_name,path) == 1) //Checking if client has manifest file or not
      {
        //Get Clien't orignal Manifest file info:
        File_Info *Array;
        char File_Path[strlen(path)+strlen(File_name)+5];
        sprintf(File_Path, "%s/%s", path, File_name);
        printf("FILE PATH: %s", File_Path);
        int sizeofArray = 0;
        char versionNum[10];
        Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);


        //Create New File called .Commit to store all the files which have updated hashcode.
        //Create Commit File:
        //./Client_Repository/ProjectName/ProjectName.Commit
        // path = "./Client_repository/Projectname/
        char Local_path[strlen(path)+ strlen(Proj_name)+10];
        sprintf(Local_path, "%s/Local.Manifest", path);


        //Assigning to 0 to version number for local manifest file
        int fdN = open(Local_path,O_CREAT|O_RDWR, 00700);
        char *One = "0";
        write(fdN, One, strlen(One));
        write(fdN, newline, 1);


        int i =0;
        //Check if File name in Manifest exists,
        //If it does than create newhash code for it,
        //and if not set Hashcode to 0, which indicates that the file in manifest
        //doesn't exists in Project Folder:

        int need_to_change =0;
        for(i=0; i<sizeofArray; i++)
        {
        char newname[strlen((Array+i)->name)-strlen(Proj_name)+1];
        int k =0;
        int u =0;
        for(k = strlen(Proj_name)+1; k<strlen((Array+i)->name); k++)
        {
          newname[u] = (Array+i)->name[k];
          u++;
        }

        newname[strlen((Array+i)->name)-1] = '\0';

        if(File_Exists(newname,path) == 1)
        {
          //Generate new Hash code for it:
          //printf("Generating New Hashcode\n");
          char newHashCode[40];
          Create_Hash((Array+i)->name,newHashCode);

          if(strcmp((Array+i)->Hashcode,newHashCode) != 1)
          {
            write(fdN,(Array+i)->name, strlen((Array+i)->name));
            write(fdN, tab, 1);

            int num = atoi((Array+i)->Version);
            num++;
            sprintf((Array+i)->Version ,"%d",num);

            write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
            write(fdN, tab, 1);

            write(fdN,newHashCode,strlen(newHashCode));
            write(fdN,newline,1);

            need_to_change = need_to_change+1;
          }
          else
          {
            write(fdN,(Array+i)->name, strlen((Array+i)->name));
            write(fdN, tab, 1);

            write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
            write(fdN, tab, 1);

            write(fdN,newHashCode,strlen(newHashCode));
            write(fdN,newline,1);
          }
        }
        else
        {
          //printf("File Does not exists\n");
          if(strlen((Array+i)->Version)==40)
          {
            write(fdN,(Array+i)->name, strlen((Array+i)->name));
            write(fdN, tab, 1);

            int num = atoi((Array+i)->Version);
            num++;
            sprintf((Array+i)->Version ,"%d",num);

            write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
            write(fdN, tab, 1);
            char *newHashCode = "0";
            write(fdN,newHashCode,strlen(newHashCode));
            write(fdN,newline,1);
            need_to_change = need_to_change+1;
          }
        }
      }
      close(fdN);
    }
    else
    {
      printf("[C]: Manifest File does not exists\n");
    }

  }


  void Generate_Client_Manifest_Local2(char *Proj_name)
  {
    const char *tab = "\t";
    const char *newline = "\n";

    //Check if Client has manifest file or not:

    //Project path: "./clinetrepo/projname"
    char path[strlen("./Client_Repository/")+sizeof(Proj_name)];
    sprintf(path, "./Client_Repository/%s", Proj_name);

    //Manifist file name: "Proj_name.Manifest"
    char File_name[strlen(Proj_name)+15];
    sprintf(File_name, "%s.Manifest", Proj_name);


    if(File_Exists(File_name,path) == 1) //Checking if client has manifest file or not
    {
      //Get Client's orignal Manifest file info:
      File_Info *Array;
      char File_Path[strlen(path)+strlen(File_name)+5];
      sprintf(File_Path, "%s/%s", path, File_name);
      int sizeofArray = 0;
      char versionNum[10];
      Array = Get_ManifestFile_Info(File_Path, &sizeofArray, versionNum);

      //Path to Local client manifest file: "./Client_Repository/ProjectName/Local.Manifest"
      char Local_path[strlen(path)+ strlen(Proj_name)+10];
      sprintf(Local_path, "%s/Local.Manifest", path);

      //Assigning to 0 to version number for local manifest file
      int fdN = open(Local_path,O_CREAT|O_RDWR, 00700);
      write(fdN, versionNum, strlen(versionNum));
      write(fdN, newline, 1);


      int i =0;
      //Check if File name in Manifest exists,
      //If it does than create newhash code for it,
      //and if not set Hashcode to 0, which indicates that the file in manifest
      //doesn't exists in Project Folder:
      char path1[1000] = "";
      struct dirent *dp;
      DIR *dir = opendir(path);

      int need_to_change =0;

      for(i=0; i<sizeofArray; i++)
      {
        char newname[strlen((Array+i)->name)-strlen(Proj_name)+1];
        int k =0;
        int u =0;
        for(k = strlen(Proj_name)+1; k<strlen((Array+i)->name); k++)
        {
          newname[u] = (Array+i)->name[k];
          u++;
        }
        newname[u] = '\0';

        int cheker = -1;

        if(dir != NULL)
        {
          while ((dp = readdir(dir)) != NULL)
          {
            if(strcmp(dp->d_name, ".")!= 0&& strcmp(dp->d_name,"..")!=0)
            {
              sprintf(path1, "%s",dp->d_name);
              if(!isDirectory(path1))
              {
                if(strcmp(dp->d_name,newname)==0)
                {
                  cheker = 1;
                  break;
                }
              }
            }
          }
        }
        else
        {
          cheker = 0;
        }
        rewinddir(dir);


        if(cheker == 1)
        {
          //Generate new Hash code for it:
          //printf("Generating New Hashcode\n");
          char newHashCode[40];
          Create_Hash((Array+i)->name,newHashCode);

          if(strcmp((Array+i)->Hashcode,newHashCode) != 1)
          {
            write(fdN,(Array+i)->name, strlen((Array+i)->name));
            write(fdN, tab, 1);
            int num = atoi((Array+i)->Version);
            num++;
            sprintf((Array+i)->Version ,"%d",num);
            write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
            write(fdN, tab, 1);
            write(fdN,newHashCode,strlen(newHashCode));
            write(fdN,newline,1);
            need_to_change = need_to_change+1;
          }
          else
          {
            write(fdN,(Array+i)->name, strlen((Array+i)->name));
            write(fdN, tab, 1);
            write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
            write(fdN, tab, 1);
            write(fdN,newHashCode,strlen(newHashCode));
            write(fdN,newline,1);
          }
        }
        else
        {
          // printf("File Does not exists\n");
          write(fdN,(Array+i)->name, strlen((Array+i)->name));
          write(fdN, tab, 1);
          int num = atoi((Array+i)->Version);
          num++;
          sprintf((Array+i)->Version ,"%d",num);
          write(fdN,(Array+i)->Version, strlen((Array+i)->Version));
          write(fdN, tab, 1);
          write(fdN,"0",strlen("0"));
          write(fdN,newline,1);
          need_to_change = need_to_change+1;
        }
      }
      close(fdN);

    }
    else
    {
      printf("[C]: Manifest File does not exists\n");
    }

  }
