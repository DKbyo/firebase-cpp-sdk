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

#include <memory>

#include "app/rest/transport_builder.h"
#include "app/src/include/firebase/app.h"
#include "app/tests/include/firebase/app_for_testing.h"
#include "auth/src/desktop/rpcs/verify_custom_token_request.h"
#include "auth/src/desktop/rpcs/verify_custom_token_response.h"
#include "auth/tests/desktop/rpcs/test_util.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace firebase {
namespace auth {

// Test VerifyCustomTokenRequest
TEST(VerifyCustomTokenTest, TestVerifyCustomTokenRequest) {
  std::unique_ptr<App> app(testing::CreateApp());
  VerifyCustomTokenRequest request(*app, "APIKEY", "token123", nullptr);
  EXPECT_EQ(
      "https://www.googleapis.com/identitytoolkit/v3/relyingparty/"
      "verifyCustomToken?key=APIKEY",
      request.options().url);
  EXPECT_EQ(
      "{\n"
      "  returnSecureToken: true,\n"
      "  token: \"token123\"\n"
      "}\n",
      request.options().post_fields);
}

// Test VerifyCustomTokenRequest with tenant
TEST(VerifyCustomTokenTest, TestVerifyCustomTokenTenantRequest) {
  std::unique_ptr<App> app(testing::CreateApp());
  VerifyCustomTokenRequest request(*app, "APIKEY", "token123", "tenant123");
  EXPECT_EQ(
      "https://www.googleapis.com/identitytoolkit/v3/relyingparty/"
      "verifyCustomToken?key=APIKEY",
      request.options().url);
  EXPECT_EQ(
      "{\n"
      "  returnSecureToken: true,\n"
      "  token: \"token123\",\n"
      "  tenantId: \"tenant123\"\n"
      "}\n",
      request.options().post_fields);
}

// Test VerifyCustomTokenResponse
TEST(VerifyCustomTokenTest, TestVerifyCustomTokenResponse) {
  std::unique_ptr<App> app(testing::CreateApp());
  VerifyCustomTokenResponse response;
  // An example HTTP response JSON.
  const char body[] =
      "{\n"
      " \"kind\": \"identitytoolkit#VerifyCustomTokenResponse\",\n"
      " \"idToken\": \"idtoken123\",\n"
      " \"refreshToken\": \"refreshtoken123\",\n"
      " \"expiresIn\": \"3600\",\n"
      "}";
  response.ProcessBody(body, sizeof(body));
  response.MarkCompleted();
  EXPECT_EQ("idtoken123", response.id_token());
  EXPECT_EQ("refreshtoken123", response.refresh_token());
}

TEST(VerifyCustomTokenTest, TestErrorResponse) {
  std::unique_ptr<App> app(testing::CreateApp());
  VerifyCustomTokenResponse response;
  const char body[] =
      "{\n"
      "  \"error\": {\n"
      "    \"code\": 400,\n"
      "    \"message\": \"CREDENTIAL_MISMATCH\",\n"
      "    \"errors\": [\n"
      "      {\n"
      "        \"reason\": \"some reason\"\n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "}";
  response.ProcessBody(body, sizeof(body));
  response.MarkCompleted();

  EXPECT_EQ(kAuthErrorCustomTokenMismatch, response.error_code());

  // Make sure response doesn't crash on access.
  EXPECT_EQ("", response.local_id());
  EXPECT_EQ("", response.id_token());
  EXPECT_EQ("", response.refresh_token());
  EXPECT_EQ(0, response.expires_in());
}
}  // namespace auth
}  // namespace firebase
