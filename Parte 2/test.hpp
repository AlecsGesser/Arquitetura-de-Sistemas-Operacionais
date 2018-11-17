#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <array>

#define inode_qnt  20
#define blocks     200
#define block_size 50 //bytes
using namespace std;

void initialize_bin();
void  node_write(struct inode input);

FILE *fp;
int current_DIR=200;

void file_write();
void dir_write();

struct inode{
	char type[4];
	int used;
	char name[15];
	int addr_i;
	int addr_f;
	int parent_addr;
	int nodes[10];
};

typedef struct inode inode;	

