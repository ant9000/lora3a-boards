#include <assert.h>
#include <string.h>
#include "cpu.h"
#include "aes.h"

static void __aes_sync_gcm_start(struct aes_sync_device *const dev, const enum aes_action enc, const uint8_t *iv,
                                 uint32_t iv_len, const uint8_t *aad, uint32_t aad_len);
static void __aes_sync_gcm_update(struct aes_sync_device *const dev, const uint8_t *input, uint8_t *output,
                                  uint32_t length);
static void __aes_sync_gcm_generate_tag(struct aes_sync_device *const dev, uint8_t *tag, uint32_t tag_len);
static inline void __aes_sync_set_key(struct aes_sync_device *const dev);
static inline void __aes_sync_set_iv(uint8_t *iv);
static inline void __aes_sync_get_indata(uint8_t *output, uint32_t words);
static inline void __aes_sync_set_indata(const uint8_t *data, uint32_t words);

int32_t aes_sync_set_encrypt_key(struct aes_sync_device *const dev, const uint8_t *key, const enum aes_keysize size)
{
    assert(dev && key);
    dev->keysize = size;
    memcpy(dev->key, key, (size + 2) << 3);
    return 0;
}

int32_t aes_sync_gcm_crypt_and_tag(struct aes_sync_device *const dev, const enum aes_action enc,
                                   const uint8_t *input, uint8_t *output, uint32_t length, const uint8_t *iv,
                                   uint32_t iv_len, const uint8_t *aad, uint32_t aad_len, uint8_t *tag,
                                   uint32_t tag_len)
{
    assert(dev && iv && iv_len);
    assert((input && output && length) || (!length));
    assert(((aad && aad_len) || !aad_len));
    assert((tag && tag_len && (tag_len <= 16)) || !tag_len);
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_AES;
    __aes_sync_gcm_start(dev, enc, iv, iv_len, aad, aad_len);
    __aes_sync_gcm_update(dev, input, output, length);
    __aes_sync_gcm_generate_tag(dev, tag, tag_len);
    MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_AES;

    return 0;
}

static void __aes_sync_gcm_start(struct aes_sync_device *const dev, const enum aes_action enc, const uint8_t *iv,
                                 uint32_t iv_len, const uint8_t *aad, uint32_t aad_len)
{
    uint8_t        index;
    uint32_t       use_len;
    int32_t        left_len;
    uint8_t        workbuf[16];
    const uint8_t *ptr;

    /* Step 1 Generate HASHKEY */
    AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
    AES->CTRLA.reg = 0;
    AES->CTRLA.reg |= AES_CTRLA_CIPHER;
    AES->CTRLA.reg |= AES_CTRLA_AESMODE(0); /* ECB */
    AES->CTRLA.reg |= AES_CTRLA_KEYSIZE(dev->keysize);
    AES->CTRLA.reg |= AES_CTRLA_ENABLE;

    __aes_sync_set_key(dev);
    for (index = 0; index < 4; index++) { AES->INDATA.reg = 0; }
    AES->CTRLB.reg |= AES_CTRLB_START;
    /* HashKey is ready*/
    while ((AES->INTFLAG.reg & AES_INTFLAG_ENCCMP) == 0);

    /* Change to GCM mode */
    AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
    AES->CTRLA.reg &= ~AES_CTRLA_STARTMODE;
    AES->CTRLA.reg |= AES_CTRLA_CIPHER;
    AES->CTRLA.reg |= AES_CTRLA_KEYSIZE(dev->keysize);
    AES->CTRLA.reg |= AES_CTRLA_AESMODE(6); /* GCM */
    AES->CTRLA.reg |= AES_CTRLA_CTYPE(0);
    AES->CTRLA.reg |= AES_CTRLA_ENABLE;

    __aes_sync_set_key(dev);
    AES->CTRLB.reg |= AES_CTRLB_GFMUL;

    /* Step 2: Generate pre-counter block j0 from the IV */
    if (iv_len == 12) {
        memcpy(dev->iv, iv, 12);
        memset(dev->iv + 12, 0, 3);
        dev->iv[15] = 0x01;
    } else {
        /* If iv_len != 96, then j0 = GHASH(H, {}, IV) */
        for (index = 0; index < 4; index++) { AES->INDATA.reg = 0x00; }
        AES->CTRLB.reg |= AES_CTRLB_START;
        while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0);

        /* GHASH IV */
        ptr      = iv;
        left_len = iv_len;
        while (left_len >= 0) {
            use_len = (left_len < 16) ? left_len : 16;
            if (use_len == 16) {
                __aes_sync_set_indata(ptr, 4);
                AES->CTRLB.reg |= AES_CTRLB_START;
                while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0)
                    ;
            } else {
                memset(workbuf, 0, 16);
                memcpy(workbuf, ptr, use_len);
                __aes_sync_set_indata(workbuf, 4);
                AES->CTRLB.reg |= AES_CTRLB_START;
                while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0)
                    ;
            }
            left_len -= use_len;
            ptr += use_len;
            left_len = left_len ? left_len : -1;
        }
        /* GHASH len(IV)64 */
        memset(workbuf, 0, 12);
        workbuf[12] = ((iv_len << 3) >> 24) & 0xFF;
        workbuf[13] = ((iv_len << 3) >> 16) & 0xFF;
        workbuf[14] = ((iv_len << 3) >> 8) & 0xFF;
        workbuf[15] = (iv_len << 3) & 0xFF;

        __aes_sync_set_indata(workbuf, 4);
        AES->CTRLB.reg |= AES_CTRLB_START;
        while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0)
            ;
        /* Get j0 from GHASH reg */
        for (index = 0; index < 4; index++) {
            ((uint32_t *)(void *)dev->iv)[index] = AES->GHASH[index].reg;
            AES->GHASH[index].reg = 0x00;
        }
    }

    /* Step 3: GHASH AAD */
    ptr      = aad;
    left_len = aad_len;
    while (left_len >= 0) {
        use_len = (left_len < 16) ? left_len : 16;
        if (use_len == 16) {
            __aes_sync_set_indata(ptr, 4);
            AES->CTRLB.reg |= AES_CTRLB_START;
            while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0)
                ;
        } else {
            memset(workbuf, 0, 16);
            memcpy(workbuf, ptr, use_len);
            __aes_sync_set_indata(workbuf, 4);
            AES->CTRLB.reg |= AES_CTRLB_START;
            while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0)
                ;
        }
        left_len -= use_len;
        ptr += use_len;
        left_len = left_len ? left_len : -1;
    }

    /* GFMUL must be clear, otherwise the interrupt flag cannot be set when
     * en/decrypt text */
    AES->CTRLB.reg &= ~AES_CTRLB_GFMUL;

    dev->aad_len = aad_len;

    /* Step 3: Change enc/dec */
    if (enc == 0) {
        AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
        AES->CTRLA.reg &= ~AES_CTRLA_CIPHER;
        AES->CTRLA.reg |= AES_CTRLA_ENABLE;
    }
    AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
    return;
}

static void __aes_sync_gcm_update(struct aes_sync_device *const dev, const uint8_t *input, uint8_t *output,
                                  uint32_t length)
{
    int8_t         index;
    uint8_t        block; /* Number of blocks (16 bytes) */
    uint8_t        workbuf[16];
    const uint8_t *inptr;
    uint8_t *      outptr;

    /* Set workbuf = j1 =  j0 + 1 */
    memcpy(workbuf, dev->iv, 16);
    for (index = 16; index > 0; index--) {
        if (++workbuf[index - 1] != 0)
            break;
    }

    /* Step 2 Plain text Processing */
    AES->CTRLA.reg |= AES_CTRLA_ENABLE;
    AES->INTFLAG.reg = AES_INTFLAG_ENCCMP;
    AES->CTRLB.reg |= AES_CTRLB_NEWMSG;
    AES->CIPLEN.reg = length;

    /* Step 2 Set J1 to IV */
    __aes_sync_set_iv(workbuf);

    /* Enc/Dec plain text */
    inptr  = input;
    outptr = output;
    AES->DATABUFPTR.reg |= AES_DATABUFPTR_INDATAPTR(0);
    for (block = 0; block < (length >> 4); block++) {
        if (((length & 0xF) == 0) && block == ((length >> 4) - 1)) {
            AES->CTRLB.reg |= AES_CTRLB_EOM;
        }
        __aes_sync_set_indata(inptr, 4);
        inptr += 16;
        AES->CTRLB.reg |= AES_CTRLB_START;
        while ((AES->INTFLAG.reg & AES_INTFLAG_ENCCMP) == 0)
            ;
        AES->CTRLB.reg &= ~AES_CTRLB_NEWMSG;
        AES->CTRLB.reg &= ~AES_CTRLB_EOM;
        __aes_sync_get_indata(outptr, 4);
        outptr += 16;
    }
    /* If length it not 16 bytes align, then process last one */
    if (length & 0xF) {
        memset(workbuf, 0, 16);
        memcpy(workbuf, input + (length & ~0xF), length & 0xF);

        AES->CTRLB.reg |= AES_CTRLB_EOM;
        __aes_sync_set_indata(workbuf, 4);
        AES->CTRLB.reg |= AES_CTRLB_START;
        while ((AES->INTFLAG.reg & AES_INTFLAG_ENCCMP) == 0)
            ;
        __aes_sync_get_indata(workbuf, 4);
        memcpy(output + (length & ~0xF), workbuf, length & 0xF);
    }

    /* Generate Final GHASH by GHASH(H, A, C) */
    memset(workbuf, 0, 16);
    ((uint8_t *)workbuf)[4]  = ((dev->aad_len << 3) >> 24) & 0xFF;
    ((uint8_t *)workbuf)[5]  = ((dev->aad_len << 3) >> 16) & 0xFF;
    ((uint8_t *)workbuf)[6]  = ((dev->aad_len << 3) >> 8) & 0xFF;
    ((uint8_t *)workbuf)[7]  = (dev->aad_len << 3) & 0xFF;
    ((uint8_t *)workbuf)[12] = ((length << 3) >> 24) & 0xFF;
    ((uint8_t *)workbuf)[13] = ((length << 3) >> 16) & 0xFF;
    ((uint8_t *)workbuf)[14] = ((length << 3) >> 8) & 0xFF;
    ((uint8_t *)workbuf)[15] = (length << 3) & 0xFF;

    __aes_sync_set_indata(workbuf, 4);
    AES->CTRLB.reg |= AES_CTRLB_GFMUL;
    AES->CTRLB.reg |= AES_CTRLB_START;
    while ((AES->INTFLAG.reg & AES_INTFLAG_GFMCMP) == 0);
    AES->CTRLB.reg &= ~AES_CTRLB_GFMUL;
    AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
}

static void __aes_sync_gcm_generate_tag(struct aes_sync_device *const dev, uint8_t *tag, uint32_t tag_len)
{
    int8_t  index;
    uint8_t j0[16];

    memcpy(j0, dev->iv, 16);
    AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
    /* When change to Counter mode, all CTRLA should be reset */
    AES->CTRLA.reg = 0;
    AES->CTRLA.reg |= AES_CTRLA_CIPHER;
    AES->CTRLA.reg |= AES_CTRLA_AESMODE(4); /* Counter */
    AES->CTRLA.reg |= AES_CTRLA_KEYSIZE(dev->keysize);
    AES->CTRLA.reg |= AES_CTRLA_ENABLE;
    AES->CTRLB.reg = 0;

    __aes_sync_set_key(dev);

    AES->DATABUFPTR.reg |= AES_DATABUFPTR_INDATAPTR(0);
    __aes_sync_set_iv(j0);
    for (index = 0; index < 4; index++) {
        AES->INDATA.reg = (uint32_t)AES->GHASH[index].reg;
    }
    AES->CTRLB.reg |= AES_CTRLB_NEWMSG;
    AES->CTRLB.reg |= AES_CTRLB_START;
    while ((AES->INTFLAG.reg & AES_INTFLAG_ENCCMP) == 0)
        ;

    __aes_sync_get_indata(j0, 4);
    memcpy(tag, j0, tag_len);

    /* Cleanup, Must be done after GCM crypt, otherwise
     * it will effect next GCM crypt.
     */
    for (index = 0; index < 4; index++) {
        AES->GHASH[index].reg = 0x00;
        AES->HASHKEY[index].reg = 0x00;
        AES->INDATA.reg = 0x00;
    }
    AES->CIPLEN.reg = 0x00;
    AES->DATABUFPTR.reg = 0x00;
    AES->CTRLA.reg &= ~AES_CTRLA_ENABLE;
}

static inline void __aes_sync_set_key(struct aes_sync_device *const dev)
{
    int i;

    for (i = 0; i < ((dev->keysize + 2) << 1); i++) {
        AES->KEYWORD[i].reg = (((uint32_t *)(void *)(dev->key))[i]);
    }
}

static inline void __aes_sync_set_iv(uint8_t *iv)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (((uint32_t)iv) & 0x3) {
            AES->INTVECTV[i].reg = ((uint8_t *)iv)[i << 2] | ((uint8_t *)iv)[(i << 2) + 1] >> 8
                                   | ((uint8_t *)iv)[(i << 2) + 2] >> 16
                                   | ((uint8_t *)iv)[(i << 2) + 3] >> 24;
        } else {
            AES->INTVECTV[i].reg =((uint32_t *)(void *)iv)[i];
        }
    }
}

static inline void __aes_sync_get_indata(uint8_t *output, uint32_t words)
{
    uint32_t i;
    uint32_t buf;

    for (i = 0; i < words; i++) {
        if (((uint32_t)output) & 0x3) {
            buf       = (uint32_t)AES->INDATA.reg;
            *output++ = buf & 0xFF;
            *output++ = (buf >> 8) & 0xFF;
            *output++ = (buf >> 16) & 0xFF;
            *output++ = (buf >> 24) & 0xFF;
        } else {
            ((uint32_t *)(void *)output)[i] = AES->INDATA.reg;
        }
    }
}

static inline void __aes_sync_set_indata(const uint8_t *data, uint32_t words)
{
    uint32_t i;

    for (i = 0; i < words; i++) {
        if (((uint32_t)data) & 0x3) {
            AES->INDATA.reg = ((uint8_t *)data)[i << 2] | ((uint8_t *)data)[(i << 2) + 1] << 8
                              | ((uint8_t *)data)[(i << 2) + 2] << 16
                              | ((uint8_t *)data)[(i << 2) + 3] << 24;
        } else {
            AES->INDATA.reg = ((uint32_t *)(void const *)data)[i];
        }
    }
}
