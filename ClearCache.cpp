//////////////////////////////////////////////////////////////////////
// Classe : CacheEntry
// Resume : Clear Selected Cache Entry
////////////

#include "stdafx.h"
#include "ClearCache.h"
#include "CacheInfo.h"
#include "CacheEntry.h"

#include <shlguid.h>
#include <urlhist.h> 
extern CString GetTime(FILETIME ft);

//////////////////////////////////////////////////////////////////////
// Methode : CClearCache
// Resume : Constructeur
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CClearCache::CClearCache()
{

}
//////////////////////////////////////////////////////////////////////
// Methode : ~CClearCache
// Resume : Destructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CClearCache::~CClearCache()
{

}

//////////////////////////////////////////////////////////////////////
// Methode : Clear
// Resume : Clear entries in a container
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CClearCache::Clear(LPSTR lpszContainer)
{
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First(lpszContainer);
	nEntries = 0;
	nEntriesDeleted = 0;

	while (pInfo)
	{
		nEntries++;
		nEntriesDeleted++;
		DelCacheEntry(pInfo);			
		pInfo = CacheEntry.Next();
	}
	if (lstrcmp(lpszContainer, HISTORY_CACHE_PREFIX) == 0)
	{
		RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Internet Explorer\\TypedURLs"));
		RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU"));
		IUrlHistoryStg2* pHistory;  // We need this interface for clearing the history.
		HRESULT hr;
		DWORD cRef;
		CoInitialize(NULL);

		// Load the correct Class and request IUrlHistoryStg2
		hr = CoCreateInstance(CLSID_CUrlHistory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IUrlHistoryStg2,
			reinterpret_cast<void **>(&pHistory));

		if (SUCCEEDED(hr))
		{
			// Clear the IE History
			hr = pHistory->ClearHistory();
		}
		// Release our reference to the 
		cRef = pHistory->Release();
		CoUninitialize();
	}
}

//////////////////////////////////////////////////////////////////////
// Methode : DelCacheEntry
// Resume : Delete specified cache entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
bool CClearCache::DelCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo)
{
	return (::DeleteUrlCacheEntry(pInfo->lpszSourceUrlName) ? true : false);
}

void CClearCache::ClearAll()
{
	Clear(TEMPORARY_CACHE_PREFIX);
	Clear(HISTORY_CACHE_PREFIX);
	Clear(COOKIE_CACHE_PREFIX);

	_tprintf(_T("All Cache Cleared\n"));
}

