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
#ifndef _TYPEPARSER_H_
#define _TYPEPARSER_H_

typedef struct _Type{
    char *url, *content;
} Type;

typedef enum _type{
    THTML,
    MANIFEST,
    PLAIN
} type;

/**
*   Procura o tipo de conteúdo de um arquivo.
**/
char *TypeBuster(char content[], type typ1);

/**
*   Converte o conteúdo recebido em um formato legivel
*   Mode igual a 0 -> Interface Gráfica (GUI)
*   Mode igual a 1 -> Interface em Texo (TUI)
**/
char *InitTypeParser(Type tp1, int mode);

/**
*   Obtém o Cabeçalho do conteúdo recebido pela rede.
**/
char *TagHeader(char *content);

/**
*   Obtem o corpo do conetúdo recebido pela rede.
**/
char *TagBody(char *content);

#endif // _TYPEPARSER_H_
