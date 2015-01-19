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
#include <tchar.h>
#include <windows.h>
#include "html.h"
#include "render.h"
#include "renderInt.h"
#include "typeparser.h"

LRESULT WINAPI Render::OnPaint(){
	HDC			hdc;
	PAINTSTRUCT ps;
	RECT		rect;
	char		*text = "Hello World!";

	HANDLE		hOldFont;
	HFONT		hFont;

	hdc = BeginPaint(m_hWnd, &ps);

	hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
	hOldFont = SelectObject(hdc, hFont);

	GetClientRect(m_hWnd, &rect);

	ExtTextOut(hdc, 10, 10, ETO_OPAQUE, &rect, text, lstrlen(text), 0);

	SelectObject(hdc, hOldFont);

	EndPaint(m_hWnd, &ps);

	return 0;
}

LRESULT WINAPI RenderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	Render *ptv = (Render *)GetWindowLong(hwnd, 0);

	switch(msg){
	// First message received by any window - make a new Render object
	// and store pointer to it in our extra-window-bytes
	case WM_NCCREATE:

		if((ptv = new Render(hwnd)) == 0)
			return FALSE;

		SetWindowLong(hwnd, 0, (LONG)ptv);
		return TRUE;

	// Last message received by any window - delete the Render object
	case WM_NCDESTROY:

		delete ptv;
		return 0;

	// Draw contents of Render whenever window needs updating
	case WM_PAINT:
		return ptv->OnPaint();

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL InitRender(){
    WNDCLASSEX	wcx;

	//Window class for the main application parent window
	wcx.cbSize			= sizeof(wcx);
	wcx.style			= 0;
	wcx.lpfnWndProc		= RenderWndProc;
	wcx.cbClsExtra		= 0;
	wcx.cbWndExtra		= sizeof(Render *);
	wcx.hInstance		= GetModuleHandle(0);
	wcx.hIcon			= 0;
	wcx.hCursor			= LoadCursor (NULL, IDC_IBEAM);
	wcx.hbrBackground	= (HBRUSH)0;		//NO FLICKERING FOR US!!
	wcx.lpszMenuName	= 0;
	wcx.lpszClassName	= RENDERCLASS;
	wcx.hIconSm			= 0;

	return RegisterClassEx(&wcx) ? TRUE : FALSE;
}

HWND CreateRender(HWND hwndParent){
	return CreateWindowEx(WS_EX_CLIENTEDGE,
		RENDERCLASS, "", WS_VSCROLL |WS_HSCROLL | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwndParent, 0, GetModuleHandle(0), 0);
}

DWORD WINAPI RenderThread(void* params){
    struct params* param = (struct params*)params;

    param->hwndRender = CreateRender(param->hwnd);

    fflush(stdout);
    Sleep(1000);

    return 0;
}
