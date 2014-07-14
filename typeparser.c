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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bonus.h"
#include "html.h"
#include "htmlText.h"
#include "manifest.h"
#include "plain.h"
#include "strs.h"
#include "typeparser.h"
#include "urlparser.h"

char types[][128] = {"text/html", "text/cache-manifest", "text/plain", "application/x-download", "application/octet-stream"};

char *TypeBuster(char content[], type typ1){
    char *aux = NULL, TypFile[VKB], *vtyp = NULL;
    int i, j;

    for(i=0; content[i]!='\0';){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = CopyManifst(content, &i);
        if(SearchString(aux, "Content-Type:")>-1){
            for(i=i, j=0; content[i]!='\r' && content[i]!='\n'; i++, j++){
                TypFile[j] = content[i];
            }
            break;
        }
    }
    for(j=0; j<5; j++){
        vtyp = strstr(TypFile, types[j]);
        if(vtyp!=NULL){
            typ1 = (type)j;
        }
    }
    return TypFile;
}

char *InitTypeParser(Type tp1, int mode){
    char *result, *vtyp, TypFile[VKB];
    int j;
    type typ1;

    for(j=0; j<5; j++){
        vtyp = strstr(tp1.content, types[j]);
        if(vtyp!=NULL){
            typ1 = (type)j;
        }
    }
    if(vtyp==NULL){
        strcpy(TypFile, TypeBuster(tp1.content, typ1));
    }
    switch(typ1){
    case THTML:
        switch(mode){
        case 0:
            break;
        case 1:
            result = InitHTMLText(tp1.content);
            break;
        default:
            fprintf(stderr, "Erro: Valor Invalido!\r\n");
        }
        break;
    case MANIFEST:
        InitManifest(tp1.content, tp1.url);
        break;
    case PLAIN:
        result = InitPlain(tp1.content, tp1.url);
        break;
    case DOWNLOAD: case OCTETSTREAM: default:
        break;
    }

    return result;
}

char *TagHeader(char *content){
    char header[8196];
    int i = 0, head = SearchString(content, "\r\n\r\n");

    for(i=0; i<=head; i++){
        header[i] = content[i];
    }

    return header;
}

char *TagBody(char *content){
    char body[strlen(content)-1024];
    int i = 0, j = 0, bodyn = SearchString(content, "\r\n\r\n"), tam = strlen(content);

    for(i=bodyn, j=0; i<tam; i++, j++){
        body[j] = content[i];
    }

    return body;
}
