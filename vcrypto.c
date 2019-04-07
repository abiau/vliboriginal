#include "vcrypto.h"


static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static char vstatic_key[32];
static char vstatic_iv[32];

int venc_base64 (char* dst, int dstLen, char* src, int srcLen)
{
	int needLen = 4 * ((srcLen/3) + (srcLen%3==0)?0:1) + 1;
	if (dstLen < needLen) {return -1;}

	memset (dst, 0, dstLen);
    
    int i = 0;
    int j = 0;
    int size = 0;
    unsigned char buf[4];
    unsigned char tmp[3];
	int len = srcLen;
    
	while (len--) {
        // read up to 3 bytes at a time into `tmp'
        tmp[i++] = *(src++);

        // if 3 bytes read then encode into `buf'
        if (3 == i) {
            buf[0] = (tmp[0] & 0xfc) >> 2;
            buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
            buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
            buf[3] = tmp[2] & 0x3f;

            for (i = 0; i < 4; ++i) {
                dst[size++] = base64_table[buf[i]];
            }

            i = 0; // reset index
        }
    }

    // remainder
    if (i > 0) {
        // fill `tmp' with `\0' at most 3 times
        for (j = i; j < 3; ++j) {
            tmp[j] = '\0';
        }

        // perform same codec as above
        buf[0] = (tmp[0] & 0xfc) >> 2;
        buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
        buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
        buf[3] = tmp[2] & 0x3f;

        for (j = 0; (j < i + 1); ++j) {
            dst[size++] = base64_table[buf[j]];
        }

        // while there is still a remainder
        // append `=' to `dst'
        while ((i++ < 3)) {
            dst[size++] = '=';
        }
    }

	return strlen(dst);
}


int venc_aes128_to_base64 (char* dst, int dstLen, char* src)
{
	if (dstLen<128) {return -1;}
	memset (dst, 0, dstLen);
	
	int len=0;
	char tmp1[128];
	char tmp2[128];
	char tmp3[128];
	memset(tmp1, 0, sizeof(tmp1));
	memset(tmp2, 0, sizeof(tmp2));
	memset(tmp3, 0, sizeof(tmp3));
	len =  venc_padding_char (tmp1, sizeof(tmp1), src, '~' );
	len = venc_padding_pkcs5 (tmp2, sizeof(tmp2), tmp1     );
	             venc_aes128 (tmp3, sizeof(tmp3), tmp2, len);
	len =        venc_base64 (dst,  sizeof(dst),  tmp3, len);
#if 0
	printf("padding_char=%s\n", tmp1);
	printf("padding_pkcs5=%s\n", tmp2);
	printf("aes128=%s\n", tmp3);
	printf("base64=%s\n", dst);
#endif
	return len;
}
	


void venc_aes128_init (char* key, char* iv)
{
	memset(vstatic_key, 0, sizeof(vstatic_key));
	snprintf (vstatic_key, sizeof(vstatic_key), "%s", key);

	memset(vstatic_iv, 0, sizeof(vstatic_iv));
	snprintf (vstatic_iv, sizeof(vstatic_iv), "%s", iv);
}

void venc_aes128 (char* dst, int dstLen, char* src, int srcLen)
{
	if (dstLen<128) {return ;}

	memset (dst, 0, dstLen);
	snprintf (dst, dstLen, "%s", src);
	
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, (unsigned char*)vstatic_key, (unsigned char*)vstatic_iv);
	//printf ("key=%s, iv=%s\n", vstatic_key, vstatic_iv);
    AES_CBC_encrypt_buffer(&ctx, (unsigned char*)dst, srcLen);
	
	return ;
}

int venc_padding_char (char* dst, int dstLen, char* src, char c)
{
	const char paddingChar = c;
	const int  nBlockSize = 16;
	const int  len = strlen(src);
	const int  nRemainder = len % nBlockSize;
	const int  nGroup = len / nBlockSize;
	const int  nSize = nBlockSize * (nGroup+1);
	int   i;

	if (dstLen < nSize) {return -1;}

	memset (dst, 0, dstLen);
	snprintf (dst, dstLen, "%s", src);
	
	//printf ("len=%d, nRemainder=%d, nGroup=%d, nSize=%d, paddingChar=%d\n", len, nRemainder, nGroup, nSize, paddingChar);

	for (i=0; i<(nBlockSize-nRemainder); i++)
	{
		dst[len+i] = paddingChar;
	}

	return strlen(dst);
}

int venc_padding_pkcs5 (char* dst, int dstLen, char* src)
{
	const int nBlockSize = 16;
	const int len = strlen(src);
	const int nRemainder = len % nBlockSize;
	const int nGroup = len / nBlockSize;
	const int nSize = nBlockSize * (nGroup+1);
	int   i;
	char  paddingChar=0;

	if (dstLen < nSize) {return -1;}

	memset (dst, 0, dstLen);
	snprintf (dst, dstLen, "%s", src);

	paddingChar = (nRemainder==0) ? nBlockSize : (nBlockSize-nRemainder);
	
	//printf ("len=%d, nRemainder=%d, nGroup=%d, nSize=%d, paddingChar=%d\n", len, nRemainder, nGroup, nSize, paddingChar);

	for (i=0; i<paddingChar; i++)
	{
		dst[len+i] = paddingChar;
	}

	return strlen(dst);

}



// Constants are the integer part of the sines of integers (in radians) * 2^32.
static const uint32_t k[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

// r specifies the per-round shift amounts
static const uint32_t r[] = {
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

static void to_bytes(uint32_t val, char *bytes)
{
	bytes[0] = (char) val;
	bytes[1] = (char) (val >> 8);
	bytes[2] = (char) (val >> 16);
	bytes[3] = (char) (val >> 24);
}

static uint32_t to_int32(const char *bytes)
{
	return (uint32_t) bytes[0]
		| ((uint32_t) bytes[1] << 8)
		| ((uint32_t) bytes[2] << 16)
		| ((uint32_t) bytes[3] << 24);
}

char* md5(char *dst, const char *src, size_t src_length)
{

	// These vars will contain the hash
	uint32_t h0, h1, h2, h3;

	// Message (to prepare)
	char *msg = NULL;

	size_t new_len, offset;
	uint32_t w[16];
	uint32_t a, b, c, d, i, f, g, temp;

	// Initialize variables - simple count in nibbles:
	h0 = 0x67452301;
	h1 = 0xefcdab89;
	h2 = 0x98badcfe;
	h3 = 0x10325476;

	//Pre-processing:
	//append "1" bit to message    
	//append "0" bits until message length in bits ≡ 448 (mod 512)
	//append length mod (2^64) to message

	for (new_len = src_length + 1; new_len % (512/8) != 448/8; new_len++)
		;

	msg = (char*)malloc(new_len + 8);
	memcpy(msg, src, src_length);
	msg[src_length] = 0x80; // append the "1" bit; most significant bit is "first"
	for (offset = src_length + 1; offset < new_len; offset++)
		msg[offset] = 0; // append "0" bits

	// append the len in bits at the end of the buffer.
	to_bytes(src_length*8, msg + new_len);
	// src_length>>29 == src_length*8>>32, but avoids overflow.
	to_bytes(src_length>>29, msg + new_len + 4);

	// Process the message in successive 512-bit chunks:
	//for each 512-bit chunk of message:
	for(offset=0; offset<new_len; offset += (512/8)) {

		// break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
		for (i = 0; i < 16; i++)
			w[i] = to_int32(msg + offset + i*4);

		// Initialize hash value for this chunk:
		a = h0;
		b = h1;
		c = h2;
		d = h3;

		// Main loop:
		for(i = 0; i<64; i++) {

			if (i < 16) {
				f = (b & c) | ((~b) & d);
				g = i;
			} else if (i < 32) {
				f = (d & b) | ((~d) & c);
				g = (5*i + 1) % 16;
			} else if (i < 48) {
				f = b ^ c ^ d;
				g = (3*i + 5) % 16;          
			} else {
				f = c ^ (b | (~d));
				g = (7*i) % 16;
			}

			temp = d;
			d = c;
			c = b;
			b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
			a = temp;

		}

		// Add this chunk's hash to result so far:
		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;

	}

	// cleanup
	free(msg);

	//var char dst[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
	char result[16];
	to_bytes(h0, result);
	to_bytes(h1, result + 4);
	to_bytes(h2, result + 8);
	to_bytes(h3, result + 12);
	
	for (i=0; i<16; i++)
	{
		snprintf (dst+(2*i), 33-(2*i), "%2.2x", (unsigned char)result[i]);
	}
	return dst;
}


#if 0
static void TestMD5 ()
{
	const char* str="The quick brown fox jumps over the lazy dog";
	printf ("str=%s\n", str);
	
	char buf[64];
	md5(buf, str, strlen(str));

	printf ("md5=%s\n", buf);
	
	exit(1);
	return ;
}
#endif


