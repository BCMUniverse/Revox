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
#include "htmlText.h"
#include "urlparser.h"

#pragma omp parallel

url UrlParser(char host[]){
    char ports[][16] = {"http", "https", "ftp", "telnet", "gopher", "file", "mailto", "news", "nntp", "wais", "prospero"};
    char bars[][4] = {":", "//", "/", "?"}, aux[16], aux2[16];
    int i, j;
    url addr;
    uports port1;

    #pragma omp parallel for schedule(guided)
    for(i=0; i<11; i++){
        strcpy(aux, CreateTag(ports[i], bars[0], "\0"));
        strcpy(aux2, strstr(host, aux));
        if(aux2==NULL){
            strcpy(aux2, strstr(host, strupr(aux)));
        }
        if(aux2!=NULL){
            port1 = i;
            switch(port1){
            case HTTP:
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case HTTPS:
                addr.port = 443;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case FTP:
                addr.port = 21;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case TELNET:
                addr.port = 23;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case GOPHER:
                addr.port = 70;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case FILES:
                addr.port = 0;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case MAILTO:
                addr.port = 25;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[3]);
                break;
            case NEWS:
                addr.port = 0;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], "\0");
                break;
            case NNTP:
                addr.port = 119;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case WAIS:
                addr.port = 210;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
                break;
            case PROSPERO:
                addr.port = 1525;
                strcpy(addr.prtcol, ports[i]);
                SubString2(host, ports[i], bars[1]);
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
    for(i=0; host[i]!=bars[0] || host[i]!=bars[2]; i++){
        addr.host[i] = host[i];
    }
    if(host[i]==bars[0]){
        SubString2(host, host[0], bars[0]);
        for(j=0; j<16; j++){
            aux[j] = '\0';
        }
        for(i=0; host[i]!=bars[2]; i++){
            aux[i] = host[i];
        }
        addr.port = atoi(aux);
    }
    strcpy(addr.url_path, bars[2]);
    strcat(addr.url_path, host);

    return addr;
}
