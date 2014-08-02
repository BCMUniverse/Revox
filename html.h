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
#ifndef _HTML_H_
#define _HTML_H_

#pragma once

typedef struct _listaHtml listaHtml;
typedef struct _pilhaHtml pilhaHtml;

#include "revox.h"

extern char elemts[][16];

typedef enum _Elemts{
    DOCTYPE = 200,
    COMMENTS,
    CDATA,
    HTML = 0,
    HEAD,
    BASE,
    LINK,
    META,
    NOSCRIPT,
    SCRIPT,
    STYLE,
    TEMPLATE,
    TITLE,
    BODY,
    A,
    ABBR,
    ADDRESS,
    AREA,
    ARTICLE,
    ASIDE,
    AUDIO,
    B,
    P,
    UNKNOWN
} Elemts;

struct _elemntHtml{
    char *id, *className, *attrs, *innerHtml;
    Elemts tagName;
    int quantFilhos;
    struct _elemntHtml *ant, *prox;
};

typedef struct _elemntHtml elemntHtml;

struct _listaHtml{
    elemntHtml *inicio, *fim;
    int tam;
};

struct _pilhaHtml{
    Elemts tagName[BUFKB];
    int filhos[BUFKB], topo;
};

/**
*	Aloca a lista.
*
*	@param lista* listaHtml - lista a ser alocada.
**/
void alocaListaHtml(listaHtml *lista);

/**
*	Cria a pilha.
*
*	@return pilhaHtml* - retorna a pilha criada.
**/
pilhaHtml *criaPilhaHtml();

/**
*	Insere o elemento na pilha.
*
*	@param pilha* pilhaHtml - pilha onde o elemento será inserido.
*	@param tagName Elemts - elemento que será adicionado.
*	@return int - retorna -1 se a pilha estiver cheia, senão retorna 0.
**/
int insereElementoNaPilhaHtml(pilhaHtml *pilha, Elemts tagName);

/**
*	Insere os dados em um elemento e o insere na lista.
*
*	@param lista* listaHtml - lista onde o elemento será inserido.
*	@param id char[] - adiciona o atributo id do elemento.
*	@param className char[] - adiciona o atributo class do elemento.
*	@param attrs char[] - adiciona o todos os atributos específicos do elemento.
*	@param innerHtml char[] - adiciona o conteúdo puro do elemento.
*	@param tagName Elemts - identifica o elemento que será adicionado.
*	@param pilha pilhaHtml* - pilha onde o elemento será adicionado.
*	@return int - retorna -1 se houver um erro na função, senão retorna 0.
**/
int insereElementoNaListaHtml(listaHtml *lista, char id[], char className[], char attrs[], char innerHtml[], Elemts tagName, pilhaHtml *pilha);

/**
*	Verifica se a pilha está vazia.
*
*	@param pilha pilhaHtml* - pilha a ser verificada.
*	@return int - retorna 1 se a pilha estiver vazia, senão retorna 0.
**/
int pilhaHtmlVazia(pilhaHtml *pilha);

/**
*	Remove o elemento da pilha.
*
*	@param pilha pilhaHtml* - pilha onde está o elemento a ser removido.
*	@param lista listaHtml* - lista onde está o elemento a ser removido da pilha.
**/
void removeElementoDaPilhaHtml(pilhaHtml *pilha, listaHtml *lista);

/**
*	Remove o elemento da lista.
*
*	@param lista listaHtml* - lista onde está o elemento a ser removido.
*	@param pos int - posição do elemento na lista.
*	@return int - retorna -1 se houver algum problema, senão retorna 0.
**/
int removeElementoDaListaHtml(listaHtml *lista, int pos);

/**
*	Exclui a pilha.
*
*	@param pilha pilhaHtml* - pilha que será excluída.
**/
void excluirPilhaHtml(pilhaHtml *pilha);

/**
*	Exclui a lista.
*
*	@param lista listaHtml* - lista que será excluída.
**/
void excluirListaHtml(listaHtml *lista);

/**
*	Copia elemento do tipo html de uma string.
*
*	@param content char[] - conteúdo da string.
*	@param i int* - posição inicial da string a ser copiada.
*	@return char* - retorna o elemento copiado.
**/
char *copiaTag(char content[], int *i);

/**
*	Analisador HTML.
*
*	@param content char[] - conteúdo html em formato de uma string.
*	@param url char[] - endereço deste conteúdo.
*	@return listaHtml* - retorna a lista criada.
**/
listaHtml *htmlParser(char content[], char url[]);

#endif // _HTML_H_
