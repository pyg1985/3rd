#ifndef _wc_utf16_h__
#define _wc_utf16_h__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
×Ö·û×ªÂë
*/

#include "iconv.h"

class WCUTF16Codec
{
public:
	WCUTF16Codec();
	~WCUTF16Codec();

	int WCToUTF16(const wchar_t* pwszSRC, int srcSize, char* pszUtf16, int utf16BufSize);
	int UTF16ToWC(const char* pszUtf16, int utf16Size, wchar_t* pwszTarget, int nTargetSize);
};

int wc_utf16(const wchar_t* pwszSRC, int srcSize, char* pszUtf16, int utf16BufSize);
int utf16_wc(const char* pszUtf16, int utf16Size, wchar_t* pwszTarget, int nTargetSize);

#endif	//_wc_utf16_h__