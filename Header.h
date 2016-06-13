
#include<stdio.h>
#include<stdlib.h>
int page_id;
typedef struct user_fat
{
	int id;
	int address;
	int next;
} user_fat;
typedef struct category_fat
{
	int index;
	int a[13];
}category_fat;
typedef struct cat_to_msg
{
	int cat_id;
	int addr;
	int next;
}cat_to_msg;
typedef struct message_inode
{
	int index;
	int a[13];
}message_inode;
typedef struct user
{
	char name[30];
	int id;
}user;
typedef struct category
{
	char name[30];
	int id;
}category;
typedef struct message
{
	char msg[120];
	int user_id;
}message;

void replyto_client(char *buf, int *csock) {
	int bytecount;

	if ((bytecount = send(*csock, buf, strlen(buf), 0)) == SOCKET_ERROR){
		fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
		//free (csock);
	}
	//printf("replied to client: %s\n",buf);	
}
int init()
{

	FILE * fp = fopen("file.bin", "rb+");
	int i = 0;
	int j = 0;
	for (i = 0; i<4; i++)
		fwrite(&j, sizeof(int), 1, fp);
	char ch1 = '1';
	char ch2 = '0';
	for (i = 0; i<64 * 1024; i++)
	{
		if (i<513)
			fwrite(&ch1, sizeof(char), 1, fp);
		else
			fwrite(&ch2, sizeof(char), 1, fp);
	}
	int a = ftell(fp);
	fclose(fp);
	return a;
}
int search_free()
{
	FILE * fp = fopen("file.bin", "rb+");
	FILE * write = fopen("file.bin", "rb+");
	fseek(fp, 16, SEEK_SET);
	char ch = '1';
	int i = 16;
	while (ch != '0')
	{
		fread(&ch, sizeof(char), 1, fp);
		i += 1;
	}
	return i - 1;
}

int write_msg(message m)
{
	FILE * fp = fopen("file.bin", "rb+");
	int offset = search_free();
	fseek(fp, offset, SEEK_SET);
	char ch = '1';
	fwrite(&ch, sizeof(char), 1, fp);
	offset *= 128;
	fseek(fp, offset, SEEK_SET);
	fwrite(&m, sizeof(message), 1, fp);
	fclose(fp);
	return offset;
}
int write_msg_fat(message_inode msg)
{
	FILE * fp = fopen("file.bin", "rb+");
	int offset = search_free();
	fseek(fp, offset, SEEK_SET);
	char ch = '1';
	fwrite(&ch, sizeof(char), 1, fp);
	offset *= 128;
	fseek(fp, offset, SEEK_SET);
	fwrite(&msg, sizeof(message_inode), 1, fp);
	fclose(fp);
	return offset;
}
int write_ctm(cat_to_msg ctm)
{
	FILE * fp = fopen("file.bin", "rb+");
	int offset = search_free();
	fseek(fp, offset, SEEK_SET);
	char ch = '1';
	fwrite(&ch, sizeof(char), 1, fp);
	offset *= 128;
	fseek(fp, offset, SEEK_SET);
	fwrite(&ctm, sizeof(cat_to_msg), 1, fp);
	fclose(fp);
	return offset;
}
int search(int cat_id)
{
	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, 2 * sizeof(int), SEEK_SET);
	int msg_off;
	cat_to_msg ctm;
	fread(&msg_off, sizeof(int), 1, fp);
	if (msg_off == 0)
	{
		return -1;
	}
	else
	{
		fseek(fp, msg_off, SEEK_SET);
		fread(&ctm, sizeof(cat_to_msg), 1, fp);
		if (ctm.cat_id == cat_id)
		{
			return ctm.addr;
		}
		else
		{
			while (ctm.next != -1)
			{
				printf("helo");
				fseek(fp, ctm.next, SEEK_SET);
				fread(&ctm, sizeof(cat_to_msg), 1, fp);
				if (ctm.cat_id == cat_id)
					return ctm.addr;
			}
			if (ctm.cat_id == cat_id)
				return ctm.addr;
			return -1;
		}
	}
}
void print_users_database(int * csock)
{
	printf("-----all users------\n");
	FILE * fp = fopen("file.bin", "rb+");
	int offset;
	fread(&offset, sizeof(int), 1, fp);
	if (offset == 0)
	{
		replyto_client("-1", csock);
	}
	else 
	{
		user_fat temp;
		fseek(fp, offset, SEEK_SET);
		fread(&temp, sizeof(user_fat), 1, fp);
		replyto_client("{\"users\" : [", csock);
		while (temp.next != -1)
		{
			fseek(fp, temp.address, SEEK_SET);
			user temp1;
			fread(&temp1, sizeof(user), 1, fp);
			printf("name:%s user id : %d\n", temp1.name, temp1.id);
			char * buff = (char *)malloc(sizeof(char) * 20);
			sprintf(buff, "{ \"name\": \"%s\" , \"id\": %d },", temp1.name, temp1.id);
			replyto_client(buff, csock);
			fseek(fp, temp.next, SEEK_SET);
			fread(&temp, sizeof(user_fat), 1, fp);
		}
		fseek(fp, temp.address, SEEK_SET);
		user temp1;
		fread(&temp1, sizeof(user), 1, fp);
		printf("name:%s user id : %d\n", temp1.name, temp1.id);
		char * buff = (char *)malloc(sizeof(char) * 20);
		sprintf(buff, "{ \"name\": \"%s\" , \"id\": %d }", temp1.name, temp1.id);
		replyto_client(buff, csock);
		replyto_client("]}", csock);
		fclose(fp);
	}
}

int new_user_database(int i,char * s)
{
	FILE * fp = fopen("file.bin", "rb+");
	user temp;
	printf("enter user name:");
	strcpy(temp.name, s);
	temp.id = i + 1;
	int offset = search_free();
	fseek(fp, offset, SEEK_SET);
	char ch = '1';
	fwrite(&ch, sizeof(char), 1, fp);
	offset = offset * 128;
	fseek(fp, offset, SEEK_SET);
	fwrite(&temp, sizeof(user), 1, fp);
	user_fat temp1;
	temp1.id = temp.id;
	temp1.address = offset;
	temp1.next = -1;
	offset = search_free();
	fseek(fp, offset, SEEK_SET);
	fwrite(&ch, sizeof(char), 1, fp);
	offset = offset * 128;
	fseek(fp, offset, SEEK_SET);
	fwrite(&temp1, sizeof(user_fat), 1, fp);
	fclose(fp);
	return offset;
}
void add_user_database(char * s, int * csock)
{
	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, 0, SEEK_SET);
	int user_off;
	fread(&user_off, sizeof(int), 1, fp);
	if (user_off == 0)
	{
		FILE * fp = fopen("file.bin", "rb+");
		int offset = new_user_database(0,s);
		fseek(fp, 0, SEEK_SET);
		fwrite(&offset, sizeof(int), 1, fp);
		fclose(fp);
	}
	else
	{
		fseek(fp, user_off, SEEK_SET);
		user_fat temp;
		fread(&temp, sizeof(user_fat), 1, fp);
		int i = 1;
		while (temp.next != -1)
		{
			fseek(fp, temp.next, SEEK_SET);
			fread(&temp, sizeof(user_fat), 1, fp);
			i += 1;
		}
		if (i <= 20)
		{
			int offset = new_user_database(i, s);
			temp.next = offset;
			fseek(fp, -sizeof(user_fat), SEEK_CUR);
			fwrite(&temp, sizeof(user_fat), 1, fp);
			fclose(fp);
		}
		else
		{
			printf("there are 20 users cant add more\n");
			return;
		}
	}
}

int validate_user(int id)
{
	FILE *  fp = fopen("file.bin", "rb+");
	fseek(fp, 0, SEEK_SET);
	int user_off;
	fread(&user_off, sizeof(int), 1, fp);
	user_fat temp;
	fseek(fp, user_off, SEEK_SET);
	fread(&temp, sizeof(user_fat), 1, fp);
	while (temp.next != -1)
	{
		fseek(fp, temp.address, SEEK_SET);
		user temp1;
		fread(&temp1, sizeof(user), 1, fp);
		if (temp1.id == id)
			return 0;
		fseek(fp, temp.next, SEEK_SET);
		fread(&temp, sizeof(user_fat), 1, fp);
	}
	fseek(fp, temp.address, SEEK_SET);
	user temp1;
	fread(&temp1, sizeof(user), 1, fp);
	if (temp1.id == id)
		return 0;
	return 1;

}

user return_name(int id)
{
	FILE *  fp = fopen("file.bin", "rb+");
	fseek(fp, 0, SEEK_SET);
	int user_off;
	fread(&user_off, sizeof(int), 1, fp);
	user_fat temp;
	fseek(fp, user_off, SEEK_SET);
	fread(&temp, sizeof(user_fat), 1, fp);
	while (temp.next != -1)
	{
		fseek(fp, temp.address, SEEK_SET);
		user temp1;
		fread(&temp1, sizeof(user), 1, fp);
		if (temp1.id == id)
			return temp1;
		fseek(fp, temp.next, SEEK_SET);
		fread(&temp, sizeof(user_fat), 1, fp);
	}

	fseek(fp, temp.address, SEEK_SET);
	user temp1;
	fread(&temp1, sizeof(user), 1, fp);
	if (temp1.id == id)
		return temp1;
}
//messages

void print_messages_database(int cat_id,int * csock)
{
	//search for category 
	//if there get the message inode thing and print all the messages in it 
	/* write a function to get username if given a user id*/
	//give option to add a reply or post a new message
	int inode_offset = search(cat_id);
	if (inode_offset == -1)
	{
		replyto_client("-1",csock);
		return;
	}
	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, inode_offset, SEEK_SET);
	message_inode inode;
	fread(&inode, sizeof(message_inode), 1, fp);
	replyto_client("{ \"messages\" : [", csock);
	while (inode.index>11)
	{
		int i = 0;
		for (i = 0; i<12; i++)
		{
			fseek(fp, inode.a[i], SEEK_SET);
			message msg;
			fread(&msg, sizeof(message), 1, fp);
			char * s = (char*)malloc(sizeof(char) * 30);
			sprintf(s,"{\"message\" :\"%s\", \"user_id\" : %d},", msg.msg, msg.user_id);
			replyto_client(s, csock);
		}
		fseek(fp, inode.a[12], SEEK_SET);
		fread(&inode, sizeof(message_inode), 1, fp);
	}
	int i;
	FILE * read = fopen("file.bin", "rb+");
	for (i = 0; i < inode.index; i++)
	{
		fseek(read, inode.a[i], SEEK_SET);
		message temp;
		fread(&temp, sizeof(message), 1, read);
		char * s = (char*)malloc(sizeof(char) * 30);
		sprintf(s, "{\"message\" :\"%s\", \"user_id\" : %d},", temp.msg, temp.user_id);
		replyto_client(s, csock);
	}
	fseek(read, inode.a[i], SEEK_SET);
	message temp;
	fread(&temp, sizeof(message), 1, read);
	char * s = (char*)malloc(sizeof(char) * 30);
	sprintf(s, "{\"message\" :\"%s\", \"user_id\" : %d}]}", temp.msg, temp.user_id);
	replyto_client(s, csock);

}


void add_msgs_database(int cat_id, int user_id,char * s)
{

	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, 2 * sizeof(int), SEEK_SET);
	int msg_off;
	fread(&msg_off, sizeof(int), 1, fp);
	if (msg_off == 0)
	{
		cat_to_msg ctm;
		message_inode minode;
		message msg;
		strcpy(msg.msg, s);
		msg.user_id = user_id;
		minode.index = 0;
		minode.a[0] = write_msg(msg);
		ctm.cat_id = cat_id;
		ctm.addr = write_msg_fat(minode);
		ctm.next = -1;
		int offset = write_ctm(ctm);
		FILE * write = fopen("file.bin", "rb+");
		fseek(write, 2 * sizeof(int), SEEK_SET);
		fwrite(&offset, sizeof(int), 1, write);
		fclose(write);
	}
	else
	{
		int offset = search(cat_id);
		//now we have to add the message to the message inode 
		if (offset == -1)
		{
			//no message has been posted in that category
			//add a new ctm and a message and message inode 
			//add the message inode to ctm , add the message to the message inode
			// write the message to the minode
			//write the inode to ctm
			//write msg to file
			//write minode to file
			//write ctm to file
			printf("no message posted in this cat");
			fseek(fp, msg_off, SEEK_SET);
			cat_to_msg ctm;
			fread(&ctm, sizeof(cat_to_msg), 1, fp);
			int last_addr = msg_off;
			while (ctm.next != -1)
			{
				last_addr = ctm.next;
				fseek(fp, ctm.next, SEEK_SET);
				fread(&ctm, sizeof(cat_to_msg), 1, fp);
			}
			message msg;
			strcpy(msg.msg, s);
			msg.user_id = user_id;
			message_inode inode;
			inode.index = 0;
			inode.a[0] = write_msg(msg);
			cat_to_msg new_ctm;
			new_ctm.next = -1;
			new_ctm.cat_id = cat_id;
			new_ctm.addr = write_msg_fat(inode);
			ctm.next = write_ctm(new_ctm);
			fseek(fp, last_addr, SEEK_SET);
			fwrite(&ctm, sizeof(cat_to_msg), 1, fp);
			fclose(fp);

		}
		else
		{
			FILE * fp = fopen("file.bin", "rb+");
			fseek(fp, offset, SEEK_SET);
			message_inode test;
			fread(&test, sizeof(message_inode), 1, fp);
			int temp = offset;
			while (test.index>11)
			{
				temp = test.a[12];
				fseek(fp, test.a[12], SEEK_SET);
				fread(&test, sizeof(message_inode), 1, fp);
			}
			//create message 
			message msg;
			strcpy(msg.msg, s);
			msg.user_id = user_id;
			test.index += 1;
			test.a[test.index] = write_msg(msg);
			/* write code to add support if index crosses the 12*/
			if (test.index == 11)
			{
				//reached the end of inode 
				//so create a new inode and link it to the node 12 th index
				message_inode temp;
				temp.index = -1;
				test.a[12] = write_msg_fat(temp);
			}
			//write message to file
			//add message to inode
			//write inode to file
			FILE * write = fopen("file.bin", "rb+");
			fseek(write, temp, SEEK_SET);
			fwrite(&test, sizeof(message_inode), 1, write);
			fclose(write);
		}
	}
	//print_messages(cat_id);
}

void print_cat_database(int user_id,int * csock)
{
	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, sizeof(int), SEEK_SET);
	int cat_off;
	fread(&cat_off, sizeof(int), 1, fp);
	if (cat_off == 0)
	{
		replyto_client("-1", csock);
		return;
	}
	else
	{
		fseek(fp, cat_off, SEEK_SET);
		category_fat cat_fat;
		fread(&cat_fat, sizeof(category_fat), 1, fp);
		replyto_client("{\"categ\" : [ ", csock);
		while (cat_fat.index>11)
		{
			int i = 0;
			for (i = 0; i<12; i++)
			{
				fseek(fp, cat_fat.a[i], SEEK_SET);
				category temp;
				fread(&temp, sizeof(category), 1, fp);
				char * s = (char *)malloc(sizeof(char) * 30);
				sprintf(s,"{\"category\" : \"%s\", \"id\":%d},", temp.name, temp.id);
				replyto_client(s, csock);
			}
			fseek(fp, cat_fat.a[12], SEEK_SET);
			fread(&cat_fat, sizeof(category_fat), 1, fp);
		}
		int i;
		FILE * read = fopen("file.bin", "rb+");
		for (i = 0; i < cat_fat.index; i++)
		{
			fseek(read, cat_fat.a[i], SEEK_SET);
			category temp;
			fread(&temp, sizeof(category), 1, read); 
			char * s = (char *)malloc(sizeof(char) * 30);
			sprintf(s, "{\"category\" : \"%s\", \"id\":%d},", temp.name, temp.id);
			replyto_client(s, csock);
		}
		fseek(read, cat_fat.a[i], SEEK_SET);
		category temp;
		fread(&temp, sizeof(category), 1, read);
		char * s = (char *)malloc(sizeof(char) * 30);
		sprintf(s, "{\"category\" : \"%s\", \"id\":%d}]}", temp.name, temp.id);
		replyto_client(s, csock);
	}

	fclose(fp);

}
int  new_cat_database(int i, int index,char * s)
{
	category temp1;
	strcpy(temp1.name, s);
	temp1.id = (i * 11) + (index + 1);
	FILE * write = fopen("file.bin", "rb+");
	int offset = search_free();
	fseek(write, offset, SEEK_SET);
	char ch = '1';
	fwrite(&ch, sizeof(char), 1, write);
	offset *= 128;
	fseek(write, offset, SEEK_SET);
	fwrite(&temp1, sizeof(category), 1, write);
	fclose(write);
	return offset;
}
void add_cat_database(char * s)
{
	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, sizeof(int), SEEK_SET);
	char ch = '1';
	int cat_off;
	fread(&cat_off, sizeof(int), 1, fp);
	if (cat_off == 0)
	{
		category_fat temp;
		temp.index = 0;
		int offset = new_cat_database(0, 0,s);
		temp.a[0] = offset;
		offset = search_free();
		FILE * write = fopen("file.bin", "rb+");
		fseek(write, offset, SEEK_SET);
		fwrite(&ch, sizeof(char), 1, write);
		offset *= 128;
		fseek(write, offset, SEEK_SET);
		fwrite(&temp, sizeof(category_fat), 1, write);
		//setting the offset of category fat table
		fseek(write, sizeof(int), SEEK_SET);
		fwrite(&offset, sizeof(int), 1, write);
		fclose(write);

	}
	else
	{
		category_fat cat_fat;
		fseek(fp, cat_off, SEEK_SET);
		fread(&cat_fat, sizeof(category_fat), 1, fp);
		int i = 0;
		while (cat_fat.index>11)
		{
			cat_off = cat_fat.a[12];
			fseek(fp, cat_fat.a[12], SEEK_SET);
			fread(&cat_fat, sizeof(category_fat), 1, fp);
			i += 1;
		}
		cat_fat.a[cat_fat.index + 1] = new_cat_database(i, cat_fat.index + 1,s);
		cat_fat.index = cat_fat.index + 1;
		if (cat_fat.index == 11)
		{
			category_fat second_fat;
			second_fat.index = -1;
			int temp = search_free();
			FILE * write = fopen("file.bin", "rb+");
			fseek(write, temp, SEEK_SET);
			fwrite(&ch, sizeof(char), 1, write);
			temp *= 128;
			fseek(write, temp, SEEK_SET);
			fwrite(&second_fat, sizeof(category_fat), 1, write);
			cat_fat.a[12] = temp;
			cat_fat.index += 1;
		}
		FILE * write = fopen("file.bin", "rb+");
		fseek(write, cat_off, SEEK_SET);
		fwrite(&cat_fat, sizeof(category_fat), 1, write);
		fclose(fp);
		fclose(write);
	}
	fclose(fp);

}
int  new_cat(int i, int index)
{
	category temp1;
	printf("Enter Category\n");
	scanf("%s", &temp1.name);
	temp1.id = (i * 11) + (index + 1);
	FILE * write = fopen("file.bin", "rb+");
	int offset = search_free();
	fseek(write, offset, SEEK_SET);
	char ch = '1';
	fwrite(&ch, sizeof(char), 1, write);
	offset *= 128;
	fseek(write, offset, SEEK_SET);
	fwrite(&temp1, sizeof(category), 1, write);
	fclose(write);
	return offset;
}
void add_cat(int user_id)
{
	FILE * fp = fopen("file.bin", "rb+");
	fseek(fp, sizeof(int), SEEK_SET);
	char ch = '1';
	int cat_off;
	fread(&cat_off, sizeof(int), 1, fp);
	if (cat_off == 0)
	{
		category_fat temp;
		temp.index = 0;
		printf("Enter Category name\n");
		int offset = new_cat(0, 0);
		temp.a[0] = offset;
		offset = search_free();
		FILE * write = fopen("file.bin", "rb+");
		fseek(write, offset, SEEK_SET);
		fwrite(&ch, sizeof(char), 1, write);
		offset *= 128;
		fseek(write, offset, SEEK_SET);
		fwrite(&temp, sizeof(category_fat), 1, write);
		//setting the offset of category fat table
		fseek(write, sizeof(int), SEEK_SET);
		fwrite(&offset, sizeof(int), 1, write);

	}
	else
	{
		category_fat cat_fat;
		fseek(fp, cat_off, SEEK_SET);
		fread(&cat_fat, sizeof(category_fat), 1, fp);
		int i = 0;
		while (cat_fat.index>11)
		{
			cat_off = cat_fat.a[12];
			fseek(fp, cat_fat.a[12], SEEK_SET);
			fread(&cat_fat, sizeof(category_fat), 1, fp);
			i += 1;
		}
		cat_fat.a[cat_fat.index + 1] = new_cat(i, cat_fat.index + 1);
		cat_fat.index = cat_fat.index + 1;
		if (cat_fat.index == 11)
		{
			category_fat second_fat;
			second_fat.index = -1;
			int temp = search_free();
			FILE * write = fopen("file.bin", "rb+");
			fseek(write, temp, SEEK_SET);
			fwrite(&ch, sizeof(char), 1, write);
			temp *= 128;
			fseek(write, temp, SEEK_SET);
			fwrite(&second_fat, sizeof(category_fat), 1, write);
			cat_fat.a[12] = temp;
			cat_fat.index += 1;
		}
		FILE * write = fopen("file.bin", "rb+");
		fseek(write, cat_off, SEEK_SET);
		fwrite(&cat_fat, sizeof(category_fat), 1, write);
		fclose(fp);
		fclose(write);
	}

}
void run_first()
{
	FILE * fp = fopen("file.bin", "wb");
	int i = 0;
	int  ch = 2;
	for (i = 0; i<8 * 1024 * 1024; i++)
		fwrite(&ch, sizeof(char), 1, fp);
	fclose(fp);
}
void initialize()
{
	//while(1)add_msgs(1,1);
	//run_first();
	FILE * fp = fopen("file.bin", "rb+");
	char ch;
	fread(&ch, sizeof(char), 1, fp);
	if (ch == 2)
	{
		int temp = init();
		fseek(fp, 12, SEEK_SET);
		fwrite(&temp, sizeof(int), 1, fp);
		//init2();
	}
	
	fclose(fp);


}


// add delete functionality 
// first show messages in category and then add the reply functon 
