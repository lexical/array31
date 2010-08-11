#include "aarrayts.h"
#include "aclassfactory.h"
#include <stdio.h>
#include <sstream>

GUID AArrayTextService::gLangProfile = CLSID_ARRAY_31;
TF_PRESERVEDKEY VK_A = { 0x41, TF_MOD_ALT };

AArrayTextService::AArrayTextService()
	: m_cRef( 0 ), ThreadFocusSinkCookie( -1 ), ThreadMgrEventSinkCookie( -1 ), pThreadMgr( 0 ), state( ST_INIT ), stateSymbolReady( false ), simpleCodeKey( 0 ), lexicon30( 0 ), lexicon40( 0 ), lexicon123( 0 )
{
	if( CoCreateInstance( CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, ( void ** ) & pProfile ) != S_OK )
	{
	}

	// get module path
	wchar_t path[ 256 ];
	int szPath;
	szPath = GetModuleFileNameW( ACandidateWindow::hInstance, path, 255 );

	int l( 0 );
	for( int i = 0; i < szPath; ++i )
		if( path[i] == '\\' )
			l = i;

	path[ l + 1 ] = 0;
	m_path = path;

}

AArrayTextService::~AArrayTextService()
{
	if( lexicon30 )
		delete lexicon30;

	if( lexicon40 )
		delete lexicon40;

	if( lexicon123 )
		delete lexicon123;

    gArrayTS = 0;
	if( isActive )
		Deactivate();
}

HRESULT AArrayTextService::QueryInterface( REFIID riid, void ** ppInterface )
{
	if( riid == IID_IUnknown )
		* ppInterface = static_cast<IUnknown *>( static_cast<ITfTextInputProcessor *>( this ) );

	else if( riid == IID_ITfTextInputProcessor )
		* ppInterface = static_cast<ITfTextInputProcessor *>( this );

	else if( riid == IID_ITfThreadFocusSink )
		* ppInterface = static_cast<ITfThreadFocusSink *>( this );

	else if( riid == IID_ITfKeyEventSink )
		* ppInterface = static_cast<ITfKeyEventSink *>( this );

	else
	{
		* ppInterface = 0;
		return E_NOINTERFACE;
	}

	static_cast<IUnknown *>( * ppInterface )->AddRef();
	return NOERROR;
}

// IUnknown

ULONG AArrayTextService::AddRef()
{
	++m_cRef;
	return m_cRef;
}

ULONG AArrayTextService::Release()
{
    --m_cRef;

	if( m_cRef != 0 )
		return m_cRef;

	else
	{
		InterlockedDecrement( & AClassFactory::m_cObj ); // incremented in AClassFactory::CreateInstance()
		delete this;
		return 0;
	}
}

// ITfTextInputProcessor

HRESULT AArrayTextService::Activate( ITfThreadMgr * pThreadMgr, TfClientId tfClientId )
{
	isActive = true;

	AArrayTextService::pThreadMgr = pThreadMgr;
	pThreadMgr->AddRef();
	m_cid = tfClientId;

	// advise sink
    ITfSource * pSource;

	if( pThreadMgr->QueryInterface( IID_ITfSource, ( void ** ) & pSource ) == S_OK )
	{
		HRESULT hr;

        // ThreadFocusSink
		if( ThreadFocusSinkCookie == -1 )
        	if( ( hr = pSource->AdviseSink( IID_ITfThreadFocusSink, (IUnknown*)static_cast<ITfThreadFocusSink *>( this ), & ThreadFocusSinkCookie ) ) != S_OK )
			{}

		pSource->Release();
	}

	// ITfKeyEventSink
    ITfKeystrokeMgr * pKeyMgr = 0;
	if( pThreadMgr->QueryInterface( IID_ITfKeystrokeMgr, ( void ** ) & pKeyMgr ) == S_OK )
	{
		if( pKeyMgr->AdviseKeyEventSink( tfClientId, static_cast<ITfKeyEventSink *>( this ), TRUE ) != S_OK )
		{}

		// register preserved key
		if( pKeyMgr->PreserveKey( tfClientId, GUID_ARRAY_31_PRESERVED_KEY, & VK_A, L"A", 1 ) != S_OK )
		{}

		pKeyMgr->Release();
	}

	// language bar
    ITfLangBarItemMgr * pLangBarItemMgr;
	if( pThreadMgr->QueryInterface( IID_ITfLangBarItemMgr, ( void ** ) & pLangBarItemMgr ) == S_OK )
	{
		// Though new a object here, but delete this will cause access violation in Vista
        keyboardBarItem = new AKeyboardBarItem;
		keyboardBarItem->init( this );
        if( pLangBarItemMgr->AddItem( keyboardBarItem ) != S_OK)
		{
			// TRACEX( "additem failed\n" );
		}
        pLangBarItemMgr->Release();
	}
	else
	{
		//TRACEX( "lanbaritemmgr qi failed\n" );
	}

	cwin.show(); // show in advance.

	// get defaultIME key
	DWORD szBuf = 8;
	wchar_t defaultIME[ 7 ];
	HKEY hk;
	RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Array31", NULL, KEY_ALL_ACCESS, &hk );
	RegQueryValueExW( hk, L"DefaultIME", NULL, NULL, reinterpret_cast<LPBYTE>( defaultIME ), &szBuf );
	RegCloseKey( hk );

	int mode;
	wstringstream ss;
	ss << defaultIME;
	ss >> mode;

	reset( mode );
    keyboardBarItem->OnMenuSelect( mode );

	return NOERROR;
}

HRESULT AArrayTextService::Deactivate()
{
	isActive = false;

	if( lexicon )
	    lexicon->put( 0 );

	// language bar
    ITfLangBarItemMgr * pLangBarItemMgr;
	if( pThreadMgr->QueryInterface( IID_ITfLangBarItemMgr, ( void ** ) & pLangBarItemMgr ) == S_OK )
	{
        pLangBarItemMgr->RemoveItem( keyboardBarItem );
        pLangBarItemMgr->Release();
	}
	else
	{
		//TRACEX( "lanbaritemmgr qi failed\n" );
	}

	// Unadvisesink
    ITfSource * pSource;
	if( pThreadMgr->QueryInterface( IID_ITfSource, ( void ** ) & pSource ) == S_OK )
	{
        if( ThreadMgrEventSinkCookie != -1 ){}

        if( ThreadFocusSinkCookie != -1 ){}
	}
	pSource->Release();

	// ITfKeyEventSink
    ITfKeystrokeMgr * pKeyMgr = 0;
	if( pThreadMgr->QueryInterface( IID_ITfKeystrokeMgr, ( void ** ) & pKeyMgr ) == S_OK )
	{
		if( pKeyMgr->UnadviseKeyEventSink( m_cid ) != S_OK )
		{
		}

		// register preserved key
		if( pKeyMgr->UnpreserveKey( GUID_ARRAY_31_PRESERVED_KEY, & VK_A ) != S_OK )
		{
		}

		pKeyMgr->Release();
	}
	else
	{
	}

	// threadmgr
	if( ! pThreadMgr )
	{
		pThreadMgr->Release();
        pThreadMgr = 0;
	}

	cwin.hide();
	return NOERROR;
}

// ITfThreadFocusSink
HRESULT AArrayTextService::OnSetThreadFocus()
{
	LANGID lid;
	GUID guidProfile;

	if( pProfile->GetActiveLanguageProfile( CLSID_ARRAY_31, & lid, & guidProfile ) == S_OK )
	{
		if( guidProfile == gLangProfile )
			cwin.show();
	}
	else
	{
	}

	return NOERROR;
}

HRESULT AArrayTextService::OnKillThreadFocus()
{
	cwin.hide();
	return NOERROR;
}


// ITfKeyEventSink
HRESULT AArrayTextService::OnSetFocus( BOOL fForeground )
{
	return NOERROR;
}

HRESULT AArrayTextService::OnTestKeyDown( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
	int vKey = static_cast<int>( wParam );
	/* 0-9 = 0x30-0x39 */
	/* A-Z = 0x41-0x5A */
	/* VK_OEM_COMMA:BC VK_OEM_PERIOD:BE VK_OEM_1(;):BA VK_OEM_2(/):BF */
	/* VK_SPACE (20), VK_BACK (08), VK_ESCAPE (1B)*/

	if( keyboardBarItem->getStatus() == 40 || keyboardBarItem->getStatus() == 30 )
	{
		if( state == ST_INIT )
		{
			if( ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf || vKey == 0xc0 /*`~*/)
				* pfEaten = TRUE;
			else if( keyboardBarItem->getStatus() == 40 && ( vKey >= 0x30 && vKey <= 0x39 ) )
				* pfEaten = TRUE;
			else
	            * pfEaten = FALSE;
		}
		else if( state == ST_COMPOSITE || state == ST_SYMBOL )
		{
			if( ( vKey >= 0x30 && vKey <= 0x39 ) || ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == 0xba || vKey == 0xbc || vKey == 0xbe || vKey == 0xbf || vKey == VK_SPACE || vKey == VK_BACK || vKey == VK_ESCAPE || vKey == VK_OEM_3 )
				* pfEaten = TRUE;

			else if( keyboardBarItem->getStatus() == 40 && ( vKey == VK_OEM_7 || vKey == VK_OEM_4 || vKey == VK_OEM_6 || vKey == VK_OEM_MINUS || vKey == VK_OEM_PLUS ) )
				* pfEaten = TRUE;

			else
				* pfEaten = FALSE;
		}
		else
			* pfEaten = FALSE;
	}
	else if( keyboardBarItem->getStatus() == 123 )
		if( state == ST_COMPOSITE && ( ( vKey >= 0x60 && vKey <=0x69 ) || vKey == VK_SPACE || vKey == VK_BACK || vKey == VK_ESCAPE || vKey == VK_RETURN ) )
			* pfEaten = TRUE;
		else if( state == ST_INIT && ( vKey >= 0x60 && vKey <=0x69 ) )
			* pfEaten = TRUE;
		else if( state == ST_SELECT && ( vKey >= 0x60 && vKey <=0x69 ) || vKey == VK_ESCAPE || vKey == VK_RETURN )
			* pfEaten = TRUE;

	else
		* pfEaten = FALSE;

	return S_OK;
}

HRESULT AArrayTextService::OnKeyDown( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
	if( keyboardBarItem->getStatus() == 30 )
	{
		if( ! lexicon30 )
            lexicon30 = new AArrayLex( L"array31.1", L"array31.2", L"simple.1" );
		lexicon = lexicon30;
        return OnKeyDown30( pContext, wParam, lParam, pfEaten );
	}

	else if( keyboardBarItem->getStatus() == 40 )
	{
		if( ! lexicon40 )
            lexicon40 = new AArrayLex( L"array.40", L"", L"", false, 41 );
		lexicon = lexicon40;
        return OnKeyDown40( pContext, wParam, lParam, pfEaten );
	}
	else if( keyboardBarItem->getStatus() == 123 )
	{
		if( ! lexicon123 )
            lexicon123 = new AArrayLex( L"array.123", L"", L"", false, 10 );
		lexicon = lexicon123;
        return OnKeyDown123( pContext, wParam, lParam, pfEaten );
	}
	else
		* pfEaten = FALSE;

	return S_OK;
}

HRESULT AArrayTextService::OnTestKeyUp( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
	* pfEaten = FALSE;
	return NOERROR;
}

HRESULT AArrayTextService::OnKeyUp( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten )
{
	* pfEaten = FALSE;
	return NOERROR;
}

HRESULT AArrayTextService::OnPreservedKey( ITfContext * pContext, REFGUID rguid, BOOL * pfEaten )
{
	return NOERROR;
}


// other function

void AArrayTextService::reset( int mode )
{
	// update defaultIME key
	wstring clsid = L"CLSID\\";
	wchar_t guid[ 256 ];
	StringFromGUID2( CLSID_ARRAY_31, guid, 255 );

	clsid += guid;

	wstringstream ss;
	ss << mode;

	HKEY hk;
	RegOpenKeyExW( HKEY_CURRENT_USER, L"Software\\Array31", NULL, KEY_ALL_ACCESS, &hk );

    RegSetValueExW( hk, L"DefaultIME", 0, REG_SZ, (LPBYTE) ss.str().c_str(), sizeof( wchar_t ) * ss.str().length() );
	RegCloseKey( hk );


	if( mode == 30 )
	{
		if( ! lexicon30 )
            lexicon30 = new AArrayLex( L"array31.1", L"array31.2", L"simple.1" );
		lexicon = lexicon30;
	}

	else if( mode == 40 )
	{
		if( ! lexicon40 )
            lexicon40 = new AArrayLex( L"array.40", L"", L"", false, 41 );
		lexicon = lexicon40;
	}
	else if( mode == 123 )
	{
		if( ! lexicon123 )
            lexicon123 = new AArrayLex( L"array.123", L"", L"", false, 10 );
		lexicon = lexicon123;
	}

	cwin.composition = L"";
	cwin.candidates = L"";

	lexicon->put( 0 );
	cwin.redraw();

}

wchar_t AArrayTextService::getAsciiFromVkey( int vKey )
{
    /* VK_OEM_COMMA:BC VK_OEM_PERIOD:BE VK_OEM_1(;):BA VK_OEM_2(/):BF */

	if( ( vKey >= 0x30 && vKey <= 0x39 ) || ( vKey >= 0x41 && vKey <= 0x5a ) || vKey == VK_SPACE )
		return static_cast<wchar_t>( vKey );

	else if( vKey >= 0x60 && vKey <= 69 )
        return static_cast<wchar_t>( vKey - 48 );

	else if( vKey == VK_OEM_COMMA )
		return static_cast<wchar_t>( 44 );

    else if( vKey == VK_OEM_PERIOD )
		return static_cast<wchar_t>( 46 );

	else if( vKey == VK_OEM_1 ) // semicolon
		return static_cast<wchar_t>( 59 );

	else if( vKey == VK_OEM_2 ) // slash
		return static_cast<wchar_t>( 47 );

	else if( vKey == VK_OEM_3 ) // tilde
	    return static_cast<wchar_t>( 96 );

	else
		return 0;
}

wstring AArrayTextService::getStrokeFromVkey( int vKey )
{
	if( vKey == 0x41 ) // a
		return L"1-";

	else if( vKey == 0x42 ) // b
		return L"5v";

	else if( vKey == 0x43 ) // c
		return L"3v";

	else if( vKey == 0x44 ) // d
		return L"3-";

	else if( vKey == 0x45 ) // e
		return L"3^";

	else if( vKey == 0x46 ) // f
		return L"4-";

	else if( vKey == 0x47 ) // g
		return L"5-";

	else if( vKey == 0x48 ) // h
		return L"6-";

	else if( vKey == 0x49 ) // i
		return L"8^";

	else if( vKey == 0x4a ) // j
		return L"7-";

	else if( vKey == 0x4b ) // k
		return L"8-";

	else if( vKey == 0x4c ) // l
		return L"9-";

	else if( vKey == 0x4d ) // m
		return L"7v";

	else if( vKey == 0x4e ) // n
		return L"6v";

	else if( vKey == 0x4f ) // o
		return L"9^";

	else if( vKey == 0x50 ) // p
		return L"0^";

	else if( vKey == 0x51 ) // q
		return L"1^";

	else if( vKey == 0x52 ) // r
		return L"4^";

	else if( vKey == 0x53 ) // s
		return L"2-";

	else if( vKey == 0x54 ) // t
		return L"5^";

	else if( vKey == 0x55 ) // u
		return L"7^";

	else if( vKey == 0x56 ) // v
		return L"4v";

	else if( vKey == 0x57 ) // w
		return L"2^";

	else if( vKey == 0x58 ) // x
		return L"2v";

	else if( vKey == 0x59 ) // y
		return L"6^";

	else if( vKey == 0x5a ) // z
		return L"1v";

	else if( vKey == VK_OEM_COMMA )
		return L"8v";

    else if( vKey == VK_OEM_PERIOD )
		return L"9v";

	else if( vKey == VK_OEM_1 ) // semicolon
		return L"0-";

	else if( vKey == VK_OEM_2 ) // slash
		return L"0v";

	else if( vKey == VK_OEM_3 )
	    return L"符號";

	else
		return L"NULL";
}

bool AArrayTextService::commit( ITfContext * pContext, wchar_t c )
{
	HRESULT hr;

	AEditSession  m_editsession;
    m_editsession.AddRef();
	m_editsession.pContext = pContext;
	m_editsession.character = c;
	pContext->RequestEditSession( m_cid, & m_editsession, TF_ES_ASYNCDONTCARE | TF_ES_READWRITE, & hr );

	if( hr != S_OK )
		return true;

	else
		return false;
}