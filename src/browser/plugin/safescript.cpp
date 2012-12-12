#ifdef QAXSERVER
#include "amplugin.h"
#include "strsafe.h"
//#include "afxver_.h"
//#include "afxstat_.h"

HRESULT CreateComponentCategory( CATID catid, WCHAR *catDescription )
{
    ICatRegister *pcr=NULL;
    HRESULT hr=S_OK;

    hr=CoCreateInstance( CLSID_StdComponentCategoriesMgr,
        NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr );
    if ( FAILED(hr) ) return hr;

    // Make sure the HKCR\Component Categories\{..catid...}
    // key is registered.
    CATEGORYINFO catinfo;
    catinfo.catid=catid;
    catinfo.lcid=0x0409 ; // english
    size_t len;
    // Make sure the provided description is not too long.
    // Only copy the first 127 characters if it is.
    // The second parameter of StringCchLength is the maximum
    // number of characters that may be read into catDescription.
    // There must be room for a NULL-terminator. The third parameter
    // contains the number of characters excluding the NULL-terminator.
    hr=StringCchLength( catDescription, STRSAFE_MAX_CCH, &len );
    if ( SUCCEEDED(hr) ) {
        if ( len>127 ) {
            len = 127;
        }
    } else {
        qWarning( "CreateComponentCategory(): error in StringCchLength()" );
    }
    // The second parameter of StringCchCopy is 128 because you need
    // room for a NULL-terminator.
    hr=StringCchCopy( catinfo.szDescription, len+1, catDescription );
    // Make sure the description is null terminated.
    catinfo.szDescription[len+1]='\0';

    hr=pcr->RegisterCategories( 1, &catinfo );
    pcr->Release();
    return hr;
}

HRESULT RegisterCLSIDInCategory( REFCLSID clsid, CATID catid )
{
    // Register your component categories information.
    ICatRegister *pcr=NULL;
    HRESULT hr=S_OK;
    hr=CoCreateInstance( CLSID_StdComponentCategoriesMgr,
        NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr );
    if ( SUCCEEDED(hr) ) {
       // Register this category as being "implemented" by the class.
       CATID rgcatid[1];
       rgcatid[0]=catid;
       hr=pcr->RegisterClassImplCategories( clsid, 1, rgcatid );
    }

    if ( pcr!=NULL ) pcr->Release();
    return hr;
}

STDAPI DllRegisterServerAmp(void)
{
    qWarning( "DLLRegisterServerAmp" );
    HRESULT  hr;    // return for safety functions
    const CATID CATID_SafeForInitializing=
        {0x7dd95802,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
    const CATID CATID_SafeForScripting=
        {0x7dd95801,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
    // AMP class ID: {042D01BD-62BA-4D7E-B25A-B56E41DB1E22}
    const IID ampClassID=
        {0x042d01bd,0x62ba,0x4d7e,{0xb2,0x5a,0xbf,0x6e,0x41,0xdb,0x1e,0x22}};
    /*
    AFX_MANAGE_STATE( _afxModuleAddrThis );

    if ( !AfxOleRegisterTypeLib( AfxGetInstanceHandle(), _tlid ) )
        return ResultFromScode( SELFREG_E_TYPELIB );

    if ( !COleObjectFactoryEx::UpdateRegistryAll( true ) )
        return ResultFromScode( SELFREG_E_CLASS );
    */

    // Mark the control as safe for initializing.
    hr=CreateComponentCategory( CATID_SafeForInitializing,
        L"Phi is safely initializable from persistent data" );
    if ( FAILED( hr ) ) return hr;
    qWarning( "1" );

    hr=RegisterCLSIDInCategory( ampClassID, CATID_SafeForInitializing );
    if ( FAILED( hr ) ) return hr;

    // Mark the control as safe for scripting.
    hr=CreateComponentCategory( CATID_SafeForScripting, L"Phi is safely scriptable" );
    if ( FAILED( hr ) ) return hr;

    hr=RegisterCLSIDInCategory( ampClassID, CATID_SafeForScripting );
    if ( FAILED( hr ) ) return hr;

    return NOERROR;
}

#endif // QAXSERVER
