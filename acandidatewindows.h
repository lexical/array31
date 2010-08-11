/*
 *   Copyright 2010 Keng-Yu Lin <kengyu@lexical.tw>
 *
 *   This file is part of Array31.
 *
 *   Array31 is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Array31 is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Array31.  If not, see <http://www.gnu.org/licenses/>.
 */

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