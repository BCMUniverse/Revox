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
    char ports[][16] = {"http", "https", "ftp", "telnet", "gopher", "file", "mailto", "news", "nntp", "wais", "prospero"};
    char bars[][4] = {":", "//", "/", "?"}, aux[16], aux2[16];
    int i, j, k;
    url addr;

    #pragma omp parallel for schedule(guided)
    for(i=0; i<11; i++){
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
            case "telnet":
                addr.port = 23;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case "gopher":
                addr.port = 70;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case "file":
                addr.port = 0;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case "mailto":
                addr.port = 25;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, port[i], bars[3]);
                break;
            case "news":
                addr.port = 0;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, port[i], "\0");
                break;
            case "nntp":
                addr.port = 119;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, port[i], bars[1]);
                break;
            case "wais":
                addr.port = 210;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, port[i], bars[1]);
                break;
            case "prospero":
                addr.port = 1525;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, port[i], bars[1]);
                break;
            default:
                fprintf(stderr, "Erro: protocolo não encontrado!\r\n");
            }
        }
        else{
            addr.port = 80;
            strcpy(addr.prtcol, ports[i]);
            SubString2(host, ports[i], bars[1]);
        }
    }
    addr.host = SubString2(host, NULL, bars[2]);
    strcpy(addr.url_path, bars[2]);
    strcat(addr.url_path, host);

    return addr;
}
