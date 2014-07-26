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

#include <windef.h>
#include "manifest.h"
#include "parser.h"

typedef struct _Type{
    char *url, *content;
    HWND hwnd;
} Type;

typedef struct _typec{
    char *plain;
    status manifst;
    struct _parser parser;
} typec;

typedef enum _type{
    THTML,
    MANIFEST,
    PLAIN,
    DOWNLOAD,
    OCTETSTREAM
} type;

/**
*   Procura o tipo do arquivo, no seu conteúdo.
*
*	@param content char - conteúdo do arquivo a ser verificado.
*	@return type - retorna o tipo de arquivo encontrado.
**/
type TypeBuster(char content[]);

/**
*   Converte o conteúdo recebido em um formato legível.
*   Mode igual a 0 -> Interface Gráfica (GUI)
*   Mode igual a 1 -> Interface em Texto (TUI)
*
*	@param tp1 Type - conteúdo do arquivo a ser analisado.
*	@param mode int - o modo que o arquivo será analisado.
*	@return typec - retorna o arquivo analisado.
**/
typec InitTypeParser(Type tp1, int mode);

/**
*   Obtém o Cabeçalho do conteúdo recebido pela rede.
*
*	@param content char* - todo o conteúdo do arquivo.
*	@return char* - retorna o cabeçalho do arquivo.
**/
char *TagHeader(char *content);

/**
*   Obtém o corpo do conteúdo recebido pela rede.
*
*	@param content char* - todo o conteúdo do arquivo.
*	@return char* - retorna o corpo do arquivo.
**/
char *TagBody(char *content);

#endif // _TYPEPARSER_H_
