/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main/server_runner.hpp"

#include <chrono>

#include <grpc/impl/codegen/grpc_types.h>
#include <boost/format.hpp>
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"
#include "main/server_runner_auth.hpp"
#include "network/impl/tls_credentials.hpp"
#include "network/peer_tls_certificates_provider.hpp"

using namespace iroha::network;

namespace {

  const auto kPortBindError = "Cannot bind server to address %s";

  std::shared_ptr<grpc::ServerCredentials> createCredentials(
      const boost::optional<std::shared_ptr<const TlsCredentials>>
          &my_tls_creds,
      const boost::optional<std::shared_ptr<const PeerTlsCertificatesProvider>>
          &peer_tls_certificates_provider,
      logger::LoggerPtr log) {
    if (not my_tls_creds) {
      return grpc::InsecureServerCredentials();
    }
    auto options = grpc::SslServerCredentialsOptions(
        peer_tls_certificates_provider
            ? GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_BUT_DONT_VERIFY
            : GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE);
    grpc::SslServerCredentialsOptions::PemKeyCertPair keypair = {
        my_tls_creds.value()->private_key, my_tls_creds.value()->certificate};
    options.pem_key_cert_pairs.push_back(keypair);
    std::shared_ptr<grpc::ServerCredentials> credentials =
        grpc::SslServerCredentials(options);
    if (peer_tls_certificates_provider) {
      credentials->SetAuthMetadataProcessor(
          std::make_shared<PeerCertificateAuthMetadataProcessor>(
              peer_tls_certificates_provider.value(), std::move(log)));
    }
    return credentials;
  }

}  // namespace

ServerRunner::ServerRunner(
    const std::string &address,
    logger::LoggerManagerTreePtr log_manager,
    bool reuse,
    const boost::optional<std::shared_ptr<const TlsCredentials>> &my_tls_creds,
    const boost::optional<std::shared_ptr<const PeerTlsCertificatesProvider>>
        &peer_tls_certificates_provider)
    : log_(log_manager->getLogger()),
      server_address_(address),
      credentials_(createCredentials(
          my_tls_creds,
          peer_tls_certificates_provider,
          log_manager->getChild("AuthMetaProcessor")->getLogger())),
      reuse_(reuse) {}

ServerRunner::~ServerRunner() {
  shutdown(std::chrono::system_clock::now());
}

ServerRunner &ServerRunner::append(std::shared_ptr<grpc::Service> service) {
  services_.push_back(service);
  return *this;
}

iroha::expected::Result<int, std::string> ServerRunner::run() {
  grpc::ServerBuilder builder;
  int selected_port = 0;

  if (not reuse_) {
    builder.AddChannelArgument(GRPC_ARG_ALLOW_REUSEPORT, 0);
  }

  builder.AddListeningPort(server_address_, credentials_, &selected_port);

  for (auto &service : services_) {
    builder.RegisterService(service.get());
  }

  // in order to bypass built-it limitation of gRPC message size
  builder.SetMaxReceiveMessageSize(INT_MAX);
  builder.SetMaxSendMessageSize(INT_MAX);

  // enable retry policy
  builder.AddChannelArgument(GRPC_ARG_ENABLE_RETRIES, 1);

  server_instance_ = builder.BuildAndStart();
  server_instance_cv_.notify_one();

  if (selected_port == 0) {
    return iroha::expected::makeError(
        (boost::format(kPortBindError) % server_address_).str());
  }

  return iroha::expected::makeValue(selected_port);
}

void ServerRunner::waitForServersReady() {
  std::unique_lock<std::mutex> lock(wait_for_server_);
  while (not server_instance_) {
    server_instance_cv_.wait(lock);
  }
}

void ServerRunner::shutdown() {
  if (server_instance_) {
    server_instance_->Shutdown();
  } else {
    log_->warn("Tried to shutdown without a server instance");
  }
}

void ServerRunner::shutdown(
    const std::chrono::system_clock::time_point &deadline) {
  if (server_instance_) {
    server_instance_->Shutdown(deadline);
  } else {
    log_->warn("Tried to shutdown without a server instance");
  }
}
