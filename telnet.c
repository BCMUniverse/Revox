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

	BCM Revox Engine v0.2
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

char *InitTelnet(char *address, int port, char *caminho){
    struct sockaddr_in Server;
    SOCKET sock;
    char buffer[1024];
    int recv_size, i;
    WSADATA wsa;
    struct hostent *he;
    struct in_addr **addr_list;
    char ip[16];

    //Inicialização
    printf("Inicializando Revox...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Falhou. Codigo de Erro: %d\n",WSAGetLastError());
        return NULL;
    }
    //Criar o socket
	//Socket IPv4
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Nao pode criar o socket: %d\n" , WSAGetLastError());
	}
    printf("Inicializado\nBCM Revox Engine v0.2 - Cliente HTTP\nAcessando %s...\n", address);

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

    if((he = gethostbyname(address)) == NULL){
        //gethostbyname failed
        printf("gethostbyname failed : %d" , WSAGetLastError());
        return NULL;
    }
    //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
    addr_list = (struct in_addr **) he->h_addr_list;

    for(i=0; addr_list[i]!=NULL; i++){
        //Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }

    //Server
    Server.sin_addr.s_addr = inet_addr(ip);
    Server.sin_family = AF_INET;
    Server.sin_port = htons(port);

    //Connect to remote server
    if (connect(sock, &Server, sizeof(Server)) < 0){
        puts("Erro ao Conectar!");
        return 1;
    }
    puts("Connectado.");
    //Send some data
    strcpy(buffer, "GET ");
    strcat(buffer, caminho);
    strcat(buffer, " HTTP/1.1\r\n");
    strcat(buffer, "Host: ");
    strcat(buffer, address);
    strcat(buffer, "\r\n");
    strcat(buffer, "Connection: keep-alive\r\n");
    strcat(buffer, "User-Agent: Revox/0.2 (Windows NT 6.1; BCM Revox Engine/0.2)\r\n");
    strcat(buffer, "Accept: text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n");
    strcat(buffer, "Accept-Language: pt-br,pt;q=0.7;en-us,en;q=0.5\r\n");
    strcat(buffer, "Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\r\n");
    strcat(buffer, "Cache-Control: no-cache\r\n");
    strcat(buffer, "Keep-Alive: 300\r\n");
    strcat(buffer, "\r\n");
}
