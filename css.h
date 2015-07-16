/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine � Software Livre; voc� pode redistribui-lo e/ou
	modific�-lo dentro dos termos da Licen�a P�blica Geral GNU como
	publicada pela Funda��o do Software Livre (FSF); na vers�o 3 da Licen�a.
	Este programa � distribu�do na esperan�a que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUA��O a
	qualquer MERCADO ou APLICA��O EM PARTICULAR. Veja a Licen�a P�blica Geral
	GNU para maiores detalhes.
	Voc� deve ter recebido uma c�pia da Licen�a P�blica Geral GNU junto com
	este programa, se n�o, escreva para a Funda��o do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.2
	BCM Revox Engine -> Ano: 2015|Tipo: WebEngine
*/

#include "html.h"

#ifndef _CSS_H_
#define _CSS_H_

typedef enum _seltp{
    DEFAULT,
    ACTIVE,
    FOCUS,
    HOVER,
    VISITED
}selTipo;

typedef enum _modoCss{
    DEFAULT_MODE,
    EXTERNAL,
    INTERNAL,
    INLINE
}modCSS;

typedef struct _elemCss{
    char *prop, *valor;
    selTipo tipo;
    modCSS modo;
    struct _elemCss *ant, *prox;
}elemCSS;

typedef struct _listaCss{
    char *id, *classe;
    Elemts tag;
    elemCSS *inicio, *fim;
}listaCSS;

typedef struct _hashCss{
    listaCSS *vet;
    int tam;
}hashCSS;

#endif // _CSS_H_
