#ifndef _iconv_codec_h__
#define _iconv_codec_h__

#include <string.h>
#include <stdlib.h>
#include <wchar.h>

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

//返回转换后的字节数(包含'\0')
int gb_utf8(const char* pszGB, int gbLen, char* pszUTF8, int UTF8BufLen);
int utf8_gb(const char* pszUTF8, int UTF8Len, char* pszGB, int gbBufLen);

int wc_utf8(const wchar_t* pwszSRC, int srcSize, char* pszUTF8, int UTF8BufLen);
int utf8_wc(const char* pszUTF8, int UTF8Size, wchar_t* pwszTarget, int nTargetSize);

template <typename TTargetType>
class Base_ICONV_Impl
{
	typedef TTargetType _TTargetType_;

public:
	Base_ICONV_Impl()
		 	: m_targetSize(0)
		 	, m_pTargetBuffer(0)
		 	, m_isExternalMem(0)
	{

	}

	~Base_ICONV_Impl()
	{
		if (!m_isExternalMem && m_pTargetBuffer)
		{
			::free(m_pTargetBuffer);
			m_pTargetBuffer = NULL;
		}
	}

public:
	operator _TTargetType_* ()
		{	return m_pTargetBuffer;	}

	operator const _TTargetType_* ()
		{	return m_pTargetBuffer;	}

	int Length() const
		{	return m_targetSize;	}

protected:
	_TTargetType_* TargetBuffer()
		{	return m_pTargetBuffer;	}

	int* SizePtr()
		{	return &m_targetSize;	}

	void Init(_TTargetType_* pTargetBuffer, int nBufferSize)
	{
		if (!pTargetBuffer)
		{
			m_pTargetBuffer = (_TTargetType_*)malloc(nBufferSize);
			memset(m_pTargetBuffer, 0, nBufferSize);
			m_isExternalMem = 0;
		}
		else
		{
			m_isExternalMem = 1;
			m_pTargetBuffer = pTargetBuffer;
		}
	}

private:
	int m_targetSize;
	_TTargetType_* m_pTargetBuffer;
	unsigned char m_isExternalMem;
};
typedef Base_ICONV_Impl<char> Base_ICONV_char;
typedef Base_ICONV_Impl<wchar_t> Base_ICONV_wchar;

class ICONV_GB_UTF8_Str : public Base_ICONV_char
{
public:
	ICONV_GB_UTF8_Str(const char* pszGB, char* pszUTF8 = 0, int UTF8BufLen = 0, int gbLen = -1)
	{
		int nUTF8Size = 0;
		int nGBSize = gbLen <= 0 ? (int)strlen(pszGB) : gbLen;

		if (!pszUTF8 || UTF8BufLen <= 0)
			nUTF8Size = (nGBSize + 1) * 4;
		else
			nUTF8Size = UTF8BufLen;

		Base_ICONV_char::Init(pszUTF8, nUTF8Size);
		*Base_ICONV_char::SizePtr() = gb_utf8(pszGB, nGBSize, Base_ICONV_char::TargetBuffer(), nUTF8Size);
	}

	~ICONV_GB_UTF8_Str()
	{

	}
};

class ICONV_UTF8_GB_Str : public Base_ICONV_char
{
public:
	ICONV_UTF8_GB_Str(const char* pszUTF8, char* pszGB = 0, int GBBufLen = 0, int UTF8Len = -1)
	{
		int nGBSize = 0;
		int nUTF8Size = UTF8Len <= 0 ? (int)strlen(pszUTF8) : UTF8Len;

		if (!pszGB || GBBufLen <= 0)
			nGBSize = (nUTF8Size + 1);
		else
			nGBSize = GBBufLen;

		Base_ICONV_char::Init(pszGB, nGBSize);
		*Base_ICONV_char::SizePtr() = utf8_gb(pszUTF8, nUTF8Size, Base_ICONV_char::TargetBuffer(), nGBSize);
	}

	~ICONV_UTF8_GB_Str()
	{

	}
};

class ICONV_WC_UTF8_Str : public Base_ICONV_char
{
public:
	ICONV_WC_UTF8_Str(const wchar_t* pwszSRC, char* pszUTF8 = 0, int UTF8BufLen = 0, int srcSize = -1)
	{
		int nUTF8Size = 0;
		int nWCSize = srcSize <= 0 ? (int)wcslen(pwszSRC) * sizeof(wchar_t) : srcSize;

		if (!pszUTF8 || UTF8BufLen <= 0)
			nUTF8Size = (nWCSize + 1) * 2;
		else
			nUTF8Size = UTF8BufLen;

		Base_ICONV_char::Init(pszUTF8, nUTF8Size);
		*Base_ICONV_char::SizePtr() = wc_utf8(pwszSRC, nUTF8Size, Base_ICONV_char::TargetBuffer(), nUTF8Size);
	}

	~ICONV_WC_UTF8_Str()
	{

	}
};

class ICONV_UTF8_WC_Str : public Base_ICONV_wchar
{
public:
	ICONV_UTF8_WC_Str(const char* pszUTF8, wchar_t* pszWCTarget = 0, int WCTargetSize = 0, int utf8Len = -1)
	{
		int nWCSize = 0;
		int nUTF8Size = utf8Len <= 0 ? (int)strlen(pszUTF8) : utf8Len;

		if (!pszWCTarget || WCTargetSize <= 0)
			nWCSize = nUTF8Size + 1;
		else
			nWCSize = WCTargetSize;

		Base_ICONV_wchar::Init(pszWCTarget, nWCSize);
		*Base_ICONV_wchar::SizePtr() = utf8_wc(pszUTF8, nUTF8Size, Base_ICONV_wchar::TargetBuffer(), nWCSize);
	}

	~ICONV_UTF8_WC_Str()
	{

	}
};

#endif	//_iconv_codec_h__