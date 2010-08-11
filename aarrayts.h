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

#ifndef AARRAYTS_H
#define AARRAYTS_H

#define UNICODE

#include <Msctf.h>
#include <olectl.h>
#include <string>
#include "acandidatewindows.h"
#include "aeditsession.h"
#include "aarraylex.h"
#include "akeyboardbaritem.h"

// {9D472BA1-C2F3-4da8-ACA3-4C732AF03DFE}
static const GUID CLSID_ARRAY_31 =
{ 0x9d472ba1, 0xc2f3, 0x4db8, { 0xac, 0xa3, 0x4c, 0x73, 0x2a, 0xf0, 0x3d, 0xfe } };

static const GUID GUID_ARRAY_31_PRESERVED_KEY =
{ 0x9d472ba1, 0xc2f3, 0x4db9, { 0xac, 0xa3, 0x4c, 0x73, 0x2a, 0xf0, 0x3d, 0xfe } };


class AArrayTextService:public ITfTextInputProcessor,
						public ITfThreadFocusSink,
						public ITfKeyEventSink
{
public:
    AArrayTextService();
	virtual ~AArrayTextService();

	void reset( int mode );

	// IUnknown;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, void ** );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	// ITfTextInputProcessor
	virtual HRESULT STDMETHODCALLTYPE Activate( ITfThreadMgr * pThreadMgr, TfClientId tfClientId );
	virtual HRESULT STDMETHODCALLTYPE Deactivate();

	// ITfKeyEventSink
	virtual HRESULT STDMETHODCALLTYPE OnSetFocus( BOOL fForeground );
	virtual HRESULT STDMETHODCALLTYPE OnTestKeyDown( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
	virtual HRESULT STDMETHODCALLTYPE OnKeyDown( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
	virtual HRESULT STDMETHODCALLTYPE OnTestKeyUp( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
	virtual HRESULT STDMETHODCALLTYPE OnKeyUp( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
	virtual HRESULT STDMETHODCALLTYPE OnPreservedKey( ITfContext * pContext, REFGUID rguid, BOOL * pfEaten );

	// ITfThreadFocusSink
    virtual HRESULT STDMETHODCALLTYPE OnSetThreadFocus();
    virtual HRESULT STDMETHODCALLTYPE OnKillThreadFocus();

	static GUID gLangProfile;

protected:
	wchar_t getAsciiFromVkey( int vkey );
	wstring getStrokeFromVkey( int vkey );
	bool commit( ITfContext * pContext, wchar_t c );

	ULONG m_cRef;
    TfClientId m_cid;
	bool isActive;
	std::wstring m_path;
	int simpleCodeKey;

    ACandidateWindow cwin;
	//AEditSession m_editsession;

    ITfInputProcessorProfiles * pProfile;
    ITfThreadMgr * pThreadMgr;

	DWORD ThreadFocusSinkCookie;
	DWORD ThreadMgrEventSinkCookie;

	AArrayLex * lexicon;
    AArrayLex * lexicon30;
    AArrayLex * lexicon40;
    AArrayLex * lexicon123;

	int state;
	bool stateSymbolReady;
	enum { ST_ERROR, ST_INIT, ST_COMPOSITE, ST_SYMBOL, ST_SELECT };

	AKeyboardBarItem * keyboardBarItem;

    HRESULT OnKeyDown30( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
    HRESULT OnKeyDown40( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
    HRESULT OnKeyDown123( ITfContext * pContext, WPARAM wParam, LPARAM lParam, BOOL * pfEaten );
};

#endif