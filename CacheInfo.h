#ifndef __CacheInfo_H__
#define __CacheInfo_H__

//////////////////////////////////////////////////////////////////////
// Classe : CCacheInfo
// Resume : Cache Entry Info
//////////////////////////////////////////////////////////////////////

#include <WinInet.h>

#define COOKIE_CACHE_PREFIX _T("cookie:")
#define HISTORY_CACHE_PREFIX _T("visited:")
#define TEMPORARY_CACHE_PREFIX _T("")

void CreateLowProcess();

class CCacheInfo
{
	public:

		CCacheInfo();
		~CCacheInfo();

	private:

		LPINTERNET_CACHE_ENTRY_INFO		m_pCacheInfo;
		DWORD							m_dwSize;

	protected:

		void							ClearMem();

	public:


		LPINTERNET_CACHE_ENTRY_INFO		GetInfo();
		DWORD							GetSize();

		void							Allocate(int);
		void							InitMemory();
};
#endif