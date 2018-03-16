#include <windows.h>
#include <string>
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")

bool getGpuName(char* str, int bufferLength)
{
	ZeroMemory(str, bufferLength);
	HRESULT hres;
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) return 0;
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres)) { CoUninitialize(); return 0; }
	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres)) { CoUninitialize(); return 0; }
	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"root\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres)) { pLoc->Release(); CoUninitialize(); return 0; }
	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) { pSvc->Release(); pLoc->Release(); CoUninitialize(); return 0; }
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_VideoController"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	if (FAILED(hres)) { pSvc->Release(); pLoc->Release(); CoUninitialize(); return 0; }
	IWbemClassObject *pclsObj = (IWbemClassObject*)malloc(sizeof(IWbemClassObject));
	
	ULONG uReturn = 0;
	bool passed = false;
	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (0 == uReturn)break;
		VARIANT vtProp;
		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
		std::wstring wGpuName = vtProp.bstrVal;
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wGpuName[0], (int)wGpuName.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wGpuName[0], (int)wGpuName.size(), &strTo[0], size_needed, NULL, NULL);
		if(!passed)
			strcpy(str, strTo.c_str());
		else
			strcpy(str + size_needed, strTo.c_str());
		passed = true;
		//wcout << " OS Name : " << vtProp.bstrVal << endl;
		VariantClear(&vtProp);
	}
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	pclsObj->Release();
	CoUninitialize();

	//free(pclsObj);

	return 0;
}