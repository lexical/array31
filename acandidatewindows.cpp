#include "acandidatewindows.h"

HINSTANCE ACandidateWindow::hInstance = 0;
HICON ACandidateWindow::hicon = 0;
HFONT ACandidateWindow::hfont = 0;
std::wstring ACandidateWindow::composition;
std::wstring ACandidateWindow::candidates;

HWND ACandidateWindow::appHwnd = 0;
int ACandidateWindow::szX = 0;
int ACandidateWindow::szY = 0;
int ACandidateWindow::avgCharWidth = 0;
int ACandidateWindow::maxCharWidth = 0;

/*
void TRACEX( std::string s )
{
	//std::ofstream f( "c:\\t.txt", std::ios::app );
	//f << s;
}

void TRACEW( std::wstring s )
{
	std::wofstream f( "c:\\users\\lexical\\array31w.txt", std::ios::app );
	f << s;
}
*/

ACandidateWindow::ACandidateWindow()
	: hwnd( 0 )
{
	// load icon
	hicon = LoadIcon( hInstance, MAKEINTRESOURCE( 100 ) );

	// choose font
	hfont = CreateFont( 0, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, CHINESEBIG5_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, L"微軟正黑體" );
    //hfont = CreateFont( 0, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, CHINESEBIG5_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, L"細明體" );

    TEXTMETRIC tm;
	HDC hdc = GetDC( hwnd );
    SelectObject( hdc, hfont );
    GetTextMetrics( hdc, & tm ) ;
    avgCharWidth = tm.tmAveCharWidth;
    maxCharWidth = tm.tmMaxCharWidth;
	//std::stringstream ss; ss << tm.tmAveCharWidth << "\t" << tm.tmMaxCharWidth << std::endl;
	//TRACEX( ss.str() );
	ReleaseDC( hwnd, hdc );

	szX = 40 + 32 * avgCharWidth + 10 * maxCharWidth; //550;
	szY = 32;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = ACandidateWindow::WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    //wc.hbrBackground = ( HBRUSH ) GetStockObject( LTGRAY_BRUSH ); // background color
    //wc.hbrBackground = CreateSolidBrush( RGB( 0, 176, 142 ) ); // background color
    wc.hbrBackground = CreateSolidBrush( RGB( 0, 150, 255 ) ); // background color

    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT( "ACandidateWindow" );

    RegisterClass( & wc );

	HWND desktopHwnd = GetDesktopWindow();
	RECT desktopRect;
	GetWindowRect( desktopHwnd, & desktopRect );

	hwnd = CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_TOPMOST, TEXT( "ACandidateWindow" ), TEXT( "caption" ), WS_POPUP, 100, desktopRect.bottom - 150, szX, szY, NULL, NULL, hInstance, NULL );
}

ACandidateWindow::~ACandidateWindow()
{
	DestroyWindow( hwnd );
	UnregisterClass( TEXT( "ACandidateWindow" ), hInstance );
}

BOOL ACandidateWindow::show()
{
    return ShowWindow( hwnd, SW_SHOWNA );
}

BOOL ACandidateWindow::update()
{
	//TRACEW( composition );
	RECT rect;
    GetClientRect( hwnd, & rect );
	InvalidateRect( hwnd, & rect, FALSE );
    return UpdateWindow( hwnd );
}

BOOL ACandidateWindow::redraw()
{
	return RedrawWindow( hwnd, NULL, NULL, RDW_ERASENOW | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_INVALIDATE );
}

BOOL ACandidateWindow::hide()
{
	return ShowWindow( hwnd, SW_HIDE );
}

BOOL ACandidateWindow::move( int x, int y )
{
	return MoveWindow( hwnd, x, y, szX, szY, TRUE );
}

LRESULT CALLBACK ACandidateWindow::WinProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	HDC hdc;
	PAINTSTRUCT ps;
	ps.fErase = TRUE;
	RECT rect;
	POINT pos;
    static POINTS cp;


	switch( msg )
	{
	case WM_CREATE:
		return 0;

	case WM_PAINT:
		hdc = BeginPaint( hwnd, & ps );
        //SelectObject( hdc, ( HBRUSH )GetStockObject( GRAY_BRUSH ) );
        SetTextColor( hdc, RGB( 255, 255, 255 ) );
        SetBkMode( hdc, TRANSPARENT );
        SelectObject( hdc, hfont );
		GetClientRect( hwnd, & rect );

		DrawIcon( hdc, 0, 0, hicon );

		rect.left += 40;
		//wchar_t kkk[256];kkk[255]=0;
		//GetTextFaceW( hdc, 255, kkk );
        //DrawTextW( hdc, kkk, -1, & rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER );
		DrawText( hdc, composition.c_str(), -1, & rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER );

		rect.left += avgCharWidth * 11;
        DrawText( hdc, candidates.c_str(), -1, & rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER );

		EndPaint( hwnd, & ps );
		return 0;
/*
	case WM_RBUTTONDOWN:
        PostQuitMessage( 0 );
		return 0;
*/
	case WM_LBUTTONDOWN:
        SetCapture( hwnd );
        cp = MAKEPOINTS( lParam );
		if( appHwnd )
			SetFocus( appHwnd );
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		//TRACEX( "WM_MOUSEMOVE\n" );
        if( wParam == MK_LBUTTON )
		{
			GetCursorPos( & pos );
            MoveWindow( hwnd, pos.x - cp.x, pos.y - cp.y, szX, szY, TRUE );
		}
		if( appHwnd )
			SetFocus( appHwnd );
		return 0;

	case WM_SETFOCUS:
		appHwnd = reinterpret_cast<HWND>( wParam );
		return 0;

	case WM_DESTROY:
        PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hwnd, msg, wParam, lParam );
}
