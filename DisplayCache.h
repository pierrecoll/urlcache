#ifndef __CDISPLAYCACHE_H__
#define __CDISPLAYCACHE_H__

#include <WinInet.h>

//////////////////////////////////////////////////////////////////////
// Classe : CDisplayCache
// Resume : Display Cache Entry
//////////////////////////////////////////////////////////////////////

class CDisplayCache
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

		CDisplayCache();
		~CDisplayCache();
		void		DisplayCacheEntry(INTERNET_CACHE_ENTRY_INFO*);


	public:

		void DisplayCookies		();
		void DisplayTemporary	();
		void DisplayHistory		();
		void DisplayAll			();

};
#endif
