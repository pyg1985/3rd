#include "iconv_codec.h"
#include "iconv.h"
#include <string.h>

int gb_utf8(const char* pszGB, int gbLen, char* pszUTF8, int UTF8BufLen)
{
	int nRetSize = 0;
	if (!pszGB || gbLen <= 0 || !pszUTF8 || UTF8BufLen <= 0)
		return nRetSize;

	char** pIn = (char**)&pszGB;
	size_t inLen = gbLen;
	char** pOut = &pszUTF8;
	size_t outLen = UTF8BufLen;

	iconv_t convfd = iconv_open("utf-8", "gb18030");
	if (!convfd)
	{
		convfd = iconv_open("utf-8", "gbk");
		if (!convfd)
			convfd = iconv_open("utf-8", "gb2312");
	}

	if (convfd)
	{
		size_t n = iconv(convfd, pIn, (size_t*)&inLen, pOut, (size_t*)&outLen);
		iconv_close(convfd);
		if (n == 0)
			nRetSize = (int)(UTF8BufLen - outLen);
	}
	return nRetSize;
}

int utf8_gb(const char* pszUTF8, int UTF8Len, char* pszGB, int gbBufLen)
{
	int nRetSize = 0;
	if (!pszUTF8 || UTF8Len <= 0 || !pszGB || gbBufLen <= 0)
		return nRetSize;

	char** pIn = (char**)&pszUTF8;
	size_t inLen = UTF8Len;
	char** pOut = &pszGB;
	size_t outLen = gbBufLen;

	iconv_t convfd = iconv_open("gb18030", "utf-8");
	if (!convfd)
	{
		convfd = iconv_open("gbk", "utf-8");
		if (!convfd)
			convfd = iconv_open("gb2312", "utf-8");
	}

	if (convfd)
	{
		size_t n = iconv(convfd, pIn, (size_t*)&inLen, pOut, (size_t*)&outLen);
		iconv_close(convfd);
		if (n == 0)
			nRetSize = (int)(gbBufLen - outLen);
	}
	return nRetSize;
}