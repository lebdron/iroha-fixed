/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "consensus/yac/impl/yac_crypto_provider_impl.hpp"

#include <gtest/gtest.h>

#include "consensus/yac/outcome_messages.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"

#include "framework/crypto_dummies.hpp"
#include "module/irohad/consensus/yac/mock_yac_crypto_provider.hpp"
#include "module/shared_model/interface_mocks.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::ReturnRefOfCopy;

const auto pubkey = iroha::createPublicKeyPadded();

namespace iroha {
  namespace consensus {
    namespace yac {

      class YacCryptoProviderTest : public ::testing::Test {
       public:
        YacCryptoProviderTest()
            : keypair(shared_model::crypto::DefaultCryptoAlgorithmType::
                          generateKeypair()) {}

        void SetUp() override {
          crypto_provider = std::make_shared<CryptoProviderImpl>(keypair);
        }

        std::shared_ptr<shared_model::interface::Signature> makeSignature() {
          return createSig(pubkey);
        }

        const shared_model::crypto::Keypair keypair;
        std::shared_ptr<CryptoProviderImpl> crypto_provider;
      };

      TEST_F(YacCryptoProviderTest, ValidWhenSameMessage) {
        YacHash hash(Round{1, 1}, "1", "1");

        hash.block_signature = makeSignature();

        auto vote = crypto_provider->getVote(hash);

        ASSERT_TRUE(crypto_provider->verify({vote}));
      }

      TEST_F(YacCryptoProviderTest, InvalidWhenMessageChanged) {
        YacHash hash(Round{1, 1}, "1", "1");

        hash.block_signature = makeSignature();

        auto vote = crypto_provider->getVote(hash);

        vote.hash.vote_hashes.block_hash = "hash changed";

        ASSERT_FALSE(crypto_provider->verify({vote}));
      }

    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha
