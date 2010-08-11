#define UNICODE

#include "aclassfactory.h"
#include "acandidatewindows.h"
#include "aarrayts.h"
#include <string.h>
#include <olectl.h>
#include <Windows.h>
#include <Msctf.h>


BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved )
{
	ACandidateWindow::hInstance = hInstance;

	switch( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		if( gArrayTS )
		{
			delete gArrayTS;
		}
		break;
	}

	return TRUE;
}

STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, void ** ppInterface )
{
	// rclsid is the factory's class id
	// riid is the interface to be queried in the factory

	if( rclsid != CLSID_ARRAY_31 )
		return CLASS_E_CLASSNOTAVAILABLE;
	else
	{
		AClassFactory * pAClassFactory = new AClassFactory;
		if( ! pAClassFactory )
			return E_OUTOFMEMORY;

		HRESULT hR = pAClassFactory->QueryInterface( riid, ppInterface );

		if( FAILED( hR ) )
		{
			delete pAClassFactory;
			return hR;
		}
		else
			return NOERROR;
	}
}

STDAPI DllCanUnloadNow()
{
	//TRACEX( "DllCanUnloadNow\n\n\n" );
	if( AClassFactory::m_cObj == 0 && AClassFactory::m_cLock == 0 )
		return S_OK;

	else
		return S_FALSE;
}

STDAPI DllRegisterServer( void )
{
	// HKEY_CLASSES_ROOT\CLSID\{}\InProcServer32 = c:\*.dll

	// wget CLSID string from arrayts.h: CLSID_ARRAY_31
	wstring clsid = L"CLSID\\";
	wchar_t guid[ 256 ];
	if( ! StringFromGUID2( CLSID_ARRAY_31, guid, 255 ) )
		return SELFREG_E_CLASS;

	clsid += guid;

	// create clsid
    HKEY hkey;
    DWORD dw;
    if( RegCreateKeyExW( HKEY_CLASSES_ROOT, clsid.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &dw) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;
    if( RegSetValueExA( hkey, NULL, 0, REG_SZ, (BYTE *) "ARRAY_31", 9 ) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;

	//InProcServer32
	HKEY hsubkey;
	if( RegCreateKeyExA( hkey, "InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hsubkey, &dw) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;

	char path[ 256 ];
	int szPath;
	if( ( szPath = GetModuleFileNameA( ACandidateWindow::hInstance, path, 255 ) ) == 0 )
		return SELFREG_E_CLASS;

	if( RegSetValueExA( hsubkey, NULL, 0, REG_SZ, (BYTE *) path, szPath + 1 ) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;
	if( RegSetValueExA( hsubkey, "ThreadingModel", 0, REG_SZ, (BYTE *) "Apartment", 10 ) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;

	// DefaultIME
    if( RegCreateKeyExW( HKEY_CURRENT_USER, L"Software\\Array31", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &dw) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;
      if( RegSetValueExA( hkey, "DefaultIME", 0, REG_SZ, (BYTE *) "30", 3 ) != ERROR_SUCCESS )
		return SELFREG_E_CLASS;

	RegCloseKey( hkey );
	RegCloseKey( hsubkey );

	// Register with TSF
    ITfInputProcessorProfiles * pInputProcessProfiles;
    HRESULT hr = CoCreateInstance( CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, ( LPVOID* ) & pInputProcessProfiles );
	if( hr != S_OK ) return SELFREG_E_CLASS;

	// Register
	hr = pInputProcessProfiles->Register( CLSID_ARRAY_31 );
    if( hr != S_OK ) return SELFREG_E_CLASS;

	// Language Profile
	WCHAR wpath[ 256 ];
	szPath = GetModuleFileNameW( ACandidateWindow::hInstance, wpath, 255 );
/*
	int l( 0 );
	for( int i = 0; i < szPath; ++i )
		if( wpath[i] == L'\\' )
			l = i;

	wpath[ l + 1 ] = 0;
	WCHAR * wpath_ico = wcscat( wpath, L"array31.ico" );
*/  WCHAR * wpath_ico = wpath;
	hr = pInputProcessProfiles->AddLanguageProfile( CLSID_ARRAY_31, MAKELANGID( LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL ), AArrayTextService::gLangProfile, L"行列輸入法(3.1α6)", -1, wpath_ico, -1, 0 );
	if( hr != S_OK ) return SELFREG_E_CLASS;

    pInputProcessProfiles->Release();

	// Register category
	ITfCategoryMgr * pCatMgr;
	hr = CoCreateInstance( CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, ( void ** ) & pCatMgr );
	if( hr != S_OK ) return SELFREG_E_CLASS;

	hr = pCatMgr->RegisterCategory( CLSID_ARRAY_31, GUID_TFCAT_TIP_KEYBOARD, CLSID_ARRAY_31 );
    if( hr != S_OK ) return SELFREG_E_CLASS;
    hr = pCatMgr->RegisterCategory( CLSID_ARRAY_31, GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER, CLSID_ARRAY_31 );
    if( hr != S_OK ) return SELFREG_E_CLASS;

	pCatMgr->Release();


	return S_OK;
}

STDAPI DllUnregisterServer( void )
{
	HRESULT hr;

	// Unregister category
	ITfCategoryMgr * pCatMgr;
	hr = CoCreateInstance( CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, ( void ** ) & pCatMgr );
	if( hr != S_OK ) return SELFREG_E_CLASS;

	hr = pCatMgr->UnregisterCategory( CLSID_ARRAY_31, GUID_TFCAT_TIP_KEYBOARD, CLSID_ARRAY_31 );
    if( hr != S_OK ) return SELFREG_E_CLASS;
    hr = pCatMgr->UnregisterCategory( CLSID_ARRAY_31, GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER, CLSID_ARRAY_31 );
    if( hr != S_OK ) return SELFREG_E_CLASS;

	pCatMgr->Release();


	// Unegister with TSF
    ITfInputProcessorProfiles * pInputProcessProfiles;
	hr = CoCreateInstance( CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER, IID_ITfInputProcessorProfiles, ( LPVOID* ) & pInputProcessProfiles );
	if( hr != S_OK ) return SELFREG_E_CLASS;

	GUID guidprofile;
	if( pInputProcessProfiles->RemoveLanguageProfile( CLSID_ARRAY_31, MAKELANGID( LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL ), guidprofile ) != S_OK )
		return SELFREG_E_CLASS;

	// Unregister
	hr = pInputProcessProfiles->Unregister( CLSID_ARRAY_31 );
    if( hr != S_OK ) return SELFREG_E_CLASS;

    pInputProcessProfiles->Release();


	return S_OK;
}
