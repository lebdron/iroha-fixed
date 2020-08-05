/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <memory>
#include <optional>
#include <string>

#include "validators/validation_error_output.hpp"

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "cryptography/crypto_provider/crypto_signer_internal.hpp"
#include "cryptography/crypto_provider/crypto_verifier.hpp"
#include "cryptography/ed25519_sha3_impl/crypto_provider.hpp"
#include "framework/crypto_literals.hpp"
#include "framework/result_gtest_checkers.hpp"
#include "framework/test_crypto_verifier.hpp"
#include "module/irohad/common/validators_config.hpp"
#include "module/shared_model/builders/protobuf/test_block_builder.hpp"
#include "module/shared_model/builders/protobuf/test_query_builder.hpp"
#include "module/shared_model/builders/protobuf/test_transaction_builder.hpp"
#include "module/shared_model/cryptography/crypto_defaults.hpp"
#include "module/shared_model/cryptography/make_default_crypto_signer.hpp"
#include "multihash/multihash.hpp"
#include "multihash/type.hpp"
#include "validators/field_validator.hpp"

#if defined(USE_LIBURSA)
#include "cryptography/ed25519_ursa_impl/crypto_provider.hpp"
#endif

using namespace common_constants;
using namespace shared_model::crypto;
using namespace shared_model::interface::types;

using shared_model::validation::ValidationError;

static const auto kBadSignatureMatcher{::testing::Optional(::testing::Property(
    &ValidationError::toString, ::testing::HasSubstr("Bad signature")))};
static const auto kNoSignatureMatcher{::testing::Optional(::testing::Property(
    &ValidationError::toString, ::testing::HasSubstr("Signatures are empty")))};

template <typename CurrentCryptoProviderParam>
class CryptoUsageTest : public ::testing::Test {
 public:
  using CurrentCryptoProvider = CurrentCryptoProviderParam;

  virtual void SetUp() {
    auto creator = "a@domain";
    auto account_id = "b@domain";

    // initialize block
    block = std::make_unique<shared_model::proto::Block>(
        TestBlockBuilder().height(1).build());

    // initialize query
    query = std::make_unique<shared_model::proto::Query>(
        TestQueryBuilder()
            .creatorAccountId(creator)
            .queryCounter(1)
            .getAccount(account_id)
            .build());

    // initialize transaction
    transaction = std::make_unique<shared_model::proto::Transaction>(
        TestTransactionBuilder()
            .creatorAccountId(account_id)
            .setAccountQuorum(account_id, 2)
            .build());

    data = Blob("raw data for signing");
  }

  template <typename T>
  void signIncorrect(T &signable) {
    // initialize wrong signature
    auto signature_hex =
        signer_->sign(shared_model::crypto::Blob{"wrong payload"});
    signable.addSignature(SignedHexStringView{signature_hex},
                          PublicKeyHexStringView{signer_->publicKey()});
  }

  template <typename T>
  std::optional<ValidationError> verify(const T &signable) const {
    return field_validator_.validateSignatures(signable.signatures(),
                                               signable.payload());
  }

  Blob data;
  std::shared_ptr<shared_model::crypto::CryptoSigner> signer_ =
      std::make_shared<CryptoSignerInternal<CurrentCryptoProvider>>(
          CurrentCryptoProvider::generateKeypair());

  template <typename T>
  void sign(T &o) {
    o.addSignature(SignedHexStringView{signer_->sign(o.payload())},
                   signer_->publicKey());
  }

  shared_model::validation::FieldValidator field_validator_{
      iroha::test::getTestsValidatorsConfig()};

  std::unique_ptr<shared_model::proto::Block> block;
  std::unique_ptr<shared_model::proto::Query> query;
  std::unique_ptr<shared_model::proto::Transaction> transaction;
};

using CryptoUsageTestTypes = ::testing::Types<CryptoProviderEd25519Sha3
#if defined(USE_LIBURSA)
                                              ,
                                              CryptoProviderEd25519Ursa
#endif
                                              >;
TYPED_TEST_CASE(CryptoUsageTest, CryptoUsageTestTypes, );

/**
 * @given Initialized keypiar with _concrete_ algorithm
 * @when sign date without knowledge of cryptography algorithm
 * @then check that siganture valid without clarification of algorithm
 */
TYPED_TEST(CryptoUsageTest, RawSignAndVerifyTest) {
  auto signature_hex = this->signer_->sign(this->data);
  using namespace shared_model::interface::types;
  auto verified = iroha::test::getTestCryptoVerifier()->verify(
      SignedHexStringView{signature_hex},
      this->data,
      PublicKeyHexStringView{this->signer_->publicKey()});
  IROHA_ASSERT_RESULT_VALUE(verified);
}

/**
 * @given unsigned block
 * @when verify block
 * @then block is not verified
 */
TYPED_TEST(CryptoUsageTest, UnsignedBlock) {
  ASSERT_THAT(this->verify(*this->block), kNoSignatureMatcher);
}

/**
 * @given properly signed block
 * @when verify block
 * @then block is verified
 */
TYPED_TEST(CryptoUsageTest, SignAndVerifyBlock) {
  this->sign(*this->block);

  EXPECT_EQ(this->verify(*this->block), std::nullopt);
}

/**
 * @given block with inctorrect sign
 * @when verify block
 * @then block is not verified
 */
TYPED_TEST(CryptoUsageTest, SignAndVerifyBlockWithWrongSignature) {
  this->signIncorrect(*this->block);

  EXPECT_THAT(this->verify(*this->block), kBadSignatureMatcher);
}

/**
 * @given unsigned query
 * @when verify query
 * @then query is not verified
 */
TYPED_TEST(CryptoUsageTest, UnsignedQuery) {
  ASSERT_THAT(this->verify(*this->query), kNoSignatureMatcher);
}

/**
 * @given properly signed query
 * @when verify query
 * @then query is verified
 */
TYPED_TEST(CryptoUsageTest, SignAndVerifyQuery) {
  this->sign(*this->query);

  EXPECT_EQ(this->verify(*this->query), std::nullopt);
}

/**
 * @given query with incorrect sign
 * @when verify query
 * @then query is not verified
 */
TYPED_TEST(CryptoUsageTest, SignAndVerifyQuerykWithWrongSignature) {
  this->signIncorrect(*this->query);

  EXPECT_THAT(this->verify(*this->query), kBadSignatureMatcher);
}

/**
 * @given query hash
 * @when sign query
 * @then query hash doesn't change
 */
TYPED_TEST(CryptoUsageTest, SameQueryHashAfterSign) {
  auto hash_before = this->query->hash();
  this->sign(*this->query);
  auto hash_signed = this->query->hash();

  ASSERT_EQ(hash_signed, hash_before);
}

/**
 * @given unsigned transaction
 * @when verify transaction
 * @then transaction is not verified
 */
TYPED_TEST(CryptoUsageTest, UnsignedTransaction) {
  ASSERT_THAT(this->verify(*this->transaction), kNoSignatureMatcher);
}

/**
 * @given properly signed transaction
 * @when verify transaction
 * @then transaction is verified
 */
TYPED_TEST(CryptoUsageTest, SignAndVerifyTransaction) {
  this->sign(*this->transaction);

  EXPECT_EQ(this->verify(*this->transaction), std::nullopt);
}

/**
 * @given transaction with incorrect sign
 * @when verify transaction
 * @then transaction is not verified
 */
TYPED_TEST(CryptoUsageTest, SignAndVerifyTransactionkWithWrongSignature) {
  this->signIncorrect(*this->transaction);

  EXPECT_THAT(this->verify(*this->transaction), kBadSignatureMatcher);
}

/**
 * @given a multihash public key of some unknown algorithm
 * @when trying to verify a signature with this public key
 * @then there is a correct error
 */
TEST(CryptoUsageTest, UnimplementedCryptoMultihashPubkey) {
  std::string hex_pubkey;
  iroha::multihash::encodeHexAppend(
      iroha::multihash::Type{123}, "blah"_byterange, hex_pubkey);

  using namespace shared_model::interface::types;
  auto verified = iroha::test::getTestCryptoVerifier()->verify(
      "F000"_hex_sig, Blob{"moo"}, PublicKeyHexStringView{hex_pubkey});
  IROHA_ASSERT_RESULT_ERROR(verified);
  EXPECT_THAT(verified.assumeError(),
              ::testing::HasSubstr("Unimplemented signature algorithm."));
}
