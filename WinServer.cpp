#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include<string.h>
#include "Header.h""
struct node
{
	char msg[128];
	int msg_id;
	node *next;
}*flist,*alist,*printid;
typedef struct post_data
{
	int index;
	char post[256];
	char key[128];
	char value[128];
	post_data * next;
}post_data;
struct bufserv{
	
		int userId;
		int forumId;
		int msgId;
		int commentId;
		int choice;
		char *forumname;
		char msg[128];
}buf1;

bool flag=true;
int mid = 0;
int count1 =0;
char *Data[100];
int count=1;
int values[100];
DWORD WINAPI SocketHandler(void*);
void replyto_client(char *buf, int *csock);

void socket_server() {

	//The port you want the server to listen on
	int host_port= 1101;

	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 )) {
	    fprintf(stderr, "No sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set options
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	//Bind and listen
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	
	/* if you get error in bind 
	make sure nothing else is listening on that port */
	if( bind( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		fprintf(stderr,"Error binding to socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	//Now lets do the actual server stuff
	while (true){
	int* csock;
	sockaddr_in sadr;
	int	addr_size = sizeof(SOCKADDR);
	
		printf("waiting for a connection\n");
		csock = (int*)malloc(sizeof(int));
		
		if((*csock = accept( hsock, (SOCKADDR*)&sadr, &addr_size))!= INVALID_SOCKET ){
			//printf("Received connection from %s",inet_ntoa(sadr.sin_addr));
			CreateThread(0,0,&SocketHandler, (void*)csock , 0,0);
		}
		else{
fprintf(stderr, "Error accepting %d\n", WSAGetLastError());
		}
	}

FINISH:
	;
}


void process_input(char *recvbuf, int recv_buf_cnt, int* csock)
{

	char replybuf[1024] = { '\0' };
	//printf("%s",recvbuf);
	replyto_client(replybuf, csock);
	replybuf[0] = '\0';
}
//
//void replyto_client(char *buf, int *csock) {
//	int bytecount;
//
//	if ((bytecount = send(*csock, buf, strlen(buf), 0)) == SOCKET_ERROR){
//		fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
//		//free (csock);
//	}
//	//printf("replied to client: %s\n",buf);	
//}

int strcm(char * s1, char * s2)
{
	int i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return 1;
		i++;
	}
	if (s1[i] != '\0' || s2[i] != '\0')
		return 1;
	return 0;
}
char * getline(char * temp)
{
	char * s = (char *)malloc(sizeof(char) * 30);
	int i = 0;
	while (temp[i] != '\n' && temp[i] != '\0' && temp[i] != '\r')
	{
		s[i] = temp[i];
		i++;
	}
	s[i] = '\0';

	return s;
}
post_data * preprocess(char * recvbuf)
{
	char * temp = strstr(recvbuf, "\r\n\r\n");
	//printf("%s", strstr(recvbuf, "\r\n\r\n"));
	int i = 0, j = 0;
	char * temp1 = (char *)malloc(sizeof(char) * 128);
	while (temp[i] != '\0')
	{
		if (temp[i] != '\n' && temp[i] != '\r' && temp[i] != ' ')
			temp1[j++] = temp[i];
		i++;
	}
	temp1[j] = '\0';
	//replyto_client(temp1, csock);
	i = 0;
	post_data * head = (post_data*)malloc(sizeof(post_data));
	head->next = NULL;
	head->index = 0;
	while (temp1[i] != '\0')
	{
		if (temp1[i] == '&')
		{
			post_data * new_head = (post_data*)malloc(sizeof(post_data));
			head->post[head->index] = '\0';
			new_head->next = head;
			head = new_head;
			head->index = 0;
		}
		else
		{
			head->post[head->index] = temp1[i];
			head->index += 1;
		}
		i++;
	}
	head->post[head->index] = '\0';
	// devide the individual node into key and value 
	post_data * temp_head = (post_data *)malloc(sizeof(post_data));
	temp_head = head;
	while (temp_head != NULL)
	{
		i = 0;
		int flag = 0, j = 0;
		for (i = 0; i < temp_head->index + 1; i++)
		{
			if (temp_head->post[i] == '=')
			{
				temp_head->key[j++] = '\0';
				flag = 1;
				j = 0;
			}
			else if (flag == 0)
				temp_head->key[j++] = temp_head->post[i];
			else
				temp_head->value[j++] = temp_head->post[i];
		}
		temp_head->value[j] = '\0';
		temp_head = temp_head->next;
	}
	temp_head = head;
	while (temp_head != NULL)
	{
		printf("%s %s\n", temp_head->key, temp_head->value);
		temp_head = temp_head->next;
	}
	return head;
}
int get_user(char * s)
{
	int id=2;
	if (strstr(s, "user_id"))
	{
		sscanf(strstr(s, "user_id"), "user_id=%d\r\n\r\n", &id);
		return id;
	}
	else
		return -1;
}
void send_template(char * s,int * csock)
{
	FILE * fp = fopen(s, "r");
	if (fp == NULL)
		closesocket(*csock);
	else
	{
		char ch = NULL;
		while (!feof(fp))
		{
			int i = 0;
			char * buff = (char *)malloc(sizeof(char) * 128);
			if (ch == NULL)ch = fgetc(fp);
			for (i = 0; i < 127 && !feof(fp); i++)
			{

				buff[i] = ch;
				//printf("%c", ch);
				ch = fgetc(fp);
			}
			buff[i] = '\0';
			replyto_client(buff, csock);
		}
	}

}
char *  escape_string(char * s)
{
	int i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '+')
			s[i] = ' ';
		i++;
	}
	return s;
}
char * get_value(post_data * new_head,char * s)
{
	while (new_head != NULL)
	{
		if (strcmp(s, new_head->key) == 0)
			return new_head->value;
		new_head = new_head->next;
	}
	return "";
}
DWORD WINAPI SocketHandler(void* lp)
{
	printf("\n-----------------------------------------------------------------------------\n");
	int *csock = (int*)lp;
	char recvbuf[1024];
	int recvbuf_len = 1024;
	int recv_byte_cnt;

	memset(recvbuf, 0, recvbuf_len);
	if ((recv_byte_cnt = recv(*csock, recvbuf, recvbuf_len, 0)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		free(csock);
		return 0;
	}
	////system("cls");
	//	
	printf("%s\n", recvbuf);

	printf("\n-----------------------------------------------------------------------------\n");
	char * url = (char *)malloc(sizeof(char) * 30);
	char * Protocol = (char *)malloc(sizeof(char) * 30);
	sscanf(getline(recvbuf), "%s %s HTTP//1.1", Protocol, url);
	//printf("User_id is %d",get_user(recvbuf));
	initialize();
	if (strcm(Protocol, "POST") == 0)
	{
		if (strcm(url, "/") == 0)
		{
			print_users_database(csock);
		}
		else if (strcm(url, "/add_user") == 0)
		{
			post_data * new_head = preprocess(recvbuf);
			if (strcmp(new_head->key, "user") == 0)
			{
				add_user_database(escape_string(new_head->value), csock);
				print_users_database(csock);
			}
			else
				replyto_client("-1", csock);
		}
		else if (strcm(url, "/add_cat") == 0)
		{
			post_data * new_head = preprocess(recvbuf);
			if (get_value(preprocess(recvbuf), "category") != "")
			{
				add_cat_database(get_value(preprocess(recvbuf), "category"));
				replyto_client("success", csock);
			}
			else
				replyto_client("-1", csock);
		}
		else if (strcm(url, "/add_msg") == 0)
		{
			if (get_value(preprocess(recvbuf), "categ_id") != "" && get_value(preprocess(recvbuf), "message") != "")
			{
				add_msgs_database(atoi(get_value(preprocess(recvbuf), "categ_id")), get_user(recvbuf), get_value(preprocess(recvbuf), "message"));
				//printf("categ_id %d", atoi(get_value(preprocess(recvbuf), "categ_id")));
				print_messages_database(atoi(get_value(preprocess(recvbuf), "categ_id")), csock);
			}
			else
				replyto_client("-1", csock);
		}
		else if (strcm(url, "/get_msg") == 0)
		{
			post_data * new_head = preprocess(recvbuf);
			if (get_value(new_head, "categ_id") != "")
				print_messages_database(atoi(get_value(preprocess(recvbuf), "categ_id")), csock);
			else
				replyto_client("-1", csock);
		}
			

		}
	else
		{
			if (strcmp(url, "/me.css") == 0)
			{
				replyto_client("HTTP/1.1 200  Okay\r\nContent-Type: text/css; charset=ISO-8859-4 \r\n\r\n", csock);
				send_template("C:/Users/HP/Desktop/test/me.css", csock);
			}
			else if (strcmp(url, "/get_user_form") == 0)
			{

				replyto_client("HTTP/1.1 200  Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n", csock);
				send_template("C:/Users/HP/Desktop/test/js/templates/add_user.html", csock);
			}
			else if (strcmp(url, "/get_user") == 0)
			{
				print_users_database(csock);
			}
			else if (strcm(url, "/get_cat") == 0)
			{
				print_cat_database(get_user(recvbuf), csock);
			}
			else if (strcmp(url, "/") == 0)
			{
				replyto_client("HTTP/1.1 200  Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n", csock);
				send_template("C:/Users/HP/Desktop/test/js/templates/get_user_template.html", csock);
				
			}
			else if (strcmp(url, "/favicon.ico") == 0)
			{
				replyto_client("HTTP/1.1 200 Okay\r\n Content-Type:text/html\r\n\r\n ", csock);
			}
			else if (strcm(url, "/todo.js") == 0)
			{
				replyto_client("HTTP/1.1 206 Okay\r\nContent-Type: text/javascript;\r\n\r\n", csock);
				send_template("C:/Users/HP/Desktop/test/js/todo.js", csock);
			}
			else if (strcm(url, "/student") == 0)
			{
				replyto_client("http/1.1 200 okay\r\ncontent-type: text/html; charset=iso-8859-4 \r\n\r\n<table border=\"1\"><tr><th>name</th></tr><tr><td>venkatesh1</td></tr></table>", csock);
			}

			else
			{
				replyto_client("HTTP/1.1 200  Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n", csock);
				send_template("C:/Users/HP/Desktop/test/js/todo.html", csock);
			}
		}
		closesocket(*csock);
		
    return 0;
}