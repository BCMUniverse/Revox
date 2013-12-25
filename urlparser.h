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
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
#ifndef _URLPARSER_H_
#define _URLPARSER_H_

typedef struct _url{
    int port;
    char host[1025], url_path[1025], prtcol[16];
}url;

typedef enum _uports{
    HTTP,
    HTTPS,
    FTP,    //Revisar na versão v0.3
    TELNET, //Revisar na versão v0.4
    GOPHER, //EmBreve
    FILES,  //Revisar na versão v0.3 ou superior
    MAILTO,
    NEWS,
    NNTP,
    WAIS,
    PROSPERO
}uports;

//Analisa URLs
url UrlParser(char host[]);
/*
    Conecta as urls nos seus protocolos e recbe a resposta
    Mode igual a 0 -> Interface Gráfica (GUI)
    Mode igual a 1 -> Interface em Texo (TUI)
*/
char *UrlConnect(char *host, int mode);

#endif // _URLPARSER_H_
