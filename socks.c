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
#include <sys/types.h>

#pragma omp parallel

int InitSock(){
    WSADATA wsa;
    printf("Inicializando Revox...\r\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Falhou. Codigo de Erro: %d\n",WSAGetLastError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

char *GetIP(char *address){
    in_addr ia;
    hostent* hbn;
    unsigned int addr;

    if(isalpha(address[0])){
        hbn = gethostbyname(address);
    }
    else{
        addr = inet_addr(address);
        hbn = gethostbyaddr((char *)&addr, 4, AF_INET);
    }
    if(hbn==0){
        fprintf(stderr, "Erro: Falha ao procurar o IP: %d\n", WSAGetLastError());
        return "";
    }
    ia.S_un.S_addr =* (DWORD *)hbn->h_addr_list[0];

    return inet_ntoa(ia);
}

int ValidateEnvelope(char *h, char *r, char *s, char *ip){
    if(h="" || r="" || s=""){
        fprintf(stderr, "Erro: Endere\0207o de email, host ou remente/destinat\0240rio inv\0240lido!\r\n");
        return EXIT_FAILURE;
    }
    *ip = GetIP(h);
    if(*ip==""){
        fprintf(stderr, "Erro: \r\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
