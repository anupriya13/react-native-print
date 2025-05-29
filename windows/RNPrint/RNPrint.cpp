#include "pch.h"

#include "RNPrint.h"
#include "winrt/Microsoft.ReactNative.h"
#include <winrt/Windows.Graphics.Printing.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include "winrt/Windows.Foundation.h"
#include <winrt/Windows.System.h>
#include <winrt/Windows.Web.Http.h>
#include <windows.h>
#include <shellapi.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;

namespace winrt::RNPrint
{

// See https://microsoft.github.io/react-native-windows/docs/native-modules for details on writing native modules

void RNPrint::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

winrt::fire_and_forget RNPrint::Print(
    RNPrintCodegen::RNPrintSpec_RNPrintOptions options,
    ::React::ReactPromise<::React::JSValue> promise) noexcept
{
    try
    {
        if (!options.filePath.has_value()) {
            promise.Reject(L"Only filePath printing is supported without XAML controls.");
            co_return;
        }

        std::string filePathStr = options.filePath.value();
        auto filePathHstring = winrt::to_hstring(filePathStr);

        // Check if the filePath is a URL (http/https)
        if (filePathStr.rfind("http://", 0) == 0 || filePathStr.rfind("https://", 0) == 0) {
            // Download the file to a temporary location first
            m_context.UIDispatcher().Post([options, filePathStr, promise = std::move(promise)]() mutable -> winrt::fire_and_forget {
                using namespace winrt::Windows::Storage;
                using namespace winrt::Windows::Web::Http;
                using namespace winrt::Windows::Foundation;

                try {
                    auto tempFolder = ApplicationData::Current().TemporaryFolder();
                    auto uri = winrt::Windows::Foundation::Uri(winrt::to_hstring(filePathStr));
                    HttpClient httpClient;

                    auto buffer = co_await httpClient.GetBufferAsync(uri);
                    auto path = uri.Path();
                    std::wstring fileName = L"printfile";
                    if (!path.empty()) {
                        std::wstring wpath = path.c_str();
                        size_t pos = wpath.find_last_of(L"/\\");
                        if (pos != std::wstring::npos && pos + 1 < wpath.size()) {
                            fileName = wpath.substr(pos + 1);
                        }
                        else {
                            fileName = wpath;
                        }
                    }
                    auto file = co_await tempFolder.CreateFileAsync(fileName, CreationCollisionOption::GenerateUniqueName);
                    co_await FileIO::WriteBufferAsync(file, buffer);

                    std::wstring nativePath = file.Path().c_str();  // Get full native path
                    ShellExecuteW(NULL, L"print", nativePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    promise.Resolve(options.jobName);
                }
                catch (...) {
                    promise.Reject(L"Exception occurred while downloading or printing file.");
                }
                co_return;
            });
        } else {
            // Local file path
            m_context.UIDispatcher().Post([options, filePathHstring, promise = std::move(promise)]() mutable -> winrt::fire_and_forget {
                using namespace winrt::Windows::Storage;
                using namespace winrt::Windows::Foundation;

                try {
                    StorageFile file = co_await StorageFile::GetFileFromPathAsync(filePathHstring);
                    std::wstring nativePath = file.Path().c_str();  // Get full native path
                    ShellExecuteW(NULL, L"print", nativePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    promise.Resolve(options.jobName);
                }
                catch (...) {
                    promise.Reject(L"Failed to open or print local file.");
                }
                co_return;
            });
        }
    }
    catch (...)
    {
        promise.Reject(L"Unknown error in Print function");
    }
    co_return;
}
} // namespace winrt::RNPrint