#include "StdAfx.h"
#include "base64.h"

static int get6bits(char c)
{
	if(c >= 'A' && c <= 'Z')
		return c - 'A';
	if(c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	if(c >= '0' && c <= '9')
		return c - '0' + 52;
	if(c == '+')
		return 62;
	if(c == '/')
		return 63;
	return -1;
}

int Base64Dec(const char *src, int len, char *r)
{
	char *p = r;
	int i, bits, code;
	int k = 0; // 指示当前code中的bit数
	for(i=0; i<len; i++)
	{
		bits = get6bits(src[i]);
		if(bits < 0)
			break;
		if(k == 0)
		{
			code = (bits << 2);
			k = 6;
		}
		else if(k == 6)
		{
			code |= (bits >> 4);
			*p++ = code;
			k = 4;
			code = (bits << 4);
		}
		else if(k == 4)
		{
			code |= (bits >> 2);
			*p++ = code;
			k = 2;
			code = (bits << 6);
		}
		else if(k == 2)
		{
			code |= bits;
			*p++ = code;
			k = 0;
		}
	}
	return (int)(p - r);
}
