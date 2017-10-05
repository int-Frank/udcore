#include "gtest/gtest.h"
#include "udCrypto.h"
#include "udValue.h"
#include "udPlatformUtil.h"

TEST(CryptoTests, AES_CBC_MonteCarlo)
{
  // Do the first only monte carlo tests for CBC mode (400 tests in official monte carlo)
  static const unsigned char aes_test_cbc_dec[2][16] =
  {
    { 0xFA, 0xCA, 0x37, 0xE0, 0xB0, 0xC8, 0x53, 0x73, 0xDF, 0x70, 0x6E, 0x73, 0xF7, 0xC9, 0xAF, 0x86 },
    { 0x48, 0x04, 0xE1, 0x81, 0x8F, 0xE6, 0x29, 0x75, 0x19, 0xA3, 0xE8, 0x8C, 0x57, 0x31, 0x04, 0x13 }
  };

  static const unsigned char aes_test_cbc_enc[2][16] =
  {
    { 0x8A, 0x05, 0xFC, 0x5E, 0x09, 0x5A, 0xF4, 0x84, 0x8A, 0x08, 0xD3, 0x28, 0xD3, 0x68, 0x8E, 0x3D },
    { 0xFE, 0x3C, 0x53, 0x65, 0x3E, 0x2F, 0x45, 0xB5, 0x6F, 0xCD, 0x88, 0xB2, 0xCC, 0x89, 0x8F, 0xF0 }
  };

  unsigned char key[32];
  unsigned char buf[64];
  unsigned char iv[16];
  unsigned char prv[16];
  udCryptoCipherContext *pCtx = nullptr;

  for (int i = 0; i < 4; i++)
  {
    int test256 = i >> 1;
    int testEncrypt = i & 1;

    memset(key, 0, sizeof(key));
    memset(iv, 0, sizeof(iv));
    memset(prv, 0, sizeof(prv));
    memset(buf, 0, sizeof(buf));

    udResult result = udCryptoCipher_Create(&pCtx, test256 ? udCC_AES256 : udCC_AES128, udCPM_None, key, udCCM_CBC);
    EXPECT_EQ(udR_Success, result);

    if (!testEncrypt)
    {
      for (int j = 0; j < 10000; j++)
        udCryptoCipher_Decrypt(pCtx, iv, sizeof(iv), buf, 16, buf, sizeof(buf), nullptr, iv); // Note: specifically decrypting exactly 16 bytes, not sizeof(buf)

      EXPECT_EQ(0, memcmp(buf, aes_test_cbc_dec[test256], 16));
    }
    else
    {
      for (int j = 0; j < 10000; j++)
      {
        udCryptoCipher_Encrypt(pCtx, iv, sizeof(iv), buf, 16, buf, sizeof(buf), nullptr, iv); // Note: specifically encrypting exactly 16 bytes, not sizeof(buf)
        unsigned char tmp[16];
        memcpy(tmp, prv, 16);
        memcpy(prv, buf, 16);
        memcpy(buf, tmp, 16);
      }

      EXPECT_EQ(0, memcmp(prv, aes_test_cbc_enc[test256], 16));
    }
    EXPECT_EQ(udR_Success, udCryptoCipher_Destroy(&pCtx));
  }
}

TEST(CryptoTests, AES_CTR_MonteCarlo)
{
  // Do the first only monte carlo tests for CTR mode (400 tests in official monte carlo)
  static const unsigned char aes_test_ctr_key[3][16] =
  {
    { 0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67, 0xCC, 0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E },
    { 0x7E, 0x24, 0x06, 0x78, 0x17, 0xFA, 0xE0, 0xD7, 0x43, 0xD6, 0xCE, 0x1F, 0x32, 0x53, 0x91, 0x63 },
    { 0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8, 0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC }
  };

  static const unsigned char aes_test_ctr_nonce_counter[3][16] =
  {
    { 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
    { 0x00, 0x6C, 0xB6, 0xDB, 0xC0, 0x54, 0x3B, 0x59, 0xDA, 0x48, 0xD9, 0x0B, 0x00, 0x00, 0x00, 0x01 },
    { 0x00, 0xE0, 0x01, 0x7B, 0x27, 0x77, 0x7F, 0x3F, 0x4A, 0x17, 0x86, 0xF0, 0x00, 0x00, 0x00, 0x01 }
  };

  static const unsigned char aes_test_ctr_pt[3][48] =
  {
    { 0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62, 0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67 },

    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F },

    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
      0x20, 0x21, 0x22, 0x23 }
  };

  static const unsigned char aes_test_ctr_ct[3][48] =
  {
    { 0xE4, 0x09, 0x5D, 0x4F, 0xB7, 0xA7, 0xB3, 0x79, 0x2D, 0x61, 0x75, 0xA3, 0x26, 0x13, 0x11, 0xB8 },

    { 0x51, 0x04, 0xA1, 0x06, 0x16, 0x8A, 0x72, 0xD9, 0x79, 0x0D, 0x41, 0xEE, 0x8E, 0xDA, 0xD3, 0x88,
      0xEB, 0x2E, 0x1E, 0xFC, 0x46, 0xDA, 0x57, 0xC8, 0xFC, 0xE6, 0x30, 0xDF, 0x91, 0x41, 0xBE, 0x28 },

    { 0xC1, 0xCF, 0x48, 0xA8, 0x9F, 0x2F, 0xFD, 0xD9, 0xCF, 0x46, 0x52, 0xE9, 0xEF, 0xDB, 0x72, 0xD7,
      0x45, 0x40, 0xA4, 0x2B, 0xDE, 0x6D, 0x78, 0x36, 0xD5, 0x9A, 0x5C, 0xEA, 0xAE, 0xF3, 0x10, 0x53,
      0x25, 0xB2, 0x07, 0x2F }
  };

  static const int aes_test_ctr_len[3] = { 16, 32, 36 };

  unsigned char key[32];
  unsigned char buf[64];
  unsigned char nonce_counter[16];
  udCryptoCipherContext *pCtx = nullptr;

  for (int i = 0; i < 4; i++)
  {
    int testNumber = i >> 1;
    int testEncrypt = i & 1;

    memset(key, 0, sizeof(key));
    memset(buf, 0, sizeof(buf));
    memcpy(nonce_counter, aes_test_ctr_nonce_counter[testNumber], 16);
    memcpy(key, aes_test_ctr_key[testNumber], 16);

    udResult result = udCryptoCipher_Create(&pCtx, udCC_AES128, udCPM_None, key, udCCM_CTR); // We only test 128-bit for CTR mode
    EXPECT_EQ(udR_Success, result);

    if (!testEncrypt)
    {
      int len = aes_test_ctr_len[testNumber];
      memcpy(buf, aes_test_ctr_ct[testNumber], len);
      udCryptoCipher_Decrypt(pCtx, aes_test_ctr_nonce_counter[testNumber], 16, buf, len, buf, len);
      EXPECT_EQ(0, memcmp(buf, aes_test_ctr_pt[testNumber], len));
    }
    else
    {
      int len = aes_test_ctr_len[testNumber];
      memcpy(buf, aes_test_ctr_pt[testNumber], len);
      udCryptoCipher_Decrypt(pCtx, aes_test_ctr_nonce_counter[testNumber], 16, buf, len, buf, len);
      EXPECT_EQ(0, memcmp(buf, aes_test_ctr_ct[testNumber], len));
    }
    EXPECT_EQ(udR_Success, udCryptoCipher_Destroy(&pCtx));
  }
}

TEST(CryptoTests, CipherErrorCodes)
{
  udResult result;
  unsigned char key[32];
  udCryptoCipherContext *pCtx = nullptr;
  unsigned char buf[64];
  unsigned char iv[16];

  memset(iv, 0, sizeof(iv));
  memset(key, 0, sizeof(key));
  memset(buf, 0, sizeof(buf));
  result = udCryptoCipher_Create(nullptr, udCC_AES128, udCPM_None, key, udCCM_CTR);
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Create(&pCtx, udCC_AES128, udCPM_None, nullptr, udCCM_CTR);
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Create(&pCtx, udCC_AES128, udCPM_None, key, udCCM_CTR);
  EXPECT_EQ(udR_Success, result);

  result = udCryptoCipher_Encrypt(nullptr, iv, 16, buf, sizeof(buf), buf, sizeof(buf)); // context
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Encrypt(pCtx, nullptr, 16, buf, sizeof(buf), buf, sizeof(buf)); // iv
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Encrypt(pCtx, iv, 15, buf, sizeof(buf), buf, sizeof(buf)); // iv length
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Encrypt(pCtx, iv, 16, nullptr, sizeof(buf), buf, sizeof(buf)); // input null
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Encrypt(pCtx, iv, 16, buf, 1, buf, sizeof(buf)); // input alignment
  EXPECT_EQ(udR_AlignmentRequirement, result);
  result = udCryptoCipher_Encrypt(pCtx, iv, 16, buf, sizeof(buf), nullptr, sizeof(buf)); // output null
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Encrypt(pCtx, iv, 16, buf, sizeof(buf), buf, sizeof(buf) - 1); // output size
  EXPECT_EQ(udR_BufferTooSmall, result);

  result = udCryptoCipher_Decrypt(nullptr, iv, 16, buf, sizeof(buf), buf, sizeof(buf)); // context
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Decrypt(pCtx, nullptr, 16, buf, sizeof(buf), buf, sizeof(buf)); // iv
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Decrypt(pCtx, iv, 15, buf, sizeof(buf), buf, sizeof(buf)); // iv length
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Decrypt(pCtx, iv, 16, nullptr, sizeof(buf), buf, sizeof(buf)); // input null
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Decrypt(pCtx, iv, 16, buf, 1, buf, sizeof(buf)); // input alignment
  EXPECT_EQ(udR_AlignmentRequirement, result);
  result = udCryptoCipher_Decrypt(pCtx, iv, 16, buf, sizeof(buf), nullptr, sizeof(buf)); // output null
  EXPECT_EQ(udR_InvalidParameter_, result);
  result = udCryptoCipher_Decrypt(pCtx, iv, 16, buf, sizeof(buf), buf, sizeof(buf) - 1); // output size
  EXPECT_EQ(udR_BufferTooSmall, result);

  result = udCryptoCipher_Destroy(&pCtx);
  EXPECT_EQ(udR_Success, result);
}


TEST(CryptoTests, SHA)
{
  /*
  * FIPS-180-1 test vectors. See https://www.di-mgt.com.au/sha_testvectors.html
  */
  static const char* s_testMessages[3] =
  {
    "abc",
    "",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
  };

  static udCryptoHashes s_testHashes[3] = { udCH_SHA1, udCH_SHA256, udCH_SHA512 };
  static const unsigned char s_testHashSizes[3] { udCHL_SHA1Length, udCHL_SHA256Length, udCHL_SHA512Length };
  static const unsigned char s_testHashResults[3][3][udCHL_MaxHashLength] =
  {
    { // SHA-1
      { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
      { 0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D, 0x32, 0x55, 0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90, 0xAF, 0xD8, 0x07, 0x09 },
      { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 },
    },
    { // SHA-256
      { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA, 0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23, 0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C, 0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD },
      { 0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14, 0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24, 0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C, 0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55 },
      { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8, 0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39, 0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67, 0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 }
    },
    { // SHA-512
      { 0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA, 0xCC, 0x41, 0x73, 0x49, 0xAE, 0x20, 0x41, 0x31, 0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9, 0x7E, 0xA2, 0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A,
        0x21, 0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8, 0x36, 0xBA, 0x3C, 0x23, 0xA3, 0xFE, 0xEB, 0xBD, 0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8, 0x0E, 0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F },
      {
        0xCF, 0x83, 0xE1, 0x35, 0x7E, 0xEF, 0xB8, 0xBD, 0xF1, 0x54, 0x28, 0x50, 0xD6, 0x6D, 0x80, 0x07, 0xD6, 0x20, 0xE4, 0x05, 0x0B, 0x57, 0x15, 0xDC, 0x83, 0xF4, 0xA9, 0x21, 0xD3, 0x6C, 0xE9, 0xCE,
        0x47, 0xD0, 0xD1, 0x3C, 0x5D, 0x85, 0xF2, 0xB0, 0xFF, 0x83, 0x18, 0xD2, 0x87, 0x7E, 0xEC, 0x2F, 0x63, 0xB9, 0x31, 0xBD, 0x47, 0x41, 0x7A, 0x81, 0xA5, 0x38, 0x32, 0x7A, 0xF9, 0x27, 0xDA, 0x3E },
      {
        0x20, 0x4A, 0x8F, 0xC6, 0xDD, 0xA8, 0x2F, 0x0A, 0x0C, 0xED, 0x7B, 0xEB, 0x8E, 0x08, 0xA4, 0x16, 0x57, 0xC1, 0x6E, 0xF4, 0x68, 0xB2, 0x28, 0xA8, 0x27, 0x9B, 0xE3, 0x31, 0xA7, 0x03, 0xC3, 0x35,
        0x96, 0xFD, 0x15, 0xC1, 0x3B, 0x1B, 0x07, 0xF9, 0xAA, 0x1D, 0x3B, 0xEA, 0x57, 0x78, 0x9C, 0xA0, 0x31, 0xAD, 0x85, 0xC7, 0xA7, 0x1D, 0xD7, 0x03, 0x54, 0xEC, 0x63, 0x12, 0x38, 0xCA, 0x34, 0x45 }
    }
  };

  for (int shaType = 0; shaType < UDARRAYSIZE(s_testHashes); ++shaType)
  {
    for (int testNumber = 0; testNumber < UDARRAYSIZE(s_testMessages); ++testNumber)
    {
      unsigned char resultHash[udCHL_MaxHashLength];
      size_t actualHashSize;
      size_t inputLength = udStrlen(s_testMessages[testNumber]);

      memset(resultHash, 0, sizeof(resultHash));
      udResult result = udCryptoHash_Hash(s_testHashes[shaType], s_testMessages[testNumber], inputLength, resultHash, sizeof(resultHash), &actualHashSize);
      EXPECT_EQ(udR_Success, result);
      EXPECT_EQ(s_testHashSizes[shaType], actualHashSize);
      EXPECT_EQ(0, memcmp(resultHash, s_testHashResults[shaType][testNumber], actualHashSize));

      // Do an additional test of digesting the string in two separate parts
      size_t length1 = inputLength / 2;
      size_t length2 = inputLength - length1;
      memset(resultHash, 0, sizeof(resultHash));
      result = udCryptoHash_Hash(s_testHashes[shaType], s_testMessages[testNumber], length1, resultHash, sizeof(resultHash), &actualHashSize, s_testMessages[testNumber] + length1, length2);
      EXPECT_EQ(udR_Success, result);
      EXPECT_EQ(s_testHashSizes[shaType], actualHashSize);
      EXPECT_EQ(0, memcmp(resultHash, s_testHashResults[shaType][testNumber], actualHashSize));
    }
  }
}

TEST(CryptoTests, Self)
{
  EXPECT_EQ(udR_Success, udCryptoCipher_SelfTest(udCC_AES128));
  EXPECT_EQ(udR_Success, udCryptoCipher_SelfTest(udCC_AES256));
  EXPECT_EQ(udR_Success, udCryptoHash_SelfTest(udCH_SHA1));
  EXPECT_EQ(udR_Success, udCryptoHash_SelfTest(udCH_SHA256));
  EXPECT_EQ(udR_Success, udCryptoHash_SelfTest(udCH_SHA512));
}

static const char *pPrivateKeyText =
  "{\n"
  "  'Type': 'RSA-Private',\n"
  "  'Size': 2048,\n"
  "  'N': 'n0pMFfJ0/vkZc1DEm4e1T33XoQZYvJxzFskuQ6E7IvTrR/KlTCMAEsU6rm+vNJQBFt+vEiNbv9qrzTQaW5XHp6k9+hwvQfkLKd2moc+G+ru5inbyOBBNpRfMAUjf6VZkyLbDFYrIFp6SJZYXhx2Pf/HpUKxpte4ZDFJARGpTLvE4GtYPs2Gj9xu6C1yRan3nLSa8MJhndNDxNGh5IF92ThMw0I+Pk1YSzpiJ9ZEX8GLqrVzWgORVdeiaYot3YSSMbf5pcDhrsvCDNSFB5396L3fc7IcWy99e9wiZiVcPUvLlNE/ziInNapOwG5EgPqTUk1jtXyLzuHZQDNADl/N/Uw==',\n"
  "  'E': 'AQAB',\n"
  "  'D': 'A2GBUenuf8brul3Zfm+X8pL6M6m90msDqlUkzTyr06cdI07MIVyQ0NUs1Kz8LAKEL2caASmM9fp/MQDNGmqIbU+TSC629hCCIyZYNhEAjWvUmVLC+1ulOj7SDqjsT7iMtRHj/B4Q9yHweinAYBbJh+6rhBHUwI7IK1HHmWwkTde6GxKiNxp4hrsxP8AKgX2+lCvPH0YT0jwFdHafcEfFCg0Mk8lIrlZRuD/5XbDVqKdcgrss63AwJthU1cEZztHH19sQfTjp2bEUFGBGQETL/NcJIO+YKbkng1UVvkTDcbJtSZ+PZxrQvzv5EJ4K0YTp26RlNNgTau2feMfyP5o3CQ==',\n"
  "  'P': '9dJtUa+v2GIwqhCJqcvSR/FdG53rAYwPYu9lJPmKbPxuSbUhtBJh+OzdVGU9H+xxRrw8w95hgEyaPLdGXfZuaGy3mFRHzCI9lfsShIIk37yKbIYbFPpvMu6M9gcB5LwbpK2IBlYZcSMohKvADv0qRaReWWLoIeVpj3M6tTZLaRU=',\n"
  "  'Q': 'peKuzv6ZYtNcERg3hxWyyWTw9TdbTHRJnmA8lVHb+JqxHipcp9uvcZBxHJiSqYY9nmdWPrEHyQIzXvEIclKtjYboKBt/bNfAZ1iLCR7bkX9wHQj6/PmAI9iFvzvvoQbC05C/RZkX3Ug00p7pR8/5qSYk72jC9hRgasNIjlOVkMc=',\n"
  "  'DP': 'vTLVUt6+n/OK8wnBer9WPGsHt37G5qzvFr2cgmXR5eov1Gkl5JuVbmqYOyGkdxKbaM7uke5x6raKq5p//UfzWEn80LBlhjcAYZQZf4VPbiiF/dsFsxLBTVkPgziHe45QVGH/ZKkV8d8Wi25JZv/xbiKBP5kBgz04DuGoWNrOFbU=',\n"
  "  'DQ': 'ePmRpl9CGTIuqEDS7e7DDeBRYWNXb7A2qAti4zppgym9FVSrcbbigZ1nAAW8n2jIsyaFXP7ZwJucPxbkpArripTh5a34BbZqGHQYITShx7/6URJlh+ukqX+UOlxJa1N07blX5De7kaLA8wD0+2wOlG6+7OGnnLJLhlCYL0OBha0=',\n"
  "  'QP': 'YSPeV0tloK7H8XGZ95KCixJfNYkhT29Lcldbm2kUbtk/rChH5OjGwINMc8CewH8/mZDAD7ZpyU9UyWyfK6PpYUjLguvsdmCWuXGVsURRj6MNsv6rHWjyGpfgxLTe2dUKK/xgIOd1mATTUpM3S3q3HRjccy0IfyTh2HdFHILUorU='\n"
  "}\n";

static const char *pPublicKeyText =
  "{\n"
  "  'Type': 'RSA-Public',\n"
  "  'Size': 2048,\n"
  "  'N': 'n0pMFfJ0/vkZc1DEm4e1T33XoQZYvJxzFskuQ6E7IvTrR/KlTCMAEsU6rm+vNJQBFt+vEiNbv9qrzTQaW5XHp6k9+hwvQfkLKd2moc+G+ru5inbyOBBNpRfMAUjf6VZkyLbDFYrIFp6SJZYXhx2Pf/HpUKxpte4ZDFJARGpTLvE4GtYPs2Gj9xu6C1yRan3nLSa8MJhndNDxNGh5IF92ThMw0I+Pk1YSzpiJ9ZEX8GLqrVzWgORVdeiaYot3YSSMbf5pcDhrsvCDNSFB5396L3fc7IcWy99e9wiZiVcPUvLlNE/ziInNapOwG5EgPqTUk1jtXyLzuHZQDNADl/N/Uw==',\n"
  "  'E': 'AQAB',\n"
  "}\n";


TEST(CryptoTests, CreateSig)
{
  udCryptoSigContext *pPrivCtx = nullptr;
  udCryptoSigContext *pPubCtx = nullptr;
  static const char *pMessage = "No problem can be solved from the same level of consciousness that created it. -Einstein";
  static const char *pExpectedSignature = "I7iQO1dRmnh1BeEw//vRB/81eKXzKgC3Uuchny/iHfzFoFGxcVa2DG9RE5pILsChDnJLdFpcaSHK258r5285jgJPn2rHTVf3xBvs5Su4fAFAOmB95dlTaux3eVt0Pl8XXvxuTl9SDLjCFeplPxsHRE5LKTj8ySVeVenWWUInvzrJ/QXFmYILt9WQb65lHeMErEAGb2mrUiLWyRaJY4/KsaYAQhZwydW49P8dVoopU7MhoIb6QgJw2azmNXuY2c22qonMOsPTbMSvBCC27iSg5mrLkd0N5eDMhwvzOGFHjHo55oCvfvuUexe5wPqRTbI2KeJElr3SA6MZFvYkfb5YGw==";
  uint8_t hash[udCHL_SHA1Length];
  const char *pSignature = nullptr;

  EXPECT_EQ(udR_Success, udCryptoHash_Hash(udCH_SHA1, pMessage, udStrlen(pMessage), hash, sizeof(hash)));

#if 0 // Enable to generate a new key
  EXPECT_EQ(udR_Success, udCryptoSig_CreateKeyPair(&pPrivCtx, udCST_RSA2048));
  EXPECT_EQ(udR_Success, udCryptoSig_ExportKeyPair(pPrivCtx, &pPrivateKeyText, true));
  udDebugPrintf("Private key:\n%s\n", pPrivateKeyText);
  EXPECT_EQ(udR_Success, udCryptoSig_ExportKeyPair(pPrivCtx, &pPublicKeyText, false));
  udDebugPrintf("Public key:\n%s\n", pPublicKeyText);
  EXPECT_EQ(udR_Success, udCryptoSig_Sign(pPrivCtx, hash, sizeof(hash), &pExpectedSignature));
  udDebugPrintf("Expected signature:\n%s\n", pExpectedSignature);
#else
  EXPECT_EQ(udR_Success, udCryptoSig_ImportKeyPair(&pPrivCtx, pPrivateKeyText));
#endif
  // Import the public key only
  EXPECT_EQ(udR_Success, udCryptoSig_ImportKeyPair(&pPubCtx, pPublicKeyText));

  // Sign a message using the private key
  EXPECT_EQ(udR_Success, udCryptoSig_Sign(pPrivCtx, hash, sizeof(hash), &pSignature));

  // Verify it's the expected signature (only works with PKCS_15)
  EXPECT_EQ(0, udStrcmp(pSignature, pExpectedSignature));

  // Verify using the private key
  EXPECT_EQ(udR_Success, udCryptoSig_Verify(pPrivCtx, hash, sizeof(hash), pSignature));

  // Verify the message using the public key
  EXPECT_EQ(udR_Success, udCryptoSig_Verify(pPubCtx, hash, sizeof(hash), pSignature));

  // Change the hash slightly to ensure the message isn't verified
  hash[1] ^= 1;
  EXPECT_EQ(udR_SignatureMismatch, udCryptoSig_Verify(pPrivCtx, hash, sizeof(hash), pSignature));
  EXPECT_EQ(udR_SignatureMismatch, udCryptoSig_Verify(pPubCtx, hash, sizeof(hash), pSignature));

  udCryptoSig_Destroy(&pPrivCtx);
  udCryptoSig_Destroy(&pPubCtx);
  udFree(pSignature);
}

TEST(CryptoTests, DHM)
{
  udResult result;
  udValue publicA, publicB;
  udCryptoDHMContext *pDHM = nullptr;
  const char *pPublicValueA = nullptr;
  const char *pPublicValueB = nullptr;
  uint8_t secretA[1000];
  uint8_t secretB[1000];

  result = udCryptoKey_CreateDHM(&pDHM, &pPublicValueA, sizeof(secretA));
  EXPECT_EQ(udR_Success, result);

  result = udCryptoKey_DeriveFromPartyA(pPublicValueA, &pPublicValueB, secretB, sizeof(secretB));
  EXPECT_EQ(udR_Success, result);

  result = udCryptoKey_DeriveFromPartyB(pDHM, pPublicValueB, secretA, sizeof(secretA));
  EXPECT_EQ(udR_Success, result);

  EXPECT_EQ(0, memcmp(secretA, secretB, sizeof(secretA)));

  EXPECT_EQ(udR_Success, publicA.Parse(pPublicValueA));
  EXPECT_EQ(udR_Success, publicB.Parse(pPublicValueB));
  EXPECT_EQ(2, publicA.MemberCount());
  EXPECT_EQ(publicA.Get("keyLen").AsInt(), (int)sizeof(secretA));
  EXPECT_EQ(1, publicB.MemberCount());
  EXPECT_EQ(false, udStrEqual(publicA.Get("PublicValue").AsString(), publicB.Get("PublicValue").AsString()));

  udFree(pPublicValueA);
  udFree(pPublicValueB);
  udCryptoKey_DestroyDHM(&pDHM);

  // Finally, generate another secret (just to secretB) and make sure it's different from the previous one
  result = udCryptoKey_CreateDHM(&pDHM, &pPublicValueA, sizeof(secretA));
  EXPECT_EQ(udR_Success, result);
  result = udCryptoKey_DeriveFromPartyA(pPublicValueA, &pPublicValueB, secretB, sizeof(secretB));
  EXPECT_EQ(udR_Success, result);
  udFree(pPublicValueA);
  udFree(pPublicValueB);
  udCryptoKey_DestroyDHM(&pDHM);
  EXPECT_NE(0, memcmp(secretA, secretB, sizeof(secretA)));
}