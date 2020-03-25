/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/iroha_internal/block.hpp"

#include "interfaces/transaction.hpp"
#include "utils/string_builder.hpp"

namespace shared_model {
  namespace interface {

    std::string Block::toString() const {
      return detail::PrettyStringBuilder()
          .init("Block")
          .appendNamed("hash", hash())
          .appendNamed("height", height())
          .appendNamed("prevHash", prevHash())
          .appendNamed("txsNumber", txsNumber())
          .appendNamed("createdtime", createdTime())
          .appendNamed("transactions", transactions())
          .appendNamed("signatures", signatures())
          .appendNamed("rejected transactions", rejected_transactions_hashes())
          .finalize();
    }

  }  // namespace interface
}  // namespace shared_model
