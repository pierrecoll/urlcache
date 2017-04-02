
#include "stdafx.h"
#include "SearchCache.h"
#include "DisplayCache.h"
#include "ClearCache.h"
#include "CacheInfo.h"
#include "CacheEntry.h"
#include <fstream>
#include <UrlHist.h>


CString DWORD2String(DWORD dw);
CString GetTime(FILETIME ft);
CString GetCacheType(DWORD dwType);

unsigned int iteration=0;

//////////////////////////////////////////////////////////////////////
// Methode : CSearchCache
// Resume : Constructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CSearchCache::CSearchCache()
{
	m_bDisp_dwStructSize			= 1;
	m_bDisp_lpszSourceUrlName		= 1;
	m_bDisp_lpszLocalFileName		= 1;
	m_bDisp_CacheEntryType			= 1;
	m_bDisp_dwUseCount				= 1;
	m_bDisp_dwHitRate				= 1;
	m_bDisp_dwSizeLow				= 1;
	m_bDisp_dwSizeHigh				= 1;
	m_bDisp_LastModifiedTime		= 1;
	m_bDisp_ExpireTime				= 1;
	m_bDisp_LastAccessTime			= 1;
	m_bDisp_LastSyncTime			= 1;
	m_bDisp_lpHeaderInfo			= 1;
	m_bDisp_dwHeaderInfoSize		= 1;
	m_bDisp_lpszFileExtension		= 1;
}
//////////////////////////////////////////////////////////////////////
// Methode : CSearchCache
// Resume : Destructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CSearchCache::~CSearchCache()
{

}
//////////////////////////////////////////////////////////////////////
// Methode : SearchCookies
// Resume : Search Cookies Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CSearchCache::SearchCookies(LPSTR lpszSourceUrlName,BOOL bDelete)
{
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstCookie();
	BOOL bFound=FALSE;
	while(pInfo)
	{
		iteration++;
		bFound=SearchCache(pInfo,lpszSourceUrlName,bDelete);
		if (bFound)
			break;
		pInfo = CacheEntry.Next();
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : SearchTemporary
// Resume : Search Temporary Internet Files Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CSearchCache::SearchTemporary(LPSTR lpszSourceUrlName,BOOL bDelete)
{
	CCacheEntry CacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First();
	BOOL bFound=FALSE;
	while(pInfo)
	{
		if(CacheEntry.IsTemporary(pInfo->CacheEntryType))
		{	
			iteration++;
			bFound=SearchCache(pInfo,lpszSourceUrlName,bDelete);
			if (bFound)
				break;
		}
		pInfo = CacheEntry.Next();
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : SearchHistory
// Resume : Search History Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CSearchCache::SearchHistory(LPSTR lpszSourceUrlName, BOOL bDelete)
{
	CCacheEntry CacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstHistory();
	BOOL bFound=FALSE;

	while(pInfo)
	{
		iteration++;
		bFound=SearchCache(pInfo,lpszSourceUrlName,bDelete);
		if (bFound)
			break;
		pInfo = CacheEntry.Next();
	}
}
void CSearchCache::SearchAll(LPSTR lpszSourceUrlName,BOOL bDelete)
{
	SearchHistory(lpszSourceUrlName,bDelete);
	SearchTemporary(lpszSourceUrlName,bDelete);
	SearchCookies(lpszSourceUrlName,bDelete);
}


//////////////////////////////////////////////////////////////////////
// Methode : CSearchCache
// Resume : Search cahce entry
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
BOOL CSearchCache::SearchCache(INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSourceUrlName,BOOL bDelete)
{
	if (lpszSourceUrlName[0]=='*')
	{
		//Wildcard found
		lpszSourceUrlName++;
		if (!strstr(pInfo->lpszSourceUrlName,lpszSourceUrlName ))
		{
			//printf(lpszSourceUrlName);
			//printf("\r\n");
			//printf(pInfo->lpszSourceUrlName);
			printf("%d.",iteration);

		}
		else
		{
			printf("\r\n");

			DisplayCacheEntry(pInfo);	
			if (bDelete)
			{
				DelCacheEntry(pInfo);
			}
			return FALSE;
		}
	}
	else
	{
		if (_strcmpi(lpszSourceUrlName, pInfo->lpszSourceUrlName))
		{
			printf("..");
			return FALSE;
		}
		else
		{
			printf("\r\n");
			DisplayCacheEntry(pInfo);	
			if (bDelete)
			{
				DelCacheEntry(pInfo);
			}
			return TRUE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Methode : CDisplayCache
// Resume : Display cache entry
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
void CSearchCache::DisplayCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo)
{
	SetConsoleCP(1252);
	CString strTemp;

	printf("------->\n");

	if (m_bDisp_dwStructSize)
	{
			printf(" Structure Size     : %s \n", DWORD2String(pInfo->dwStructSize));
	}
	if (m_bDisp_lpszSourceUrlName)
	{
		if (pInfo->lpszSourceUrlName) printf(" Source URL Name    : %s \n", pInfo->lpszSourceUrlName);
		else printf(" Source URL Name    : \n");
	}
	if (m_bDisp_lpszLocalFileName)
	{
		if (pInfo->lpszLocalFileName) printf(" Local File Name    : %s \n", pInfo->lpszLocalFileName);
		else printf(" Local File Name    : \n");
	}
	if (m_bDisp_CacheEntryType)
	{
		printf(" Type               : %s \n", GetCacheType(pInfo->CacheEntryType));
	}
	if (m_bDisp_dwUseCount)
	{
		printf(" Use Count          : %s \n", DWORD2String(pInfo->dwUseCount));
	}
	if (m_bDisp_dwHitRate)
	{
		printf(" Hit Rate           : %s \n", DWORD2String(pInfo->dwHitRate));
	}
	if (m_bDisp_dwSizeLow)
	{
		printf(" Size Low           : %s \n", DWORD2String(pInfo->dwSizeLow));
	}
	if (m_bDisp_dwSizeHigh)
	{
		printf(" Size High          : %s \n", DWORD2String(pInfo->dwSizeHigh));
	}
	if (m_bDisp_LastModifiedTime)
	{
		printf(" Last Modified Time : %s \n", GetTime(pInfo->LastModifiedTime));
	}
	if (m_bDisp_ExpireTime)
	{
		printf(" Expire Time        : %s \n", GetTime(pInfo->ExpireTime));
	}
	if (m_bDisp_LastAccessTime)
	{
		printf(" Last Access Time   : %s \n", GetTime(pInfo->LastAccessTime));
	}
	if (m_bDisp_LastSyncTime)
	{
		printf(" Last Sync Time     : %s \n", GetTime(pInfo->LastSyncTime));
	}
	if (m_bDisp_lpHeaderInfo)
	{
		if (pInfo->lpHeaderInfo) 
		{
			printf(" Header Info        : ");
			printf("%s \n",pInfo->lpHeaderInfo);
		}
		else printf(" Header Info        : \n");

	}
	if (m_bDisp_dwHeaderInfoSize)
	{
		printf(" Header Info Size   : %s \n", DWORD2String(pInfo->dwHeaderInfoSize));
	}
	if (m_bDisp_lpszFileExtension)
	{
		if (pInfo->lpszFileExtension) printf(" File Extension     : %s \n",pInfo->lpszFileExtension);
		else printf(" File Extension     : \n");
	}
}

//////////////////////////////////////////////////////////////////////
// Methode : DelCacheEntry
// Resume : Delete specified cache entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
bool CSearchCache::DelCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo)
{
	return (::DeleteUrlCacheEntry(pInfo->lpszSourceUrlName) ? true : false);
}
