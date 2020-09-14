/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CONSENSUS_OUTCOME_TYPE_HPP
#define IROHA_CONSENSUS_OUTCOME_TYPE_HPP

namespace iroha {
  namespace consensus {
    namespace yac {
      enum class ConsensusOutcomeType {
        kCommit,   /// commit for current round
        kFuture,   /// future round event
        kNothing,  /// peers voted for an empty hash
        kReject,   /// peers voted for different hashes
      };
    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha

#endif  // IROHA_CONSENSUS_CONSISTENCY_MODEL_HPP
