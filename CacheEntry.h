#ifndef __CacheEntry_H__
#define __CacheEntry_H__
//////////////////////////////////////////////////////////////////////
// Classe : CacheEntry
// Resume : Enumerate Cache Entry
//////////////////////////////////////////////////////////////////////

#include "CacheInfo.h"
#include <WinInet.h>

#define EMIE_SITELIST_CACHE_NAME _T("EmieSiteList")
#define EMIE_SITELIST_CACHE_PREFIX _T("EmieSiteList:")
#define EMIE_USERLIST_CACHE_NAME _T("EmieUserList")
#define EMIE_USERLIST_CACHE_PREFIX _T("EmieUserList:")
#define DOMSTORE_CACHE_PREFIX _T("DOMStore:")

class CCacheEntry  
{
	public:

		CCacheEntry();
		~CCacheEntry();

	private:

		CCacheInfo	m_CacheInfo;
		HANDLE		m_CacheHandle;

	public:

		LPINTERNET_CACHE_ENTRY_INFO		First(LPCTSTR lpszSearchPattern = NULL);
		LPINTERNET_CACHE_ENTRY_INFO		FirstCookie();
		LPINTERNET_CACHE_ENTRY_INFO		FirstHistory();
		LPINTERNET_CACHE_ENTRY_INFO		FirstEmieUser();
		LPINTERNET_CACHE_ENTRY_INFO		FirstEmieSite();
		LPINTERNET_CACHE_ENTRY_INFO		FirstDOMStore();

		LPINTERNET_CACHE_ENTRY_INFO		Next();

		bool							IsCookie(DWORD);
		bool							IsHistory(DWORD);
		bool							IsTemporary(DWORD);

};
#endif