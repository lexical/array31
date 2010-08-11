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
