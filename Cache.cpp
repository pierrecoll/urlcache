
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
// Methode : Search
// Resume : Search an Entry in a container
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::Search(LPSTR lpszContainer, LPSTR lpszSearch, BOOL bDelete)
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	unsigned int found = 0;
	cDeleted = 0;
	TCHAR EntryName[256];

	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First(lpszContainer);

	while (pInfo)
	{
		iteration++;
		if (SearchCache(pInfo, lpszSearch, bDelete))
		{
			found++;
		}
		pInfo = CacheEntry.Next();
	}

	if (lpszContainer == "") {
		lstrcpy(EntryName, "temporary internet files");
	}
	else
	{
		lstrcpy(EntryName, lpszContainer);
		EntryName[strlen(lpszContainer) - 1] = '\0';
	}

	if (iteration == 0)
	{
		printf("\nNo %s entries to search.\n", EntryName);
		return;
	}

	if (found == 0)
	{		
		printf("\nNo entry found searching %d %s entries.\n",  iteration, EntryName);
	}
	else if (found == 1)
	{
		printf("\nOne entry found searching %d %s entries.\n",  iteration, EntryName);
	}
	else
	{
		printf("\n%d entries found searching %d %s entries.\n", found, iteration, EntryName);
	}
	
	if (bDelete)
	{
		printf("\n%d entries deleted.\n", cDeleted);
	}
}

void CCache::SearchAll(LPSTR lpszSearch,BOOL bDelete)
{
	Search(HISTORY_CACHE_PREFIX,lpszSearch,bDelete);
	Search(TEMPORARY_CACHE_PREFIX,lpszSearch,bDelete);
	Search(COOKIE_CACHE_PREFIX, lpszSearch,bDelete);
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
		//210113  handling + separator

		return (SearchCacheByItem(pInfo->lpszSourceUrlName, pInfo, lpszSearch, bDelete));

	}
	if (m_bSearch_lpszLocalFileName == 1)
	{		
		return (SearchCacheByItem(pInfo->lpszLocalFileName, pInfo, lpszSearch, bDelete));
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
// Resume : Search cache entry work function
// In : INTERNET_CACHE_ENTRY_INFO
// Out : None
//////////////////////////////////////////////////////////////////////
BOOL  CCache::SearchCacheByItem(LPSTR StringToSearch, INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSearch, BOOL bDelete)
{
	BOOL bFound = FALSE;

	//160614  handling * separator
	//210113  handling + separator

	char SearchCopy[MAX_PATH + 1] = "";
	lstrcpynA(SearchCopy, lpszSearch, MAX_PATH);
	char* seps = new char[2];
	if (strrchr(SearchCopy, '*'))
	{
		seps[0] = '*';
		seps[1] = '\0';
		char* token = NULL;
		char* next_token;
		//Returns a pointer to the next token found in strToken. They return NULL when no more tokens are found. 
		//Each call modifies strToken by substituting a NULL character for the first delimiter that occurs after the returned token.
		//returns SearchCopy when no delimiter is present!
		token = strtok_s(SearchCopy, seps, &next_token);

		while (token != NULL)
		{
			if ((token) && (strstr(StringToSearch, token)))
			{
				bFound = TRUE;
				DisplayCacheEntry(pInfo);
				if (bDelete)
				{
					DelCacheEntry(pInfo);
				}
				return bFound;
			}
			token = strtok_s((LPSTR)NULL, seps, &next_token);
		}
	}
	else if (strrchr(SearchCopy, '+'))
	{
		seps[0] = '+';
		seps[1] = '\0';
		char* token = NULL;
		char* next_token;
		//Returns a pointer to the next token found in strToken. They return NULL when no more tokens are found. 
		//Each call modifies strToken by substituting a NULL character for the first delimiter that occurs after the returned token.
		//returns SearchCopy when no delimiter is present!
		token = strtok_s(SearchCopy, seps, &next_token);
		bFound = FALSE;
		while (token != NULL)
		{
			if ((token) && (strstr(StringToSearch, token)))
			{
				bFound = TRUE;
			}
			else
			{
				bFound = FALSE;
				return FALSE;
			}
			token = strtok_s((LPSTR)NULL, seps, &next_token);
		}
		if (bFound == TRUE)
		{
			DisplayCacheEntry(pInfo);
			if (bDelete)
			{
				DelCacheEntry(pInfo);
			}
			return bFound;
		}
	}
	else  //no separator
	{
		if (strstr(StringToSearch, SearchCopy))
		{
			bFound = TRUE;
			DisplayCacheEntry(pInfo);
			if (bDelete)
			{
				DelCacheEntry(pInfo);
			}
			return bFound;
		}
		else
		{
			return FALSE;
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
			printf(" Structure Size     : %u \n", pInfo->dwStructSize);
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
		printf(" Use Count          : %u \n", pInfo->dwUseCount);
	}
	if (m_bDisp_dwHitRate)
	{
		printf(" Hit Rate           : %u \n", pInfo->dwHitRate);
	}
	if (m_bDisp_dwSizeLow)
	{
		printf(" Size Low           : %u \n", pInfo->dwSizeLow);
	}
	if (m_bDisp_dwSizeHigh)
	{
		printf(" Size High          : %u \n", pInfo->dwSizeHigh);
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
		printf(" Header Info Size   : %u \n", pInfo->dwHeaderInfoSize);
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

	str.Format((LPCSTR)"(0x%08x) => %s", dwType, strTemp);

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
	SYSTEMTIME st = {};

	char szLocalDate[255]="";
	char szLocalTime[255]="";   

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
					(LPSTR)szLocalDate, 
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
// Methode : Display
// Resume : Display  Entry
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
void CCache::Display(LPSTR lpszContainer)
{
	CCacheEntry CacheEntry;
	unsigned int iteration = 0;
	LPINTERNET_CACHE_ENTRY_INFO pInfo = CacheEntry.First(lpszContainer);
	TCHAR EntryName[256];
	while (pInfo)
	{
		iteration++;
		DisplayCacheEntry(pInfo);
		pInfo = CacheEntry.Next();
	}

	if (lpszContainer == "") {
		lstrcpy(EntryName, "temporary internet files");
	}
	else
	{
		lstrcpy(EntryName, lpszContainer);
		EntryName[strlen(lpszContainer)-1] = '\0';
	}
	
	if (iteration == 0)
	{ 
		printf("\nno %s entry\n", EntryName);
	}
	else if (iteration == 1)
	{
		printf("\none %s entry\n",  EntryName);
	}
	else
	{
		printf("\n%d %s entries\n", iteration, EntryName);
	}
}


void CCache::DisplayAll()
{
	Display(HISTORY_CACHE_PREFIX);
	Display(TEMPORARY_CACHE_PREFIX);
	Display(COOKIE_CACHE_PREFIX);
}

//1.23

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
			// Check the entry if interval since last checked
			// is less than 25% of the time we had it cached.
			//
			printf("Check the entry if interval since last checked is less than 25 percent of the time we had it cached.IsExpired STATIC_CACHE_ENTRY Now: %I64d ; Synced: %I64d ; Now-Synced: %I64d \n", llNow, llSynced, llNow-llSynced);

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
