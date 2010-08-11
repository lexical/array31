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