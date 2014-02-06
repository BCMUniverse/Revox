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
#include "bonus.h"
#include "socks.h"
#include "guicon.h"
#include "resource.h"

#pragma omp

char sendr[1025];

BOOL CALLBACK AboutDlgProc2(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
    switch(Message){
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case IDOK2:
            GetDlgItemText(hwnd, IDCTEXT, (LPSTR)sendr, 1025);
            EndDialog(hwnd, IDOK2);
            #ifdef _DEBUG
            RedirectIOToConsole();
            #endif
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


void InitNNTPGUI(char address[], int port, char path[], HINSTANCE hInst, HWND hwnd){
    char ip[TKB], comnd[(4*STKB)], ng[STKB], tmp[VKB];
    int cs = 0, i;
    SOCKET sock;

    InitSock();
    strcpy(ip, ValidEnvelope2(address));
    if(!ip){
        return;
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock==SOCKET_ERROR){
        return;
    }
    if (!ConnectTo(port, ip, sock)){
        fprintf(stderr, "Erro: Nao \0202 poss\0241vel conectar ao servidor!\r\n");
        return;
    }
    for(i=1; path[i]!='/' || path[i]!='\0'; i++){
        ng[i] = path[i];
    }
    sprintf(tmp, "GROUP %s", ng);
    snd(tmp, 1, sock);
    do{
        DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), hwnd, AboutDlgProc2);
        fprintf(stdout, "Revox %s NNTP-> ", VERSION);
        fgets(comnd, 4096, stdin);
        sprintf(tmp, "%s", comnd);
        if(strstr(comnd, "QUIT")!=NULL){
            cs = closesocket(sock);
        }
    }while(cs!=0);
    WSACleanup();
}

void InitNNTPText(char address[], int port, char path[]){
    char ip[TKB], comnd[(4*STKB)], ng[STKB], tmp[VKB];
    int cs = 0, i;
    SOCKET sock;

    InitSock();
    strcpy(ip, ValidEnvelope2(address));
    if(!ip){
        return;
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock==SOCKET_ERROR){
        return;
    }
    if (!ConnectTo(port, ip, sock)){
        fprintf(stderr, "Erro: Nao \0202 poss\0241vel conectar ao servidor!\r\n");
        return;
    }
    for(i=1; path[i]!='/' || path[i]!='\0'; i++){
        ng[i] = path[i];
    }
    sprintf(tmp, "GROUP %s", ng);
    snd(tmp, 1, sock);
    do{
        printf("Revox %s NNTP-> ", VERSION);
        fgets(comnd, 4096, stdin);
        sprintf(tmp, "%s", comnd);
        if(strstr(comnd, "QUIT")!=NULL){
            cs = closesocket(sock);
        }
    }while(cs!=0);
    WSACleanup();

    return;
}
