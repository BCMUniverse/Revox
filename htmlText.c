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
#include <ctype.h>
#include "bonus.h"
#include "html.h"
#include "htmlText.h"
#include "typeparser.h"

char *CreateTag(char *inicio, char *tag, char *fim){
    char *element = (char *)malloc(sizeof(char)*(strlen(inicio)+strlen(tag)+strlen(fim)));

    strcpy(element, inicio);
    strcat(element, tag);
    strcat(element, fim);

    return element;
}

char *SubString(char *str, int inicio, int fim){
    char *sub;
    int i, j, k;

    for(k=0; str[k]!='\0'; k++);
    if(inicio >= fim || fim>k){
        fprintf(stderr, "Erro! SubString nao Encontrada!\n");
        return "\0";
    }
    if(inicio<0){
        inicio = 0;
    }
    sub = (char *)malloc(sizeof(char)*((fim-inicio)+1));
    for(i=inicio, j=0; i<fim; i++, j++){
        sub[j] = str[i];
    }
    sub[j] = '\0';

    return sub;
}

char *SubString2(char *str, char *inicio, char *fim){
    int i, j, k, l = 0, m1, n1;
    char *result, *m, *n;
    result = (char *)malloc(sizeof(char)*(strlen(str)));
    m = (char *)malloc(sizeof(char)*(strlen(str)));
    n = (char *)malloc(sizeof(char)*(strlen(str)));

    for(i=0; result[i]!='\0'; i++){
        result[i] = '\0';
    }
    for(j=0; str[j]!='\0'; j++);
    for(k=0; inicio[k]!='\0'; k++);
    for(l=0; fim[l]!='\0'; l++);
    for(i=0; str[i]!='\0'; i++){
        m = strstr(str, inicio);
        n = strstr(str, fim);
        if(m==NULL){
            m = strstr(str, strupr(inicio));
        }
        if(n==NULL){
            n = strstr(str, strupr(fim));
        }
        m1 = m-str;
        n1 = n-str;
        if(m1<0){
            m1 = 0;
        }
        if(n1<0){
            n1 = 0;
        }
        if(m1==0 && n1==0){
            fprintf(stderr, "Nao tem mais %s ou %s!\n", inicio, fim);
            break;
        }
        else{
            strcat(result, SubString(str, m1+(k+1), n1-(l/8)));
            strcat(result, "\r\n");
            str = SubString(str, n1+l, j);
            for(j=0; str[j]!='\0'; j++);
        }
    }
    free(m);
    free(n);

    return result;
}

void RemvSubString(char *str, char *substr){
    while(*str){
        char *sub=substr, *s=str;
        while(*sub && *sub==*s){
            ++sub, ++s;
        }
        if(!*sub){
            while(*s){
                *str++ = *s++;
            }
            *str = 0;
            return;
        }
        ++str;
    }
}
