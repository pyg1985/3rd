#ifndef _base64_h__
#define _base64_h__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C"
{
#endif

int Base64Enc(const char *src, int len, char *r);
int Base64Dec(const char *src, int len, char *r);

#ifdef __cplusplus
}
#endif

#endif /* _base64_h_ */
