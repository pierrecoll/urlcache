#ifndef __CSearchCache_H__
#define __CSearchCache_H__

#include <WinInet.h>

//////////////////////////////////////////////////////////////////////
// Classe : CSearchCache
// Resume : Search Cache Entry
//////////////////////////////////////////////////////////////////////

class CSearchCache
{
		public:

		bool m_bDisp_dwStructSize;			
		bool m_bDisp_lpszSourceUrlName;
		bool m_bDisp_lpszLocalFileName;
		bool m_bDisp_CacheEntryType;
		bool m_bDisp_dwUseCount;
		bool m_bDisp_dwHitRate;
		bool m_bDisp_dwSizeLow;
		bool m_bDisp_dwSizeHigh;
		bool m_bDisp_LastModifiedTime;
		bool m_bDisp_ExpireTime;
		bool m_bDisp_LastAccessTime;
		bool m_bDisp_LastSyncTime;;
		bool m_bDisp_lpHeaderInfo;
		bool m_bDisp_dwHeaderInfoSize;
		bool m_bDisp_lpszFileExtension;

		bool m_b_String;
		bool m_b_Remove;

	public:

		CSearchCache();
		~CSearchCache();
		BOOL CSearchCache::SearchCache(INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSourceUrlName,BOOL bDelete);
		void SearchCookies		(LPSTR lpszSourceUrlName,BOOL bDelete);
		void SearchTemporary	(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchHistory		(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchAll			(LPSTR lpszSourceUrlName, BOOL bDelete);
		void		DisplayCacheEntry(INTERNET_CACHE_ENTRY_INFO*);
		bool CSearchCache::DelCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo);
};
#endif
