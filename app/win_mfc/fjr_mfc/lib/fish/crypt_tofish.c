#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <share.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "aes.h"


static const unsigned char raw_key[] = {	/* 112byte */
 0x12, 0x9c, 0x91, 0x34, 0x54, 0x00, 0x42, 0x86, 0x8d, 0x80, 0x78, 0xf3, 0xc5, 0xc9, 0x8d, 0xc6,
 0x7b, 0x24, 0xb1, 0xb8, 0x50, 0x7a, 0x41, 0x7b, 0xae, 0x11, 0x4c, 0xd3, 0x15, 0x4d, 0xd2, 0x58,
 0x2e, 0x93, 0xec, 0xe9, 0x88, 0xbd, 0x4e, 0x15, 0x8c, 0x41, 0xbf, 0xd5, 0x35, 0x6b, 0xa7, 0xae,
 0xb7, 0x06, 0x08, 0x09, 0x10, 0x2a, 0x41, 0xc5, 0xa4, 0x35, 0x42, 0xbf, 0xf5, 0xe7, 0x21, 0x19,
 0xde, 0xf3, 0x75, 0x8f, 0xb6, 0x5a, 0x4d, 0x96, 0x96, 0x5, 0x7b, 0xe1, 0xaa, 0xb7, 0x1b, 0x4f,
 0x70, 0x92, 0x0a, 0x62, 0x49, 0xc3, 0x4a, 0x51, 0xbc, 0x84, 0x58, 0x2a, 0x81, 0x6, 0x63, 0xf6,
 0x6c, 0xc9, 0xc7, 0x4f, 0x36, 0xe8, 0x47, 0x8f, 0x81, 0xd, 0xfc, 0xf6, 0x51, 0x52, 0x2a, 0xa0,
 };

static unsigned char iv[BLOCK_SIZE/8] = 	/* BLOCK_SIZE=128 in aes.h */
{
	0x32, 0x09, 0x89,  0x04,  0x24,  0x9f,  0xc3,  0x4f,  0x78,  0x0f,  0xb2,  0x0a,  0x4b,  0xe5,  0xde,  0x1a, 
};

static unsigned char key[24];
#define CBC_BLKSIZ  (BLOCK_SIZE/8)
void pkcs5(unsigned char* raw_data, unsigned size, unsigned flen)
{
	unsigned int n;
	unsigned int i;
	
	n = size - flen;
	for (i = 0; i < n; i++) {
		*(raw_data + flen + i) = (unsigned char)n;
	}
}

int encdec(int isenc, unsigned char* buffer, unsigned size, unsigned flen)
{
	unsigned i;
	keyInstance ki;
	cipherInstance ci;
	unsigned n;

	if (isenc) {
		*(buffer + flen) = 0xff;	// stopper
		pkcs5(buffer, size, flen + 1);
	}

	makeKey(&ki, DIR_ENCRYPT, 192, 0);
	cipherInit(&ci, MODE_CBC, 0);
	
	for (i = 0; i < 192; i++) {
		*((char*)((char*)ki.key32 + i)) = key[i];
	}
	reKey(&ki);
	memcpy(ci.iv32, iv, sizeof(ci.iv32));

	if (isenc) {
		for (n = 0; n < size; n += CBC_BLKSIZ) {
			blockEncrypt(&ci, &ki, buffer + n, CBC_BLKSIZ, buffer + n);
		}
	} else {
		for (n = 0; n < size; n += CBC_BLKSIZ) {
			blockDecrypt(&ci, &ki, buffer + n, CBC_BLKSIZ, buffer + n);
		}
	}
	if (!isenc) {
		n = *(buffer + size - 1);	// last byte
		for (i = 0; n == *(buffer + size - 1 - i); i++) {
			;
		}
		if (n == i) {
			return size - i - 1;	// remove stopper
		} else {
			return size - 1;		// remove stopper
		}
	} else {
		return size;
	}
}


static void _key192(void)
{
	key[0] = raw_key[111] ^ (((raw_key[13] << 1)) ^ (raw_key[24] >> 1));
	key[1] = raw_key[41] ^ (raw_key[4] << 2) ^ (raw_key[2] >> 1);
	key[2] =  ((raw_key[56] >> 4) | (raw_key[81] << 4)) ^ raw_key[18];
	key[3] =  ((raw_key[11] >> 4) | (raw_key[98] << 4));
	key[4] =  ((raw_key[8] >> 4) | (raw_key[94] << 3));
	key[5] =  ((raw_key[43] >> 4) | (raw_key[55] << 4));
	key[6] =  raw_key[101] ^ (raw_key[95] << 2) ^ (raw_key[9] >> 1);
	key[7] =  ((raw_key[32] >> 4) | (raw_key[66] << 6));
	key[8] =  ((raw_key[52] >> 4) | (raw_key[78] << 4));
	key[9] =  ((raw_key[92] >> 3) | (raw_key[96] << 4));
	key[10] = raw_key[110] ^ (raw_key[14] << 2) ^ (raw_key[7] >> 1);
	key[11] = ((raw_key[32] >> 4) | (raw_key[51] << 4));
	key[12] = ((raw_key[22] >> 7) | (raw_key[3] << 2));
	key[13] = raw_key[108] ^ (raw_key[84] << 2) ^ (raw_key[12] >> 1);
	key[14] = ((raw_key[64] >> 4) | (raw_key[5] << 4));
	key[15] = ((raw_key[67] >> 4) | (raw_key[105] << 5));
	key[16] = ((raw_key[26] >> 4) | (raw_key[6] << 5));
	key[17] = ((raw_key[34] >> 4) | (raw_key[16] << 5));
	key[18] = ((raw_key[84] >> 4) | (raw_key[27] << 5));
	key[19] = ((raw_key[25] >> 4) | (raw_key[19] << 5));
	key[20] = ((raw_key[29] >> 4) | (raw_key[36] << 5));
	key[21] = ((raw_key[73] >> 4) | (raw_key[102] << 5));
	key[22] = ((raw_key[70] >> 4) | (raw_key[100] << 5));
	key[23] = ((raw_key[81] >> 4) | (raw_key[90] << 5));
}

#ifdef _TEST_

int test_main(int argc, char** argv)
{
	int rfh;
	int wfh;
	int isenc = 1;
	int rc;
	unsigned char* buffer;
	long flen;
	int enclen;
	char fname[260];
	
	if (argc != 3) {
		printf("crypt_tofish <enc|dec> <file>\n");
		printf("  file cbc-tofish encrypt/decrypt to '<file>.enc' or '<file>.dec'\n");
		return -1;
	}
	
	isenc = strcmp(*(argv + 1), "enc") == 0;
	strcpy_s(fname, 260, *(argv + 2));
	rc = _sopen_s(&rfh, fname, _O_RDONLY|_O_BINARY, _SH_DENYNO, _S_IREAD);	/* _SH_DENYNO を0にすると飛ぶ */
	if (rc != 0) {
		printf("Can't read open file %s(err=%u)\n", fname, rc);
		return -1;
	}
	flen = _lseek(rfh, 0, SEEK_END);
	if (flen == -1) {
		printf("seek error: %u\n", errno);
		_close(rfh);
		return -1;
	}

	strcat_s(fname, 260, isenc ? ".enc" : ".dec");						/* !!!!!! */
	rc = _sopen_s(&wfh, fname, _O_CREAT| _O_WRONLY | _O_TRUNC |/*_O_EXCL |*/_O_BINARY, _SH_DENYNO, _S_IWRITE);
					// _O_EXCL and fnameあるとき_writeで飛ぶ
					// 仕方ないのでfile存在時は破壊
					// さらに_O_TRUNC を付加しないと既存ファイルに上書きすると前のファイルのサイズがでかいとその差分バイト分余計に書かれる
					// 
	if (rc != 0) {
		printf("Can't write open file %s(err=%u)\n", fname, rc);
		_close(rfh);
		return -1;
	}
	/* retrive read file size */
	rc = _lseek(rfh, 0, SEEK_SET);
	if (rc == -1) {
		printf("seek error: %u\n", errno);
		_close(rfh);
		_close(wfh);
		return -1;
	}
	if (isenc) {
		enclen = (flen + 1 + (CBC_BLKSIZ - 1)) & ~(CBC_BLKSIZ - 1);
	} else {
		enclen = flen;
	}
	buffer = (unsigned char*)malloc(enclen);		/* encrypt key = 192bit */
	if (buffer == NULL) {
		printf("Can't allocate memory.\n");
		_close(rfh);
		_close(wfh);
		return -1;
	}
	printf("file length = %d\n", flen);
	
	if (flen != (rc = _read(rfh, buffer, flen))) {
		printf("Can't read file. %u, %u\n", rc, errno);
		free(buffer);
		_close(wfh);
		_close(rfh);
		return -1;
	}

//////////////////////////////////////////////////////
	_key192();
printf("%d, %d, %d\n", enclen, flen, enclen - flen);
	flen = encdec(isenc, buffer, enclen, flen);
	if (flen < 0) {
		printf("crypt error %d\n", flen);
	}
////////////////////////////////////////////////////////
	rc = _write(wfh, buffer, flen);
	if (flen != rc) {
		printf("Can't write %d/%d byte write. err=%u\n", rc, flen, errno);
	} else {
		printf("succeed. %d byte write.\n", flen);
	}
	free(buffer);
	_close(rfh);
	_close(wfh);
	return 0;
}

#endif	/* _TEST_ */

// int restore_db(unsigned char* buffer, unsigned enclen)
int rd(unsigned char* buffer, unsigned enclen)
{
	_key192();
	return encdec(0, buffer, enclen, enclen);
}

