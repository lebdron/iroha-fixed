/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module/shared_model/mock_objects_factories/mock_query_factory.hpp"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::ReturnRefOfCopy;

using namespace shared_model::interface;

template <typename QueryMock, typename ExpectationsSetter>
MockQueryFactory::FactoryResult<QueryMock>
MockQueryFactory::createFactoryResult(
    const ExpectationsSetter &expectations_setter) const {
  auto result = std::make_unique<QueryMock>();
  expectations_setter(*result);
  return result;
}

MockQueryFactory::FactoryResult<MockAssetPaginationMeta>
MockQueryFactory::constructAssetPaginationMeta(
    types::TransactionsNumberType page_size,
    boost::optional<types::AssetIdType> first_asset_id) const {
  return createFactoryResult<MockAssetPaginationMeta>(
      [&page_size, &first_asset_id](MockAssetPaginationMeta &mock) {
        EXPECT_CALL(mock, pageSize()).WillRepeatedly(Return(page_size));
        EXPECT_CALL(mock, firstAssetId())
            .WillRepeatedly(Return(first_asset_id));
      });
}

MockQueryFactory::FactoryResult<MockGetAccountAssets>
MockQueryFactory::constructGetAccountAssets(
    const types::AccountIdType &account_id,
    boost::optional<const interface::AssetPaginationMeta &> pagination_meta)
    const {
  return createFactoryResult<MockGetAccountAssets>(
      [&account_id, &pagination_meta](MockGetAccountAssets &mock) {
        EXPECT_CALL(mock, accountId()).WillRepeatedly(ReturnRef(account_id));
        EXPECT_CALL(mock, paginationMeta())
            .WillRepeatedly(Return(pagination_meta));
      });
}

MockQueryFactory::FactoryResult<MockGetAccountAssetTransactions>
MockQueryFactory::constructGetAccountAssetTransactions(
    const types::AccountIdType &account_id,
    const types::AccountIdType &asset_id,
    const TxPaginationMeta &pagination_meta) const {
  return createFactoryResult<MockGetAccountAssetTransactions>(
      [&account_id, &asset_id, &pagination_meta](
          MockGetAccountAssetTransactions &mock) {
        EXPECT_CALL(mock, accountId()).WillRepeatedly(ReturnRef(account_id));
        EXPECT_CALL(mock, assetId()).WillRepeatedly(ReturnRef(asset_id));
        EXPECT_CALL(mock, paginationMeta())
            .WillRepeatedly(ReturnRef(pagination_meta));
      });
}

MockQueryFactory::FactoryResult<MockGetAccountDetail>
MockQueryFactory::constructGetAccountDetail(
    const types::AccountIdType &account_id,
    boost::optional<types::AccountDetailKeyType> key,
    boost::optional<types::AccountIdType> writer) const {
  return createFactoryResult<MockGetAccountDetail>(
      [&account_id, &key, &writer](MockGetAccountDetail &mock) {
        EXPECT_CALL(mock, accountId()).WillRepeatedly(ReturnRef(account_id));
        EXPECT_CALL(mock, key()).WillRepeatedly(Return(key));
        EXPECT_CALL(mock, writer()).WillRepeatedly(Return(writer));
      });
}

MockQueryFactory::FactoryResult<MockGetAccount>
MockQueryFactory::constructGetAccount(
    const types::AccountIdType &account_id) const {
  return createFactoryResult<MockGetAccount>(
      [&account_id](MockGetAccount &mock) {
        EXPECT_CALL(mock, accountId()).WillRepeatedly(ReturnRef(account_id));
      });
}

MockQueryFactory::FactoryResult<MockGetAccountTransactions>
MockQueryFactory::constructGetAccountTransactions(
    const types::AccountIdType &account_id,
    const TxPaginationMeta &pagination_meta) const {
  return createFactoryResult<MockGetAccountTransactions>(
      [&account_id, &pagination_meta](MockGetAccountTransactions &mock) {
        EXPECT_CALL(mock, accountId()).WillRepeatedly(ReturnRef(account_id));
        EXPECT_CALL(mock, paginationMeta())
            .WillRepeatedly(ReturnRef(pagination_meta));
      });
}

MockQueryFactory::FactoryResult<MockGetAssetInfo>
MockQueryFactory::constructGetAssetInfo(
    const types::AssetIdType &asset_id) const {
  return createFactoryResult<MockGetAssetInfo>(
      [&asset_id](MockGetAssetInfo &mock) {
        EXPECT_CALL(mock, assetId()).WillRepeatedly(ReturnRef(asset_id));
      });
}

MockQueryFactory::FactoryResult<MockGetBlock>
MockQueryFactory::constructGetBlock(types::HeightType height) const {
  return createFactoryResult<MockGetBlock>([&height](MockGetBlock &mock) {
    EXPECT_CALL(mock, height()).WillRepeatedly(Return(height));
  });
}

MockQueryFactory::FactoryResult<MockGetRolePermissions>
MockQueryFactory::constructGetRolePermissions(
    const types::RoleIdType &role_id) const {
  return createFactoryResult<MockGetRolePermissions>(
      [&role_id](MockGetRolePermissions &mock) {
        EXPECT_CALL(mock, roleId()).WillRepeatedly(ReturnRef(role_id));
      });
}

MockQueryFactory::FactoryResult<MockGetSignatories>
MockQueryFactory::constructGetSignatories(
    const types::AccountIdType &account_id) const {
  return createFactoryResult<MockGetSignatories>(
      [&account_id](MockGetSignatories &mock) {
        EXPECT_CALL(mock, accountId()).WillRepeatedly(ReturnRef(account_id));
      });
}

MockQueryFactory::FactoryResult<MockGetTransactions>
MockQueryFactory::constructGetTransactions(
    const GetTransactions::TransactionHashesType &transaction_hashes) const {
  return createFactoryResult<MockGetTransactions>(
      [&transaction_hashes](MockGetTransactions &mock) {
        EXPECT_CALL(mock, transactionHashes())
            .WillRepeatedly(ReturnRef(transaction_hashes));
      });
}

MockQueryFactory::FactoryResult<MockTxPaginationMeta>
MockQueryFactory::constructTxPaginationMeta(
    types::TransactionsNumberType page_size,
    boost::optional<types::HashType> first_tx_hash) const {
  return createFactoryResult<MockTxPaginationMeta>(
      [&page_size, &first_tx_hash](MockTxPaginationMeta &mock) {
        EXPECT_CALL(mock, pageSize()).WillRepeatedly(Return(page_size));
        EXPECT_CALL(mock, firstTxHash()).WillRepeatedly(Return(first_tx_hash));
      });
}
