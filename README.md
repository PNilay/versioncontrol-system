# Version Control System: Where's the File?
## Systems Programming
## Overview  
Version control system similar to Git is implemented in this project. The project is based on the centralized version control systems (CVCS), in which some remote server holds canonical and definitive repository, and each client (user) gets their own working copy of the project on their local device. Any number of clients can fetch projects from the repository, and push changes to projects on the server. Version control systems are a way of managing the most large, intricate software projects, where multiple people form different parts of the world are contributing to a single set of source code. Version control systems permit developers to keep track of all the changes in software development projects, and allows them to collaborate on different projects.

To enforce the synchronization, the program does not allow any user to make changes to the server’s copy of  the project, if that user hasn't seen the recent changes made by other contributors to the repository (user’s project version is different from the latest version of the project available on server). In that situation, the client has to first update it’s local copy with the latest version of the server's project, and then it can make additional changes to it and submit it to the server’s repository. 

This project saves all the updates and changes made to a repository/Project by all the contributors since the creation of the project. Every new version of the project is saved in Server’s side so it is possible to fetch older versions when necessary. This capability is really useful when the programmer makes a mistake in a newer version of the project, or realizes that the current direction is flawed then it can be easily reverted back to the older version. This system also protects contributors from development accidents such as deleting a file, since the programmer can just update from the server and get it restored.  
## Implementation
The project is divided into two major programs:  
Client Side: Client program takes “.\WTF” command and numbers of arguments, and performs specific tasks requested by the user. Tasks such as create or destroy a project, add or remove files from a project, fetch current version of entire project from the server or make changes to the current version of the project, or get metadata about the project. 

Server Side: Server side maintains a single canonical version of the project, as well as metadata and backups/historical data. The server program is multithreaded to accommodate numbers of clients at once. Server program performs tasks required by the users. 
## Usage
Both WTF_Server and WTF_Client program can build by running make commands in their respective directories:  
```
make
```
To remove all build files run make clean command in in both WTF_Server and WTF_Client directory:
```
make clean
```
### WTF_Server program:
Server program is executed by a single command line argument, port number to listen on;  

```
./WTFServer <port>
```
The server program now can listen to clients reaching out to the <port>. The server program can be quit with a SIGINT (ctrl+c) in the foreground of its process.
  
### WTF_Client program:
The client program will send commands to the server and the server will send a response back to the client.  
##### WTF_Client commands:
* configure : This command needs to be run before running any other command, since this command saves the IP address and port number of the server, which is used by almost all later commands.
```
./WTF configure <IP> <port>
```
* checkout: With this program clients can request specific projects from the server, and create a local copy of the project on the local machine.
```
./WTF checkout <project_name>
```
* Update and upgrade: sync local project with server’s project. (Client side file changes)
  * update: This command records all the files, which needs to be modified, added or removed from the client side in .Update file. Commands work by comparing local copy of the .Manifest file with the server’s copy.
  ```
  ./WTF update <project_name>
  ```
  * upgrade: This command will apply the changes listed in the .Update to the client’s local copy of the project. It deletes all the files tagged with “D”, and fetch newer files from the server and overwrite all files on the client side that are tagged with “M” or “A”. (command requires the update to be run before this)
  ```
  ./WTF upgrade <project_name>
  ```
* Commit and push: Sync server’s project with changes from the local repository. (Server’s project changes)
  * commit: This command determines which files need to be deleted, added and modified in the server’s project, and save it in .Commit file.
  ```
  ./WTF commit <project_name>
  ```
  * push: This command applies all the changes listed in the .Commit file to the server’s copy of the project after saving the current version of the server’s project to history. (command requires the commit to be run before this)
  ```
  ./WTF push <project_name>
  ```
* create: The create command creates a new project if it doesn't already exist on the server or the client side. The server will create a project folder with the given name, initialize a .Manifest for it and send it to the client. The client sets up a local version of the project folder in its directory and places the .Manifest file sent in it.
```
./WTF create <project_name>
```
* destroy: On a receiving destroy command the server locks the repository, expire all the pending commands and delete all the files and subdirectories under the project and send back a success message.
```
./WTF destroy <project_name>
```
* add: The add command locally adds an entry for the file in it’s .Manifest with new version number and hashcode if the file exists in the project directory.
```
./WTF add <project_name> <file_name>
```
* remove: The client will remove the entry for the given file from its own .Manifest.
```
./WTF remove <project_name>
```
* currentversion: The currentversion will request the server the current state of a project from the server. This command does not require that the client has a copy of the project locally. The client outputs a list of all files under the project name, along with their version number.
```
./WTF currentversion <project_name>
```
* history: The history command will request the server a file containing the history of all operations performed on all successful pushes since the project’s creation. This command does not require that the client has a copy of the project locally. The client outputs all the history information to the terminal.
```
./WTF history <project_name>
```
* rollback: This command does not require that the client has a copy of the project locally. The server reverts its current version  of the project back to the version number requested by the client by deleting all more recent versions saved on the server side.
```
./WTF rollback <project_name> <version_number>
```
