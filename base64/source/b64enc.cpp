#include "StdAfx.h"
#include "base64.h"

static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64Enc(const char *src, int len, char *r)
{
	unsigned char* q=(unsigned char*)src;
	char *p = r;
	for(; len >= 3; len -= 3, q += 3)
	{
	    *p++ = base64[q[0] >> 2];
	    *p++ = base64[((q[0] & 0x3) << 4) | (q[1] >> 4)];
		*p++ = base64[((q[1] & 0xF) << 2) | (q[2] >> 6)];
		*p++ = base64[q[2] & 0x3F];
	}

	if(len > 0)
	{
	    *p++ = base64[q[0] >> 2];
		if(len == 1 )
		{
		    *p++ = base64[(q[0] & 0x3) << 4];
			*p++ = '=';
		}
		else // len == 2
		{
		    *p++ = base64[((q[0] & 0x3) << 4) | (q[1] >> 4)];
			*p++ = base64[(q[1] & 0xF) << 2];
		}
		*p++ = '=';
    }
	return (int)(p - r);
}
