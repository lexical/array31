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

#include "aarraylex.h"
#include "acandidatewindows.h"
#include <Windows.h>
#include <sstream>

AArrayLex::AArrayLex( wstring primary, wstring secondary, wstring simple, bool useSimpleCandidate, int nkey )
	:state( 0 ), root( 0 ), backable( false ), parent( 0 ), simpleCodeReady( false ), nKey( 31 )
{
	AArrayLex::useSimpleCandidate = useSimpleCandidate;

	nKey = nkey;

	// get module path

	wchar_t path[ 256 ];
	int szPath;
	szPath = GetModuleFileNameW( ACandidateWindow::hInstance, path, 255 );

	int l( 0 );
	for( int i = 0; i < szPath; ++i )
		if( path[i] == L'\\' )
			l = i;

	path[ l + 1 ] = 0;

	mPath = path;

	// init root
	root = new struct Node;
	state = root;
	root->desc.resize( nKey );
	//root->desc = new struct Node * [ nKey ];

	for( int i = 0; i < nkey; ++i )
		root->desc[ i ] = 0;

	if( primary != L"" )	tinit( mPath + primary, false );
	if( secondary != L"" )	tinit( mPath + secondary, true );
	if( simple != L"" )
	{
		sinit( mPath + simple );
		simpleCodeReady = true;
	}
}

AArrayLex::~AArrayLex()
{
	if( root )
		tdelete( root );
}

int AArrayLex::put( int asciikey )
{
	// reset
	if( asciikey == 0 )
	{
		state = root;
		parent = 0;
		backable = false;
		return ST_RESET;
	}

	struct Node * sdesc = state->desc[ getStrokeNumFromChar( asciikey ) ];
	if( sdesc )
	{
		parent = state;
		backable = true;
		state = sdesc;
        return ST_VALID;
	}
	else
	{
		//state = root;
		return ST_RESET;
	}
}

void AArrayLex::back()
{
	if( backable )
	{
		state = parent;
		backable = false;
	}
}

vector<wchar_t> AArrayLex::getCandidates( wchar_t ascii )
{
    vector<wchar_t> candidates = state->characters;

	if( simpleCodeReady && parent == root && ascii != 0 && simpleCode.find( ascii ) != simpleCode.end() )
		for( int i = state->characters.size(); i < simpleCode[ ascii ].size(); ++i )
			candidates.push_back( simpleCode[ ascii ].at( i ) );

	else if( useSimpleCandidate )
		for( int i = state->characters.size(); i < 10; ++i )
			candidates.push_back( getOneChild( state, i + 1 ) );

	return candidates;
}

wstring AArrayLex::getCandidateString( wchar_t ascii )
{
	wstring cstring;

    vector<wchar_t> & canvec = getCandidates( ascii );
	for( int i = 1; i <= canvec.size(); ++i )
	{
        std::wstringstream ss;

		if( i == 10 )
			ss << L"0" ;
		else
        	ss << i;

		if( canvec.at( i - 1 ) == 0 )
			cstring += ss.str() + L"□ ";
		else
            cstring += ss.str() + canvec.at( i - 1 ) + L" ";
	}

	return cstring;
}

wchar_t AArrayLex::getOneChild( struct Node * node, int n )
{
	vector<wchar_t> c;
	if( n == 1 )
	{
		c.push_back( 'Q' ); c.push_back( 'A' ); c.push_back( 'Z' );
	}
	else if ( n == 2 )
	{
		c.push_back( 'W' ); c.push_back( 'S' ); c.push_back( 'X' );
	}
	else if ( n == 3 )
	{
		c.push_back( 'E' ); c.push_back( 'D' ); c.push_back( 'C' );
	}
	else if ( n == 4 )
	{
		c.push_back( 'R' ); c.push_back( 'F' ); c.push_back( 'V' );
	}
	else if ( n == 5 )
	{
		c.push_back( 'T' ); c.push_back( 'G' ); c.push_back( 'B' );
	}
	else if ( n == 6 )
	{
		c.push_back( 'Y' ); c.push_back( 'H' ); c.push_back( 'N' );
	}
	else if ( n == 7 )
	{
		c.push_back( 'U' ); c.push_back( 'J' ); c.push_back( 'M' );
	}
	else if ( n == 8 )
	{
		c.push_back( 'I' ); c.push_back( 'K' ); c.push_back( ',' );
	}
	else if ( n == 9 )
	{
		c.push_back( 'O' ); c.push_back( 'L' ); c.push_back( '.' );
	}
	else if ( n == 0 )
	{
		c.push_back( 'P' ); c.push_back( ';' ); c.push_back( '/' );
	}


	for( vector<wchar_t>::iterator i = c.begin(); i != c.end(); ++i )
	{
		struct Node * sdesc = node->desc[ getStrokeNumFromChar( * i ) ];
		if( sdesc && sdesc->characters.size() != 0 )
			return sdesc->characters[0];
	}
	return 0;
}

void AArrayLex::tinit( wstring & fi, bool ext )
{
	FILE * fs;
	_wfopen_s( & fs, fi.c_str(), L"r" );
	ifstream f( fs );
	if( ! f )
	{
	}

	put( 0 );

	char s[256];
	wchar_t ws[256];
	int szws;
	while( f.getline( s, 255 ) )
	{
		// szws include null terminator
        szws = MultiByteToWideChar( CP_UTF8, 0, s, -1, ws, 255 );

		wstringstream ss;
		ss << ws;
		wstring stroke; wchar_t character;
        ss >> character >> stroke;

		if( ext )
			stroke += L"I";

		tmake( root, 0, stroke, character );
	}
}

void AArrayLex::sinit( wstring & fi )
{
	FILE * fs;
	_wfopen_s( & fs, fi.c_str(), L"r" );
	ifstream f( fs );
	if( ! f )
	{
	}

	char s[256];
	wchar_t ws[256];
	int szws;
	while( f.getline( s, 255 ) )
	{
		// szws include null terminator
        szws = MultiByteToWideChar( CP_UTF8, 0, s, -1, ws, 255 );

		wstringstream ss;
		ss << ws;
		wstring codestr; wchar_t code;
        ss >> code >> codestr;

		simpleCode.insert( make_pair( code, codestr ) );
	}
}

void AArrayLex::tmake( struct Node * node, int strokeIndex, const wstring & stroke, const wchar_t & ch )
{
	if( strokeIndex == stroke.size() )
	{
		node->characters.push_back( ch );
		return;
	}

	int strokeNum = getStrokeNumFromChar( stroke.at( strokeIndex ) );

	if( node->desc[ strokeNum ] == 0 )
	{
		struct Node * child = new struct Node;
        //child->desc = new struct Node * [ nKey ];
        child->desc.resize( nKey );
		for( int i = 0; i < nKey; ++i )
			child->desc[ i ] = 0;

		node->desc[ strokeNum ] = child;
	}

	tmake( node->desc[ strokeNum ], strokeIndex + 1, stroke, ch );
}

void AArrayLex::tdelete( struct Node * node )
{
	if( node == 0 )
		node = root;

	for( int i = 0; i < nKey; ++i )
		if( node->desc[ i ] != 0 )
		{
			tdelete( node->desc[ i ] );
			delete node->desc[ i ];
		}

	//delete node->desc;
}

int AArrayLex::getStrokeNumFromChar( wchar_t c )
{
	if( nKey == 10 )
	{
		return c - 48;
	}

	else
	{
		if( c == 44 ) // `
			return 0;
		else if( c == 46 ) // .
			return 1;
		else if( c == 47 ) // /
			return 2;
		else if( c == 59 ) // ;
			return 3;
		else if( c >= 65 && c <= 90 ) // A-Z
			return c - 61;
		else if( c == 96 ) // `
			return 30;
		else if( c >= 48 && c <= 57 ) // 0-9
			return c - 17;
		else
			return -1;
	}
}

wchar_t AArrayLex::getStrokeCharFromNum( int n )
{
	if( nKey == 10 )
	{
		return n - 48;
	}

	else
	{
		if( n == 0 )
			return 44;
		else if( n == 1 )
			return 46;
	    else if( n == 2 )
			return 47;
	    else if( n == 3 )
			return 59;
	    else if( n > 3 && n < 30 ) // A-Z
			return n + 61;
		else if( n == 40 ) // `
			return 96;
		else if( n >= 31 && n <= 40 ) // 0-9
			return n + 17;
		else
			return -1;
	}
}
