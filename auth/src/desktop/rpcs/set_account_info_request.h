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

#ifndef FIREBASE_AUTH_SRC_DESKTOP_RPCS_SET_ACCOUNT_INFO_REQUEST_H_
#define FIREBASE_AUTH_SRC_DESKTOP_RPCS_SET_ACCOUNT_INFO_REQUEST_H_

#include <memory>
#include <string>

#include "app/src/include/firebase/app.h"
#include "app/src/log.h"
#include "auth/request_generated.h"
#include "auth/request_resource.h"
#include "auth/src/desktop/rpcs/auth_request.h"

namespace firebase {
namespace auth {

class SetAccountInfoRequest : public AuthRequest {
 public:
  static std::unique_ptr<SetAccountInfoRequest> CreateUpdateEmailRequest(
      ::firebase::App& app, const char* api_key, const char* email,
      const char* tenant_id);
  static std::unique_ptr<SetAccountInfoRequest> CreateUpdatePasswordRequest(
      ::firebase::App& app, const char* api_key, const char* password,
      const char* language_code = nullptr, const char* tenant_id = nullptr);
  static std::unique_ptr<SetAccountInfoRequest>
  CreateLinkWithEmailAndPasswordRequest(::firebase::App& app,
                                        const char* api_key, const char* email,
                                        const char* password, const char* tenant_id);
  static std::unique_ptr<SetAccountInfoRequest> CreateUpdateProfileRequest(
      ::firebase::App& app, const char* api_key, const char* set_display_name,
      const char* set_photo_url, const char* tenant_id);
  static std::unique_ptr<SetAccountInfoRequest> CreateUnlinkProviderRequest(
      ::firebase::App& app, const char* api_key, const char* provider,
      const char* tenant_id);

  void SetIdToken(const char* const id_token) {
    if (id_token) {
      application_data_->idToken = id_token;
      UpdatePostFields();
    } else {
      LogError("No id token given.");
    }
  }

 private:
  explicit SetAccountInfoRequest(::firebase::App& app, const char* api_key,
                                 const char* tenant_id);
  static std::unique_ptr<SetAccountInfoRequest> CreateRequest(
      ::firebase::App& app, const char* api_key, const char* tenant_id);
};

}  // namespace auth
}  // namespace firebase

#endif  // FIREBASE_AUTH_SRC_DESKTOP_RPCS_SET_ACCOUNT_INFO_REQUEST_H_
