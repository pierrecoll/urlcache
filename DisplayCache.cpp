
#include "stdafx.h"
#include "DisplayCache.h"
#include "CacheInfo.h"
#include "CacheEntry.h"
#include <fstream>
#include <UrlHist.h>

//////////////////////////////////////////////////////////////////////
// Classe : CDisplayCache
// Resume : Display Cache Entry
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Methode : CDisplayCache
// Resume : Constructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CDisplayCache::CDisplayCache()
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
// Methode : CDisplayCache
// Resume : Destructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CDisplayCache::~CDisplayCache()
{

}
//////////////////////////////////////////////////////////////////////
// Methode : GetCacheType
// Resume : Return Cstring corresponding to Type of the Cache Entry
// In : DWORD dwType =  Cache Entry Type
// Out : CString = Type of the Cache Entry in Cstring
//////////////////////////////////////////////////////////////////////
CString GetCacheType(DWORD dwType)
{
	CString str;
	CString strTemp;

	if(dwType & NORMAL_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "NORMAL_CACHE_ENTRY";
	}

	if(dwType & STICKY_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "STICKY_CACHE_ENTRY";
	}

/*	if(dwType & EDITED_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "EDITED_CACHE_ENTRY";
	}*/

	if(dwType & TRACK_OFFLINE_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "TRACK_OFFLINE_CACHE_ENTRY";
	}

	if(dwType & TRACK_ONLINE_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "TRACK_ONLINE_CACHE_ENTRY";
	}

	if(dwType & SPARSE_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "SPARSE_CACHE_ENTRY";
	}

	if(dwType & COOKIE_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "COOKIE_CACHE_ENTRY";
	}

	if(dwType & URLHISTORY_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "URLHISTORY_CACHE_ENTRY";
	}

	str.Format("(0x%08x) => %s", dwType, strTemp);

	return str;
}
//////////////////////////////////////////////////////////////////////
// Methode : DWORD2String
// Resume : Translate Dword to String
// In : DWORD
// Out : CString
//////////////////////////////////////////////////////////////////////
CString DWORD2String(DWORD dw)
{
	CString szBuf("");
	szBuf.Format("%X", dw);
	return (szBuf);
}
//////////////////////////////////////////////////////////////////////
// Methode : GetTime
// Resume : Return Time cache entry
// In : FILETIME ft : the cache entry
// Out : CString Time of the cache entry 
//////////////////////////////////////////////////////////////////////
CString GetTime(FILETIME ft)
{
	SYSTEMTIME st;

	char szLocalDate[255];
	char szLocalTime[255];   

	CString strtemp;

	FileTimeToLocalFileTime( &ft, &ft );
	FileTimeToSystemTime( &ft, &st );

	GetDateFormat(	LOCALE_USER_DEFAULT, 
					DATE_SHORTDATE, 
					&st, 
					NULL,
					szLocalDate, 
					255 );

	GetTimeFormat(	LOCALE_USER_DEFAULT, 
					0, 
					&st, 
					NULL, 
					szLocalTime, 
					255 );
	
	strtemp = szLocalDate;
	strtemp += " | ";
	strtemp += szLocalTime ;
	return (strtemp);
}
//////////////////////////////////////////////////////////////////////
// Methode : CDisplayCache
// Resume : Display cahce entry
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
void CDisplayCache::DisplayCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo)
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
// Methode : DisplayCookies
// Resume : Display Cookies Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CDisplayCache::DisplayCookies()
{
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstCookie();
	
	while(pInfo)
	{
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : DisplayTemporary
// Resume : Display Temporary Internet Files Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CDisplayCache::DisplayTemporary()
{
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First();

	while(pInfo)
	{
		if(CacheEntry.IsTemporary(pInfo->CacheEntryType))
		{	
			DisplayCacheEntry(pInfo);
		}
		pInfo = CacheEntry.Next();
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : DisplayHistory
// Resume : Display History Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CDisplayCache::DisplayHistory()
{
	CCacheEntry CacheEntry;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstHistory();

	while(pInfo)
	{
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}
}
void CDisplayCache::DisplayAll()
{
	DisplayHistory();
	DisplayTemporary();
	DisplayCookies();
}



