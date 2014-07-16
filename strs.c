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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int SearchString(char content[], char subStr[]){
    char *aux = NULL, auxstr[strlen(subStr)];
    int i = 0, result = 0;

    strcpy(auxstr, subStr);
    aux = strstr(content, auxstr);
    if(aux==NULL){
        aux = strstr(content, strupr(auxstr));
        if(aux==NULL){
            strlwr(auxstr);
            for(i=0; i<strlen(subStr); i++){
                auxstr[i] = toupper(auxstr[i]);
                aux = strstr(content, auxstr);
                if(aux!=NULL){
                    break;
                }
                else{
                    auxstr[i] = tolower(auxstr[i]);
                }
            }
            if(aux==NULL){
                return -1;
            }
        }
    }
    result = aux-content;
    if(result<0){
        result = 0;
    }

    return result;
}

int streql(char *str1, char *str2){
    while((*str1 == *str2) && (*str1)){
        str1++;
        str2++;
    }
    return((*str1 == NULL) && (*str2 == NULL));
}

char *copiaLinha(char content[], int *i){
    char result[strlen(content)], c;
    int j = 0, k = *i;

    memset(result, 0, sizeof(result));
    while(content[k]!='\r' && content[k]!='\n'){
        result[j++] = content[k++];
    }
    while(content[k]=='\r' || content[k]=='\n'){
        c = content[k++];
    }
    *i = k;

    return result;
}
