#ifndef __KING_ALGORITHM_SHA_H__
#define __KING_ALGORITHM_SHA_H__


/**
 * @brief          Output = SHA-1( input buffer )
 *
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 * @param output   SHA-1 checksum result
 *
 * @return         0 success, -1 fail.
 */
int KING_Sha1(const uint8 *input, size_t ilen, uint8 output[20]);


/**
 * @brief          Output = SHA-256( input buffer )
 *
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 * @param output   SHA-224/256 checksum result
 * @param is224    0 = use SHA256, 1 = use SHA224
 *
 * @return         0 success, -1 fail.
 */
int KING_Sha256(const uint8 *input, size_t ilen, uint8 output[32], int is224);



#endif

