
#ifndef __VVV_CRYPTO_H__
#define __VVV_CRYPTO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aes.h" 

#ifdef __cplusplus
extern "C" {
#endif

	
//const char* utc_key = "1494374400000000";
//const char* utc_iv = "1494374400001000";

char* md5(char *dst, const char *src, size_t src_length);
int   venc_base64 (char* dst, int dstLen, char* src, int srcLen);
int   venc_padding_char (char* dst, int dstLen, char* src, char c);
int   venc_padding_pkcs5 (char* dst, int dstLen, char* src);
void  venc_aes128_init (char* key, char* iv);
void  venc_aes128 (char* dst, int dstLen, char* src, int srcLen);
int   venc_aes128_to_base64 (char* dst, int dstLen, char* src);

#ifdef __cplusplus
}
#endif

#endif  //__VVV_CRYPTO_H__

