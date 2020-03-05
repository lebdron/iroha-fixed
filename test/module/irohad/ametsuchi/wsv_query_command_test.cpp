/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gmock/gmock.h>

#include "ametsuchi/impl/postgres_wsv_command.hpp"
#include "ametsuchi/impl/postgres_wsv_query.hpp"
#include "framework/result_gtest_checkers.hpp"
#include "framework/test_logger.hpp"
#include "module/irohad/ametsuchi/ametsuchi_fixture.hpp"
#include "module/shared_model/interface_mocks.hpp"

namespace iroha {
  namespace ametsuchi {

    class WsvQueryCommandTest : public AmetsuchiTest {
     public:
      void SetUp() override {
        AmetsuchiTest::SetUp();
        sql = std::make_unique<soci::session>(*soci::factory_postgresql(),
                                              pgopt_);

        command = std::make_unique<PostgresWsvCommand>(*sql);
        query =
            std::make_unique<PostgresWsvQuery>(*sql, getTestLogger("WsvQuery"));
      }

      void TearDown() override {
        sql->close();
        AmetsuchiTest::TearDown();
      }

      std::unique_ptr<soci::session> sql;

      std::unique_ptr<WsvCommand> command;
      std::unique_ptr<WsvQuery> query;
    };

    class RoleTest : public WsvQueryCommandTest {};

    TEST_F(RoleTest, InsertTwoRole) {
      IROHA_ASSERT_RESULT_VALUE(command->insertRole("role"));
      IROHA_ASSERT_RESULT_ERROR(command->insertRole("role"));
    }

    class DeletePeerTest : public WsvQueryCommandTest {
     public:
      void SetUp() override {
        WsvQueryCommandTest::SetUp();

        peer = makePeer(address, pk);
      }
      std::shared_ptr<MockPeer> peer;
      shared_model::interface::types::AddressType address{""};
      shared_model::interface::types::PubkeyType pk{""};
    };

    /**
     * @given storage with peer
     * @when trying to delete existing peer
     * @then peer is successfully deleted
     */
    TEST_F(DeletePeerTest, DeletePeerValidWhenPeerExists) {
      IROHA_ASSERT_RESULT_VALUE(command->insertPeer(*peer));

      IROHA_ASSERT_RESULT_VALUE(command->deletePeer(*peer));
    }

  }  // namespace ametsuchi
}  // namespace iroha
