#ifndef __KING_ALGORITHM_MD5_H__
#define __KING_ALGORITHM_MD5_H__


/**
 * @brief          Output = MD5( input buffer )
 *
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 * @param output   MD5 checksum result
 *
 * @return         0 success, -1 fail.
 */
int KING_Md5(const uint8 *input, size_t ilen, uint8 output[16]);


#endif

