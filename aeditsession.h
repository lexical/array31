#ifndef AEditSession_H
#define AEditSession_H

#include <Msctf.h>

class AEditSession:public ITfEditSession
{
public:
	AEditSession();
	virtual ~AEditSession();

	// IUnknown;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, void ** );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

    virtual HRESULT STDMETHODCALLTYPE DoEditSession( TfEditCookie ec );

    ITfContext * pContext;
	wchar_t character;

private:
    int m_cRef;
};

#endif