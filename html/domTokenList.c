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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "domTokenList.h"
#include "../utils/filaStrs.h"
#include "../utils/utils.h"

DOMTokenList* criaDOMTokenList(){
    DOMTokenList *dtl = (DOMTokenList *)malloc(sizeof(DOMTokenList));
    dtl->length = 0;
    dtl->tokens = criaFilaString();
    return dtl;
}

unsigned long DTLgetLength(DOMTokenList* dtl){return dtl->length;}

char* DTLitem(DOMTokenList *dtl, unsigned long index){
    if(index >= dtl->length){return NULL;}
    nodeString *atual = dtl->tokens->inicio;
    for(int i=0; atual->prox != NULL; i++){
        if(index == i){return atual->strings;}
        atual = atual->prox;
    }
    return NULL;
}

bool DTLcontains(DOMTokenList *dtl, char *token){
    nodeString *atual = dtl->tokens->inicio;
    while(atual->prox != NULL){
        if(atual->strings == token){return true;}
        atual = atual->prox;
    }
    return false;
}

void DTLadd(DOMTokenList *dtl, char** tokens){
    int lenTokens = strlen(*tokens);
    for(int i=0; i<(lenTokens - 1); i++){
        pushFS(dtl->tokens, tokens[i]);
    }
    dtl->length = dtl->tokens->tam;
    //TODO: Add Update Steps
}

void DTLremove(DOMTokenList *dtl, char **tokens){
    int lenTokens = strlen(*tokens);
    for(int i=0; i<(lenTokens - 1); i++){
        pop2FS(dtl->tokens, tokens[i]);
    }
    dtl->length = dtl->tokens->tam;
    //TODO: Add Update Steps
}

bool DTLtoggle(DOMTokenList *dtl, char *token, bool force){
    if(token == NULL || token == "\0" || strpbrk(token, "\x0009\x000A\x000C\x000D\x0020") != NULL){return false;}
    nodeString *atual = dtl->tokens->inicio;
    while(atual->prox != NULL){
        if(atual->strings == token){
            if(force){return true;}
            else{
                pop2FS(dtl->tokens, token);
                dtl->length--;
                //TODO: Add Update Steps
                return false;
            }
        }
        atual = atual->prox;
    }
    if(force){
        pushFS(dtl->tokens, token);
        dtl->length++;
        return true;
    }
    return false;
}

void DTLreplace(DOMTokenList *dtl, char *token, char *newToken){
    if(token == NULL || token == "\0" || strpbrk(token, "\x0009\x000A\x000C\x000D\x0020") != NULL){return;}
    if(newToken == NULL || newToken == "\0" || strpbrk(newToken, "\x0009\x000A\x000C\x000D\x0020") != NULL){return;}
    nodeString *atual = dtl->tokens->inicio;
    while(atual->prox != NULL){
        if(atual->strings == token){
            atual->strings = newToken;
            break;
        }
        atual = atual->prox;
    }
    if(atual == NULL){return;}
    //TODO: Add Update Steps
}

bool DTLsupports(DOMTokenList *dtl, char *token){
    for(int i=0; i<LEN_LOCAL_NAMES_SUPPORTED; i++){
        if(token == localName[i]){return true;}
    }
    return false;
}

//TODO: Implementar getValue
char* DTLgetValue(DOMTokenList *dtl){
    return NULL;
}

//TODO: Implementar setValue
void DTLsetValue(DOMTokenList *dtl, char* value){}
