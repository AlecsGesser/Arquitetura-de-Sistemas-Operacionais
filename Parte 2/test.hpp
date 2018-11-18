#ifndef MEMIO_HPP
#define MEMIO_HPP

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <array>

#define inode_qnt  20
#define blocks     200
#define block_size 50 //bytes
using namespace std;

FILE *fp;
int current_DIR= sizeof(int)*blocks;

struct inode{
	char type[5];
	int used;
	char name[15];
	int addr_i;
	int addr_f;
	int parent_addr;
	int nodes[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
};
typedef struct inode inode;


void file_write(char* name, char* data);
void mark_file(int i, int j, char* namefile, char* data);
void read_inodes();
void read_file(char* namefile);
void dir_write(char* name);
void initialize_bin();


#endif // MEMIO_DEFINED
