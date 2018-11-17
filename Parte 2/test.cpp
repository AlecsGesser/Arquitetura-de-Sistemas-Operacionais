#include "test.hpp"

void mark_file(int i, int j, char* namefile, char* data);

void file_write(char* name, char* data){
	int bit;

	inode temp;
	rewind(fp);
	for (int i = 0; i < 200; i++)
	{
		fread(&bit, sizeof(int), 1, fp);
		if(bit == 0)
		{
			fseek(fp, 200-i , SEEK_SET);
			for (int j = 0; j < inode_qnt*sizeof(inode); j++)
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

void mark_file(int i, int j, char* namefile, char* data){

	int temp=1;
	cout<<i<<endl;
	cout<<j<<endl;
	cout<<namefile<<endl;

	fseek(fp, i, SEEK_SET);
	fwrite(&temp, sizeof(int), 1, fp);

	inode aux;
	strcpy(aux.name, namefile);
	strcpy(aux.type, "file");
	aux.used = 1;
	aux.addr_i = ( blocks + ( inode_qnt * sizeof(inode) ) + (i*block_size));
	aux.addr_f = aux.addr_i + block_size; // * number_blocks;
	cout<<"ola"<<endl;


	fseek(fp, (blocks+j*sizeof(inode)), SEEK_SET);
	fwrite(&aux, sizeof(inode),1,fp);

	fseek(fp, current_DIR, SEEK_SET);
	fread(&aux, sizeof(inode),1, fp);








}



int main()
{
	int temp=5;

	initialize_bin();

	file_write("alexinho","caio baitolassa");

	struct inode out;

	// fseek(fp, 200, SEEK_SET);
	// fread(&out, sizeof(struct inode), 1, fp);
	// cout<<out.name<<endl;
	// cout<<out.used<<endl;
	// cout<<out.addr_f<<endl;
	// cout<<out.addr_i<<endl;

	fclose(fp);
	return 0;
}





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
	for (int i = 0; i < blocks; i++)
	{
		fwrite(&temp, sizeof(int),1,fp); // populando memoria de inodes com inodes vazios
	}



	fseek(fp, blocks, SEEK_SET);
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

	fseek(fp, 200, SEEK_SET);  // pulando para memoria de inodes

	fwrite(&root, sizeof(inode), 1, fp);

	fseek(fp, ( blocks +  (inode_qnt * sizeof(inode))  + block_size) , SEEK_SET);
	for (int i = 0; i < block_size; i++) {
		fwrite(&temp, sizeof(int), 1, fp);
	}


}
