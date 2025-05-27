
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

// #include "NativeRNPrintDataTypes.g.h" before this file to use the generated type definition
#include <NativeModules.h>
#include <tuple>

namespace RNPrintCodegen {

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(RNPrintSpec_RNPrintOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"html", &RNPrintSpec_RNPrintOptions::html},
        {L"filePath", &RNPrintSpec_RNPrintOptions::filePath},
        {L"isLandscape", &RNPrintSpec_RNPrintOptions::isLandscape},
        {L"jobName", &RNPrintSpec_RNPrintOptions::jobName},
    };
    return fieldMap;
}

struct RNPrintSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(RNPrintSpec_RNPrintOptions, Promise<::React::JSValue>) noexcept>{0, L"print"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, RNPrintSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "print",
          "    REACT_METHOD(print) void print(RNPrintSpec_RNPrintOptions && options, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(print) static void print(RNPrintSpec_RNPrintOptions && options, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace RNPrintCodegen
