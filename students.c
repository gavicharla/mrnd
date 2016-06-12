#include<stdio.h>
#include<stdlib.h>

typedef struct student
{
	int roll_no;
	char name[28];
	char college[31];
	char flag;
}student;
typedef struct non_leaf
{
	int keys[2];
	int offset[3];
	char unused[43];
	char flag;
}non_leaf;
void build_no_leaf(int pages,int where,int offset)
{
	if(pages == 1)
		return;
	FILE * fp = fopen("students.bin","rb+");
	fseek(fp,where,SEEK_SET);
	int i = 0;
	while((i*3)<pages)
	{

		non_leaf s[3];
		memset(&s,0,sizeof(non_leaf)*3);
		fread(&s[0],sizeof(non_leaf),1,fp);
		fread(&s[1],sizeof(non_leaf),1,fp);
		fread(&s[2],sizeof(non_leaf),1,fp);
		non_leaf temp ;
		memset(&temp,0,sizeof(non_leaf));
		temp.keys[0] = s[0].keys[1]+1;
		temp.keys[1] = s[1].keys[1] +1;
		temp.offset[0] = offset*3;
		temp.offset[1] = offset*3 +1;
		temp.offset[2] = offset*3 +2;
		temp.flag = 1;
		printf("level x %d %d %d %d %d\n",temp.keys[0],temp.keys[1],temp.offset[0],temp.offset[1],temp.offset[2]);
		FILE * write  = fopen("students.bin","ab+");	
		fwrite(&temp,sizeof(non_leaf),1,write);
		fclose(write);
		i++;
		offset++;
	}
	build_no_leaf(pages/3,ftell(fp),offset);
}
void build_btree(int pages)
{
	if(pages == 1)
		return ;
	FILE * fp = fopen("students.bin","rb+");
	int i = 0;
	student s[3];
	while((i*3)<pages)
	{
		memset(&s,0,sizeof(student)*3);
		fread(&s[0],sizeof(student),1,fp);
		fread(&s[1],sizeof(student),1,fp);
		fread(&s[2],sizeof(student),1,fp);
		non_leaf temp ;
		memset(&temp,0,sizeof(non_leaf));
		temp.keys[0] = s[1].roll_no;
		temp.keys[1] = s[2].roll_no;
		temp.offset[0] =  i*3;
		temp.offset[1] = i*3 +1;
		temp.offset[2] = i*3 +2;
		temp.flag =1;
		printf("leve 1 %d %d %d %d %d\n",temp.keys[0],temp.keys[1],temp.offset[0],temp.offset[1],temp.offset[2]);
		FILE * write  = fopen("students.bin","ab+");
		fwrite(&temp,sizeof(non_leaf),1,write);
		fclose(write);
		i++;
	}
	int j = ftell(fp);
	fclose(fp);
	build_no_leaf(pages/3,j,i);
}
void search(int id,int offset)
{
	FILE * fp = fopen("students.bin","rb+");
	non_leaf a;
	fseek(fp,offset,SEEK_SET);
	fread(&a,sizeof(non_leaf),1,fp);
	if(a.flag ==1)
	{
		if(a.keys[0] > id)
		{
			//printf("less than %d\n",a.keys[0] );
			search(id,a.offset[0]*64);
		}
		else if(a.keys[0] <= id  && a.keys[1] >id)
		{
			//printf("middle %d %d\n",a.keys[0],a.keys[1] );
			search(id,a.offset[1]*64);
		}
		else
		{
			//printf("greater than %d\n",a.keys[1] );
			search(id,a.offset[2]*64);
		}
	}
	else
	{
		fseek(fp,-sizeof(non_leaf),SEEK_CUR);
		student temp ;
		fread(&temp,sizeof(student),1,fp);
		if(temp.roll_no == id)
			printf("%s %s\n",temp.name,temp.college );
		else
			printf("wrong %s\n",temp.name );
	}
}
int get_offset(int id,int offset)
{
	FILE * fp = fopen("students.bin","rb+");
	non_leaf a;
	fseek(fp,offset,SEEK_SET);
	fread(&a,sizeof(non_leaf),1,fp);
	if(a.flag ==1)
	{
		if(a.keys[0] > id)
		{
			//printf("less than %d\n",a.keys[0] );
			search(id,a.offset[0]*64);
		}
		else if(a.keys[0] <= id  && a.keys[1] >id)
		{
			//printf("middle %d %d\n",a.keys[0],a.keys[1] );
			search(id,a.offset[1]*64);
		}
		else
		{
			//printf("greater than %d\n",a.keys[1] );
			search(id,a.offset[2]*64);
		}
	}
	else
			return offset;
		
}
void parse_query(char * s)
{
	int id;
	sscanf(s,"roll > %d",&id);
	printf("%d\n",get_offset(id));
}
int main()
{
	FILE * fp = fopen("data.csv","r");
	if(fp == NULL)
		return;
	FILE * write = fopen("students.bin","wb");
	if(fp == NULL)
		return 1;
	student s;
	int pages =0;
	char line[64];
	while(!feof(fp))
	{
		memset(&s,0,sizeof(student));
		fscanf(fp,"%[^\n]\n",&line);
		//printf("%s\n",line );
		sscanf(line,"%d,%[^,],%s",&s.roll_no,&s.name,&s.college);
		s.flag = 0;
		fwrite(&s,sizeof(student),1,write);
		pages++;
	}
	fclose(fp);
	fclose(write);
	//printf("%d",pages);
	
	build_btree(pages);
	fp = fopen("students.bin","rb+");
	fseek(fp,0,SEEK_END);
	printf("Enter Id:\n");
	int id;
	scanf("%d",&id);
	search(id,ftell(fp)-sizeof(student));
	return 1;										
}

