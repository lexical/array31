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

#include "akeyboardbaritem.h"
#include "aarrayts.h"

static const GUID GUID_KEYBOARD_BAR_ITEM =
{ 0x9d472ba1, 0xc2f3, 0x4daa, { 0xac, 0xa3, 0x4c, 0x73, 0x2a, 0xf0, 0x3d, 0xfe } };


AKeyboardBarItem::AKeyboardBarItem()
	:m_cRef( 0 ), pLangBarItemSink( 0 ), status( 30 )
{
	//reset = f;

	hicon_30 = LoadIcon( ACandidateWindow::hInstance, MAKEINTRESOURCE( 101 ) );
    hicon_40 = LoadIcon( ACandidateWindow::hInstance, MAKEINTRESOURCE( 102 ) );
    hicon_123 = LoadIcon( ACandidateWindow::hInstance, MAKEINTRESOURCE( 103 ) );

	msg30 = L"30鍵";
    msg40 = L"40鍵";
	msg123 = L"數字鍵";

	//TRACEX("AKeyboardBarItem::AKeyboardBarItem\n");
    langBarItemInfo.clsidService = CLSID_ARRAY_31;
    langBarItemInfo.guidItem = GUID_KEYBOARD_BAR_ITEM;
    langBarItemInfo.dwStyle = TF_LBI_STYLE_BTN_MENU;
    langBarItemInfo.ulSort = 1;
    langBarItemInfo.szDescription[ 0 ] = L'C';
    langBarItemInfo.szDescription[ 1 ] = L'B';
    langBarItemInfo.szDescription[ 2 ] = L'A';
    //SafeStringCopy( langBarItemInfo.szDescription, ARRAYSIZE( langBarItemInfo.szDescription ), L"szdes" );
}

AKeyboardBarItem::~AKeyboardBarItem()
{
    //TRACEX("AKeyboardBarItem::~AKeyboardBarItem\n");
}

void AKeyboardBarItem::init( AArrayTextService * t )
{
	ts = t;
}

int AKeyboardBarItem::getStatus()
{
	return status;
}

HRESULT AKeyboardBarItem::GetInfo( TF_LANGBARITEMINFO * pInfo )
{
    //TRACEX("AKeyboardBarItem::GetInfo\n");
	* pInfo = langBarItemInfo;
	return S_OK;
}

HRESULT AKeyboardBarItem::GetStatus( DWORD * pdwStatus )
{
    //TRACEX("AKeyboardBarItem::GetStatus\n");
	* pdwStatus = 0;
	return S_OK;
}

HRESULT AKeyboardBarItem::Show( BOOL fShow )
{
    return E_NOTIMPL;
}

HRESULT AKeyboardBarItem::GetTooltipString( BSTR * pbstrToolTip )
{
    * pbstrToolTip = SysAllocString( L"行列鍵數" );
	return S_OK;
}


// ITfLangBarItemButton
HRESULT AKeyboardBarItem::OnClick( TfLBIClick click, POINT pt, const RECT * prcArea )
{
	return S_OK;
}

HRESULT AKeyboardBarItem::InitMenu( ITfMenu * pMenu )
{
	pMenu->AddMenuItem( 30, 0, NULL, NULL, msg30, static_cast<ULONG>( wcslen( msg30 ) ), NULL );
	pMenu->AddMenuItem( 40, 0, NULL, NULL, msg40, static_cast<ULONG>( wcslen( msg40 ) ), NULL );
    pMenu->AddMenuItem( 123, 0, NULL, NULL, msg123, static_cast<ULONG>( wcslen( msg123 ) ), NULL );

	return S_OK;
}

HRESULT AKeyboardBarItem::OnMenuSelect( UINT wID )
{
	if( wID == 30 )
	{
		status = 30;
		ts->reset( 30 );
	}

	else if( wID == 40 )
	{
		status = 40;
		ts->reset( 40 );
	}

	else if( wID == 123 )
	{
		status = 123;
		ts->reset( 123 );
	}

    pLangBarItemSink->OnUpdate( TF_LBI_ICON | TF_LBI_TEXT );
	return S_OK;
}

HRESULT AKeyboardBarItem::GetIcon( HICON * phIcon )
{
    //TRACEX("AKeyboardBarItem::GetIcon\n");
	if( status == 40 )
		* phIcon = hicon_40;

	else if( status == 30 )
		* phIcon = hicon_30;

	else if( status == 123 )
		* phIcon = hicon_123;

	return S_OK;
}

HRESULT AKeyboardBarItem::GetText( BSTR * pbstrText )
{

	if( status == 40 )
	{
		* pbstrText = SysAllocString( msg40 );
        //TRACEX("AKeyboardBarItem::GetText 40\n");
	}

	else if( status == 30 )
	{
		* pbstrText = SysAllocString( msg30 );
        //TRACEX("AKeyboardBarItem::GetText 30\n");
	}
	else if( status == 123 )
        * pbstrText = SysAllocString( msg123 );

	return S_OK;
}


// ITfSource
HRESULT AKeyboardBarItem::AdviseSink( REFIID riid, IUnknown * punk, DWORD * pdwCookie )
{
    //TRACEX("AKeyboardBarItem::AdviseSink\n");
	if( riid != IID_ITfLangBarItemSink )
		return CONNECT_E_CANNOTCONNECT;

	if( pLangBarItemSink != 0 )
		return CONNECT_E_ADVISELIMIT;

	if( punk->QueryInterface( IID_ITfLangBarItemSink, (void **) & pLangBarItemSink ) != S_OK )
	{
        pLangBarItemSink = 0;
		return E_NOINTERFACE;
	}

	* pdwCookie = 0xabc;
	return S_OK;
}

HRESULT AKeyboardBarItem::UnadviseSink( DWORD dwCookie )
{
    //TRACEX("AKeyboardBarItem::UnadviseSink\n");
	if( dwCookie != 0xabc )
		return CONNECT_E_NOCONNECTION;

	if( pLangBarItemSink == 0 )
		return CONNECT_E_NOCONNECTION;

    pLangBarItemSink->Release();
    pLangBarItemSink = 0;

	return S_OK;
}

// IUnknown

HRESULT AKeyboardBarItem::QueryInterface( REFIID riid, void ** ppInterface )
{
    //TRACEX("AKeyboardBarItem::QueryInterface\n");
	if( riid == IID_IUnknown )
		* ppInterface = static_cast<ITfLangBarItemButton *>( this );

	else if( riid == IID_ITfLangBarItem )
		* ppInterface = static_cast<ITfLangBarItemButton *>( this );

	else if( riid == IID_ITfLangBarItemButton )
		* ppInterface = static_cast<ITfLangBarItemButton *>( this );

	else if( riid == IID_ITfSource )
		* ppInterface = static_cast<ITfSource *>( this );

	else
	{
		* ppInterface = 0;
		return E_NOINTERFACE;
	}

	static_cast<IUnknown *>( * ppInterface )->AddRef();
	return NOERROR;
}


ULONG AKeyboardBarItem::AddRef()
{
    //TRACEX("AKeyboardBarItem::AddRef()\n");
	++m_cRef;
	return m_cRef;
}

ULONG AKeyboardBarItem::Release()
{
    //TRACEX("AKeyboardBarItem::Release()\n");
    --m_cRef;

	if( m_cRef != 0 )
		return m_cRef;

	else
	{
		delete this;
		return 0;
	}
}
