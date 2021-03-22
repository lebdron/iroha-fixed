/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/gost3410_impl/crypto_provider.hpp"

#include "cryptography/gost3410_impl/signer.hpp"
#include "cryptography/gost3410_impl/verifier.hpp"
#include "cryptography/gost3410_impl/internal/gost_impl.hpp"
#include "common/hexutils.hpp"

using namespace shared_model::interface::types;

namespace shared_model::crypto{
  std::string CryptoProviderGOST3410::sign(const Blob &blob,
                                            const Keypair &keypair){
    return gost3410::Signer::sign(blob, keypair);
  }
  
  bool CryptoProviderGOST3410::verify(SignatureByteRangeView signature,
                                          const Blob &orig,
                                          PublicKeyByteRangeView public_key) {
    return gost3410::Verifier::verifyGost3410Sha512(signature, orig.range(), public_key);
  }

  Keypair CryptoProviderGOST3410::generateKeypair() {
    auto key_pair = gost3410::create_keypair();
    
    return Keypair(shared_model::interface::types::PublicKeyHexStringView(
      iroha::bytestringToHexstring(
        interface::types::makeByteRange(key_pair.first.data(), key_pair.first.size()))),
        shared_model::crypto::PrivateKey(shared_model::crypto::Blob(key_pair.second)));
  }

  constexpr size_t CryptoProviderGOST3410::kHashLength;
  constexpr size_t CryptoProviderGOST3410::kPublicKeyLength;
  constexpr size_t CryptoProviderGOST3410::kPrivateKeyLength;
  constexpr size_t CryptoProviderGOST3410::kSignatureLength;
  constexpr size_t CryptoProviderGOST3410::kSeedLength;

  const char *CryptoProviderGOST3410::kName = "Gost3410 with SHA512";
}  // namespace shared_model::crypto
