/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geral GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.2
	BCM Revox Engine -> Ano: 2014|Tipo: WebEngine
*/
#include <windows.h>
#include <ras.h>
#include <stdio.h>
#include <conio.h>

#ifndef __cplusplus
typedef enum { false, true } bool;
#endif

#define ESC 27

//---------------------------------------------------------------------------
int __fastcall EnumConnections(LPRASCONN *lppRasConn){
    LPRASCONN lpRasConn;
    DWORD cb = 0, cConnections = 0;

    // Verifica quantas conexões estão ativas
    RasEnumConnections(0, &cb, &cConnections);
    if(cConnections){
        // Aloca memória p/ as conexões
        lpRasConn = (LPRASCONN)GlobalAlloc(GPTR, sizeof(RASCONN) * cConnections);

        // Se falhar retorna -1
        if(!lpRasConn)
            return -1;

        // Pega as propriedades das conexões
        lpRasConn[0].dwSize = sizeof(RASCONN);
        cb = sizeof(RASCONN) * cConnections;
        if(RasEnumConnections(lpRasConn, &cb, &cConnections)){
            // Se falhar, libera memória e retorna -1
            GlobalFree(lpRasConn);
            return -1;
        }

        // Atribue o buffer ao ponteiro passado como parâmetro,
        // nunca se esquecer de liberar a memória quando o buffer
        // não for mais utilizado.
        *lppRasConn = lpRasConn;
    }

    // Retorna o número de conexões
    return cConnections;
}
//---------------------------------------------------------------------------
char *IsConnected(){
    LPRASCONN lpRasConn;
    int cConnections;
    char key = 0, conct[256];
    int oldCount = -2;

    while(key != ESC){
        cConnections = EnumConnections(&lpRasConn);

        if(kbhit())
            key = getch();

        if(oldCount != cConnections){
            oldCount = cConnections;
            clrscr();

            switch(cConnections){
            case -1:
                printf("Erro verificando conexões.");
                return;

            case 0:
                printf("Nenhuma conexão ativa.");
                break;

            default:
                for(int c = 0; c < cConnections; c++){
                    RASCONNSTATUS RasConnStatus;
                    RasConnStatus.dwSize = sizeof(RASCONNSTATUS);
                    RasGetConnectStatus(lpRasConn[c].hrasconn, &RasConnStatus);

                    printf("%s - %s - %s\n", lpRasConn[c].szEntryName, lpRasConn[c].szDeviceName, (conct = (RasConnStatus.rasconnstate == RASCS_Connected ? "Conectado" : "Não conectado")));

                }
            }

            GlobalFree(lpRasConn);
        }
    }
    return conct;
}
