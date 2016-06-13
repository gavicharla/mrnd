#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct student
{
	int roll_no;
	char name[24];
	char college[20];
}student;
typedef struct leaf
{
	student data[10];
	int index;
	char unused[27];
	char flag;
}leaf;
typedef struct non_leaf
{
	int keys[63];
	int offset[64];
	int index;
}non_leaf;
typedef struct tablesector
{
	int tableid[8];
	int root_page_id[8];
}table_sector;
#define BASE 64
#define PAGE_SIZE 512
// int create_non_leafs(int pages)
// {
// 	FILE * fp = fopen("students.bin","rb");
// 	FILE * write = fopen("students.bin","rb+");
// 	fseek(fp,512,SEEK_SET);
// 	int offset = 1,i;
// 	non_leaf s;
// 	leaf temp;
// 	memset(&s,0,sizeof(non_leaf));
// 	s.offset[0] = offset++;
// 	fseek(fp,1024,SEEK_SET);
// 	for(i=1;i<pages;i++)
// 	{
// 		memset(&temp,0,sizeof(leaf));
// 		fread(&temp,sizeof(leaf),1,fp);
// 		s.keys[i] = temp.data[0].roll_no;
// 		s.offset[i] = offset++;
// 		printf("%d %d\n",s.keys[i],s.offset[i]);
// 	}
// 	fwrite(&s,sizeof(non_leaf),1,write);
// 	fclose(fp);
// 	fclose(write);
// }
// int read_and_paste()
// {
// 	FILE * fp = fopen("data.csv","r");
// 	if(fp == NULL)
// 		return;
// 	FILE * write = fopen("students.bin","wb");
// 	if(fp == NULL)
// 		return 1;
// 	fseek(write,512,SEEK_SET);
// 	leaf s;
// 	int pages =0,i;
// 	char line[64];
// 	while(!feof(fp))
// 	{

// 		memset(&s,0,sizeof(leaf));
// 		//printf("%d\n",sizeof(leaf) );
// 		for(i=0;i<10;i++)
// 		{
// 			fscanf(fp,"%[^\n]\n",&line);
// 			sscanf(line,"%d,%[^,],%s",&s.data[i].roll_no,&s.data[i].name,&s.data[i].college);
// 			s.flag = 0;
// 		}
// 		fwrite(&s,sizeof(leaf),1,write);
// 		pages++;
// 	}
// 	fclose(fp);
// 	fclose(write);
// 	create_non_leafs(pages);
// 	return pages;
// }
// int read_and_paste2()
// {
// 	FILE * fp = fopen("data.csv","r");
// 	if(fp == NULL)
// 		return;
// 	FILE * read2 = fopen("students.bin","rb+");
// 	fseek(read2,sizeof(leaf),SEEK_SET);
// 	if(fp == NULL)
// 		return 1;
// 	leaf s;
// 	char line[64];
// 	int flag =0;
// 	while(!feof(fp))
// 	{
// 		fscanf(fp,"%[^\n]\n",&line);
// 		fread(&s,sizeof(leaf),1,read2);
// 		while(s.index>10)
// 		{
// 			fseek(fp,sizeof(leaf),SEEK_CUR);
// 			fread(&s,sizeof(leaf),1,read2);
// 		}
// 		sscanf(line,"%d,%[^,],%s",&s.data[s.index].roll_no,&s.data[s.index].name,&s.data[s.index].college);  
// 		s.flag = 0;   
// 		s.index++;

// 	}
//// }
//void update_root(int offset_root, int offset_leaf, int key)
//{
//	offset_root*= PAGE_SIZE;
//	offset_root += 64;
//	FILE * fp = fopen("students.bin", "rb+");
//	fseek(fp, offset_root , SEEK_SET);
//	non_leaf temp;
//	fread(&temp, sizeof(non_leaf), 1, fp);
//	temp.keys[temp.index - 1] = key;
//	temp.offset[temp.index] = offset_leaf;
//	printf("%d %d %d\n", temp.keys[temp.index - 1], temp.offset[temp.index], temp.index);
//	temp.index += 1;
//	FILE * write = fopen("students.bin", "ab");
//	fseek(write, 0, SEEK_END);
//	offset_leaf = (ftell(write) - BASE) / PAGE_SIZE;
//	fwrite(&temp, sizeof(non_leaf), 1, write);
//	fclose(write);
//	fseek(fp, 0, SEEK_SET);
//	table_sector temp1;
//	fread(&temp1, sizeof(table_sector), 1, fp);
//	temp1.root_page_id[0] = offset_leaf;
//	fclose(fp);
//	fp = fopen("students.bin", "rb+");
//	fwrite(&temp1, sizeof(table_sector), 1, fp);
//	fclose(fp);
//
//}
//int read_and_paste3()
//{
//	FILE * fp = fopen("data.csv", "r");
//	if (fp == NULL)
//		return 1;
//	FILE * read2 = fopen("students.bin", "rb");
//
//	FILE * write = fopen("students.bin", "wb");
//	table_sector new_sector;
//	memset(&new_sector, 0, sizeof(table_sector));
//	new_sector.root_page_id[0] = 0;
//	fwrite(&new_sector, sizeof(table_sector), 1, write);
//	fclose(write);
//	leaf s;
//	int pages = 0, i;
//	char line[64];
//	while (!feof(fp))
//	{
//		memset(&s, 0, sizeof(leaf));
//		memset(&line, '/0', sizeof(line));
//		for (i = 0; i<10; i++)
//		{
//			fscanf(fp, "%[^\n]\n", &line);
//			sscanf(line, "%d,%[^,],%s", &s.data[i].roll_no, &s.data[i].name, &s.data[i].college);
//		}
//		s.flag = 0;
//		write = fopen("students.bin", "ab");
//		fseek(write, 0, SEEK_END);
//		int writable = ftell(write);
//		fwrite(&s, sizeof(leaf), 1, write);
//		fclose(write);
//		//memset(&new_sector, 0, sizeof(new_sector));
//		FILE * read = fopen("students.bin", "rb+");
//		fseek(read, 0, SEEK_SET);
//		fread(&new_sector, sizeof(new_sector), 1, read);
//		fclose(read2);
//		if (new_sector.root_page_id[0] == 0)
//		{
//			// then its the first leaf node 
//			//add the leaf node to the root page id
//			//create a non leaf root node
//			//add the non leaf to the file
//			//write the offset to the new_sector
//			printf("entered here\n");
//			non_leaf temp;
//			temp.index = 1;
//			temp.offset[0] = writable;
//			write = fopen("students.bin", "ab");
//			fseek(write, 0, SEEK_END);
//			writable = ftell(write);
//			fwrite(&temp, sizeof(non_leaf), 1, write);
//			fclose(write);
//			new_sector.root_page_id[0] = (writable - BASE) / PAGE_SIZE;
//			write = fopen("students.bin", "rb+");
//			fseek(write, 0, SEEK_SET);
//			fwrite(&new_sector, sizeof(table_sector), 1, write);
//			fclose(write);
//		}
//		else
//		{
//			update_root(new_sector.root_page_id[0], (writable - BASE) / PAGE_SIZE, s.data[9].roll_no + 1);
//		}
//
//		pages++;
//	}
//	fclose(fp);
//	//create_non_leafs3(pages);
//	return pages;
//}

void teachers_students()
{
	FILE * stud = fopen("data.csv", "r");
	FILE  * teach = fopen("teachers.csv", "r");
	int i = 0;
	while (!feof(stud) || !feof(teach))
	{

	}

}

int main()
{
	return 1;
}