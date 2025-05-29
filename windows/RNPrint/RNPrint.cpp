#include "pch.h"

#include "RNPrint.h"
#include "winrt/Microsoft.ReactNative.h"
#include <winrt/Windows.Graphics.Printing.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include "winrt/Windows.Foundation.h"
#include <winrt/Windows.System.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;

namespace winrt::RNPrint
{

// See https://microsoft.github.io/react-native-windows/docs/native-modules for details on writing native modules

void RNPrint::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void RNPrint::Print(RNPrintCodegen::RNPrintSpec_RNPrintOptions&& options, ::React::ReactPromise<::React::JSValue>&& promise) noexcept
{
    try
    {
        if (!options.filePath.has_value()) {
            promise.Reject(L"Only filePath printing is supported without XAML controls.");
            return;
        }

        std::string filePathStr = options.filePath.value();
        auto filePathHstring = winrt::to_hstring(filePathStr);

        // Check if the filePath is a URL (http/https)
        if (filePathStr.rfind("http://", 0) == 0 || filePathStr.rfind("https://", 0) == 0) {
            // Download the file to a temporary location first
            m_context.UIDispatcher().Post([filePathStr, promise = std::move(promise)]() mutable {
                using namespace winrt::Windows::Storage;
                using namespace winrt::Windows::Web::Http;
                using namespace winrt::Windows::Foundation;

                try {
                    auto tempFolder = ApplicationData::Current().TemporaryFolder();
                    auto uri = winrt::Windows::Foundation::Uri(winrt::to_hstring(filePathStr));
                    HttpClient httpClient;

                    httpClient.GetBufferAsync(uri).Completed(
                        [tempFolder, uri, promise = std::move(promise)](
                            IAsyncOperationWithProgress<winrt::Windows::Storage::Streams::IBuffer, winrt::Windows::Web::Http::HttpProgress> op,
                            AsyncStatus status) mutable
                        {
                            if (status != AsyncStatus::Completed) {
                                promise.Reject(L"Failed to download file for printing.");
                                return;
                            }
                            auto buffer = op.GetResults();
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
                            tempFolder.CreateFileAsync(fileName, CreationCollisionOption::GenerateUniqueName).Completed(
                                [buffer, promise = std::move(promise)](IAsyncOperation<StorageFile> fileOp, AsyncStatus fileStatus) mutable {
                                    if (fileStatus != AsyncStatus::Completed) {
                                        promise.Reject(L"Failed to create temp file for printing.");
                                        return;
                                    }
                                    StorageFile file = fileOp.GetResults();
                                    FileIO::WriteBufferAsync(file, buffer).Completed(
                                        [file, promise = std::move(promise)](IAsyncAction /*writeOp*/, AsyncStatus writeStatus) mutable {
                                            if (writeStatus != AsyncStatus::Completed) {
                                                promise.Reject(L"Failed to write to temp file for printing.");
                                                return;
                                            }
                                            winrt::Windows::System::LauncherOptions options;
                                            options.DisplayApplicationPicker(false);
                                            options.PreferredApplicationDisplayName(L"Print");
                                            options.PreferredApplicationPackageFamilyName(L"");

                                            winrt::Windows::System::Launcher::LaunchFileAsync(file, options).Completed(
                                                [promise = std::move(promise)](IAsyncOperation<bool> op, AsyncStatus status) mutable {
                                                    if (status == AsyncStatus::Completed && op.GetResults()) {
                                                        promise.Resolve(::React::JSValue(true));
                                                    }
                                                    else {
                                                        promise.Reject(L"Failed to launch print handler for file.");
                                                    }
                                                }
                                            );
                                        }
                                    );
                                }
                            );
                        }
                    );
                }
                catch (...) {
                    promise.Reject(L"Exception occurred while downloading file for printing.");
                }
                });
        } else {
            // Local file path
            m_context.UIDispatcher().Post([filePathHstring, promise = std::move(promise)]() mutable {
                using namespace winrt::Windows::Storage;
                using namespace winrt::Windows::Foundation;

                StorageFile::GetFileFromPathAsync(filePathHstring).Completed(
                    [promise = std::move(promise)](IAsyncOperation<StorageFile> op, AsyncStatus status) mutable {
                        if (status != AsyncStatus::Completed) {
                            promise.Reject(L"Failed to open file for printing.");
                            return;
                        }
                        StorageFile file = op.GetResults();

                        winrt::Windows::System::LauncherOptions options;
                        options.DisplayApplicationPicker(false);
                        options.PreferredApplicationDisplayName(L"Print");
                        options.PreferredApplicationPackageFamilyName(L"");

                        winrt::Windows::System::Launcher::LaunchFileAsync(file, options).Completed(
                            [promise = std::move(promise)](IAsyncOperation<bool> op, AsyncStatus status) mutable {
                                if (status == AsyncStatus::Completed && op.GetResults()) {
                                    promise.Resolve(::React::JSValue(true));
                                } else {
                                    promise.Reject(L"Failed to launch print handler for file.");
                                }
                            }
                        );
                    }
                );
            });
        }
    }
    catch (...)
    {
        promise.Reject(L"Unknown error in Print function");
    }
}
} // namespace winrt::RNPrint