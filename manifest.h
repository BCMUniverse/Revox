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
#ifndef _MANIFEST_H_
#define _MANIFEST_H_

typedef enum _CacheStatus{
    UNCACHED,
    IDLE,
    CHECKING,
    DOWNLOADING,
    UPDATEREADY,
    OBSOLETE
} status;
/**
*	Verifica se o arquivo está armazenado no cache.
*
*	@param url char - endereço do arquivo a ser verificado.
*	@return char - conteúdo do arquivo armazenado.
**/
char *IsCached(char url[]);
/**
*	Copia linha por linha pulando as linhas de comentários ou espaços.
*
*	@param content char - conteudo a ser dividido em linhas.
*	@param i int - posição atual da última linha.
*	@return char - devolve a linha.
**/
char *CopyManifst(char content[], int *i);
/**
*   Adiciona arquivos no cache.
*
*	@param content char - conteúdo do arquivo a ser armazenado.
*	@param address char - endereço do arquivo a ser armazenado.
*	@return status - retorna o estado do arquivo.
**/
status addCache(char content[], char address[]);
/**
*	Inicializa o armazenamento no cache.
*
*	@param content char - conteúdo do arquivo manifest.
*	@param url1 char - enderço do arquivo manifest.
*	@return status - retorna o estado do manifest.
**/
status InitManifest(char content[], char url1[]);

#endif // _MANIFEST_H_
