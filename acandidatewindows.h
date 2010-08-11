#ifndef ACandidateWindow_H
#define ACandidateWindow_H
#define UNICODE

#include <Windows.h>
#include <string>
#include <sstream>
#include <fstream>

/*
void TRACEX( std::string s );
void TRACEW( std::wstring s );
*/

class ACandidateWindow
{
public:
    ACandidateWindow();
	~ACandidateWindow();

	BOOL show();
	BOOL update();
	BOOL redraw();
	BOOL hide();
	BOOL move( int x, int y );

	static LRESULT CALLBACK WinProc( HWND, UINT, WPARAM, LPARAM );
    static HINSTANCE hInstance;
   	static std::wstring composition;
    static std::wstring candidates;

private:
    HWND hwnd;
	static HWND appHwnd;
    WNDCLASS wc;

	static int szX, szY;
	static int avgCharWidth, maxCharWidth;
	static HICON hicon;
	static HFONT hfont;
};

#endif