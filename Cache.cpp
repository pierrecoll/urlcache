
#include "stdafx.h"
#include "cache.h"
#include "CacheInfo.h"
#include "CacheEntry.h"
#include <fstream>
#include <UrlHist.h>


CString DWORD2String(DWORD dw);
CString GetTime(FILETIME ft);
CString GetCacheType(DWORD dwType);
BOOL IsExpired(
	__in DWORD dwEntryType,
	__in FILETIME ftExpires,
	__in FILETIME ftModified,
	__in FILETIME ftSynced,
	//__in FILETIME ftDownload,
	__in DWORD dwCacheFlags,
	__in BOOL fAllowHeuristics
	);
unsigned int cDeleted=0;

//////////////////////////////////////////////////////////////////////
// Methode : CCache
// Resume : Constructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CCache::CCache()
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
	m_bDisp_Expiration				= 1;

	m_bSearch_dwStructSize			= 0;
	m_bSearch_lpszSourceUrlName		= 0;
	m_bSearch_lpszLocalFileName		= 0;
	m_bSearch_CacheEntryType		= 0;
	m_bSearch_dwUseCount			= 0;
	m_bSearch_dwHitRate				= 0;
	m_bSearch_dwSizeLow				= 0;
	m_bSearch_dwSizeHigh			= 0;
	m_bSearch_LastModifiedTime		= 0;
	m_bSearch_ExpireTime			= 0;
	m_bSearch_LastAccessTime		= 0;
	m_bSearch_LastSyncTime			= 0;
	m_bSearch_lpHeaderInfo			= 0;
	m_bSearch_dwHeaderInfoSize		= 0;
	m_bSearch_lpszFileExtension		= 0;
}
//////////////////////////////////////////////////////////////////////
// Methode : CCache
// Resume : Destructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CCache::~CCache()
{

}
//////////////////////////////////////////////////////////////////////
// Methode : SearchCookies
// Resume : Search Cookies Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::SearchCookies(LPSTR lpszSearch,BOOL bDelete)
{
	CCacheEntry CacheEntry;
	unsigned int iteration=0;
	unsigned int found=0;
	cDeleted=0;

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstCookie();
	while(pInfo)
	{
		iteration++;
		if (SearchCache(pInfo,lpszSearch,bDelete))
		{
			found++;
		}
		pInfo = CacheEntry.Next();
	}
	printf("\n%d entries found searching  %d entries.\n",found, iteration);
	if (bDelete)
	{
		printf("\n%d entries deleted.\n", cDeleted);
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : SearchTemporary
// Resume : Search Temporary Internet Files Entry
// In : None
// Out : None
// 120713 adding CDeleted counter
//////////////////////////////////////////////////////////////////////
void CCache::SearchTemporary(LPSTR lpszSearch,BOOL bDelete)
{
	CCacheEntry CacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First();
	unsigned int iteration=0;
	unsigned int found=0;
	cDeleted=0;
	while(pInfo)
	{
		if(CacheEntry.IsTemporary(pInfo->CacheEntryType))
		{	
			iteration++;
			if (SearchCache(pInfo,lpszSearch,bDelete))
			{
				found++;
			}
		}
		pInfo = CacheEntry.Next();
	}
	printf("\n%d entries found searching  %d entries.\n", found, iteration);
	if (bDelete)
	{
		printf("\n%d entries deleted.\n", cDeleted);
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : SearchHistory
// Resume : Search History Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::SearchHistory(LPSTR lpszSearch, BOOL bDelete)
{
	CCacheEntry CacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstHistory();
	unsigned int iteration=0;
	unsigned int found=0;
	cDeleted=0;

	while(pInfo)
	{
		iteration++;
		if (SearchCache(pInfo,lpszSearch,bDelete))
		{
			found++;
		}
		pInfo = CacheEntry.Next();
	}
	printf("\n%d entries found searching  %d entries.\n",found, iteration);
	if (bDelete)
	{
		printf("\n%d entries deleted.\n", cDeleted);
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : SearchEmieUserList
// Resume : Search EMIE User List Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::SearchEmieUserList(LPSTR lpszSearch, BOOL bDelete)
{
	CCacheEntry CacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstEmieUser();
	unsigned int iteration = 0;
	unsigned int found = 0;
	cDeleted = 0;

	while (pInfo)
	{
		iteration++;
		if (SearchCache(pInfo, lpszSearch, bDelete))
		{
			found++;
		}
		pInfo = CacheEntry.Next();
	}
	printf("\n%d entries found searching  %d entries.\n", found, iteration);
	if (bDelete)
	{
		printf("\n%d entries deleted.\n", cDeleted);
	}
}
//////////////////////////////////////////////////////////////////////
// Methode : SearchEmieSiteList
// Resume : Search EMIE User List Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::SearchEmieSiteList(LPSTR lpszSearch, BOOL bDelete)
{
	CCacheEntry CacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstEmieSite();
	unsigned int iteration = 0;
	unsigned int found = 0;
	cDeleted = 0;

	while (pInfo)
	{
		iteration++;
		if (SearchCache(pInfo, lpszSearch, bDelete))
		{
			found++;
		}
		pInfo = CacheEntry.Next();
	}
	printf("\n%d entries found searching  %d entries.\n", found, iteration);
	if (bDelete)
	{
		printf("\n%d entries deleted.\n", cDeleted);
	}
}
void CCache::SearchAll(LPSTR lpszSearch,BOOL bDelete)
{
	SearchHistory(lpszSearch,bDelete);
	SearchTemporary(lpszSearch,bDelete);
	SearchCookies(lpszSearch,bDelete);
	//Version 1.21
	SearchEmieUserList(lpszSearch, bDelete);
	SearchEmieUserList(lpszSearch, bDelete);
}


//////////////////////////////////////////////////////////////////////
// Methode : CCache
// Resume : Search cache entry
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
BOOL  CCache::SearchCache(INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSearch,BOOL bDelete)
{
	BOOL bFound = FALSE;
	if (m_bSearch_lpszSourceUrlName == 1)
	{		
		//160614  handling * separator
		char SearchCopy[L_MAX_URL_LENGTH+1];
		lstrcpyn(SearchCopy, lpszSearch, L_MAX_URL_LENGTH);
		char seps[] = "*";
		char *token = NULL;
		char *next_token;
		//Returns a pointer to the next token found in strToken. They return NULL when no more tokens are found. 
		//Each call modifies strToken by substituting a NULL character for the first delimiter that occurs after the returned token.
		token= strtok_s(SearchCopy, seps, &next_token);

		while (token != NULL)
		{
			if ((token) && (strstr(pInfo->lpszSourceUrlName, token)))
			{
				bFound = TRUE;
				DisplayCacheEntry(pInfo);
				if (bDelete)
				{
					DelCacheEntry(pInfo);
				}	
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
				DisplayCacheEntry(pInfo);
				if (bDelete)
				{
					DelCacheEntry(pInfo);
				}
				return bFound;
			}
			token = strtok_s(NULL, seps, &next_token);
		}
	}

	if (m_bSearch_lpHeaderInfo == 1)
	{		
		if ((pInfo->lpHeaderInfo) && (strstr(pInfo->lpHeaderInfo,lpszSearch )))
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
	//10/20/2015
	if (m_bSearch_ExpireTime == 1)
	{
		if ((GetTime(pInfo->ExpireTime)) && (strstr(GetTime(pInfo->ExpireTime), lpszSearch)))
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
// Methode : CCache
// Resume : Display cache entry
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::DisplayCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo)
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
		//else printf(" Source URL Name    : \n");
	}
	if (m_bDisp_lpszLocalFileName)
	{
		if (pInfo->lpszLocalFileName) printf(" Local File Name    : %s \n", pInfo->lpszLocalFileName);
		//else printf(" Local File Name    : \n");
	}
	if (m_bDisp_CacheEntryType)
	{
		printf(" Type               : %s \n", GetCacheType(pInfo->CacheEntryType));
	}
	if (m_bDisp_dwUseCount)
	{
		printf(" Use Count          : %d \n", pInfo->dwUseCount);
	}
	if (m_bDisp_dwHitRate)
	{
		printf(" Hit Rate           : %d \n", pInfo->dwHitRate);
	}
	if (m_bDisp_dwSizeLow)
	{
		printf(" Size Low           : %d \n", pInfo->dwSizeLow);
	}
	if (m_bDisp_dwSizeHigh)
	{
		printf(" Size High          : %d \n", pInfo->dwSizeHigh);
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
	if (m_bDisp_dwHeaderInfoSize)
	{
		printf(" Header Info Size   : %d \n", pInfo->dwHeaderInfoSize);
	}
	if (m_bDisp_lpHeaderInfo)
	{
		if (pInfo->lpHeaderInfo) 
		{
			printf(" Header Info        : ");
			printf("\n\n%s \n",pInfo->lpHeaderInfo);
		}
		//else printf(" Header Info        : \n");

	}

	if (m_bDisp_lpszFileExtension)
	{
		if (pInfo->lpszFileExtension) 
			printf(" File Extension     : %s \n",pInfo->lpszFileExtension);
		//else printf(" No File Extension\n");
	}

	//1.24
	if (m_bDisp_Expiration)
	{
		IsExpired(m_bDisp_CacheEntryType, pInfo->ExpireTime, pInfo->LastModifiedTime, pInfo->LastSyncTime, 0, TRUE);
	}

}

//////////////////////////////////////////////////////////////////////
// Methode : DelCacheEntry
// Resume : Delete specified cache entry
// In : None
// Out : None
// 12/07/13  Error checking 
// 12/07/13  CDeleted counter 
//////////////////////////////////////////////////////////////////////
BOOL CCache::DelCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo)
{
	BOOL bReturn=FALSE;
	bReturn=::DeleteUrlCacheEntry(pInfo->lpszSourceUrlName);
	if (bReturn != TRUE)
	{
		printf("Error calling DeleteUrlCacheEntry for entry with source url name %s: %x\r\n",pInfo->lpszSourceUrlName, GetLastError());
	}	
	else
	{
		cDeleted++;
	}
	return bReturn;
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

	//120713 uncommenting 
	if(dwType & EDITED_CACHE_ENTRY)
	{
		if(!strTemp.IsEmpty())
		{
			strTemp += " | ";
		}
		strTemp += "EDITED_CACHE_ENTRY";
	}

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
//1.23 
/* 
	 Last Modified Time : 1/1/1601 | 01:00:00
	 Expire Time        : 1/1/1601 | 01:00:00
	 +		LastModifiedTime	{dwLowDateTime=0 dwHighDateTime=0 }	_FILETIME
	 +		ExpireTime	{dwLowDateTime=1 dwHighDateTime=0 }	_FILETIME
	 Means non existing in fact
 */
	if (ft.dwHighDateTime == 0) 
	{
		if (ft.dwLowDateTime == 0) 
		{
			return("N/A"); 
		}
		if (ft.dwLowDateTime == 1)
		{
			return("N/A  dwLowDateTime=1 !");
		}
	}
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
// Methode : DisplayCookies
// Resume : Display Cookies Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::DisplayCookies()
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstCookie();
	
	while(pInfo)
	{
		iteration++;
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}
	printf("\n%d cookies found\n", iteration);
}
//////////////////////////////////////////////////////////////////////
// Methode : DisplayTemporary
// Resume : Display Temporary Internet Files Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::DisplayTemporary()
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First();

	while(pInfo)
	{
		if(CacheEntry.IsTemporary(pInfo->CacheEntryType))
		{	
			iteration++;
			DisplayCacheEntry(pInfo);
		}
		pInfo = CacheEntry.Next();
	}
	printf("\n%d temporary internet files found\n", iteration);
}
//////////////////////////////////////////////////////////////////////
// Methode : DisplayHistory
// Resume : Display History Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::DisplayHistory()
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstHistory();

	while(pInfo)
	{
		iteration++;
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}
	printf("\n%d history entries found\n", iteration);
}

//////////////////////////////////////////////////////////////////////
// Methode : DisplayEmieUserList
// Resume : Display Emie User List 
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::DisplayEmieUserList()
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstEmieUser();

	while (pInfo)
	{
		iteration++;
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}
	printf("\n%d EMIE user list entries found\n", iteration);
}

//////////////////////////////////////////////////////////////////////
// Methode : DisplayEmieSiteList
// Resume : Display Emie Site List 
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::DisplayEmieSiteList()
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.FirstEmieSite();

	while (pInfo)
	{
		iteration++;
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}
	printf("\n%d EMIE site list entries files found\n", iteration);
}

void CCache::DisplayAll()
{
	DisplayHistory();
	DisplayTemporary();
	DisplayCookies();
}



//1.23
//%SDXROOT%\inetcore\wininet\common\util.cxx

BOOL
PrintFileTimeInInternetFormat(
__in FILETIME *lpft,
__out_bcount(dwSize) LPSTR lpszBuff,
__in DWORD   dwSize
)
{
	SYSTEMTIME sSysTime;

	if (dwSize < INTERNET_RFC1123_BUFSIZE)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return (FALSE);
	}
	if (!FileTimeToSystemTime(((CONST FILETIME *)lpft), &sSysTime))
	{
		return (FALSE);
	}
	return (InternetTimeFromSystemTime(&sSysTime,
		INTERNET_RFC1123_FORMAT,
		lpszBuff,
		dwSize));

}
//%SDXROOT%\inetcore\wininet\http\cache.cxx
//
// Calculate an implicit validity period of 10% on any 200 response which has a
// Last-Modified header.
//

BOOL
IsHeuristicallyFresh(
LONGLONG llNow,
LONGLONG llModified,
LONGLONG llSynced
)
{
	LONGLONG llImplicitValidityPeriod = 0LL;

	if (llModified == 0)
	{
		//
		// Must be a last modified time to do the heuristic.
		//
		//printf("Must be a last modified time to do the heuristic\n");
		return FALSE;
	}


	llImplicitValidityPeriod = (llSynced - llModified) / 10;
	//printf("IsHeuristicallyFresh now: %I64d ; Synced: %I64d ; Modified: %I64d ; Validity (llSynced - llModified) / 10: %I64d \n", llNow, llSynced, llModified, llImplicitValidityPeriod);
	//printf("IsHeuristicallyFresh : (validity +llSynced) : %I64d ; now - (validity +llSynced) : %I64d\n", (llImplicitValidityPeriod + llSynced), llNow - (llImplicitValidityPeriod + llSynced));
	//
	// Check to see if the current time is within the validity period.
	//
	if (llImplicitValidityPeriod < 0 || llNow >(llImplicitValidityPeriod + llSynced))
	{
		printf("IsHeuristicallyFresh returning FALSE\n");
		return FALSE;
	}
	FILETIME ft;
	ft.dwLowDateTime = (DWORD)((llImplicitValidityPeriod + llSynced) & 0xFFFFFFFF);
	ft.dwHighDateTime = (DWORD)((llImplicitValidityPeriod + llSynced) >> 32);
	char buff[64];
	
	PrintFileTimeInInternetFormat((FILETIME *)&ft, buff, sizeof(buff));
	printf("IsHeuristicallyFresh returning TRUE. Should expire at : %s / %s\n", GetTime(ft), buff);
	return TRUE;
}


VOID
GetCurrentGmtTime(
__out LPFILETIME  pftGmt
)
/*++

Routine Description:
This routine returns the current GMT time

Arguments:

pftGmt - FILETIME strucutre in which this is returned

Returns:

Comments:

--*/
{
	return GetSystemTimeAsFileTime(pftGmt);
}

//%SDXROOT%\inetcore\wininet\http\cache.cxx

/*============================================================================
IsExpired (...)

4/17/00 (RajeevD) Corrected back arrow behavior and wrote detailed comment.
5/15/10 (EricLaw) Corrected FWD_BACK behavior, corrected heuristic expiration.

We have a cache entry for the URL.  This routine determines whether we should
synchronize, i.e. do an if-modified-since request.  This answer depends on 3
factors: navigation mode, expiry on cache entry if any, and syncmode setting.

1. There are two navigation modes:
a. back/forward - using the back or forward buttons in the browser.
b. hyperlinking - everything else.

In b/f case we want to display what was previously shown.  Ideally
wininet would cache multiple versions of a given URL and trident would specify
which one to use when hitting back arrow.  For now, the best we can do is use
the latest (only) cache entry.

2. Expiry may fall into one of 3 buckets:
a. no expiry information
b. expiry in past of current time
c. expiry in future of current time

3. Syncmode may have 3 settings
a. always - err on side of freshest data at expense of net perf.
b. never - err on side of best net perf at expense of stale data.
c. once-per-session - middle-of-the-road setting
d. automatic - This is the default. Follow RFC2616 rules for heuristic
expiration.

Based on these factors, there are 5 possible result values in matrices below:
0   don't sync
1   sync
A   don't sync if URL is heuristically fresh, else fallback to per-session
S   sync if last-sync time was before start of the current session

BACK/FORWARD
When going back or forward, we don't sync.

No Expiry       Expiry in Future    Expiry in Past
Syncmode                        of Last-Access Time of Last-Access Time

Always           0                   0                   0
Never            0                   0                   0
Per-Session      0                   0                   0
Automatic        0                   0                   0


NON-BACK/FORWARD
Expiry info takes precedence, then we look at syncmode.

No Expiry       Expiry in Future    Expiry in Past
Syncmode                        of Current Time     of Current Time

Always           1                   0                   1
Never            0                   0                   1
Per-Session      S                   0                   1
Automatic        A                   0                   1

============================================================================*/

BOOL IsExpired(
	__in DWORD dwEntryType,
	__in FILETIME ftExpires,
	__in FILETIME ftModified,
	__in FILETIME ftSynced,
	//__in FILETIME ftDownload,
	__in DWORD dwCacheFlags,
	__in BOOL fAllowHeuristics
	)
{
	//
	// macro to cast FILETIME to LONGLONG
	//
#define FT2LL(x) ( ((LONGLONG)((x).dwLowDateTime)) | (((LONGLONG)((x).dwHighDateTime))<<32) )
	typedef enum {
		WININET_SYNC_MODE_NEVER = 0,
		WININET_SYNC_MODE_ON_EXPIRY, // bogus
		WININET_SYNC_MODE_ONCE_PER_SESSION,
		WININET_SYNC_MODE_ALWAYS,
		WININET_SYNC_MODE_AUTOMATIC,
		WININET_SYNC_MODE_DEFAULT = WININET_SYNC_MODE_AUTOMATIC
	} WININET_SYNC_MODE;
#define STATIC_CACHE_ENTRY              0x00000080
	LONGLONG dwdwHttpDefaultExpiryDelta = 12 * 60 * 60 * (LONGLONG)10000000;  // 12 hours in 100ns units
	LONGLONG dwdwFtpDefaultExpiryDelta = 24 * 60 * 60 * (LONGLONG)10000000;  // 24 hours in 100ns units
	LONGLONG dwdwSessionStartTime;
	GetCurrentGmtTime((LPFILETIME)&dwdwSessionStartTime);
	LONGLONG dwdwSessionStartTimeDefaultDelta = 0;

	/****************************************/
	BOOL fExpired = FALSE;
	LONGLONG llExpires = 0LL;
	LONGLONG llModified = 0LL;
	LONGLONG llSynced = 0LL;
	LONGLONG llDownload = 0LL;
	LONGLONG llSinceLastCheck = 0LL;
	LONGLONG llSinceDownload = 0LL;
	FILETIME ftNow = {};
	LONGLONG llNow = 0LL;


	DWORD GlobalUrlCacheSyncMode = WININET_SYNC_MODE_DEFAULT;

	llExpires = FT2LL(ftExpires);
	llModified = FT2LL(ftModified);
	llSynced = FT2LL(ftSynced);
	//llDownload = FT2LL(ftDownload);

	GetSystemTimeAsFileTime(&ftNow);
	llNow = FT2LL(ftNow);

	if ((dwCacheFlags & INTERNET_FLAG_FWD_BACK) != 0)
	{
		//
		// BACK/FORWARD CASE.
		//
		// RFC2616 and our documentation of INTERNET_FLAG_FWD_BACK plainly state
		// that BACK/FORWARD navigation should the user what they just saw,
		// regardless of any cache directives.
		//

		fExpired = FALSE;
		goto Done;
	}

	//
	// If the entry has explicit expiration then use it.
	//

	//1.23 
	/*
	Last Modified Time : 1/1/1601 | 01:00:00
	Expire Time        : 1/1/1601 | 01:00:00
	+		LastModifiedTime	{dwLowDateTime=0 dwHighDateTime=0 }	_FILETIME
	+		ExpireTime	{dwLowDateTime=1 dwHighDateTime=0 }	_FILETIME
	Means non existing in fact
	*/
	if (llExpires != 0LL)
	{
		if ((ftExpires.dwHighDateTime == 0) &&  (ftExpires.dwLowDateTime == 1))
		{
			printf("ftExpires.dwLowDateTime == 1 !!!!!!!\n");
			goto FalseExpire;
		}
		printf("Entry has explicit expiration. llNow-llExpires: %I64d \n", llNow-llExpires);
		fExpired = llExpires <= llNow;
		goto Done;
	}
FalseExpire:
	if (!fAllowHeuristics)
	{
		fExpired = TRUE;
		goto Done;
	}

	//
	// No Expires Time was specified. Use SyncMode/Heuristic Expiration
	// rules.
	//

	switch (GlobalUrlCacheSyncMode)
	{
	case WININET_SYNC_MODE_NEVER:
		fExpired = FALSE;
		break;

	case WININET_SYNC_MODE_ALWAYS:
		fExpired = TRUE;
		break;

	default:
	case WININET_SYNC_MODE_AUTOMATIC:

		if ((dwEntryType & STATIC_CACHE_ENTRY) != 0)
		{
			//
			// We believe this entry never actually changes.
			// Check the entry if interval since last checked
			// is less than 25% of the time we had it cached.
			//
			printf("Check the entry if interval since last checked is less than 25 percent of the time we had it cached.IsExpired STATIC_CACHE_ENTRY Now: %I64d ; Synced: %I64d ; Now-Synced: %I64d \n", llNow, llSynced, llNow-llSynced);
			/*
			struct CACHE_ENTRY_INFOEXW : INTERNET_CACHE_ENTRY_INFOW
			{
			FILETIME ftDownload;
			FILETIME ftPostCheck;
			};

			typedef struct _INTERNAL_CACHE_ENTRY_INFOEX {
			DWORD dwStructSize;         // version of cache system.
			PSTR lpszSourceUrlName;     // embedded pointer to the URL name string.
			PWSTR lpwszLocalFileName;   // embedded pointer to the local file name.
			DWORD CacheEntryType;       // cache type bit mask.
			DWORD dwUseCount;           // current users count of the cache entry.
			DWORD dwHitRate;            // num of times the cache entry was retrieved.
			DWORD dwSizeLow;            // low DWORD of the file size.
			DWORD dwSizeHigh;           // high DWORD of the file size.
			FILETIME LastModifiedTime;  // last modified time of the file in GMT format.
			FILETIME ExpireTime;        // expire time of the file in GMT format
			FILETIME LastAccessTime;    // last accessed time in GMT format
			FILETIME LastSyncTime;      // last time the URL was synchronized
			// with the source
			PSTR lpHeaderInfo;          // embedded pointer to the header info.
			DWORD dwHeaderInfoSize;     // size of the above header.
			PSTR lpszFileExtension;     // File extension used to retrive the urldata as a file.
			DWORD dwExemptDelta;        // Exemption delta from last access time.
			FILETIME ftDownload;
			FILETIME ftPostCheck;
			} INTERNAL_CACHE_ENTRY_INFOEX, *PINTERNAL_CACHE_ENTRY_INFOEX;
			*/
			llSinceLastCheck = llNow - llSynced;
			llSinceDownload = llNow - llDownload;

			if (llSinceLastCheck < llSinceDownload / 4)
			{
				fExpired = FALSE;
				break;
			}
		}

		if (IsHeuristicallyFresh(llNow, llModified, llSynced))
		{
			fExpired = FALSE;
			break;
		}

		//
		// else intentional fall through to once-per-session rules.
		//

	case WININET_SYNC_MODE_ONCE_PER_SESSION:

		fExpired = TRUE;

		//
		// If the server didn't send us expiry time or lastmodifiedtime
		// then this entry is expired
		//

		if ((dwCacheFlags & INTERNET_FLAG_HYPERLINK) != 0 && !llModified)
		{
			printf("Hyperlink semantics\n");
			ASSERT(fExpired);
			break;
		}

		//
		// Resync if we've not resync'd this session, or if
		// dwdwHttpDefaultExpiryDelta has been exceeded
		//

		if (dwdwSessionStartTime < llSynced &&
			(llNow < llSynced + dwdwHttpDefaultExpiryDelta))
		{
			fExpired = FALSE;
		}
		break;
	}

Done:


	char buff[64];
	/*PrintFileTimeInInternetFormat(&ftNow, buff, sizeof(buff));
	printf("Current Time: %s\n", buff);
	PrintFileTimeInInternetFormat(&ftExpires, buff, sizeof(buff));
	printf("Expiry Time: %s\n", buff);
	PrintFileTimeInInternetFormat(&ftSynced, buff, sizeof(buff));
	printf("Last Sync Time: %s\n", buff);*/
	//PrintFileTimeInInternetFormat(&ftDownload, buff, sizeof(buff));
	//printf("Last Download Time: %s\n", buff);
	PrintFileTimeInInternetFormat((FILETIME *)&dwdwSessionStartTime, buff, sizeof(buff));
	//printf("Session start Time: %s\n", buff);
	//printf("CacheFlags=%x\n", dwCacheFlags);
	printf("CheckExpired: Url %s Expired\r\n\r\n", (fExpired ? "" : "Not"));


	return fExpired;
}
