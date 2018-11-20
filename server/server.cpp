/*TRABALHO SISTEMAS OPERACIONAIS*
  /*NOMES : ALECSANDER GESSER, CAIO BLUMER*/
  /**/
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>
  #include <dirent.h>
  #include <pthread.h>
  #include <semaphore.h>
  #include <sys/socket.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <dirent.h>
#include <arpa/inet.h>
       #include <signal.h>
  #define porta 8088

  #include "test.hpp"

  int strcmpst1nl (const char * s1, const char * s2);
  void *connection_handler(void *welcomeSocket);

  // operations
  void create_file(int sock, char* name, int current_dir);
  int mark_file(int sock, int i, int j, char* namefile,  int current_dir);
  void remove_file(int sock, char* name, int current_dir);
  void  edit_file(int sock, char* name, int current_dir);
  void show_file(int sock, char* namefile, int current_dir);
  void cd_directory(int sock, char* name, int& current_dir);
  void list_directory(int sock, int current_dir);
  void create_dir(int sock, char* name, int current_dir);
  void rmv_dir(int sock, char* name, int current_dir);
  void rmv_dir_rec(int sock, char* name, int current_dir);

  pthread_mutex_t lock;

  int main(){

    int welcomeSocket, newSocket, *new_socket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in client;

    socklen_t addr_size;

    initialize_bin(); // fp bin file




    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(welcomeSocket == -1)
    {
      printf("Error - Create socket");
    }

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(porta);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr =INADDR_ANY;
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '0', sizeof serverAddr.sin_zero);

    /*---- Bind the address struct to the socket ----*/
    if(bind(welcomeSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
      printf("Error: Bind");
      return 1;
    }

    if (pthread_mutex_init(&lock, NULL) != 0)
     {
         printf("\n mutex init failed\n");
         return 1;
     }

    /*---- Listen on the socket, with 5 max connection requests queued ----*/
    while(1)
    {

        if(listen(welcomeSocket,5)==0)
          printf("Listening\n");
        else
          printf("Error\n");

        /*---- Accept call creates a new socket for the incoming connection ----*/
        addr_size = sizeof (struct sockaddr_in);

        while(newSocket = accept(welcomeSocket, (struct sockaddr *) &client, (socklen_t*)&addr_size))
        {
          struct in_addr ipAddr = client.sin_addr;
          char ip[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);

          puts("Connection accepted");
          pthread_t thread_id;
          new_socket = (int*)malloc(sizeof(int));
          *new_socket = newSocket;


          if( pthread_create( &thread_id, NULL ,  connection_handler , (void*)new_socket) < 0)
              {
                  perror("could not create thread");
                  return 1;
              }
              puts("Handler assigned");
        }
        if (new_socket < 0)
        {
            perror("accept failed");
            return 1;
        }
        pthread_mutex_destroy(&lock);
    }


    fclose(fp);
    return 0;


    /*---- Send message to the socket of the incoming connection ----*/
    //strcpy(buffer,"Hello World\n");
    //send(newSocket,buffer,13,0);


  }

  void *connection_handler(void *welcomeSocket)
  {
      //Get the socket descriptor
      int current_dir = current_DIR;
      char current_dir_name[1024];
      int sock = *(int*)welcomeSocket;
      int read_size;
      char *op, *name;
      char message[1024] , client_message[1024] = {'\0'};
      getcwd(current_dir_name, sizeof(current_dir_name));
      //current_dir = opendir(current_dir_name); // aponta para diretorio atual

      //Send some messages to the client
      strcpy(message, "Greetings! I am your connection handler");
      send(sock, message, strlen(message), 0);
      read_size = read(sock, client_message, 1024);

      op = strtok(client_message," \0");
      name = strtok(NULL," \0");

      if(strcmpst1nl(op,"ls") == 0){
          name = "default";
          //printf("entrou if op==ls\n" );
      }else if(strcmpst1nl(op,"exit") == 0){name = "default";}
      else
      {
      //  printf("entrou else ls\n");
        name[strlen(name)-1] = '\0';
      }
      printf("%s\n",op );
      printf("%s\n",name );
      //Receive a message from client
      while( 1)
      {
        if(strcmpst1nl(op,"mkdir") == 0)
        {
          create_dir(sock, name, current_dir);
        }
        else if (strcmpst1nl(op,"rmdir") == 0)
        {
          pthread_mutex_lock(&lock);
          rmv_dir(sock, name, current_dir);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "mkfile") == 0)
        {
        //  pthread_mutex_lock(&lock);
          create_file(sock, name, current_dir);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "rmfile") == 0)
        {
          pthread_mutex_lock(&lock);
          remove_file(sock, name, current_dir);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op,"edit") == 0)
        {
          pthread_mutex_lock(&lock);
          edit_file(sock, name, current_dir);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "show") == 0)
        {
          pthread_mutex_lock(&lock);
          show_file(sock, name, current_dir);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op,"cd") == 0)
        {
        //  pthread_mutex_lock(&lock);
          cd_directory(sock, name, current_dir);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "ls") == 0)
        {
        //  pthread_mutex_lock(&lock);
          list_directory(sock, current_dir);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "exit") == 0)
        {
          close(sock);
          pthread_kill(pthread_self(), 0);
        }else
        {
          strcpy(message, "fail");
          send(sock, message, strlen(message), 0);
        }
        memset(client_message, '\0', 1024);
        read_size = read(sock, client_message, 1024);
        op = strtok (client_message," \0");
        name = strtok (NULL," \0");
        if(strcmpst1nl(op,"ls") == 0){
            name = "default";
            //printf("entrou if op==ls\n" );
        }else if(strcmpst1nl(op,"exit") == 0){name = "default";}
        else
        {
        //  printf("entrou else ls\n");
          name[strlen(name)-1] = '\0';
        }


        printf("%s\n",op );
        printf("%s\n",name );
      }
}

  void list_directory(int sock, int current_dir)
  {
     inode aux,aux2;
     int cont=0;
     char msg[1024] = "";
     fseek(fp, current_dir, SEEK_SET);
     fread(&aux, sizeof(inode),1,fp);
    // printf("location:  %s", aux.name);
     for(int i = 0 ; i < 10 ; i++){
        if( aux.nodes[i] != -1){
           fseek(fp, aux.nodes[i], SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           strcat(msg, aux2.name);
           strcat(msg,"\n");
           cont++;
        }
     }
     if(cont == 0) sprintf(msg,"empty");
     send(sock, msg, strlen(msg), 0);




    // char current[1024] = "";
    // getcwd(current,sizeof(current));
    // struct dirent *dir = NULL;
    // DIR* current_dir = opendir(current);
    // dir = readdir(current_dir);
    // memset(msg, 0, sizeof(msg));
    // while(dir = readdir(current_dir)){
    //   strcat(msg, dir->d_name);
    //   strcat(msg, "\n");
    // }
    // rewinddir(current_dir);
    // send(sock, msg, strlen(msg), 0);
}

  void cd_directory(int sock, char* name, int& current_dir)
  {
     char msg[1024] = "";
     inode aux, aux2;

     if( name[0] == '.' && name[1] == '.')
     {
        fseek(fp, current_dir, SEEK_SET);
        fread(&aux, sizeof(inode), 1, fp);
        if( aux.parent_addr == -1)
        {
           strcpy(msg,"Error already on ROOT");
           send(sock, msg, strlen(msg), 0 );
           return;
        }
        else
        {
           current_dir = aux.parent_addr;
           strcpy(msg,"Sucess cd");
           send(sock, msg, strlen(msg), 0 );
           return;
        }
     }
     fseek(fp, current_dir, SEEK_SET);
     fread(&aux, sizeof(inode), 1, fp);
     for(int i = 0 ; i < 10 ; i++)
     {
        fseek(fp, aux.nodes[i], SEEK_SET);
        fread(&aux2, sizeof(inode), 1, fp);
        if( strcmp( aux2.name, name) == 0 && aux2.type[0] == 'd' )
        {
           current_dir = aux.nodes[i];
           strcpy(msg,"Sucess cd");
           send(sock, msg, strlen(msg), 0 );
           return;
        }
     }

     strcpy(msg,"Failed cd");
     send(sock, msg, strlen(msg), 0 );

    // FILE* fp;
    // char msg[1024];
    // int read_size;
    // char current[1024];
    //
    // if(chdir(name) == -1)
    // {
    //   strcpy(msg,"Error changing directory");
    //   send(sock, msg, strlen(msg), 0 );
    // }
    // else
    // {
    //   getcwd(current, sizeof(current));
    //   current_dir = opendir(current);
    //   strcpy(msg, "Success!");
    //   send(sock, msg, strlen(msg), 0);
    // }
  }

  void show_file(int sock, char* namefile, int current_dir)
  {
      inode aux, aux2;
   	char dado[1024];

   	fseek(fp, current_dir, SEEK_SET);
   	fread(&aux, sizeof(inode), 1, fp);
   	for(int i = 0; i < 10 ; i++)
   	{
   		if( aux.nodes[i] != -1)
   		{
   			fseek(fp, aux.nodes[i], SEEK_SET);
   			fread(&aux2, sizeof(inode), 1, fp);
   			if(strcmp(namefile, aux2.name) == 0 && aux2.type[0] == 'f')
   			{
   				fseek(fp, aux2.addr_i, SEEK_SET);
   				fread(&dado, aux2.addr_f - aux2.addr_i, 1, fp);
   				send(sock, dado, strlen(dado), 0);
               return ;
   			}
   		}
   	}

      strcpy(dado, "Failed opening");
      send(sock, dado, strlen(dado),0);

    // FILE* fp;
    // char msg[1024];
    // int read_size;
    //
    // fp = fopen(name, "r");
    // if(fp == NULL){
    //   strcpy(msg,"Error on opening file");
    //   send(sock, msg, strlen(msg), 0 );
    // }
    // else
    // {
    //   fgets(msg, 1024, fp);
    //   send(sock, msg, strlen(msg), 0 );
    // }
    // fclose(fp);

  }

  void  edit_file(int sock, char* name, int current_dir){

     inode aux,aux2;
     int cont=0;
     char msg[1024] = "";
     char buf[1024] = "";

     fseek(fp, current_dir, SEEK_SET);
     fread(&aux, sizeof(inode),1,fp);
     for(int i = 0 ; i < 10 ; i++){
        if( aux.nodes[i] != -1){
           fseek(fp, aux.nodes[i], SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           if( strcmp(aux2.name, name) == 0 )
           {
              fseek(fp, aux2.addr_i, SEEK_SET);
              strcpy(msg,"Type your text: ");
              send(sock, msg, strlen(msg), 0 );
              int read_size = read(sock, buf, 1024);
              buf[read_size-1] = '\0';
              fwrite(&buf, sizeof(char), strlen(buf), fp);
              strcpy(msg, "Sucess edit");
              send(sock, msg, strlen(msg), 0);
              return;
           }
        }
     }

     strcpy(msg, "Failed edit");
     send(sock, msg, strlen(msg), 0);




    // FILE* fp;
    // char msg[1024];
    // int read_size;
    // fp = fopen(name, "w");
    // if(fp == NULL){
    //   strcpy(msg,"Error on opening file");
    //   send(sock, msg, strlen(msg), 0 );
    // }
    // else
    // {
    //   strcpy(msg,"Type your text: ");
    //   send(sock, msg, strlen(msg), 0 );
    //   read_size = read(sock, name, 1024);
    //   name[read_size-1] = '\0';
    //
    //   if(fprintf(fp, "%s\n",name) < 0)
    //   {
    //     strcpy(msg,"Edit failed: ");
    //     send(sock, msg, strlen(msg), 0 );
    //   }
    //   else{
    //     strcpy(msg,"Edit sucessed: ");
    //     send(sock, msg, strlen(msg), 0 );
    //   }
    // }
    // fclose(fp);
  }


  void remove_file(int sock, char* name, int current_dir)
  {
     inode aux,aux2, blank;
     int cont=0, zero=0, addr_temp;
     char msg[1024] = "";
     char buf[1024] = "";

     int temp=0;

     blank.used = 0;
  	  strcpy(blank.name, "blank");
  	  strcpy(blank.type, "none");
  	  blank.addr_i = 0;
  	  blank.addr_f = 0;
  	  blank.parent_addr = 0;




     fseek(fp, current_dir, SEEK_SET);
     fread(&aux, sizeof(inode),1,fp);
     for(int i = 0 ; i < 10 ; i++){
       if( aux.nodes[i] != -1){
          fseek(fp, aux.nodes[i], SEEK_SET);
          fread(&aux2, sizeof(inode),1,fp);
          if( strcmp(aux2.name, name) == 0 && aux2.type[0] == 'f' )
          {

             // finding bit map refered
            addr_temp = aux2.addr_i - ( blocks*sizeof(int) + ( inode_qnt * sizeof(inode) ) );
            addr_temp = addr_temp/block_size;
            fseek(fp, addr_temp*sizeof(int), SEEK_SET);
            fwrite(&zero, sizeof(int), 1, fp);

            // writing blank inode
            fseek(fp, aux.nodes[i], SEEK_SET);
            fwrite(&blank, sizeof(inode), 1, fp);

            // altering reference inodes nodes
            fseek(fp, current_dir, SEEK_SET);
            fread(&aux, sizeof(inode), 1, fp);
            aux.nodes[i] = -1;
            fseek(fp, current_dir, SEEK_SET);
            fwrite(&aux, sizeof(inode), 1, fp);

            strcpy(msg, "Sucess remove_file");
            send(sock, msg, strlen(msg), 0);
            return;
          }
       }
     }

     strcpy(msg, "Failed remove_file");
     send(sock, msg, strlen(msg), 0);



    // char msg[1024];
    // int read_size;
    // if ( remove(name) != 0) {
    //   strcpy(msg,"Failed to remove file");
    //   send(sock,msg,strlen(msg),0);
    // }
    // else
    // {
    //   strcpy(msg,"File removed with success");
    //   send(sock,msg,strlen(msg),0);
    // }
  }

  void remove_file_rec(int sock, int dir)
  {
     inode aux,aux2, blank;
     int cont=0, zero=0, addr_temp;
     char msg[1024] = "";
     char buf[1024] = "";

     int temp=0;


     blank.used = 0;
     strcpy(blank.name, "blank");
     strcpy(blank.type, "none");
     blank.addr_i = 0;
     blank.addr_f = 0;
     blank.parent_addr = 0;

     fseek(fp, dir, SEEK_SET);
     fread(&aux, sizeof(inode), 1, fp);

     // calc block map bit position
     addr_temp = aux.addr_i - ( blocks*sizeof(int) + ( inode_qnt * sizeof(inode) ) );
     addr_temp = addr_temp/block_size;
     fseek(fp, addr_temp*sizeof(int), SEEK_SET);
     fwrite(&zero, sizeof(int), 1, fp);

     // writing blank inode
     fseek(fp, dir, SEEK_SET);
     fwrite(&blank, sizeof(inode), 1, fp);

     return;
  }





  void create_file(int sock, char* name, int current_dir)
  {
     int bit;
     inode temp;
     char msg[1024];
     int returned = 2 ;
     fseek(fp,0,SEEK_SET);
     for(int i = 0 ; i < blocks; i++)
     {
        fread(&bit, sizeof(int), 1, fp);
        if(bit == 0)
        {
           fseek(fp, blocks*sizeof(int), SEEK_SET);
           for(int j = 0 ; j < inode_qnt; j++)
           {
             fread(&temp, sizeof(inode), 1, fp);
             if( temp.used == 0)
             {
                returned = mark_file(sock, i, j, name, current_dir);
                break;
             }
          }
          break;
        }
     }

     if( returned == 1)
     {
        strcpy(msg,"Failed to create file");
        send(sock,msg,strlen(msg),0);
     }
     if( returned == 0)
     {
        strcpy(msg,"File created with success");
        send(sock,msg,strlen(msg),0);
     }
     if( returned == 2)
     {
        strcpy(msg,"Fail Unknown");
        send(sock,msg,strlen(msg),0);
     }



    // FILE* fp;
    // char msg[1024];
    // int read_size;
    //
    //
    // fp = fopen(name, "w");
    // if (fp == NULL) {
    //   strcpy(msg,"Failed to create file");
    //   send(sock,msg,strlen(msg),0);
    // }
    // else
    // {
    //   strcpy(msg,"File created with success");
    //   send(sock,msg,strlen(msg),0);
    // }
    // fclose(fp);
  }

  int mark_file(int sock, int i, int j, char* namefile, int current_dir)
  {
     int temp=1;

     // marking bit map

     fseek(fp, i*sizeof(int), SEEK_SET);
     fwrite(&temp, sizeof(int), 1, fp);


     // creating struct temp for add to te memory
     inode aux;
     strcpy(aux.name, namefile);
     strcpy(aux.type, "file");
     aux.used = 1;
     aux.parent_addr = current_dir;
     aux.addr_i = ( blocks*sizeof(int) + ( inode_qnt * sizeof(inode) ) + (i*block_size));
     aux.addr_f = aux.addr_i + block_size; // * number_blocks;

     fseek(fp, ( blocks*sizeof(int) + j*sizeof(inode) ), SEEK_SET);
     fwrite(&aux, sizeof(inode), 1, fp);



     // referecing nodes vector

      fseek(fp, current_dir, SEEK_SET);
      fread(&aux, sizeof(inode),1, fp);



      for (int x = 0; x < 10; x++)
      {
         if( aux.nodes[x] == -1 )
         {
            // cout<<x<<endl;
            aux.nodes[x] = (blocks*sizeof(int) + j*sizeof(inode));
            // cout<<aux.nodes[x]<<endl;
            fseek(fp, current_dir, SEEK_SET);
            fwrite(&aux, sizeof(inode),1, fp);
            return 0;
         }
      }
      return 1;





  }

  void create_dir(int sock, char* name, int current_dir)
  {
     inode aux, aux2;
     char message[1024];
     fseek(fp, current_DIR, SEEK_SET);
     for(int i  = 0 ; i < inode_qnt ; i++)
     {
        fread(&aux, sizeof(inode),1,fp);
        if(aux.used == 0)
        {
           strcpy(aux2.type, "dir");
 		     strcpy(aux2.name, name);
           aux2.used = 1;
 		     aux2.addr_i=-1;
 		     aux2.addr_f=-1;
 		     aux2.parent_addr=current_dir;

           fseek(fp, blocks*sizeof(int) + i*sizeof(inode), SEEK_SET);
           fwrite(&aux2, sizeof(inode),1,fp);

           // referenciing

           fseek(fp, current_dir, SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           for (size_t x = 0; x < 10; x++)
           {
               if( aux2.nodes[x] == -1 )
               {
                 // cout<<x<<endl;
                 aux2.nodes[x] = (blocks*sizeof(int) + i*sizeof(inode));
                 // cout<<aux2.nodes[x]<<endl;
                 fseek(fp, current_dir, SEEK_SET);
                 fwrite(&aux2, sizeof(inode),1, fp);
                 strcpy(message, "Directory created with success!");
                 break;
               }
           }
           break;
        }
        strcpy(message, "Error, failed to create directory ..no inodes");
     }
      send(sock, message, strlen(message), 0);


    // int read_size;
    // char message[1024];
    // if(mkdir(name,ALLPERMS) == -1)
    // {
    //   strcpy(message, "Error, failed to create directory");
    //   send(sock, message, strlen(message), 0);
    // }
    // else
    // {
    //   strcpy(message, "Directory created with success!");
    //   send(sock, message, strlen(message), 0);
    // }
    // return;
  }
  void rmv_dir(int sock, char* name, int current_dir)
  {
     char msg[1024] = "";
     rmv_dir_rec(sock, name, current_dir);
     strcpy(msg, "Directory removed with success!");
     send(sock, msg, strlen(msg), 0);

  }

  void rmv_dir_rec(int sock, char* name, int current_dir)
  {
     inode aux,aux2,aux3, blank;
     int cont=0;
     char msg[1024] = "";
     char buf[1024] = "";

     blank.used = 0;
     strcpy(blank.name, "blank");
  	  strcpy(blank.type, "none");
  	  blank.addr_i = 0;
  	  blank.addr_f = 0;
  	  blank.parent_addr = 0;

     fseek(fp, current_dir, SEEK_SET);
     fread(&aux, sizeof(inode),1,fp);
     for(int i = 0 ; i < 10 ; i++)
     {
        if( aux.nodes[i] != -1)
        {
           fseek(fp, aux.nodes[i], SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           if( strcmp(aux2.name, name) == 0 )
           {
              for(int j = 0; j < 10 ; j++)
              {
                 if(aux2.nodes[i] != -1)
                 {
                    fseek(fp,aux2.nodes[i],SEEK_SET);
                    fread(&aux3, sizeof(inode), 1, fp);
                    if( aux3.type[0] == 'd')
                    {
                       rmv_dir(sock, aux3.name, aux3.parent_addr);
                    }
                    if( aux3.type[0] == 'f')
                    {
                       remove_file_rec(sock, aux2.nodes[i]);
                    }
                  }
               }
               fseek(fp, aux.nodes[i], SEEK_SET);
               fwrite(&blank, sizeof(inode), 1, fp);

               // altering reference inodes nodes
               fseek(fp, current_dir, SEEK_SET);
               fread(&aux, sizeof(inode), 1, fp);
               aux.nodes[i] = -1;
               fseek(fp, current_dir, SEEK_SET);
               fwrite(&aux, sizeof(inode), 1, fp);
            }
         }
      }

    // int read_size;
    // char message[1024];
    // if(rmdir(name) == -1)
    // {
    //   strcpy(message, "Error, failed to remove directory");
    //   send(sock, message, strlen(message), 0);
    // }
    // else
    // {
    //   strcpy(message, "Directory removed with success!");
    //   send(sock, message, strlen(message), 0);
    // }
    // return;

  }

  int strcmpst1nl (const char * s1, const char * s2)
  {
    char s1c;
    if( s1 == NULL) return -1;
    do
      {
        s1c = *s1;
        if (s1c == '\n')
            s1c = 0;
        if (s1c != *s2)
            return 1;
        s1++;
        s2++;
      } while (s1c);
    return 0;
}
