
#ifndef MEMIO_HPP
#define MEMIO_HPP

#include <stdio.h>
#include <cstring>
#include <vector>
#include <array>

#define inode_qnt  20
#define blocks     200
#define block_size 50 //bytes


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

void file_write(char* name, char* data)
{
	int bit;
	inode temp;

	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i < 200; i++)
	{
		fread(&bit, sizeof(int), 1, fp);
		if(bit == 0)
		{
			fseek(fp, blocks*sizeof(int), SEEK_SET);
			for (int j = 0; j < inode_qnt; j++)
			{
				fread(&temp, sizeof(inode), 1, fp);
				if( temp.used == 0)
				{
					mark_file(i,j, name, data);
					break;
				}
			}
			break;
		}
	}
}

void mark_file(int i, int j, char* namefile, char* data)
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
	aux.parent_addr = current_DIR;
	aux.addr_i = ( blocks*sizeof(int) + ( inode_qnt * sizeof(inode) ) + (i*block_size));
	aux.addr_f = aux.addr_i + block_size; // * number_blocks;

   fseek(fp, ( blocks*sizeof(int) + j*sizeof(inode) ), SEEK_SET);
	fwrite(&aux, sizeof(inode), 1, fp);


	// reading current DIR
	fseek(fp, current_DIR*sizeof(inode), SEEK_SET);
	fread(&aux, sizeof(inode), 1, fp);

	// adding data to the 3rd layers
	char buf[50];
	for (size_t i = 0; i < 50; i++)
	{
		buf[i] = data[i];
	}
	//strcpy(data, buf);

	int addr = aux.addr_i;

	fseek(fp,addr, SEEK_SET);
	fwrite(&buf, sizeof(char), strlen(buf), fp);

	// referecing nodes vector
 	fseek(fp, current_DIR, SEEK_SET);
 	fread(&aux, sizeof(inode),1, fp);



	for (int x = 0; x < 10; x++)
	{
		if( aux.nodes[x] == -1 )
		{
			// cout<<x<<endl;
			aux.nodes[x] = (blocks*sizeof(int) + j*sizeof(inode));
			// cout<<aux.nodes[x]<<endl;
			fseek(fp, current_DIR, SEEK_SET);
			fwrite(&aux, sizeof(inode),1, fp);
			break;
		}
	}
}

void read_inodes()
{
	inode aux;

	fseek(fp, blocks*sizeof(int), SEEK_SET);

	for (int i = 0; i < 10; i++)
	{
		fread(&aux, sizeof(inode), 1, fp);
		// cout<<"{ name: "<<aux.name
		// 	 <<"| type: "<<aux.type
		// 	 <<"| addr_i: "<<aux.addr_i
		// 	 <<"| addr_f: "<<aux.addr_f
		// 	 <<"| parent: "<<aux.parent_addr
		// 	 <<"| node: "<<aux.nodes[0]<<"||"<<aux.nodes[1]<<"||"<<aux.nodes[2]
		// 	 <<"}"<<endl;
		// cout<<sizeof(inode)<<endl;
		// cout<<i<<endl<<endl;
	}
}


void read_file(char* namefile)
{
	inode aux, aux2;
	char dado[50];

	fseek(fp, current_DIR, SEEK_SET);
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
				// cout<<"dados::::: "<<dado<<endl;
			}
		}
	}
}

void dir_write(char* name)
{
	inode aux,aux2;
	fseek(fp, blocks*sizeof(int), SEEK_SET);
	for(int i = 0; i < inode_qnt; i++)
	{
		fread(&aux, sizeof(inode), 1, fp);
		if( aux.used == 0 )
		{
			strcpy(aux2.type, "dir");
			strcpy(aux2.name, name);
			aux2.addr_i=-1;
			aux2.addr_f=-1;
			aux2.parent_addr=current_DIR;

			fseek(fp, blocks*sizeof(int) + i*sizeof(inode), SEEK_SET);
			fwrite(&aux2, sizeof(inode),1,fp);

			// referenciing inode on the current folder
			fseek(fp, current_DIR, SEEK_SET);
		 	fread(&aux2, sizeof(inode),1, fp);
			for (int x = 0; x < 10; x++)
			{
				if( aux2.nodes[x] == -1 )
				{
					// cout<<x<<endl;
					aux2.nodes[x] = (blocks*sizeof(int) + i*sizeof(inode));
					// cout<<aux2.nodes[x]<<endl;
					fseek(fp, current_DIR, SEEK_SET);
					fwrite(&aux2, sizeof(inode),1, fp);
					break;
				}
			}
			break;
		}
	}
}



// int main()
// {
// 	int temp=5;
//
// 	initialize_bin();
// 	file_write("filename","data content");
// 	file_write("FILENAME","data content SECOND");
// 	//read_file("FILENAME");
// 	dir_write("nova pasta");
//  	read_inodes();
//
//
//
// 	fclose(fp);
// 	return 0;
// }





void initialize_bin()
{
	fp = fopen("binario.bin","w+b");
	inode blank;
	int temp=0;

	blank.used = 0;
	strcpy(blank.name, "blank");
	strcpy(blank.type, "none");
	blank.addr_i = 0;
	blank.addr_f = 0;
	blank.parent_addr = 0;

	//blank.nodes =  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	for (int i = 0; i < blocks; i++)
	{
		fwrite(&temp, sizeof(int),1,fp); // populando memoria de inodes com inodes vazios
	}



	fseek(fp, blocks*sizeof(int), SEEK_SET);

	for (int i = 0; i < inode_qnt; i++)
	{
		fwrite(&blank, sizeof(inode),1,fp); // populando memoria de inodes com inodes vazios
	}

	inode root;				// criando informacoes de root
	strcpy(root.type, "dir");
	root.used = 1;
	strcpy(root.name, "root");
	root.addr_i = -1;
	root.addr_f = -1;
	root.parent_addr = -1;

	//root.nodes = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

	fseek(fp, blocks*sizeof(int), SEEK_SET);  // pulando para memoria de inodes
	fwrite(&root, sizeof(inode), 1, fp);



	fseek(fp, ( blocks*sizeof(int) +  (inode_qnt * sizeof(inode)) ) , SEEK_SET);


	for (int i = 0; i < blocks*block_size; i++)
	{
		fwrite(&temp, sizeof(char), 1, fp);
	}


}

#endif // MEMIO_DEFINED
