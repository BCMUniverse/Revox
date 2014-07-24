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

#include "revox.h"

extern char elemts[][16];

typedef struct _htr{
    char title[4096], *content, *favicon;
} htr;

typedef enum _Elemts{
    DOCTYPE = 200,
    COMMENTS = 201,
    CDATA = 202,
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

typedef struct _elemntHtml{
    char *id, *className, *attrs, *innerHtml;
    Elemts tagName;
    int quantFilhos;
    struct _elemntHtml *ant, *prox;
} elemntHtml;

typedef struct _listaHtml{
    elemntHtml *inicio, *fim;
    int tam;
} listaHtml;

typedef struct _pilhaHtml{
    Elemts tagName[BUFKB];
    int filhos[BUFKB], topo;
} pilhaHtml;

//Aloca a lista html
void alocaListaHtml(listaHtml *lista);

// cria a pilha
pilhaHtml *criaPilhaHtml();

//insere o elemento na pilha
int insereElementoNaPilhaHtml(pilhaHtml *pilha, Elemts tagName);

// Insere os dados em um elemento e o insere na lista
int insereElementoNaListaHtml(listaHtml *lista, char id[], char className[], char attrs[], char innerHtml[], Elemts tagName, pilhaHtml *pilha);

//verifica se a pilha está vazia
int pilhaHtmlVazia(pilhaHtml *pilha);

// remove o elemento da pilha
void removeElementoDaPilhaHtml(pilhaHtml *pilha, listaHtml *lista);

//remove o elemento da lista
int removeElementoDaListaHtml(listaHtml *lista, int pos);

//exclue a pilha
void excluirPilhaHtml(pilhaHtml *pilha);

//exclue a lista
void excluirListaHtml(listaHtml *lista);

#endif // _HTML_H_
