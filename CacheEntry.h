#ifndef __CacheEntry_H__
#define __CacheEntry_H__
//////////////////////////////////////////////////////////////////////
// Classe : CacheEntry
// Resume : Enumerate Cache Entry
//////////////////////////////////////////////////////////////////////

#include "CacheInfo.h"
#include <WinInet.h>

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

		LPINTERNET_CACHE_ENTRY_INFO		Next();

		bool							IsCookie(DWORD);
		bool							IsHistory(DWORD);
		bool							IsTemporary(DWORD);

};
#endif