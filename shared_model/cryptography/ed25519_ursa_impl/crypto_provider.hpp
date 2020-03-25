/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_URSA_CRYPTOPROVIDER_HPP
#define IROHA_URSA_CRYPTOPROVIDER_HPP

#include "cryptography/bytes_view.hpp"
#include "cryptography/keypair.hpp"
#include "cryptography/private_key.hpp"
#include "cryptography/public_key.hpp"
#include "cryptography/seed.hpp"
#include "cryptography/signed.hpp"

namespace shared_model {
  namespace crypto {
    /**
     * Wrapper class for signing-related stuff.
     */
    class CryptoProviderEd25519Ursa {
     public:
      /**
       * Signs the message.
       * @param blob - blob to sign
       * @param keypair - keypair
       * @return Signed object with signed data
       */
      static Signed sign(const BytesView &blob, const Keypair &keypair);

      /**
       * Verifies signature.
       * @param signedData - data to verify
       * @param orig - original message
       * @param publicKey - public key
       * @return true if verify was OK or false otherwise
       */
      static bool verify(const Signed &signed_data,
                         const BytesView &orig,
                         const PublicKey &public_key);

      /**
       * Generates new keypair with a default seed
       * @return Keypair generated
       */
      static Keypair generateKeypair();

      /**
       * Generates new keypair from a provided seed
       * @param seed - provided seed
       * @return generated keypair
       */
      static Keypair generateKeypair(const Seed &seed);

      static const size_t kHashLength;
      static const size_t kPublicKeyLength;
      static const size_t kPrivateKeyLength;
      static const size_t kSignatureLength;
    };
  }  // namespace crypto
}  // namespace shared_model

#endif  // IROHA_URSA_CRYPTOPROVIDER_HPP
