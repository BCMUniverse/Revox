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

	BCM Revox Engine v0.1
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
#ifndef BONUS_H_
#define BONUS_H_

//Buffer de saída de 512KB
#define BUF32KB 524288
//Tamanho do Vetor p.v
#define VKB 256

//Pilha
typedef struct _pilha{
    char v[VKB];
    int topo, ind[VKB];
}pilha;
void InitPilha(pilha *p);
void InsrtPilha(pilha *p, char novo);
char RemovePilha(pilha *p);

void limpaVetor(char *vet, int tam);
static size_t strnlen(const char *s, size_t max);
int inet_pton4(const char *src, char *dst);
int inet_pton6(const char *src, char *dst);
int inet_pton(int af, const char *src, char *dst);

#endif // BONUS_H_
