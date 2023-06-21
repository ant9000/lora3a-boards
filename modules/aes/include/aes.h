#ifndef AES_H
#define AES_H

#include <inttypes.h>

enum aes_action { AES_DECRYPT, AES_ENCRYPT };
enum aes_keysize { AES_KEY_128, AES_KEY_192, AES_KEY_256 };

struct aes_sync_device {
    uint8_t          key[32]; /*!< Key value 128/192/256 bits */
    uint8_t          iv[16];  /*!< Initialization Vector */
    uint32_t         aad_len; /*!< length of additional data(GCM) */
    enum aes_keysize keysize; /*!< bit length of key */
};

int32_t aes_sync_set_encrypt_key(struct aes_sync_device *const dev, const uint8_t *key, const enum aes_keysize size);
int32_t aes_sync_gcm_crypt_and_tag(struct aes_sync_device *const dev, const enum aes_action enc,
                                   const uint8_t *input, uint8_t *output, uint32_t length, const uint8_t *iv,
                                   uint32_t iv_len, const uint8_t *aad, uint32_t aad_len, uint8_t *tag,
                                   uint32_t tag_len);

#endif /* AES_H */
