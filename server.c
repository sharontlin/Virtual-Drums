#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include "networking.h"
#include "print_ascii.h"


//#define MAX_CLIENTS 4

void process( char * s , int sd);
void sub_server( int sd );

//int directory_num = 0;

//int total_clients = 0;

//sem_t mutex;
//int counter;

int main(){

  int sd, connection;

  sd = server_setup();


  while (1) {

    connection = server_connect( sd );

    int f = fork();
    if ( f == 0 ) {      
      close(sd);
      sub_server( connection );
      exit(0);      
    }
    else{
      close( connection );
    }
  }
  return 0;
}


void sub_server( int sd ) {
  
  char buffer[MESSAGE_BUFFER_SIZE];
  while (read( sd, buffer, sizeof(buffer) )) {

    printf("[SERVER %d] received: %s\n", getpid(), buffer );
    process( buffer ,sd);
    write( sd, buffer, sizeof(buffer));
  }

}

void get_message (char * filename, char * args, int sd)
{

  int c;
  FILE *file;
  char message[100];
  int i =0;

  char str[100];
  strcpy(str, filename);
  strcat(str, ".txt");


  file = fopen(str, "r");

  if (file) {
    while ((c = getc(file)) != EOF){
      message[i] = c;
      i++;
    }
    message[i] = 0;

    fclose(file);

    strcpy(args, message);
  }
}

void delete_file(char *filename, char *args){
  int ret;
  char str[100];
  strcpy(str, filename);
  strcat(str, ".txt");

  ret = remove(str);

  if(ret == 0) 
    strcpy(args,"File deleted successfully");
  else 
    strcpy(args,"Error: unable to delete the file");
}

void list_files(char * args, int sd){
  char *buffer;
  struct dirent *entry;
  DIR *dir = opendir(".");
  if (dir == NULL) {
    return;
  }
  strcpy(args, "");
  while ((entry = readdir(dir)) != NULL) {
    //   printf("%s\n",entry->d_name);
    if(strlen(entry->d_name) > 4 && memcmp(entry->d_name + strlen(entry->d_name) - 4, ".txt", 4) == 0){
      strcat(args,entry->d_name);
      strcat(args,"\n ");
    }
    //write(sd,entry->d_name, 20);
  }
  //strcat(buffer,0);
  //write(sd, buffer, sizeof(buffer));
  closedir(dir);

 }

void postmessage (char * sequence, char * filename, int sd)
{
   int i, bytesread;
  char buffer[255];

  strcpy(buffer, sequence);
  FILE *outputfile;
 
  //  bzero(buffer,255);

  char str[30];
  strcpy(str, filename);
  strcat(str, ".txt");
 
  if ((outputfile = fopen(str, "w")) == NULL)
    write(sd, "Error creating file.", 20);
 
  for (i = 0; i < strlen(sequence); i++)
    {
      if (putc(sequence[i], outputfile) == EOF)
        {
	  write(sd,"Write error\n",13);
	  break; // stop the for loop 
        }
    }
  putc(0, outputfile);

  //directory_num++;
     
  fclose(outputfile);

}


void play_sound(char * sequence){

  int length = (int)strlen(sequence);
  int i = 0;

  int fd = open("ascii.txt", O_RDONLY);
  char anime[ASCII_SIZE][MAX_ASCII_SIZE];
  read_all( anime, fd );
  close(fd);


  while(!isdigit(sequence[i++]));
  int numdrums = sequence[i-1];
  print_drums( anime, numdrums, "0" );

  for (;i<length;i++){
    int c = tolower(sequence[i]);
    if ((c=='a') && (numdrums > 0)) {
      print_drums( anime, numdrums, "a" );
      system("mpg123 sounds/sound1.mp3");
    }
    else if ((c=='b') && (numdrums > 1)) {
      print_drums( anime, numdrums, "b" );
      system("mpg123 sounds/sound2.mp3");
    }
    else if ((c=='c') && (numdrums > 2)) {
      print_drums( anime, numdrums, "c" );
      system("mpg123 sounds/sound3.mp3");
    }
    else if ((c=='d') && (numdrums > 3)) {
      print_drums( anime, numdrums, "d" );
      system("mpg123 sounds/sound3.mp3");
    }
    else if ((c=='e') && (numdrums > 4)) {
      print_drums( anime, numdrums, "e" );
      system("mpg123 sounds/sound5.mp3");
    }
    else if ((c=='f') && (numdrums > 5)) {
      print_drums( anime, numdrums, "f" );
      system("mpg123 sounds/sound6.mp3");
    }
    else if ((c=='g') && (numdrums > 6)) {
      print_drums( anime, numdrums, "g" );
      system("mpg123 sounds/sound7.mp3");
    }
  }
}




void process( char * args , int sd) {
  char * cmd;
  char * filename;

  cmd = strtok(args, " ");
  lower_string(cmd);
  
  if (strcmp(cmd,"play") == 0){
    cmd = strtok(NULL," ");
    //play_sound(cmd);
    //    play_original(cmd);
  }
  
  else if (strcmp(cmd,"save")==0){
    filename = strtok(NULL," ");
    cmd = strtok(NULL," ");
    postmessage(cmd, filename, sd);
  }
  
  else if (strcmp(cmd,"get")==0){
    filename = strtok(NULL," ");
    get_message(filename, args, sd);
  }

  else if (strcmp(cmd,"list")==0){
    list_files(args, sd);
  }

  else if(strcmp(cmd,"delete")==0){
    filename = strtok(NULL, " ");
    delete_file(filename, args);
  }
  
}
