#ifndef __KING_ALGORITHM_RSA_H__
#define __KING_ALGORITHM_RSA_H__


/*
 * RSA constants
 */
#define RSA_MODE_PUBLIC     0
#define RSA_MODE_PRIVATE    1

#define RSA_PKCS_V15        0
#define RSA_PKCS_V21        1

typedef enum {
    RSA_HASH_NONE = 0,
    RSA_HASH_MD2,
    RSA_HASH_MD4,
    RSA_HASH_MD5,
    RSA_HASH_SHA1,
    RSA_HASH_SHA224,
    RSA_HASH_SHA256,
    RSA_HASH_SHA384,
    RSA_HASH_SHA512,
    RSA_HASH_RIPEMD160,
} RSA_HASH_TYPE;

typedef void* RSA_CTX;
typedef int (*RNG_FUN)(void *p_rng, uint8 *output, size_t output_len);


/**
 * @brief          Initialize an RSA context
 *
 *                 Note: Set padding to RSA_PKCS_V21 for the RSAES-OAEP
 *                 encryption scheme and the RSASSA-PSS signature scheme.
 *
 * @param[out] ctx  RSA context to be initialized
 * @param padding  RSA_PKCS_V15 or RSA_PKCS_V21
 * @param hash_id  RSA_PKCS_V21 hash identifier
 * @param f_rng    RNG function (Needed for padding and PKCS#1 v2.1 encoding
 *                               and MBEDTLS_RSA_PRIVATE)
 * @param p_rng    RNG parameter
 *
 * @return         0 if successful, -1 if failed
 *
 * @note           The hash_id parameter is actually ignored
 *                 when using RSA_PKCS_V15 padding.
 *
 * @note           Choice of padding mode is strictly enforced for private key
 *                 operations, since there might be security concerns in
 *                 mixing padding modes. For public key operations it's merely
 *                 a default value, which can be overriden by calling specific
 *                 rsa_rsaes_xxx or rsa_rsassa_xxx functions.
 *
 * @note           The chosen hash is always used for OEAP encryption.
 *                 For PSS signatures, it's always used for making signatures,
 *                 but can be overriden (and always is, if set to
 *                 RSA_HASH_NONE) for verifying them.
 */
int KING_RsaInit(RSA_CTX *ctx, int padding, RSA_HASH_TYPE hash_id, RNG_FUN f_rng, void *p_rng);


/**
 * @brief          Generate an RSA keypair
 *
 * @param ctx      RSA context that will hold the key
 * @param nbits    size of the public key in bits
 * @param exponent public exponent (e.g., 65537)
 *
 * @note           KING_RsaInit() must be called beforehand to setup
 *                 the RSA context.
 *
 * @return         0 if successful, -1 if failed
 */
int KING_RsaGenKey(RSA_CTX ctx, int nbits, int exponent);


/**
 * @brief          Check a public RSA key
 *
 * @param ctx      RSA context to be checked
 *
 * @return         0 if successful, -1 if failed
 */
int KING_RsaCheckPubkey(const RSA_CTX ctx);



/**
 * @brief          Check a private RSA key
 *
 * @param ctx      RSA context to be checked
 *
 * @return         0 if successful, -1 if failed
 */
int KING_RsaCheckPrivkey(const RSA_CTX ctx);


/**
 * @brief          Generic wrapper to perform a PKCS#1 encryption using the
 *                 mode from the context. Add the message padding, then do an
 *                 RSA operation.
 *
 * @param ctx      RSA context
 * @param mode     RSA_MODE_PUBLIC or RSA_MODE_PRIVATE
 * @param input    buffer holding the data to be encrypted
 * @param ilen     contains the plaintext length
 * @param output   buffer that will hold the ciphertext
 *
 * @return         0 if successful, -1 if failed
 *
 * @note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int KING_RsaPkcs1Encrypt(RSA_CTX ctx, int mode, const uint8 *input, int ilen, uint8 *output);


/**
 * @brief          Generic wrapper to perform a PKCS#1 decryption using the
 *                 mode from the context. Do an RSA operation, then remove
 *                 the message padding
 *
 * @param ctx      RSA context
 * @param mode     RSA_MODE_PUBLIC or RSA_MODE_PRIVATE
 * @param input    buffer holding the encrypted data
 * @param olen     will contain the plaintext length
 * @param output   buffer that will hold the plaintext
 * @param output_size    maximum length of the output buffer
 *
 * @return         0 if successful, -1 if failed
 *
 * @note           The output buffer length output_size should be
 *                 as large as the size ctx->len of ctx->N (eg. 128 bytes
 *                 if RSA-1024 is used) to be able to hold an arbitrary
 *                 decrypted message. If it is not large enough to hold
 *                 the decryption of the particular ciphertext provided,
 *                 the function will return RSA_ERR_OUTPUT_TOO_LARGE.
 *
 * @note           The input buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int KING_RsaPkcs1Decrypt(RSA_CTX ctx, int mode, const uint8 *input, int *olen, uint8 *output, size_t output_size);


/**
 * @brief          Generic wrapper to perform a PKCS#1 signature using the
 *                 mode from the context. Do a private RSA operation to sign
 *                 a message digest
 *
 * @param ctx      RSA context
 * @param mode     RSA_MODE_PUBLIC or RSA_MODE_PRIVATE
 * @param hash_id  a RSA_HASH_XXX (use RSA_HASH_NONE for signing raw data)
 * @param hashlen  message digest length (for RSA_HASH_NONE only)
 * @param hash     buffer holding the message digest
 * @param sig      buffer that will hold the ciphertext
 *
 * @return         0 if the signing operation was successful,
 *                 -1 if failed
 *
 * @note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int KING_RsaPkcs1Sign(RSA_CTX ctx, int mode, RSA_HASH_TYPE hash_id, int hashlen, const uint8 *hash, uint8 *sig);


/**
 * @brief          Generic wrapper to perform a PKCS#1 verification using the
 *                 mode from the context. Do a public RSA operation and check
 *                 the message digest
 *
 * @param ctx      points to an RSA public key
 * @param mode     RSA_MODE_PUBLIC or RSA_MODE_PRIVATE
 * @param hash_id  a RSA_HASH_XXX (use RSA_HASH_NONE for signing raw data)
 * @param hashlen  message digest length (for RSA_HASH_NONE only)
 * @param hash     buffer holding the message digest
 * @param sig      buffer holding the ciphertext
 *
 * @return         0 if the verify operation was successful,
 *                 -1 if failed
 *
 * @note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int KING_RsaPkcs1Verify(RSA_CTX ctx, int mode, RSA_HASH_TYPE hash_id, int hashlen, const uint8 *hash, const uint8 *sig);


/**
 * @brief          Free the components of an RSA key
 *
 * @param ctx      RSA Context to free
 *
 * @return         0 if successful, -1 if failed
 */
int KING_RsaFree(RSA_CTX ctx);


#endif

