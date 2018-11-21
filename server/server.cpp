/*TRABALHO PARTE 2 DE SISTEMAS OPERACIONAIS*
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
  #define porta 8080

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

    initialize_bin(); // INICIALIZA O BIN FILE EMPTY

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
          fp = fopen("binario.bin","r+b");
          create_dir(sock, name, current_dir);
          fclose(fp);
        }
        else if (strcmpst1nl(op,"rmdir") == 0)
        {
          pthread_mutex_lock(&lock);
          fp = fopen("binario.bin","r+b");
          rmv_dir(sock, name, current_dir);
          fclose(fp);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "mkfile") == 0)
        {
        //  pthread_mutex_lock(&lock);
          fp = fopen("binario.bin","r+b");
          create_file(sock, name, current_dir);
          fclose(fp);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "rmfile") == 0)
        {
          pthread_mutex_lock(&lock);
          fp = fopen("binario.bin","r+b");
          remove_file(sock, name, current_dir);
          fclose(fp);
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
          fp = fopen("binario.bin","r+b");
          show_file(sock, name, current_dir);
          fclose(fp);
          pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op,"cd") == 0)
        {
        //  pthread_mutex_lock(&lock);
          fp = fopen("binario.bin","r+b");
          cd_directory(sock, name, current_dir);
          fclose(fp);
        //  pthread_mutex_unlock(&lock);
        }
        else if(strcmpst1nl(op, "ls") == 0)
        {
        //  pthread_mutex_lock(&lock);
          fp = fopen("binario.bin","r+b");
          list_directory(sock, current_dir);
          fclose(fp);
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
     for(int i = 0 ; i < 10 ; i++){             //NAVEGA PELO VETOR DE NODES DO INODE ATUAL
        if( aux.nodes[i] != -1){                //SE NAO ESTIVER SENDO USADO CONCATENA E PRINTA
           fseek(fp, aux.nodes[i], SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           strcat(msg, aux2.name);
           strcat(msg,"\n");
           cont++;
        }
     }
     if(cont == 0) sprintf(msg,"empty");
     send(sock, msg, strlen(msg), 0);
}

  void cd_directory(int sock, char* name, int& current_dir)
  {
     char msg[1024] = "";
     inode aux, aux2;

     if( name[0] == '.' && name[1] == '.')        //VOLTA PARA O DIRETORIO PAI
     {
        fseek(fp, current_dir, SEEK_SET);
        fread(&aux, sizeof(inode), 1, fp);
        if( aux.parent_addr == -1)                 //PARENT_ADDR EH UM ATRIBUTO DA STRUCT
        {
           strcpy(msg,"Error already on ROOT");  //SE ESTIVER NO ROOT NAO DEIXA VOLTAR
           send(sock, msg, strlen(msg), 0 );
           return;
        }
        else
        {
           current_dir = aux.parent_addr;       //ATUALIZA O CURRENT_DIR GLOBAL DA THREAD
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
        if( strcmp( aux2.name, name) == 0 && aux2.type[0] == 'd' ) //PROCURA NO INODE ATUAL O DIRETORIO DE DESTINO
        {
           current_dir = aux.nodes[i];
           strcpy(msg,"Sucess cd");
           send(sock, msg, strlen(msg), 0 );
           return;
        }
     }

     strcpy(msg,"Failed cd");
     send(sock, msg, strlen(msg), 0 );
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
   			if(strcmp(namefile, aux2.name) == 0 && aux2.type[0] == 'f') //PROCURA NO INODE ATUAL O NOME DO FILE E PRINTA
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
   }

  void  edit_file(int sock, char* name, int current_dir){

     inode aux,aux2;
     int cont=0;
     char msg[1024] = "";
     char buf[1024] = "";
     fp = fopen("binario.bin","r+b");
     fseek(fp, current_dir, SEEK_SET);
     fread(&aux, sizeof(inode),1,fp);
     for(int i = 0 ; i < 10 ; i++){
        if( aux.nodes[i] != -1){
           fseek(fp, aux.nodes[i], SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           if( strcmp(aux2.name, name) == 0  && aux2.type[0] == 'f')  //PROCURA O NOME DO FILE NOS INODES DA PASTA ATUAL
           {
              fseek(fp, aux2.addr_i, SEEK_SET);
              strcpy(msg,"Type your text: ");
              send(sock, msg, strlen(msg), 0 );
              int read_size = read(sock, buf, 1024);
              buf[read_size-1] = '\0';
              fwrite(&buf, sizeof(char), strlen(buf), fp);
              strcpy(msg, "Sucess edit");
              send(sock, msg, strlen(msg), 0);
              fclose(fp);
              return;
           }
        }
     }

     strcpy(msg, "Failed edit");
     send(sock, msg, strlen(msg), 0);
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
          if( strcmp(aux2.name, name) == 0 && aux2.type[0] == 'f' )  //REMOVE O FILE DO DIR ATUAL PELO NOME
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

            fseek(fp, aux2.addr_i, SEEK_SET);
            char blnk[50] = "";
            fwrite(blnk, sizeof(char), 50, fp);

            strcpy(msg, "Sucess remove_file");
            send(sock, msg, strlen(msg), 0);
            return;
          }
       }
     }

     strcpy(msg, "Failed remove_file");
     send(sock, msg, strlen(msg), 0);
  }

  void remove_file_rec(int sock, int dir)       //REMOVE O FILE PELO ENDERECO DO INODE
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

     fseek(fp, aux.addr_i, SEEK_SET);
     char blnk[50] = "";
     fwrite(blnk, sizeof(char), 50, fp);

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
     for(int i = 0 ; i < blocks; i++)    // i REPRESENTA O BIT VAZIO DO MAPA DE BITS
     {
        fread(&bit, sizeof(int), 1, fp);
        if(bit == 0)
        {
           fseek(fp, blocks*sizeof(int), SEEK_SET);
           for(int j = 0 ; j < inode_qnt; j++)  // j REPRESENTA O INODE LIVRE
           {
             fread(&temp, sizeof(inode), 1, fp);
             if( temp.used == 0)
             {
                returned = mark_file(sock, i, j, name, current_dir); //REALIZA AS MARCACOES
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
     aux.addr_i = ( blocks*sizeof(int) + ( inode_qnt * sizeof(inode) ) + (i*block_size)); //CALCULA POSICAO DO BLOCO DE MEMORIA
     aux.addr_f = aux.addr_i + block_size; // * number_blocks;

     fseek(fp, ( blocks*sizeof(int) + j*sizeof(inode) ), SEEK_SET); //ESCREVE O INODE CRIADO
     fwrite(&aux, sizeof(inode), 1, fp);


     // referecing nodes vector

      fseek(fp, current_dir, SEEK_SET);
      fread(&aux, sizeof(inode),1, fp);


      for (int x = 0; x < 10; x++) //REFERENCIA O INODE CRIADO NO VETOR DO INODE ATUAL
      {
         if( aux.nodes[x] == -1 )
         {
            aux.nodes[x] = (blocks*sizeof(int) + j*sizeof(inode));
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
        if(aux.used == 0) //SE NAO ESTA SENDO USADO O INODE
        {
           strcpy(aux2.type, "dir");
 		     strcpy(aux2.name, name);
           aux2.used = 1;
 		     aux2.addr_i=-1;
 		     aux2.addr_f=-1;
 		     aux2.parent_addr=current_dir;

           fseek(fp, blocks*sizeof(int) + i*sizeof(inode), SEEK_SET);
           fwrite(&aux2, sizeof(inode),1,fp);

           // referenciing in the current inode vector

           fseek(fp, current_dir, SEEK_SET);
           fread(&aux2, sizeof(inode),1,fp);
           for (size_t x = 0; x < 10; x++)
           {
               if( aux2.nodes[x] == -1 )
               {
                 aux2.nodes[x] = (blocks*sizeof(int) + i*sizeof(inode));
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
  }
  void rmv_dir(int sock, char* name, int current_dir)
  {
     char msg[1024] = "";
     rmv_dir_rec(sock, name, current_dir);
     strcpy(msg, "Directory removed with success!");
     send(sock, msg, strlen(msg), 0);

  }

  void rmv_dir_rec(int sock, char* name, int current_dir) //FUNCAO QUE REMOVE AS PASTAS E OS ARQUIVOS RECURSIVAMENTE
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
                 if(aux2.nodes[j] != -1)
                 {
                    fseek(fp,aux2.nodes[j],SEEK_SET);
                    fread(&aux3, sizeof(inode), 1, fp);
                    if( aux3.type[0] == 'd')
                    {
                       rmv_dir_rec(sock, aux3.name, aux3.parent_addr);  //CHAMA A FUNCAO DENOVO E REMOVE O ATUAL
                       fseek(fp, aux2.nodes[j], SEEK_SET);
                       fwrite(&blank, sizeof(inode), 1, fp);

                       // altering reference inodes nodes
                       fseek(fp, current_dir, SEEK_SET);
                       fread(&aux, sizeof(inode), 1, fp);
                       aux.nodes[i] = -1;
                       fseek(fp, current_dir, SEEK_SET);
                       fwrite(&aux, sizeof(inode), 1, fp);
                    }
                    if( aux3.type[0] == 'f')
                    {
                       remove_file_rec(sock, aux2.nodes[j]);
                    }
                  }
               }
               fseek(fp, aux.nodes[i], SEEK_SET); //APAGA A PRIMEIRA PASTA
               fwrite(&blank, sizeof(inode), 1, fp);
            }
         }
      }
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
