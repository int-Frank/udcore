#include "udPlatform.h"


// **** Symmetric cipher algorithms ****

enum udCryptoCiphers
{
  udCC_AES128,  // 16-byte key
  udCC_AES256,  // 32-byte key
};

enum udCryptoPaddingMode
{
  udCPM_None
};

enum udCryptoChainMode
{
  udCCM_None,   // Sentinal meaning no chaining mode has been set yet
  udCCM_ECB,    // Insecure, used as a building block in combination with other operations
  udCCM_CBC,    // Sequential access, requires IV unique to each call to encrypt
  udCCM_CTR,    // Random access, requires a nonce unique to file
};

struct udCryptoCipherContext;

// Initialise a cipher
udResult udCrypto_CreateCipher(udCryptoCipherContext **ppCtx, udCryptoCiphers cipher, udCryptoPaddingMode padMode, const uint8_t *pKey, udCryptoChainMode chainMode);

// Set the nonce (for CTR mode)
udResult udCrypto_SetNonce(udCryptoCipherContext *pCtx, const uint8_t *pNonce, int nonceLen);

// Set the Nonce/counter (for CTR mode)
udResult udCrypto_CreateIVForCTRMode(udCryptoCipherContext *pCtx, uint8_t *pIV, int ivLen, uint64_t counter);

// Encrypt/decrypt using current mode/iv/nonce. Optional pOutIV arameter only applicable to CBC mode
udResult udCrypto_Encrypt(udCryptoCipherContext *pCtx, const uint8_t *pIV, int ivLen, const void *pPlainText, size_t plainTextLen, void *pCipherText, size_t cipherTextLen, size_t *pPaddedCipherTextLen = nullptr, uint8_t *pOutIV = nullptr);
udResult udCrypto_Decrypt(udCryptoCipherContext *pCtx, const uint8_t *pIV, int ivLen, const void *pCipherText, size_t cipherTextLen, void *pPlainText, size_t plainTextLen, size_t *pActualPlainTextLen = nullptr, uint8_t *pOutIV = nullptr);

// Free resources
udResult udCrypto_DestroyCipher(udCryptoCipherContext **ppCtx);


// **** Hash algorithms ****

enum udCryptoHashes
{
  udCH_SHA1,
  udCH_SHA256
};

struct udCryptoHashContext;

// Initialise a hash
udResult udCrypto_CreateHash(udCryptoHashContext **ppCtx, udCryptoHashes hash);

// Digest some bytes
udResult udCrypto_Digest(udCryptoHashContext *pCtx, const void *pBytes, size_t length);

// Digest some bytes
udResult udCrypto_Finalise(udCryptoHashContext *pCtx, uint8_t *pHash, size_t length, size_t *pActualHashLength = nullptr);

// Free resources
udResult udCrypto_DestroyHash(udCryptoHashContext **ppCtx);

// Internal test of algorithms
udResult udCrypto_TestCipher(udCryptoCiphers cipher);
udResult udCrypto_TestHash(udCryptoHashes hash);

// **** Key derivation functions ****
udResult udCrypto_KDF(const char *pPassword, uint8_t *pKey, int keyLen);
