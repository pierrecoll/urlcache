#ifndef __ClearCache_H__
#define __ClearCache_H__

//////////////////////////////////////////////////////////////////////
// Classe : CacheEntry
// Resume : Clear Selected Cache Entry
//////////////////////////////////////////////////////////////////////

#include <WinInet.h>

class CClearCache
{
	public:

		CClearCache();
		~CClearCache();
		bool		DelCacheEntry(INTERNET_CACHE_ENTRY_INFO*);

		bool m_bSearch_lpszSourceUrlName;
		bool m_bSearch_lpszLocalFileName;
		bool m_bSearch_ExpireTime;
		bool m_bSearch_lpHeaderInfo;
		int nEntriesDeleted;
		int nEntries;

	public:
		void		Clear(LPSTR);
		void		ClearAll();
		BOOL  CClearCache::SearchCache(INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSearch);

};
#endif
