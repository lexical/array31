#ifndef AKEYBOARD_BARITEM_H
#define AKEYBOARD_BARITEM_H

#include <Msctf.h>
#include <Ctfutb.h>

class AArrayTextService;

class AKeyboardBarItem : public ITfLangBarItemButton, public ITfSource
{
public:
    AKeyboardBarItem();
	virtual ~AKeyboardBarItem();

	void init( AArrayTextService * t );

	// IUnknown;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, void ** );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

    // ITfLangBarItem
    virtual HRESULT STDMETHODCALLTYPE GetInfo( TF_LANGBARITEMINFO * pInfo );
    virtual HRESULT STDMETHODCALLTYPE GetStatus( DWORD * pdwStatus );
    virtual HRESULT STDMETHODCALLTYPE Show( BOOL fShow );
    virtual HRESULT STDMETHODCALLTYPE GetTooltipString( BSTR * pbstrToolTip );

	// ITfLangBarItemButton
	virtual HRESULT STDMETHODCALLTYPE OnClick( TfLBIClick click, POINT pt, const RECT * prcArea );
	virtual HRESULT STDMETHODCALLTYPE InitMenu( ITfMenu * pMenu );
	virtual HRESULT STDMETHODCALLTYPE OnMenuSelect( UINT wID );
	virtual HRESULT STDMETHODCALLTYPE GetIcon( HICON * phIcon );
	virtual HRESULT STDMETHODCALLTYPE GetText( BSTR * pbstrText );

    // ITfSource
	virtual HRESULT STDMETHODCALLTYPE AdviseSink( REFIID riid, IUnknown * punk, DWORD * pdwCookie );
	virtual HRESULT STDMETHODCALLTYPE UnadviseSink( DWORD dwCookie );

	int getStatus();

protected:
    ULONG m_cRef;
    TF_LANGBARITEMINFO langBarItemInfo;
    ITfLangBarItemSink * pLangBarItemSink;

	HICON hicon_40;
	HICON hicon_30;
    HICON hicon_123;

	wchar_t * msg30;
    wchar_t * msg40;
    wchar_t * msg123;

	int status;
    //void ( AArrayTextService::* reset )( int );
    AArrayTextService * ts;
};

#endif