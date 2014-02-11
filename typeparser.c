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
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bonus.h"
#include "html.h"
#include "htmlText.h"
#include "manifest.h"
#include "typeparser.h"
#include "urlparser.h"

char types[][128] = {"text/html", "text/cache-manifest", "text/plain"};

char *InitTypeParser(Type tp1, int mode){
    char *result, *vtyp = (char *)malloc(sizeof(char)*4096);
    int i, j;
    type typ1;

    for(i=0; tp1.content[i]!='\0'; i++){
        for(j=0; j<3; j++){
            vtyp = strstr(tp1.content, types[j]);
            if(vtyp!=NULL){
                typ1 = j;
                break;
            }
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
            result = InitManifest(tp1.content, tp1.url);
            break;
        case PLAIN:
            break;
        default:
            break;
        }
    }

    return result;
}
