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
	BCM Revox Engine -> Ano: 2017|Tipo: WebEngine
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "filaStrs.h"

filaString *criaFilaString(){
    filaString *fs = (filaString *)malloc(sizeof(filaString));
    fs->inicio = NULL;
    fs->fim = NULL;
    fs->tam = 0;
    return fs;
}

void pushFS(filaString* fs, char *string){
    if(string == NULL || string == "\0" || strpbrk(string, "\x0009\x000A\x000C\x000D\x0020") != NULL){return;}
    nodeString *ns = (nodeString *)malloc(sizeof(nodeString));
    ns->strings = string;
    if(fs->inicio == NULL){
        fs->inicio = ns;
    }else{
        fs->fim->prox = ns;
        ns->prox = NULL;
    }
    fs->fim = ns;
    fs->tam++;
}

char* popFS(filaString* fs){
    if(fs->inicio == NULL){return NULL;}
    char *string = fs->inicio->strings;
    nodeString *ns = fs->inicio;
    fs->inicio = fs->inicio->prox;
    ns->prox = NULL;
    free(ns);
    fs->tam--;
    return string;
}

void pop2FS(filaString* fs, char* string){
    if(fs->inicio == NULL || strpbrk(string, "\x0009\x000A\x000C\x000D\x0020") != NULL){return;}
    nodeString *ant = fs->inicio, *atual = fs->inicio;
    while(atual->prox != NULL){
        if(atual->strings == string){break;}
    }
    if(atual == ant){
        fs->inicio == NULL;
        fs->fim == NULL;
    }
    else if(atual == fs->fim){
        fs->fim = ant;
        ant->prox = NULL;
    }else{ant->prox = atual->prox;}
    free(atual);
}

char* peekFS(filaString* fs){
    if(fs->inicio == NULL){return NULL;}
    return fs->inicio->strings;
}

bool isEmptyFS(filaString *fs){
    if(fs->inicio == NULL && fs->fim == NULL){return true;}
    return false;
}

void destroyFS(filaString *fs){
    while(fs->inicio != NULL){popFS(fs);}
    free(fs);
}

void printFS(filaString *fs){
    nodeString *atual = fs->inicio;

    while(atual->prox != NULL){
        printf("%s ", atual->strings);
        atual = atual->prox;
    }
    printf("\n");
}
