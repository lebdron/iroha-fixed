/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_YAC_GATE_HPP
#define IROHA_YAC_GATE_HPP

#include <optional>

#include "consensus/yac/cluster_order.hpp"
#include "consensus/yac/storage/storage_result.hpp"
#include "network/consensus_gate.hpp"

namespace iroha::consensus {
  struct Round;
}

namespace iroha::consensus::yac {
  class YacHash;
  class ClusterOrdering;

  class YacGate : public network::ConsensusGate {};

  /**
   * Provide gate for ya consensus
   */
  class HashGate {
   public:
    /**
     * Proposal new hash in network
     * @param hash - hash for voting
     * @param order - peer ordering for round in hash
     * @param alternative_order - peer order
     */
    virtual void vote(
        YacHash hash,
        ClusterOrdering order,
        boost::optional<ClusterOrdering> alternative_order = boost::none) = 0;

    virtual std::optional<Answer> processRoundSwitch(
        consensus::Round const &round,
        shared_model::interface::types::PeerList const &peers) = 0;

    /// Prevent any new outgoing network activity. Be passive.
    virtual void stop() = 0;

    virtual ~HashGate() = default;
  };
}  // namespace iroha::consensus::yac

#endif  // IROHA_YAC_GATE_HPP
