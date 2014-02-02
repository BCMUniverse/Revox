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
#include "bonus.h"
#include "ftp.h"
#include "http.h"
#include "mail.h"
#include "htmlText.h"
#include "nntp.h"
#include "prospero.h"
#include "urlparser.h"
#include "usenet.h"

#pragma omp parallel

char ports[][16] = {"http", "https", "ftp", "telnet", "gopher", "file", "mailto", "news", "nntp", "wais", "prospero"};
char bars[][4] = {":", "//", "/", "?"};

url UrlParser(char host[]){
    char aux[16], aux2[16];
    int i, j;
    url addr;
    uports port1;

    limpaVetor(addr.host, 1025);
    limpaVetor(addr.url_path, 1025);
    #pragma omp parallel for schedule(guided)
    for(i=0; i<11; i++){
        strcpy(aux, CreateTag(ports[i], bars[0], bars[1]));
        strcpy(aux2, strstr(host, aux));
        if(aux2==NULL){
            strcpy(aux2, strstr(host, strupr(aux)));
        }
        if(aux2==NULL){
            strcpy(aux, CreateTag(ports[i], bars[0], "\0"));
            strcpy(aux2, strstr(host, aux));
            if(aux2==NULL){
                strcpy(aux2, strstr(host, strupr(aux)));
            }
        }
        if(aux2!=NULL){
            port1 = i;
            switch(port1){
            case HTTP:
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case HTTPS:
                addr.port = 443;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case FTP:
                addr.port = 21;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case TELNET:
                addr.port = 23;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case GOPHER:
                addr.port = 70;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case FILES:
                addr.port = 80;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case MAILTO:
                addr.port = 25;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case NEWS:
                addr.port = 0;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case NNTP:
                addr.port = 119;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case WAIS:
                addr.port = 210;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            case PROSPERO:
                addr.port = 1525;
                strcpy(addr.prtcol, ports[i]);
                RemvSubString(host, aux);
                break;
            default:
                fprintf(stderr, "Erro: protocolo não encontrado!\r\n");
            }
            break;
        }
        else{
            addr.port = 80;
            strcpy(addr.prtcol, ports[i]);
            RemvSubString(host, aux);
        }
    }
    for(i=0; i<strlen(host) && host[i]!=':' && host[i]!='/' && host[i]!='?'; i++){
        addr.host[i] = host[i];
    }
    RemvSubString(host, addr.host);
    if(host[i]==':'){
        RemvSubString(host, bars[0]);
        limpaVetor(aux, 16);
        for(i=0; host[i]!='/'; i++){
            aux[i] = host[i];
        }
        addr.port = atoi(aux);
    }
    else{
        if(host[i]=='/'){
            strcpy(addr.url_path, host);
        }
        else{
            if(host[i]=='?'){
                strcpy(addr.url_path, host);
            }
        }
    }
    if(strchr(host, '/')==NULL){
        strcpy(addr.url_path, "/");
    }

    return addr;
}

char *UrlConnect(char *host, int mode, HINSTANCE hInst, HWND hwnd){
    char *result;
    int i;
    uports up1;
    url url1 = UrlParser(host);
    for(i=0; i<11; i++){
        if(strcmp(url1.prtcol, ports[i])==0){
            up1 = i;
            break;
        }
    }
    switch(up1){
    case HTTP:
        result = InitHTTP(url1.host, url1.port, url1.url_path, NULL);
        break;
    case HTTPS:
        result = InitHTTP(url1.host, url1.port, url1.url_path, NULL);
        break;
    case FTP:
        InitFTP(url1.host);
        break;
    case TELNET:
        char msg[2*BUFKB];
        strcpy(msg, "telnet ");
        strcat(msg, url1.host);
        strcat(msg, " ");
        strcat(msg, url1.port);
        system(msg);
        result = NULL;
        break;
    case GOPHER:
        result = NULL;
        break;
    case FILES:
        FILE *input;
        if(url1.host==NULL){
            RemvSubString(url1.url_path, "/");
            if((input = fopen(url1.url_path, "r"))==NULL){
                fprintf(stderr, "Erro: Arquivo Invalido!\n");
                return NULL;
            }
            fgets(result, BUF32KB, input);
            fclose(input);
        }
        else{
            result = InitHTTP(url1.host, url1.port, url1.url_path, NULL);
        }
        break;
    case MAILTO:
        switch(mode){
        case 0:
            result = InitMailGUI(url1.host, url1.url_path, url1.port, hInst, hwnd);
            break;
        case 1:
            result = InitMailText(url1.host, url1.url_path, url1.port);
            break;
        default:
            fprintf(stderr, "Erro: Valor inexistente!\r\n");
            result = NULL;
        }
        break;
    case NEWS:
        result = NULL;
        break;
    case NNTP:
        switch(mode){
        case 0:
            InitNNTPGUI(url1.host, url1.port, url1.url_path, hInst, hwnd);
            break;
        case 1:
            InitNNTPText(url1.host, url1.port, url1.url_path);
            break;
        default:
            fprintf(stderr, "Erro: Valor inexistente!\r\n");
        }
        result = NULL;
        break;
    case WAIS:
        result = NULL;
        break;
    case PROSPERO:
        result = InitProspero(url1.host, url1.port, url1.url_path, mode, hInst, hwnd);
        break;
    }

    return result;
}
