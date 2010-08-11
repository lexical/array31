#define UNICODE

#include "aclassfactory.h"
#include <Windows.h> // link Kernel32.lib

LONG AClassFactory::m_cObj = 0;
LONG AClassFactory::m_cLock = 0;
AArrayTextService * gArrayTS;

AClassFactory::AClassFactory()
	: m_cRef( 0 )
{
}

AClassFactory::~AClassFactory()
{
}

HRESULT AClassFactory::CreateInstance( IUnknown * pUnkOuter, REFIID riid, void ** ppInterface )
{
	* ppInterface = 0;

	if( pUnkOuter && riid != IID_IUnknown )
		return CLASS_E_NOAGGREGATION;

	AArrayTextService * ts = new AArrayTextService;

	// keep a global reference copy
    gArrayTS = ts;

	if( ! ts )
		return E_OUTOFMEMORY;

	HRESULT hResult = ts->QueryInterface( riid, ppInterface );

	if( FAILED( hResult ) )
	{
		delete ts;
		return hResult;
	}
	else
	{
        InterlockedIncrement( & m_cObj );
		return NOERROR;
	}
}

HRESULT AClassFactory::LockServer( BOOL fLock )
{
	if( fLock )
        InterlockedIncrement( & m_cLock );

	else
        InterlockedDecrement( & m_cLock );


	return NOERROR;
}

// IUnknown

HRESULT AClassFactory::QueryInterface( REFIID riid, void ** ppInterface )
{
	if( riid == IID_IUnknown )
	{
		* ppInterface = static_cast<IUnknown *>( this );
	}
	else if( riid == IID_IClassFactory )
	{
		* ppInterface = static_cast<IClassFactory *>( this );
	}
	else
	{
		* ppInterface = 0;
		return E_NOINTERFACE;
	}

	static_cast<IUnknown *>( * ppInterface )->AddRef();
	return NOERROR;
}

ULONG AClassFactory::AddRef()
{
	++m_cRef;
	return m_cRef;
}

ULONG AClassFactory::Release()
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