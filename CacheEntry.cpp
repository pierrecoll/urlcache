//////////////////////////////////////////////////////////////////////
// Classe : CacheEntry
// Resume : Enumerate Cache Entry
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CacheEntry.h"

#define CACHE_BUFSIZE 4096

//////////////////////////////////////////////////////////////////////
// Methode : CCacheEntry
// Resume : Constructeur
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CCacheEntry::CCacheEntry()
{

}
//////////////////////////////////////////////////////////////////////
// Methode : ~CCacheEntry
// Resume : Destructor
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
CCacheEntry::~CCacheEntry()
{

}
//////////////////////////////////////////////////////////////////////
// Methode : First
// Resume : Return the first cache Entry
// In : LPCTSTR lpszSearchPattern
// Out : LPINTERNET_CACHE_ENTRY_INFO
// 120713 Adding error checking
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::First(LPCTSTR lpszSearchPattern)
{
	//Close previous handle

	if(m_CacheHandle == NULL)
	{
		BOOL bReturn=FALSE;
		bReturn=::FindCloseUrlCache(m_CacheHandle);
		if (bReturn != TRUE)
		{
			printf("Error calling FindCloseUrlCache: %x\r\n", GetLastError());
		}	

	}

	LPINTERNET_CACHE_ENTRY_INFO pInfoReturn = NULL;
	DWORD dwSize = CACHE_BUFSIZE;
	bool bContinue = true;

	//Contiue the make the first find until done with a large enough cache size
	do
	{
		if(m_CacheInfo.GetSize() < dwSize)
		{
			m_CacheInfo.Allocate(dwSize);
		}
		else
		{
			m_CacheInfo.InitMemory();
		}

		m_CacheHandle = ::FindFirstUrlCacheEntryEx(lpszSearchPattern, 0, 0xFFFFFFFF, 0, m_CacheInfo.GetInfo(), &dwSize,0,0,0);
		
		if(m_CacheHandle)
		{

			//Return info
			pInfoReturn = m_CacheInfo.GetInfo();

			//Stop the loop
			bContinue = false;
		}
		else
		{
			if(ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
			{
				//dwSize should now be set to the size needed
				bContinue = true;
			}
			else
			{
				//Some other error
				bContinue = false;
			}
		}
	}while(bContinue);

	return pInfoReturn;
}
//////////////////////////////////////////////////////////////////////
// Methode : FirstCookie
// Resume : Find Cookies
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::FirstCookie()
{
	return First(_T("cookie:"));
}
//////////////////////////////////////////////////////////////////////
// Methode : FirstHistory
// Resume : Find History
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::FirstHistory()
{
	return First(_T("visited:"));
}
//////////////////////////////////////////////////////////////////////
// Methode : FirstEmieUSer
// Resume : Find EMIE user list entry
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::FirstEmieUser()
{
	return First(EMIE_USERLIST_CACHE_PREFIX);
}
//////////////////////////////////////////////////////////////////////
// Methode : FirstDOMStore
// Resume : Find DOMStore entry
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::FirstEmieSite()
{
	return First(EMIE_SITELIST_CACHE_PREFIX);
}

//////////////////////////////////////////////////////////////////////
// Methode : FirstDOMStore
// Resume : Find EMIE site list entry
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::FirstDOMStore()
{
	return First(DOMSTORE_CACHE_PREFIX);
}

//////////////////////////////////////////////////////////////////////
// Methode : Firstiedownload
// Resume : Find iedownload entry
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::Firstiedownload()
{
	return First(IEDOWNLOAD_CACHE_PREFIX);
}

//////////////////////////////////////////////////////////////////////
// Methode : Next
// Resume : Find next Url Cache Entry 
// In : None
// Out : LPINTERNET_CACHE_ENTRY_INFO
//////////////////////////////////////////////////////////////////////
LPINTERNET_CACHE_ENTRY_INFO CCacheEntry::Next()
{
	if(m_CacheHandle == NULL)
	{
		return NULL;
	}

	LPINTERNET_CACHE_ENTRY_INFO pInfoReturn = NULL;
	DWORD dwSize = m_CacheInfo.GetSize();
	bool bContinue = true;

	//find First find Next until  cache size
	do
	{
		if(m_CacheInfo.GetSize() < dwSize)
		{
			m_CacheInfo.Allocate(dwSize);
		}
		else
		{
			m_CacheInfo.InitMemory();
		}

		BOOL bFindNext = ::FindNextUrlCacheEntry(m_CacheHandle, m_CacheInfo.GetInfo(), &dwSize);

		if(bFindNext)
		{
			//Return info
			pInfoReturn = m_CacheInfo.GetInfo();
			bContinue = false;
		}
		else
		{
			DWORD dwError = ::GetLastError();
			switch(dwError)
			{
			case ERROR_INSUFFICIENT_BUFFER:
				bContinue = true;
				break;
			case ERROR_NO_MORE_ITEMS:
				bContinue = false;
				break;
			default:
				bContinue = false;
				break;
			}
		}
	}while(bContinue);

	return pInfoReturn;
}
//////////////////////////////////////////////////////////////////////
// Methode : IsCookie
// Resume : Test if Cache entry is a Cookies
// In : None
// Out : bool
//////////////////////////////////////////////////////////////////////
bool CCacheEntry::IsCookie(DWORD dwType)
{
	return ((dwType & COOKIE_CACHE_ENTRY) == COOKIE_CACHE_ENTRY);
}
//////////////////////////////////////////////////////////////////////
// Methode : IsHistory
// Resume : Test if Cache entry is a History 
// In : None
// Out : bool
//////////////////////////////////////////////////////////////////////
bool CCacheEntry::IsHistory(DWORD dwType)
{
	return ((dwType & URLHISTORY_CACHE_ENTRY) == URLHISTORY_CACHE_ENTRY);
}
//////////////////////////////////////////////////////////////////////
// Methode : IsTemporary
// Resume : Test if Cache entry is a Temporary Internet File 
// In : None
// Out : bool
//////////////////////////////////////////////////////////////////////
bool CCacheEntry::IsTemporary(DWORD dwType)
{
	return (!IsCookie(dwType) && !IsHistory(dwType));
}


