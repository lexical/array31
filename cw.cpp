#include "acandidatewindows.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
	MSG msg;

	ACandidateWindow::hInstance = hInstance;
    ACandidateWindow aw;
	aw.composition = L"1^2v3-4^";
	aw.candidates = L"1人 2入 3八 4做 5他 6進 7你 8坐 9作 0個";

	aw.show();
	aw.update();

	while( GetMessage( & msg, NULL, 0, 0 ) )
	{
		TranslateMessage( & msg );
		DispatchMessage( & msg );
	}
	return msg.wParam;
}
