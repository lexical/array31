#include "aeditsession.h"

#include "acandidatewindows.h"

AEditSession::AEditSession()
	: m_cRef( 0 ), pContext( 0 )
{
}

AEditSession::~AEditSession()
{
}

HRESULT AEditSession::DoEditSession( TfEditCookie ec )
{
	if( ! pContext )
		return E_FAIL;

    ITfRange * pRange;
    ITfInsertAtSelection * pInsertAtSelection;
    if( pContext->QueryInterface( IID_ITfInsertAtSelection, ( void ** ) & pInsertAtSelection ) != S_OK )
	{
		return E_FAIL;
	}

	WCHAR s[ 2 ];
	s[ 0 ] = character;
	s[ 1 ] = 0;

	if( s[ 0 ] != 0 )
		pInsertAtSelection->InsertTextAtSelection( ec, 0, s, 1, & pRange );
	else
		return E_FAIL;

    TF_SELECTION selection;

/* seem to be useeless code snippets, for clear array?
	if( pRange->Collapse( ec, TF_ANCHOR_END ) != S_OK )
	{
		//TRACEX( "pRange->Collapse failed\n" );
	}
*/

	selection.range = pRange;
	selection.style.ase = TF_AE_NONE;
	selection.style.fInterimChar = FALSE; // true for composing

	if( pContext->SetSelection( ec, 1, & selection ) != S_OK )
	{
		//TRACEX( "pContext->SetSelection failed\n" );
	}

	pRange->Release();
    pInsertAtSelection->Release();

	return S_OK;
}

HRESULT AEditSession::QueryInterface( REFIID riid, void ** ppInterface )
{
	if( riid == IID_IUnknown )
		* ppInterface = static_cast<IUnknown *>( this );

	else if( riid == IID_ITfEditSession )
		* ppInterface = static_cast<ITfEditSession *>( this );

	else
	{
		* ppInterface = 0;
		return E_NOINTERFACE;
	}

	static_cast<IUnknown *>( * ppInterface )->AddRef();
	return NOERROR;
}

ULONG AEditSession::AddRef()
{
	++m_cRef;
	return m_cRef;
}

ULONG AEditSession::Release()
{
    --m_cRef;

	if( m_cRef != 0 )
		return m_cRef;

	else
	{
		delete this;
		return 0;
	}
}