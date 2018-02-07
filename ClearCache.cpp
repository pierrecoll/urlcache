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
void CClearCache::Clear(LPSTR lpszContainer, BOOL bSelectiveDelete, LPSTR lpszSearch)
{
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First(lpszContainer);
	nEntries = 0;
	nEntriesDeleted = 0;
	if (bSelectiveDelete == FALSE)
	{
		while (pInfo)
		{
			nEntries++;
			if (CacheEntry.IsTemporary(pInfo->CacheEntryType))
			{
				nEntriesDeleted++;
				DelCacheEntry(pInfo);
			}
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
	else
	{
		while (pInfo)
		{
			nEntries++;
			BOOL bFound = SearchCache(pInfo, lpszSearch);
			if (!bFound)
			{
				nEntriesDeleted++;
				DelCacheEntry(pInfo);
			}
			pInfo = CacheEntry.Next();
		}
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
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First(TEMPORARY_CACHE_PREFIX);

	while(pInfo)
	{

		DelCacheEntry(pInfo);

		pInfo = CacheEntry.Next();
	}

	_tprintf(_T("Cache Cleared\n"));
}
//////////////////////////////////////////////////////////////////////
// Methode : CCache
// Resume : Search cache entry
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
BOOL  CClearCache::SearchCache(INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSearch)
{
	BOOL bFound = FALSE;
	if (m_bSearch_lpszSourceUrlName == 1)
	{
		//160614  handling * separator
		char SearchCopy[L_MAX_URL_LENGTH + 1];
		lstrcpyn(SearchCopy, lpszSearch, L_MAX_URL_LENGTH);
		char seps[] = "*";
		char *token = NULL;
		char *next_token;
		//Returns a pointer to the next token found in strToken. They return NULL when no more tokens are found. 
		//Each call modifies strToken by substituting a NULL character for the first delimiter that occurs after the returned token.
		token = strtok_s(SearchCopy, seps, &next_token);

		while (token != NULL)
		{
			if ((token) && (strstr(pInfo->lpszSourceUrlName, token)))
			{
				bFound = TRUE;
				//DisplayCacheEntry(pInfo);
				return bFound;
			}
			token = strtok_s(NULL, seps, &next_token);
		}
	}
	if (m_bSearch_lpszLocalFileName == 1)
	{
		//160614  handling * separator 
		char SearchCopy[L_MAX_URL_LENGTH + 1];
		lstrcpyn(SearchCopy, lpszSearch, L_MAX_URL_LENGTH);
		char seps[] = "*";
		char *token = NULL;
		char *next_token;
		//Returns a pointer to the next token found in strToken. They return NULL when no more tokens are found. 
		//Each call modifies strToken by substituting a NULL character for the first delimiter that occurs after the returned token.
		token = strtok_s(SearchCopy, seps, &next_token);

		while (token != NULL)
		{
			if ((token) && (strstr(pInfo->lpszLocalFileName, token)))
			{
				bFound = TRUE;
				//DisplayCacheEntry(pInfo);
				return bFound;
			}
			token = strtok_s(NULL, seps, &next_token);
		}
	}

	if (m_bSearch_lpHeaderInfo == 1)
	{
		if ((pInfo->lpHeaderInfo) && (strstr(pInfo->lpHeaderInfo, lpszSearch)))
		{
			return TRUE;
		}
	}
	//10/20/2015
	if (m_bSearch_ExpireTime == 1)
	{
		if ((GetTime(pInfo->ExpireTime)) && (strstr(GetTime(pInfo->ExpireTime), lpszSearch)))
		{
			return TRUE;
		}
	}
	return FALSE;
}

