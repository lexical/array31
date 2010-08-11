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