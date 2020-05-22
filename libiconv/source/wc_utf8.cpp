#include "iconv_codec.h"
#include "iconv.h"

int wc_utf8(const wchar_t* pwszSRC, int srcSize, char* pszUTF8, int UTF8BufLen)
{
	int nRetSize = 0;
	if (!pwszSRC || srcSize <= 0 || !pszUTF8 || UTF8BufLen <= 0)
		return nRetSize;

	char** pIn = (char**)&pwszSRC;
	size_t inLen = srcSize;
	char** pOut = &pszUTF8;
	size_t outSize = UTF8BufLen;

	iconv_t convfd = iconv_open("utf-8", "wchar_t");
	if (convfd)
	{
		size_t n = iconv(convfd, pIn, (size_t*)&inLen, pOut, (size_t*)&outSize);
		iconv_close(convfd);
		if (n == 0)
			nRetSize = (int)(UTF8BufLen - outSize);
	}
	return nRetSize;
}

int utf8_wc(const char* pszUTF8, int UTF8Size, wchar_t* pwszTarget, int nTargetSize)
{
	int nRetSize = 0;
	if (!pszUTF8 || UTF8Size <= 0 || !pwszTarget || nTargetSize <= 0)
		return nRetSize;

	char** pIn = (char**)&pszUTF8;
	size_t inLen = UTF8Size;
	char** pOut = (char**)&pwszTarget;
	size_t outSize = nTargetSize;

	iconv_t convfd = iconv_open("wchar_t", "utf-8");
	if (convfd)
	{
		size_t n = iconv(convfd, pIn, (size_t*)&inLen, pOut, (size_t*)&outSize);
		iconv_close(convfd);
		if (n == 0)
			nRetSize =(int)(nTargetSize - outSize);
	}
	return nRetSize;
}