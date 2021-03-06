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
	BCM Revox Engine -> Ano: 2014|Tipo: WebEngine
*/

#ifndef _RENDERINT_H_
#define _RENDERINT_H_

#ifdef __cplusplus
extern "C"{
#endif
class Render{
    public:

	Render(HWND hwnd) : m_hWnd(hwnd) {}
	HWND	m_hWnd;
	int x;

	LRESULT WINAPI OnPaint();
};

#ifdef __cplusplus
}
#endif

#endif // _RENDERINT_H_
