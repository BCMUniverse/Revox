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
	BCM Revox Engine -> Ano: 2014|Tipo: WebEngine
*/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html.h"

#pragma omp

char extras[][16] = {"!doctype", "!--", "![CDATA["};
char elemts[][16] = {"html", "head", "base", "link", "meta", "noscript", "script", "style", "template", "title", "body", "a", "abbr", "address", "area", "article", "aside", "audio", "b", "p"};

Fhtp *aloca(){
    Fhtp *f = malloc(sizeof(Fhtp));
    f->start = NULL;
    f->end1 = NULL;
    f->tam = 0;
    return f;
}

void CreateToken(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid, int mode, int pos){
    switch(mode){
    case 0:
        CreateTokenEmpty(f, stag, etag, cont, ClassName, id, style, attrs, tagid);
        break;
    case 1:
        CreateTokenNormal(f, stag, etag, cont, ClassName, id, style, attrs, tagid);
        break;
    case 2:
        CreateTokenInCurse(f, stag, etag, cont, ClassName, id, style, attrs, tagid, pos);
        break;
    default:
        fprintf(stderr, "Erro: Modo inexistente!\r\n");
    }
}

void CreateTokenEmpty(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid){
    htp *f2;

    if((f2 = malloc(sizeof(htp)) == NULL)){
        return;
    }
    //Copia os dados
    strcpy(f2->stag, stag);
    strcpy(f2->etag, etag);
    strcpy(f2->cont, cont);
    strcpy(f2->ClassName, ClassName);
    strcpy(f2->id, id);
    strcpy(f2->style, style);
    strcpy(f2->attrs, attrs);
    f2->TagID = tagid;
    //Atualiza a Lista
    f2->ant = f->start;
    f2->prox = f->end1;
    f->start = f2;
    f->end1 = f2;
    f->tam++;
}

void CreateTokenNormal(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid){
    htp *f2;

    if((f2 = malloc(sizeof(htp)) == NULL)){
        return;
    }
    //Copia os dados
    strcpy(f2->stag, stag);
    strcpy(f2->etag, etag);
    strcpy(f2->cont, cont);
    strcpy(f2->ClassName, ClassName);
    strcpy(f2->id, id);
    strcpy(f2->style, style);
    strcpy(f2->attrs, attrs);
    f2->TagID = tagid;
    //Atualiza a Lista
    f2->ant = f->end1;
    f2->prox = NULL;
    f->end1->prox = f2;
    f->end1 = f2;
    f->tam++;
}

void CreateTokenInCurse(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid, int pos){
    htp *f2, *atual;
    int i;

    if((f2 = malloc(sizeof(htp)) == NULL)){
        return;
    }
    //Copia os dados
    strcpy(f2->stag, stag);
    strcpy(f2->etag, etag);
    strcpy(f2->cont, cont);
    strcpy(f2->ClassName, ClassName);
    strcpy(f2->id, id);
    strcpy(f2->style, style);
    strcpy(f2->attrs, attrs);
    f2->TagID = tagid;
    //Atualiza a Lista
    atual = f->start;
    for(i=1; i<pos; i++){
        atual = atual->prox;
    }
    f2->ant = atual->ant;
    f2->prox = atual;
    if(atual->ant == NULL){
        f->start = f2;
    }
    else{
        atual->ant->prox = f2;
    }
    atual->ant = f2;
    f->tam++;
}

void RemvToken(Fhtp *f, int pos){
    int i;
    htp *remvElm, *atual;

    if(f->tam == 0){
        return;
    }
    if(pos == 1){
        remvElm = f->start;
        f->start = f->start->prox;
        if(f->start == NULL){
            f->end1 = NULL;
        }
        else{
            f->start->ant = NULL;
        }
    }
    else if(pos == f->tam){
        remvElm = f->end1;
        f->end1->ant->prox = NULL;
        f->end1 = f->end1->ant;
    }
    else{
        atual = f->start;
        for(i=1; i<pos; i++){
            atual = atual->prox;
        }
        remvElm = atual;
        atual->ant->prox = atual->prox;
        atual->prox->ant = atual->ant;
    }
    free(remvElm->attrs);
    free(remvElm->ClassName);
    free(remvElm->cont);
    free(remvElm->etag);
    free(remvElm->id);
    free(remvElm->stag);
    free(remvElm->style);
    free(remvElm);
    f->tam--;
}

void Delete(Fhtp *f){
    while(f->tam>0){
        RemvToken(f, 1);
    }
}
