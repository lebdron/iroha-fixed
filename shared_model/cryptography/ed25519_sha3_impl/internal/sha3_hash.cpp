/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"

#include <ed25519/ed25519/sha256.h>
#include <ed25519/ed25519/sha512.h>

namespace iroha {

  void sha3_256(uint8_t *output, const uint8_t *input, size_t in_size) {
    iroha_ed25519_sha256(output, input, in_size);
  }

  void sha3_512(uint8_t *output, const uint8_t *input, size_t in_size) {
    iroha_ed25519_sha512(output, input, in_size);
  }

  hash256_t sha3_256(shared_model::interface::types::ByteRange input) {
    hash256_t h;
    sha3_256(h.data(),
             reinterpret_cast<uint8_t const *>(input.data()),
             input.size());
    return h;
  }

  hash512_t sha3_512(const uint8_t *input, size_t in_size) {
    hash512_t h;
    sha3_512(h.data(), input, in_size);
    return h;
  }

  hash256_t sha3_256(const std::string &msg) {
    hash256_t h;
    sha3_256(
        h.data(), reinterpret_cast<const uint8_t *>(msg.data()), msg.size());
    return h;
  }

  hash512_t sha3_512(const std::string &msg) {
    hash512_t h;
    sha3_512(
        h.data(), reinterpret_cast<const uint8_t *>(msg.data()), msg.size());
    return h;
  }

  hash512_t sha3_512(const std::vector<uint8_t> &msg) {
    hash512_t h;
    sha3_512(h.data(), msg.data(), msg.size());
    return h;
  }

  hash256_t sha3_256(const std::vector<uint8_t> &msg) {
    hash256_t h;
    sha3_256(h.data(), msg.data(), msg.size());
    return h;
  }
}  // namespace iroha
