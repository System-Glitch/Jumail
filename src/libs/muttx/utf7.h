/**
 * 	utf7.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 30/12/2017
 *  Description : Custom header for muttx ut7 lib usage. See utf7.c for Copyright.
 */


#ifndef SRC_LIBS_MUTTX_UTF7_H_
#define SRC_LIBS_MUTTX_UTF7_H_

/*
 * Convert the data (u7,u7len) from RFC 2060's UTF-7 to UTF-8.
 * The result is null-terminated and returned, and also stored
 * in (*u8,*u8len) if u8 or u8len is non-zero.
 * If input data is invalid, return 0 and don't store anything.
 * RFC 2060 obviously intends the encoding to be unique (see
 * point 5 in section 5.1.3), so we reject any non-canonical
 * form, such as &ACY- (instead of &-) or &AMA-&AMA- (instead
 * of &AMAAwA-).
 */
char *utf7_to_utf8 (const char *u7, size_t u7len, char **u8, size_t *u8len);

/*
 * Convert the data (u8,u8len) from UTF-8 to RFC 2060's UTF-7.
 * The result is null-terminated and returned, and also stored
 * in (*u7,*u7len) if u7 or u7len is non-zero.
 * Unicode characters above U+FFFF are replaced by U+FFFE.
 * If input data is invalid, return 0 and don't store anything.
 */
char *utf8_to_utf7 (const char *u8, size_t u8len, char **u7, size_t *u7len);

#endif /* SRC_LIBS_MUTTX_UTF7_H_ */
