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
#include "htmlText.h"
#include "urlparser.h"

#pragma omp parallel

url UrlParser(char host[]){
    char ports[][8] = {"http", "https", "ftp", "telnet"}, bars[][4] = {":", "//", "/"}, aux[16], aux2[16];
    int i, j, k;
    url addr;

    #pragma omp parallel schedule(guided)
    for(i=0; i<4; i++){
        aux = CreateTag(ports[i], bars[0], "\0");
        for(k=0; aux[k]!='\0'; k++);
        aux2 = strstr(host, aux);
        if(aux2==NULL){
            aux2 = strstr(host, strupr(aux));
        }
        if(aux2!=NULL){
            switch(aux){
            case "http:":
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case "https:":
                addr.port = 443;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case "ftp":
                addr.port = 21;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            }
        }
    }

    return addr;
}
