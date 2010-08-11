#ifndef ACLASSFACTORY_H
#define ACLASSFACTORY_H

#include <rpc.h>
#include "aarrayts.h"

extern AArrayTextService * gArrayTS;

class AClassFactory:public IClassFactory
{
public:
	AClassFactory();
	virtual ~AClassFactory();

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, void ** );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	//IClassFactory
	virtual HRESULT STDMETHODCALLTYPE CreateInstance( IUnknown * pUnkOuter, REFIID riid, void ** ppvOjbect );
	virtual HRESULT STDMETHODCALLTYPE LockServer( BOOL fLock );

	static LONG m_cObj;
	static LONG m_cLock;

protected:
	ULONG m_cRef;
};


#endif