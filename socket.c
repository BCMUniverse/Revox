/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geru GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.1
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
#include <stdio.h>
#include <sys/types.h>
#include <winsock.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bonus.h"

void InitSocket(char *hostname){
    struct sockaddr_in server;
    SOCKET sock_id;
    char *message, server_reply[BUF32KB];
    int recv_size, i;
    WSADATA wsa;
    struct hostent *he;
    struct in_addr **addr_list;
    char ip[100];

	//Inicializa o WinSock
    printf("Iniciando Revox...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Falhou. Codigo de Erro: %d\n",WSAGetLastError());
        return 1;
    }
    printf("Inicializado.\n");

	//Criar o socket
	//Socket IPv4
	if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Nao pode criar o socket: %d\n" , WSAGetLastError());
	}
    printf("Socket criado com sucessso.\n");
    //Socket IPv6
    /*if((sock_id = socket(AF_INET6, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Nao pode criar o socket IPv6: %d\n" , WSAGetLastError());
	}
    printf("Socket IPv6 criado com sucessso.\n");*/

    //Estruturas do IPv4
    struct sockaddr_in {
        short            sin_family;   // e.g. AF_INET, AF_INET6
        unsigned short   sin_port;     // e.g. htons(3490)
        struct in_addr   sin_addr;     // see struct in_addr, below
        char             sin_zero[8];  // zero this if you want to
    };
    typedef struct in_addr {
        union {
            struct {
                u_char s_b1,s_b2,s_b3,s_b4;
            } S_un_b;
            struct {
                u_short s_w1,s_w2;
            } S_un_w;
            u_long S_addr;
        } S_un;
    } IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;
    struct sockaddr {
        unsigned short    sa_family;    // address family, AF_xxx
        char              sa_data[14];  // 14 bytes of protocol address
    };
    struct hostent{
        char *h_name;         /* Official name of host.  */
        char **h_aliases;     /* Alias list.  */
        int h_addrtype;       /* Host address type.  */
        int h_length;         /* Length of address.  */
        char **h_addr_list;       /* List of addresses from name server.  */
    };

    if((he = gethostbyname(hostname)) == NULL){
        //gethostbyname failed
        printf("gethostbyname failed : %d" , WSAGetLastError());
        return 1;
    }
    //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++){
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]));
    }

    //Server
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );

    //Connect to remote server
    if (connect(sock_id, (struct sockaddr *)&server , sizeof(server)) < 0){
        puts("Erro ao Conectar!");
        return 1;
    }
    puts("Connectado.");
    //Send some data
    message = "GET / HTTP/1.1\r\n\r\n";
    if(send(sock_id, message, strlen(message), 0) < 0){
        puts("Falhou ao enviar");
        return 1;
    }
    puts("Dados enviados");

    //Receive a reply from the server
    if((recv_size = recv(sock_id, server_reply, BUF32KB, 0)) == SOCKET_ERROR){
        puts("recv falhou");
    }

    puts("Resposta recebida\n");

    //Add a NULL terminating character to make it a proper string before printing
    server_reply[recv_size] = '\0';
    puts(server_reply);

    closesocket(sock_id);
    WSACleanup();

	return;
}
