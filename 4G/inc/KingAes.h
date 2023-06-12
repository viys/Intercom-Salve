#ifndef __KING_ALGORITHM_AES_H__
#define __KING_ALGORITHM_AES_H__


#define AES_MODE_DECRYPT     0
#define AES_MODE_ENCRYPT     1

typedef void* AES_CTX;

/**
 * @brief           Initialize AES context
 *
 * @param[out] ctx  AES context to be initialized
 */
int KING_AesInit(AES_CTX *ctx);

/**
 * @brief          AES key schedule (encryption)
 *
 * @param ctx      AES context to be initialized
 * @param key      encryption key
 * @param keybits  must be 128, 192 or 256
 *
 * @return         0 if successful, or -1 if failed
 */
int KING_AesSetKeyEnc(AES_CTX ctx, const uint8 *key, uint32 keybits);

/**
 * @brief          AES key schedule (decryption)
 *
 * @param ctx      AES context to be initialized
 * @param key      decryption key
 * @param keybits  must be 128, 192 or 256
 *
 * @return         0 if successful, or -1 if failed
 */
int KING_AesSetKeyDec(AES_CTX ctx, const uint8 *key, uint32 keybits);

/**
 * @brief          AES-ECB block encryption/decryption
 *
 * @param ctx      AES context
 * @param mode     AES_MODE_ENCRYPT or AES_MODE_DECRYPT
 * @param input    16-byte input block
 * @param output   16-byte output block
 *
 * @return         0 if successful
 */
int KING_AesCryptECB(AES_CTX ctx, int mode, const uint8 input[16], uint8 output[16]);

/**
 * @brief          AES-CBC buffer encryption/decryption
 *                 Length should be a multiple of the block
 *                 size (16 bytes)
 *
 * @note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * @param ctx      AES context
 * @param mode     AES_MODE_ENCRYPT or AES_MODE_DECRYPT
 * @param length   length of the input data
 * @param iv       initialization vector (updated after use)
 * @param input    buffer holding the input data
 * @param output   buffer holding the output data
 *
 * @return         0 if successful, or -1 if failed
 */
int KING_AesCryptCBC(AES_CTX ctx, int mode, size_t length, uint8 iv[16], const uint8 *input, uint8 *output);

/**
 * @brief          AES-CFB128 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * KING_AesSetKeyEnc() for both AES_MODE_ENCRYPT and AES_MODE_DECRYPT.
 *
 * @note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * @param ctx      AES context
 * @param mode     AES_MODE_ENCRYPT or AES_MODE_DECRYPT
 * @param length   length of the input data
 * @param iv_off   offset in IV (updated after use)
 * @param iv       initialization vector (updated after use)
 * @param input    buffer holding the input data
 * @param output   buffer holding the output data
 *
 * @return         0 if successful
 */
int KING_AesCryptCFB128(AES_CTX ctx, int mode, size_t length, size_t *iv_off, uint8 iv[16],
                        const uint8 *input, uint8 *output);

/**
 * @brief          AES-CFB8 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * KING_AesSetKeyEnc() for both AES_MODE_ENCRYPT and AES_MODE_DECRYPT.
 *
 * @note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * @param ctx      AES context
 * @param mode     AES_MODE_ENCRYPT or AES_MODE_DECRYPT
 * @param length   length of the input data
 * @param iv       initialization vector (updated after use)
 * @param input    buffer holding the input data
 * @param output   buffer holding the output data
 *
 * @return         0 if successful
 */
int KING_AesCryptCFB8(AES_CTX ctx, int mode, size_t length, uint8 iv[16],
                      const uint8 *input, uint8 *output);

/**
 * @brief               AES-CTR buffer encryption/decryption
 *
 * Warning: You have to keep the maximum use of your counter in mind!
 *
 * Note: Due to the nature of CTR you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * KING_AesSetKeyEnc() for both AES_MODE_ENCRYPT and AES_MODE_DECRYPT.
 *
 * @param ctx           AES context
 * @param length        The length of the data
 * @param nc_off        The offset in the current stream_block (for resuming
 *                      within current cipher stream). The offset pointer to
 *                      should be 0 at the start of a stream.
 * @param nonce_counter The 128-bit nonce and counter.
 * @param stream_block  The saved stream-block for resuming. Is overwritten
 *                      by the function.
 * @param input         The input data stream
 * @param output        The output data stream
 *
 * @return         0 if successful
 */
int KING_AesCryptCTR(AES_CTX ctx, size_t length, size_t *nc_off, uint8 nonce_counter[16], uint8 stream_block[16],
                       const uint8 *input, uint8 *output);

/**
 * @brief          Clear AES context
 *
 * @param ctx      AES context to be cleared
 */
int KING_AesFree(AES_CTX ctx);


#endif

