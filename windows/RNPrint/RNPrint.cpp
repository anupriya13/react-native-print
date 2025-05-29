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

        if (filePathStr.rfind("http://", 0) == 0 || filePathStr.rfind("https://", 0) == 0)
        {
            auto context = m_context; // Capture context safely
            m_context.UIDispatcher().Post([context, filePathStr, jobName = options.jobName, promise]() mutable -> winrt::fire_and_forget {
                using namespace winrt::Windows::Storage;
                using namespace winrt::Windows::Web::Http;

                try {
                    auto tempFolder = ApplicationData::Current().TemporaryFolder();
                    winrt::Windows::Foundation::Uri uri{ winrt::to_hstring(filePathStr) };
                    HttpClient httpClient;

                    auto buffer = co_await httpClient.GetBufferAsync(uri);
                    std::wstring fileName = L"printfile.pdf";

                    auto path = uri.Path();
                    if (!path.empty()) {
                        std::wstring wpath = path.c_str();
                        size_t pos = wpath.find_last_of(L"/\\");
                        if (pos != std::wstring::npos && pos + 1 < wpath.length()) {
                            fileName = wpath.substr(pos + 1);
                        }
                    }

                    auto file = co_await tempFolder.CreateFileAsync(fileName, CreationCollisionOption::GenerateUniqueName);
                    co_await FileIO::WriteBufferAsync(file, buffer);

                    std::wstring nativePath = file.Path().c_str();
                    auto result = ShellExecuteW(nullptr, L"print", nativePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

                    if ((INT_PTR)result <= 32) {
                        promise.Reject(L"Failed to print downloaded file.");
                    } else {
                        promise.Resolve(jobName);
                    }
                }
                catch (...) {
                    promise.Reject(L"Exception occurred while downloading or printing file.");
                }

                co_return;
            });
        }
        else
        {
            auto context = m_context; // Capture safely
            m_context.UIDispatcher().Post([context, filePathHstring, jobName = options.jobName, promise]() mutable -> winrt::fire_and_forget {
                using namespace winrt::Windows::Storage;

                try {
                    StorageFile file = co_await StorageFile::GetFileFromPathAsync(filePathHstring);
                    std::wstring nativePath = file.Path().c_str();
                    auto result = ShellExecuteW(nullptr, L"print", nativePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

                    if ((INT_PTR)result <= 32) {
                        promise.Reject(L"Failed to print local file.");
                    } else {
                        promise.Resolve(jobName);
                    }
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
        promise.Reject(L"Unknown error in Print function.");
    }

    co_return;
}
} // namespace winrt::RNPrint