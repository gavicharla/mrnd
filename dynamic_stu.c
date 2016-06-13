#include<stdio.h>
#include<stdlib.h>

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
	char unused[3];
	char flag;
}non_leaf;
int create_non_leafs(int pages)
{
	FILE * fp = fopen("students.bin","rb");
	FILE * write = fopen("students.bin","rb+");
	fseek(fp,512,SEEK_SET);
	int offset = 1,i;
	non_leaf s;
	leaf temp;
	memset(&s,0,sizeof(non_leaf));
	s.offset[0] = offset++;
	fseek(fp,1024,SEEK_SET);
	for(i=1;i<pages;i++)
	{
		memset(&temp,0,sizeof(leaf));
		fread(&temp,sizeof(leaf),1,fp);
		s.keys[i] = temp.data[0].roll_no;
		s.offset[i] = offset++;
		printf("%d %d\n",s.keys[i],s.offset[i]);
	}
	fwrite(&s,sizeof(non_leaf),1,write);
	fclose(fp);
	fclose(write);
}
int read_and_paste()
{
	FILE * fp = fopen("data.csv","r");
	if(fp == NULL)
		return;
	FILE * write = fopen("students.bin","wb");
	if(fp == NULL)
		return 1;
	fseek(write,512,SEEK_SET);
	leaf s;
	int pages =0,i;
	char line[64];
	while(!feof(fp))
	{

		memset(&s,0,sizeof(leaf));
		//printf("%d\n",sizeof(leaf) );
		for(i=0;i<10;i++)
		{
			fscanf(fp,"%[^\n]\n",&line);
			sscanf(line,"%d,%[^,],%s",&s.data[i].roll_no,&s.data[i].name,&s.data[i].college);
			s.flag = 0;
		}
		fwrite(&s,sizeof(leaf),1,write);
		pages++;
	}
	fclose(fp);
	fclose(write);
	create_non_leafs(pages);
	return pages;
}
int main()
{
	printf("%d\n",read_and_paste());

}