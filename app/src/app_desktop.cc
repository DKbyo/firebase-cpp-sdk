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

#include "app/src/app_desktop.h"

#include <string.h>

#include <fstream>
#include <memory>
#include <string>

#include "app/src/app_common.h"
#include "app/src/function_registry.h"
#include "app/src/heartbeat/heartbeat_controller_desktop.h"
#include "app/src/include/firebase/app.h"
#include "app/src/include/firebase/internal/common.h"
#include "app/src/include/firebase/version.h"
#include "app/src/log.h"
#include "app/src/semaphore.h"
#include "app/src/util.h"

namespace firebase {
DEFINE_FIREBASE_VERSION_STRING(Firebase);

namespace {

// Size is arbitrary, just making sure that there is a sane limit.
static const int kMaxBuffersize = 1024 * 500;

// Attempts to load a config file from the path specified, and use it to
// populate the AppOptions pointer.  Returns true on success, false otherwise.
static bool LoadAppOptionsFromJsonConfigFile(const char* path,
                                             AppOptions* options) {
  bool loaded_options = false;
  std::ifstream infile(path, std::ifstream::binary);
  if (infile) {
    infile.seekg(0, infile.end);
    int file_length = infile.tellg();
    // Make sure the file is a sane size:
    if (file_length > kMaxBuffersize) return false;
    infile.seekg(0, infile.beg);

    // Make sure our seeks/tells succeeded
    if (file_length == -1) return false;
    if (infile.fail()) return false;

    char* buffer = new char[file_length + 1];
    infile.read(buffer, file_length);
    if (infile.fail()) return false;
    // Make sure it is null-terminated, as this represents string data.
    buffer[file_length] = '\0';

    loaded_options = AppOptions::LoadFromJsonConfig(buffer, options) != nullptr;

    delete[] buffer;
  }
  return loaded_options;
}

}  // namespace

// Searches internal::g_default_config_path for filenames matching
// kDefaultGoogleServicesNames attempting to load the app options from each
// config file in turn.
AppOptions* AppOptions::LoadDefault(AppOptions* options) {
  static const char* kDefaultGoogleServicesNames[] = {
      "google-services-desktop.json", "google-services.json"};
  bool allocated_options;
  if (options) {
    allocated_options = false;
  } else {
    options = new AppOptions();
    allocated_options = true;
  }
  std::string config_files;
  size_t number_of_config_filenames =
      FIREBASE_ARRAYSIZE(kDefaultGoogleServicesNames);
  for (size_t i = 0; i < number_of_config_filenames; i++) {
    std::string full_path =
        internal::g_default_config_path + kDefaultGoogleServicesNames[i];
    if (LoadAppOptionsFromJsonConfigFile(full_path.c_str(), options)) {
      return options;
    }
    config_files += full_path;
    if (i < number_of_config_filenames - 1) config_files += ", ";
  }
  if (allocated_options) delete options;
  LogError(
      "Unable to load Firebase app options ([%s] are missing or "
      "malformed)",
      config_files.c_str());
  return nullptr;
}

void App::Initialize() { internal_ = new internal::AppInternal(); }

App::~App() {
  app_common::RemoveApp(this);
  delete internal_;
  internal_ = nullptr;
}

// On desktop, if you create an app with no arguments, it will try to
// load any data it can find from the google-services-desktop.json
// file, or the google-services.json file, in that order.
App* App::Create() {
  AppOptions options;
  return AppOptions::LoadDefault(&options) ? Create(options) : nullptr;
}

App* App::Create(const AppOptions& options) {  // NOLINT
  return Create(options, kDefaultAppName);
}

App* App::Create(const AppOptions& options, const char* name) {  // NOLINT
  App* app = GetInstance(name);
  if (app) {
    LogError("App %s already created, options will not be applied.", name);
    return app;
  }
  LogDebug("Creating Firebase App %s for %s", name, kFirebaseVersionString);
  LogDebug("Validating semaphore creation.");
  { firebase::Semaphore sem_test(0); }

  AppOptions options_with_defaults = options;
  if (options_with_defaults.PopulateRequiredWithDefaults()) {
    // Register C++/Unity user-agents
    app_common::RegisterSdkUsage(nullptr);

    app = new App();
    app->name_ = name;
    app->options_ = options_with_defaults;
    std::string unique_name =
        std::string(app->options_.package_name()) + "." + app->name_;
    app = app_common::AddApp(app, &app->init_results_);
    app->internal_->heartbeat_controller_ =
        std::make_shared<heartbeat::HeartbeatController>(
            unique_name, *app_common::FindAppLoggerByName(name),
            app->internal_->date_provider_);
#ifndef SWIG
    // Log a heartbeat after creating an App. In the Unity SDK this will happen
    // at a later time, after additional user agents have been registered.
    app->internal_->heartbeat_controller_->LogHeartbeat();
#endif  // SWIG
  }
  return app;
}

App* App::GetInstance() {  // NOLINT
  return app_common::GetDefaultApp();
}

App* App::GetInstance(const char* name) {  // NOLINT
  return app_common::FindAppByName(name);
}

#ifdef INTERNAL_EXPERIMENTAL
internal::FunctionRegistry* App::function_registry() {
  return &internal_->function_registry;
}
#endif  // INTERNAL_EXPERIMENTAL

void App::RegisterLibrary(const char* library, const char* version,
                          void* /* platform_resource */) {
  app_common::RegisterLibrary(library, version);
}

const char* App::GetUserAgent() { return app_common::GetUserAgent(); }

void App::SetDefaultConfigPath(const char* path) {
  internal::g_default_config_path = path;

#if defined(WIN32)
  const char kSeperator = '\\';
#else
  const char kSeperator = '/';
#endif

  if (!internal::g_default_config_path.empty()) {
    char last_character = internal::g_default_config_path.back();
    if (last_character != '\\' && last_character != '/') {
      internal::g_default_config_path += kSeperator;
    }
  }
}

void App::LogHeartbeat() const {
  if (internal_ != nullptr && internal_->heartbeat_controller_) {
    internal_->heartbeat_controller_->LogHeartbeat();
  }
}

std::shared_ptr<heartbeat::HeartbeatController> App::GetHeartbeatController()
    const {
  if (internal_ != nullptr) {
    return internal_->heartbeat_controller_;
  } else {
    return std::shared_ptr<heartbeat::HeartbeatController>();
  }
}

// Desktop support is for developer workflow only, so automatic data collection
// is always enabled.
void App::SetDataCollectionDefaultEnabled(bool /* enabled */) {}

// Desktop support is for developer workflow only, so automatic data collection
// is always enabled.
bool App::IsDataCollectionDefaultEnabled() const { return true; }

}  // namespace firebase
