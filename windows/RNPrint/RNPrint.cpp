#include "pch.h"

#include "RNPrint.h"

namespace winrt::RNPrint
{

// See https://microsoft.github.io/react-native-windows/docs/native-modules for details on writing native modules

void RNPrint::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void RNPrint::Print(RNPrintCodegen::RNPrintSpec_RNPrintOptions&& options, ::React::ReactPromise<::React::JSValue>&& promise) noexcept {
}

} // namespace winrt::RNPrint