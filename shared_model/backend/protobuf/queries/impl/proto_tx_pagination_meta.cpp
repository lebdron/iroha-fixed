/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_tx_pagination_meta.hpp"

#include <optional>
#include "cryptography/hash.hpp"
#include <iostream>
#include <google/protobuf/util/time_util.h>
// TODO remove iostream in final version
namespace types = shared_model::interface::types;

using namespace shared_model::proto;


TxPaginationMeta::TxPaginationMeta(iroha::protocol::TxPaginationMeta &meta)
    : meta_{meta}, ordering_(meta.ordering()) {
  /// default values
  ordering_.append(interface::Ordering::Field::kPosition,
                   interface::Ordering::Direction::kAscending);
}

types::TransactionsNumberType TxPaginationMeta::pageSize() const {
  return meta_.page_size();
}

std::optional<types::HashType> TxPaginationMeta::firstTxHash() const {
  if (meta_.opt_first_tx_hash_case()
      == iroha::protocol::TxPaginationMeta::OptFirstTxHashCase::
             OPT_FIRST_TX_HASH_NOT_SET) {
    return std::nullopt;
  }
  return types::HashType::fromHexString(meta_.first_tx_hash());
}
std::optional<types::TimestampType> TxPaginationMeta::firstTxTime() const {
  if (meta_.opt_first_tx_time_case()
      == iroha::protocol::TxPaginationMeta::OptFirstTxTimeCase::
             OPT_FIRST_TX_TIME_NOT_SET) {
    return std::nullopt;
  }
  std::cout<<"before conversion "<<google::protobuf::util::TimeUtil::TimestampToNanoseconds(meta_.first_tx_time())<<std::endl;
  return google::protobuf::util::TimeUtil::TimestampToNanoseconds(meta_.first_tx_time());
}
std::optional<types::TimestampType> TxPaginationMeta::lastTxTime() const {
  if (meta_.opt_last_tx_time_case()
      == iroha::protocol::TxPaginationMeta::  OptLastTxTimeCase::
             OPT_LAST_TX_TIME_NOT_SET) {
    return std::nullopt;
  }
  return google::protobuf::util::TimeUtil::TimestampToNanoseconds(meta_.last_tx_time());
}
shared_model::interface::Ordering const &TxPaginationMeta::ordering() const {
  return ordering_;
}