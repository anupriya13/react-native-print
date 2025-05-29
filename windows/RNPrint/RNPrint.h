#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/NativeRNPrintDataTypes.g.h")
  #include "codegen/NativeRNPrintDataTypes.g.h"
#endif
#include "codegen/NativeRNPrintSpec.g.h"

#include "NativeModules.h"

#include <functional>
#include <sstream>
#include <mutex>
#include <string_view>

#include "JSValue.h"

using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;

namespace winrt::RNPrint
{

REACT_MODULE(RNPrint)
struct RNPrint
{
  using ModuleSpec = RNPrintCodegen::RNPrintSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(Print, L"print");
  winrt::fire_and_forget Print(RNPrintCodegen::RNPrintSpec_RNPrintOptions options, ::React::ReactPromise<::React::JSValue> promise) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::RNPrint