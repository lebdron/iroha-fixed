/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_SIGNER_HPP
#define IROHA_CRYPTO_SIGNER_HPP

#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "cryptography/signed.hpp"

namespace shared_model {
  namespace crypto {
    class BytesView;
    class Keypair;

    /**
     * CryptoSigner - wrapper for generalization signing for different
     * cryptographic algorithms
     * @tparam Algorithm - cryptographic algorithm for singing
     */
    template <typename Algorithm = DefaultCryptoAlgorithmType>
    class CryptoSigner {
     public:
      /**
       * Generate signature for target data
       * @param blob - data for signing
       * @param keypair - (public, private) keys for signing
       * @return signature's blob
       */
      static Signed sign(const BytesView &blob, const Keypair &keypair) {
        return Algorithm::sign(blob, keypair);
      }

      /// close constructor for forbidding instantiation
      CryptoSigner() = delete;
    };
  }  // namespace crypto
}  // namespace shared_model
#endif  // IROHA_CRYPTO_SIGNER_HPP
