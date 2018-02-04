#ifndef __CCache_H__
#define __CCache_H__

#include <WinInet.h>

//////////////////////////////////////////////////////////////////////
// Classe : CCache
// Resume : Display Cache Entry
//////////////////////////////////////////////////////////////////////

class CCache
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
		bool m_bDisp_Expiration;

		bool m_bSearch_dwStructSize;			
		bool m_bSearch_lpszSourceUrlName;
		bool m_bSearch_lpszLocalFileName;
		bool m_bSearch_CacheEntryType;
		bool m_bSearch_dwUseCount;
		bool m_bSearch_dwHitRate;
		bool m_bSearch_dwSizeLow;
		bool m_bSearch_dwSizeHigh;
		bool m_bSearch_LastModifiedTime;
		bool m_bSearch_ExpireTime;
		bool m_bSearch_LastAccessTime;
		bool m_bSearch_LastSyncTime;;
		bool m_bSearch_lpHeaderInfo;
		bool m_bSearch_dwHeaderInfoSize;
		bool m_bSearch_lpszFileExtension;

		bool m_b_String;
		bool m_b_Remove;
	
	public:

		CCache();
		~CCache();


	public:

		void DisplayCookies		();
		void DisplayTemporary	();
		void DisplayHistory		();
		void DisplayEmieUserList();
		void DisplayEmieSiteList();
		void DisplayDOMStore();
		void Displayiedownload();
		void DisplayAll();


		BOOL  SearchCache(INTERNET_CACHE_ENTRY_INFO* pInfo, LPSTR lpszSearch, BOOL bDelete);
		void SearchCookies		(LPSTR lpszSourceUrlName,BOOL bDelete);
		void SearchTemporary	(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchHistory		(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchEmieUserList		(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchEmieSiteList		(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchDOMStore (LPSTR lpszSourceUrlName, BOOL bDelete);
		void Searchiedownload(LPSTR lpszSourceUrlName, BOOL bDelete);
		void SearchAll			(LPSTR lpszSourceUrlName, BOOL bDelete);
		void DisplayCacheEntry(INTERNET_CACHE_ENTRY_INFO*);
		BOOL DelCacheEntry(INTERNET_CACHE_ENTRY_INFO* pInfo);
};
#endif
