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

#ifndef AARRAYLEX_H
#define AARRAYLEX_H

#include <string>
#include <vector>
#include <map>
//#include "aarrayts.h"

using namespace std;

class AArrayTextService;

struct Node
{
	vector<wchar_t> characters;
	vector<struct Node *> desc;
};


class AArrayLex
{
public:
	AArrayLex( wstring primary = L"", wstring secondary = L"", wstring simple = L"", bool useSimpleCandidate = true, int nkey = 31 );
	~AArrayLex();

	enum { ST_RESET, ST_VALID };

	int put( int );
    vector<wchar_t> getCandidates( wchar_t ascii = 0 );
	wstring getCandidateString( wchar_t ascii = 0 );

	bool backable;
	void back();

	bool simpleCodeReady;

private:
	std::wstring mPath;
	wchar_t getOneChild( struct Node * node, int n );

	int getStrokeNumFromChar( wchar_t c );
	wchar_t getStrokeCharFromNum( int n );

	struct Node * root;
	struct Node * state;
    struct Node * parent;
	void tinit( wstring & f, bool ext );
	void tmake( struct Node * node, int strokeIndex, const wstring & stroke, const wchar_t & ch );
	void tdelete( struct Node * node = 0 );

	map<wchar_t, wstring> simpleCode;
	void sinit( wstring & f );

    bool useSimpleCandidate;

	int nKey;
};

#endif