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

#include "bonus.h"

char doctp[16] = {"!doctype"};
char extra[][16] = {"!--", "![CDATA["};
char elemts[][16] = {"html", "head", "base", "link", "meta", "noscript", "script", "style", "template", "title", "body", "a", "abbr", "address", "area", "article", "aside", "audio", "b", "p"};

typedef struct _htr{
    char title[4096], *content, *favicon;
} htr;

typedef struct _htp{
    char *stag, cont[5*BUFKB], *etag;
    struct _htp *ant, **prox;
} htp;

typedef struct{
    htp start;
} Fhtp;

typedef enum _Elemts{
    HTML,
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

#endif // _HTML_H_
