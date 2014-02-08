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
#ifndef _HTML_H_
#define _HTML_H_

#include "http.h"

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

//Nesta etrutura é um token onde se obten os atributos, id e class do elemento, que será usado para cria o layout da página
typedef struct _htp{
    char stag[16], cont[5*BUFKB], etag[16], id[1024], ClassName[1024], style[4096], attrs[4096];
    struct _htp *ant, *prox;
    Elemts TagID;
} htp;

typedef struct _Fhtp{
    htp *start, *end1;
    int tam;
} Fhtp;

//Cria um token
Fhtp *aloca();
/*
    Insere os dados no token numa lista ligada
    Mode igual a 0 -> Cria o Token numa lista vazia
    Mode igual a 1 -> Cria o Token numa lista que tenha conteúdo
    Mode igual a 2 -> Cria o Token antes de um elemento numa lista
*/
void CreateToken(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid, int mode, int pos);
void CreateTokenEmpty(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid);
void CreateTokenNormal(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid);
void CreateTokenInCurse(Fhtp *f, char stag[], char etag[], char cont[], char ClassName[], char id[], char style[], char attrs[], Elemts tagid, int pos);
//Remove Token
void RemvToken(Fhtp *f, int pos);
//Destroi o Token
void Delete(Fhtp *f);

#endif // _HTML_H_
