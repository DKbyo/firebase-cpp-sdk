/*
 * Copyright 2017 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FIREBASE_AUTH_SRC_DESKTOP_RPCS_VERIFY_ASSERTION_REQUEST_H_
#define FIREBASE_AUTH_SRC_DESKTOP_RPCS_VERIFY_ASSERTION_REQUEST_H_

#include <memory>
#include <string>

#include "app/src/include/firebase/app.h"
#include "app/src/log.h"
#include "auth/request_generated.h"
#include "auth/request_resource.h"
#include "auth/src/desktop/rpcs/auth_request.h"

namespace firebase {
namespace auth {

class VerifyAssertionRequest : public AuthRequest {
 public:
  static std::unique_ptr<VerifyAssertionRequest> FromIdToken(
      ::firebase::App& app, const char* api_key, const char* provider_id,
      const char* id_token, const char* tenant_id);
  static std::unique_ptr<VerifyAssertionRequest> FromIdToken(
      ::firebase::App& app, const char* api_key, const char* provider_id,
      const char* id_token, const char* nonce, const char* tenant_id);
  static std::unique_ptr<VerifyAssertionRequest> FromAccessToken(
      ::firebase::App& app, const char* api_key, const char* provider_id,
      const char* access_token, const char* tenant_id);
  static std::unique_ptr<VerifyAssertionRequest> FromAccessToken(
      ::firebase::App& app, const char* api_key, const char* provider_id,
      const char* access_token, const char* nonce, const char* tenant_id);
  static std::unique_ptr<VerifyAssertionRequest> FromAccessTokenAndOAuthSecret(
      ::firebase::App& app, const char* api_key, const char* provider_id,
      const char* access_token, const char* oauth_secret, const char* tenant_id);
  static std::unique_ptr<VerifyAssertionRequest> FromAuthCode(
      ::firebase::App& app, const char* api_key, const char* provider_id,
      const char* auth_code, const char* tenant_id);

  void SetIdToken(const char* const id_token) {
    if (id_token) {
      application_data_->idToken = id_token;
      UpdatePostFields();
    } else {
      LogError("No id token given.");
    }
  }

 private:
  VerifyAssertionRequest(::firebase::App& app, const char* api_key,
                         const char* provider_id, const char* tenant_id);

  std::string post_body_;
};

}  // namespace auth
}  // namespace firebase

#endif  // FIREBASE_AUTH_SRC_DESKTOP_RPCS_VERIFY_ASSERTION_REQUEST_H_
