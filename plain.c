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
#include <string.h>
#include "bonus.h"
#include "strs.h"
#include "typeparser.h"

char *InitPlain(char content[], char url[]){
    char body[strlen(content)+1], *aux = NULL;
    int i = 0;

    strcpy(body, TagBody(content));
    while(body[i]!='\0'){
        if(aux!=NULL){
            limpaVetor(aux);
            aux = NULL;
        }
        aux = copiaLinha(body, &i);
    }

    return body;
}
