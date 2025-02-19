// Copyright 2017 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "auth/src/desktop/rpcs/verify_assertion_request.h"

#include "app/src/assert.h"
#include "app/src/include/firebase/app.h"

namespace firebase {
namespace auth {

VerifyAssertionRequest::VerifyAssertionRequest(::firebase::App& app,
                                               const char* const api_key,
                                               const char* const provider_id,
                                               const char* tenant_id)
    : AuthRequest(app, request_resource_data, true) {
  FIREBASE_ASSERT_RETURN_VOID(api_key);

  const char api_host[] =
      "https://www.googleapis.com/identitytoolkit/v3/relyingparty/"
      "verifyAssertion?key=";
  std::string url;
  url.reserve(strlen(api_host) + strlen(api_key));
  url.append(api_host);
  url.append(api_key);
  set_url(url.c_str());
  application_data_->requestUri = url;

  if (provider_id) {
    post_body_ = std::string("providerId=") + provider_id;
  } else {
    LogError("No provider id given");
  }
  if (tenant_id != nullptr){
    application_data_->tenantId = tenant_id;
  }
  application_data_->returnSecureToken = true;
}

std::unique_ptr<VerifyAssertionRequest> VerifyAssertionRequest::FromIdToken(
    ::firebase::App& app, const char* const api_key,
    const char* const provider_id, const char* const id_token, const char* tenant_id) {
  return FromIdToken(app, api_key, provider_id, id_token, /*nonce=*/nullptr,
                     tenant_id);
}

std::unique_ptr<VerifyAssertionRequest> VerifyAssertionRequest::FromIdToken(
    ::firebase::App& app, const char* const api_key,
    const char* const provider_id, const char* const id_token,
    const char* nonce, const char* tenant_id) {
  auto request = std::unique_ptr<VerifyAssertionRequest>(
      new VerifyAssertionRequest{app, api_key, provider_id, tenant_id});  // NOLINT

  if (id_token) {
    request->post_body_ += std::string("&id_token=") + id_token;
  } else {
    LogError("No id token given");
  }

  if (nonce) {
    request->post_body_ += std::string("&nonce=") + nonce;
  }
  request->application_data_->postBody = request->post_body_;
  request->UpdatePostFields();
  return request;
}

std::unique_ptr<VerifyAssertionRequest> VerifyAssertionRequest::FromAccessToken(
    ::firebase::App& app, const char* const api_key,
    const char* const provider_id, const char* const access_token,
    const char* tenant_id) {
  return FromAccessToken(app, api_key, provider_id, access_token,
                         /*nonce=*/nullptr, tenant_id);
}

std::unique_ptr<VerifyAssertionRequest> VerifyAssertionRequest::FromAccessToken(
    ::firebase::App& app, const char* const api_key,
    const char* const provider_id, const char* const access_token,
    const char* nonce, const char* tenant_id) {
  auto request = std::unique_ptr<VerifyAssertionRequest>(
      new VerifyAssertionRequest{app, api_key, provider_id, tenant_id});  // NOLINT

  if (access_token) {
    request->post_body_ += std::string("&access_token=") + access_token;
  } else {
    LogError("No access token given");
  }

  if (nonce) {
    request->post_body_ += std::string("&nonce=") + nonce;
  }

  request->application_data_->postBody = request->post_body_;
  request->UpdatePostFields();
  return request;
}

std::unique_ptr<VerifyAssertionRequest>
VerifyAssertionRequest::FromAccessTokenAndOAuthSecret(
    ::firebase::App& app, const char* const api_key,
    const char* const provider_id, const char* const access_token,
    const char* const oauth_secret,
    const char* tenant_id) {
  auto request = std::unique_ptr<VerifyAssertionRequest>(
      new VerifyAssertionRequest{app, api_key, provider_id, tenant_id});  // NOLINT

  if (access_token) {
    request->post_body_ += std::string("&access_token=") + access_token;
  } else {
    LogError("No access token given");
  }
  if (oauth_secret) {
    request->post_body_ += std::string("&oauth_token_secret=") + oauth_secret;
  } else {
    LogError("No OAuth secret given");
  }

  request->application_data_->postBody = request->post_body_;
  request->UpdatePostFields();
  return request;
}

static std::unique_ptr<VerifyAssertionRequest> FromAuthCode(
    ::firebase::App& app, const char* api_key, const char* provider_id,
    const char* auth_code);

std::unique_ptr<VerifyAssertionRequest> VerifyAssertionRequest::FromAuthCode(
    ::firebase::App& app, const char* const api_key,
    const char* const provider_id, const char* const auth_code,
    const char* tenant_id) {
  auto request = std::unique_ptr<VerifyAssertionRequest>(
      new VerifyAssertionRequest{app, api_key, provider_id, tenant_id});  // NOLINT

  if (auth_code) {
    request->post_body_ += std::string("&code=") + auth_code;
  } else {
    LogError("No server auth code given");
  }

  request->application_data_->postBody = request->post_body_;
  request->UpdatePostFields();
  return request;
}

}  // namespace auth
}  // namespace firebase
