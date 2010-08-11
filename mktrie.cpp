#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <Windows.h>

using namespace std;

/*
,(44) .(46) /(47) ;(59)
A-Z(65-90)
0-9(48-57)
*/

struct Node
{
	vector<wchar_t> characters;
	struct Node * desc[ 40 ];
};


inline int getStokeNum( char c )
{
	if( c == 44 )
		return 0;
	else if( c == 46 )
		return 1;
	else if( c == 47 )
		return 2;
	else if( c == 59 )
		return 3;
	else if( c >= 65 && c <= 90 )
		return c - 61;
	else if( c >= 48 && c <= 57 )
		return c - 18;
	else
		return -1;
}

inline char getStrokeChar( wchar_t n )
{
	if( n == 0 )
		return 44;
	else if( n == 1 )
		return 46;
    else if( n == 2 )
		return 47;
    else if( n == 3 )
		return 59;
    else if( n > 3 && n < 30 )
		return n + 61;
	else if( n >= 30 && n < 40 )
		return n + 18;
	else
		return -1;
}

void makeBranch( struct Node * node, int strokeIndex, const wstring & stroke, const short & ch )
{
	wprintf( L"\tstrokeidx = %d, stroke = %s, char = %d\n", strokeIndex, stroke.c_str(), ch );

	if( strokeIndex == stroke.size() )
	{
		node->characters.push_back( ch );
		return;
	}

	int strokeNum = getStokeNum( stroke.at( strokeIndex ) );

	if( node->desc[ strokeNum ] == 0 )
	{
		struct Node * child = new struct Node;
		for( int i = 0; i < 40; ++i )
			child->desc[ i ] = 0;

		node->desc[ strokeNum ] = child;
	}

	makeBranch( node->desc[ strokeNum ], strokeIndex + 1, stroke, ch );
}

FILE * fs;

void traverse( struct Node * node, string stroke, int layer )
{
    printf( "layer = %d\t%s\n", layer, stroke.c_str() );
	
	for( int i = 0; i < 40; ++i )
		if( node->desc[ i ] != 0 )
		{
			//stroke.push_back( getStrokeChar( i ) );
			traverse( node->desc[ i ], stroke + char( getStrokeChar( i ) ), layer + 1 );
		}

	for( vector<wchar_t>::iterator i = node->characters.begin(); i != node->characters.end(); ++i )
	{
		wchar_t wc[2]; wc[0] = * i; wc[1] = 0;
		char ss[ 128 ]; ss[127] = 0;
		BOOL t; LPCSTR k;
        WideCharToMultiByte( CP_UTF8, 0, wc, -1, ss, 127, NULL, NULL );
		fprintf( fs, "%d\t%s\t%s\n", *i, stroke.c_str(), ss );
	}
}

void deletetree( struct Node * node )
{
	for( int i = 0; i < 40; ++i )
		if( node->desc[ i ] != 0 )
		{
			deletetree( node->desc[ i ] );
			delete node->desc[ i ];
		}
}

struct Node * mktrie( char * file )
{
	ifstream f( file );

	// init root
	struct Node * root = new struct Node;
	for( int i = 0; i < 40; ++i )
		root->desc[ i ] = 0;

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
		//ss >> stroke >> character;
        ss >> character >> stroke;

		wprintf( L"%s\t%d\n", stroke.c_str(), character );
		makeBranch( root, 0, stroke, character );
	}


	return root;
}


int main( int argc, char * argv[] )
{
	struct Node * root = mktrie( argv[1] );

	//fopen_s( & fs, "t.txt", "r" );

	//traverse( root, "", 0 );

	deletetree( root );
	//fclose( fs );
	return 0;
}
