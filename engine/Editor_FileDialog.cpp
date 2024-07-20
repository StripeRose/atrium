// Filter "Editor"
#include "stdafx.hpp"
#include "Editor_FileDialog.hpp"

#if _WIN32
#include <atomic>
#include <stdexcept>

#include <Windows.h>

namespace RoseGold::Editor
{
	namespace _Win32Dialog
	{
		struct ComInitializer
		{
			ComInitializer()
			{
				HRESULT result = ::CoInitializeEx(nullptr, ::COINIT_APARTMENTTHREADED | ::COINIT_DISABLE_OLE1DDE);
				if (SUCCEEDED(result))
					myNeedsCleanup = true;
				else if (result == RPC_E_CHANGED_MODE)
					myNeedsCleanup = false;
				else
					throw std::runtime_error("Com library failed initialize.");
			}

			~ComInitializer()
			{
				if (myNeedsCleanup)
					::CoUninitialize();
			}

			bool myNeedsCleanup = false;
		};

		ComInitializer ourComInitializer;

		std::wstring ToWide(const std::string_view& aString)
		{
			std::wstring wideString;
			wideString.resize(MultiByteToWideChar(CP_UTF8, 0, aString.data(), -1, nullptr, 0));
			MultiByteToWideChar(CP_UTF8, 0, aString.data(), -1, wideString.data(), static_cast<int>(wideString.size()));
			return wideString;
		}
	}

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	void FileDialog::SetFileNameLabel(const std::string_view& aLabel)
	{
		myFileDialog->SetFileNameLabel(_Win32Dialog::ToWide(aLabel).c_str());
	}

	void FileDialog::SetOKButtonLabel(const std::string_view& aLabel)
	{
		myFileDialog->SetOkButtonLabel(_Win32Dialog::ToWide(aLabel).c_str());
	}

	void FileDialog::SetTitle(const std::string_view& aLabel)
	{
		myFileDialog->SetTitle(_Win32Dialog::ToWide(aLabel).c_str());
	}

	void FileDialog::SetFilter(const std::span<FileDialogFilter>& aFilterList)
	{
		std::vector<std::wstring> wideLabels;
		wideLabels.reserve(aFilterList.size());

		std::vector<COMDLG_FILTERSPEC> filterSpecs;
		filterSpecs.reserve(aFilterList.size());
		for (const auto& it : aFilterList)
		{
			if (it.first.empty() || it.second.empty())
				continue;

			COMDLG_FILTERSPEC& spec = filterSpecs.emplace_back();
			spec.pszName = wideLabels.emplace_back(_Win32Dialog::ToWide(it.first.data())).c_str();
			spec.pszSpec = it.second.c_str();
		}

		myFileDialog->SetFileTypes(static_cast<UINT>(filterSpecs.size()), &filterSpecs.front());
	}

	void FileDialog::SetDefaultPath(const std::filesystem::path& aPath)
	{
		if (aPath.empty())
			return;

		IShellItem* folder;
		HRESULT result = ::SHCreateItemFromParsingName(aPath.c_str(), nullptr, IID_PPV_ARGS(&folder));

		// Valid non results.
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) ||
			result == HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE)) {
			return;
		}

		if (FAILED(result))
			throw std::runtime_error("Failed to create ShellItem for setting the default path.");

		if (FAILED(myFileDialog->SetDefaultFolder(folder)))
			throw std::runtime_error("Failed to set default path.");
	}

	void FileDialog::AddOptions(FILEOPENDIALOGOPTIONS someOptions)
	{
		::FILEOPENDIALOGOPTIONS existingOptions;
		if (FAILED(myFileDialog->GetOptions(&existingOptions)))
			return;

		myFileDialog->SetOptions(existingOptions | someOptions);
	}

	void FileDialog::RemoveOptions(FILEOPENDIALOGOPTIONS someOptions)
	{
		::FILEOPENDIALOGOPTIONS existingOptions;
		if (FAILED(myFileDialog->GetOptions(&existingOptions)))
			return;

		myFileDialog->SetOptions(existingOptions & (~someOptions));
	}

	OpenFileDialog::OpenFileDialog()
	{
		const HRESULT createResult = ::CoCreateInstance(
			::CLSID_FileOpenDialog,
			nullptr,
			CLSCTX_ALL,
			IID_PPV_ARGS(&myFileDialog));

		if (FAILED(createResult))
			throw std::runtime_error("Could not create dialog.");

		AddOptions(::FOS_FORCEFILESYSTEM);
	}

	OpenFileDialog::~OpenFileDialog()
	{
		myFileDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
	}

	std::optional<std::filesystem::path> OpenFileDialog::GetSingle()
	{
		RemoveOptions(::FOS_ALLOWMULTISELECT);

		HRESULT dialogStatus = myFileDialog->Show(nullptr);
		if (SUCCEEDED(dialogStatus))
		{
			ComPtr<IShellItem> resultItem;
			if (FAILED(myFileDialog->GetResult(resultItem.ReleaseAndGetAddressOf())))
				throw std::runtime_error("Could not get shell item from dialog.");

			LPWSTR selectedPath;
			if (FAILED(resultItem->GetDisplayName(::SIGDN_FILESYSPATH, &selectedPath)))
				throw std::runtime_error("Could not get file path from file dialog.");

			return selectedPath;
		}
		else if (dialogStatus == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			return { };
		}
		else
		{
			throw std::runtime_error("Failed to show file dialog.");
		}
	}

	std::vector<std::filesystem::path> OpenFileDialog::GetMultiple()
	{
		AddOptions(::FOS_ALLOWMULTISELECT);

		ComPtr<IFileOpenDialog> openDialog;
		myFileDialog.As<IFileOpenDialog>(&openDialog);

		HRESULT dialogStatus = openDialog->Show(nullptr);
		if (SUCCEEDED(dialogStatus))
		{
			ComPtr<IShellItemArray> resultItems;
			if (FAILED(openDialog->GetResults(resultItems.ReleaseAndGetAddressOf())))
				throw std::runtime_error("Could not get shell items from dialog.");

			DWORD numItems;
			resultItems->GetCount(&numItems);

			std::vector<std::filesystem::path> selectedPaths;
			for (DWORD i = 0; i < numItems; ++i)
			{
				ComPtr<IShellItem> resultItem;
				resultItems->GetItemAt(i, resultItem.ReleaseAndGetAddressOf());
				LPWSTR selectedPath;
				if (FAILED(resultItem->GetDisplayName(::SIGDN_FILESYSPATH, &selectedPath)))
					throw std::runtime_error("Could not get file path from file dialog.");

				selectedPaths.push_back(selectedPath);
			}

			return selectedPaths;
		}
		else if (dialogStatus == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			return { };
		}
		else
		{
			throw std::runtime_error("Failed to show file dialog.");
		}
	}

	SaveFileDialog::SaveFileDialog()
	{
		const HRESULT createResult = ::CoCreateInstance(
			::CLSID_FileSaveDialog,
			nullptr,
			CLSCTX_ALL,
			IID_PPV_ARGS(&myFileDialog));

		if (FAILED(createResult))
			throw std::runtime_error("Could not create dialog.");

		AddOptions(::FOS_FORCEFILESYSTEM);
	}

	SaveFileDialog::~SaveFileDialog()
	{
		myFileDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
	}

	std::optional<std::filesystem::path> SaveFileDialog::GetSingle()
	{
		RemoveOptions(::FOS_ALLOWMULTISELECT);

		HRESULT dialogStatus = myFileDialog->Show(nullptr);
		if (SUCCEEDED(dialogStatus))
		{
			ComPtr<IShellItem> resultItem;
			if (FAILED(myFileDialog->GetResult(resultItem.ReleaseAndGetAddressOf())))
				throw std::runtime_error("Could not get shell item from dialog.");

			LPWSTR selectedPath;
			if (FAILED(resultItem->GetDisplayName(::SIGDN_FILESYSPATH, &selectedPath)))
				throw std::runtime_error("Could not get file path from file dialog.");

			return selectedPath;
		}
		else if (dialogStatus == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			return { };
		}
		else
		{
			throw std::runtime_error("Failed to show file dialog.");
		}
	}

	void SaveFileDialog::SetDefaultName(const std::filesystem::path& aFilename)
	{
		if (!aFilename.empty())
			myFileDialog->SetFileName(aFilename.c_str());
	}

	void SaveFileDialog::SetDefaultExtension(const std::filesystem::path& anExtension)
	{
		// set the first item as the default index, and set the default extension
		if (FAILED(myFileDialog->SetFileTypeIndex(1)))
			throw std::runtime_error("Failed to set the selected file type index.");

		// single file extension for this type (no need to allocate memory)
		if (FAILED(myFileDialog->SetDefaultExtension(anExtension.c_str())))
			throw std::runtime_error("Failed to set default extension.");
	}

	FolderBrowserDialog::FolderBrowserDialog()
	{
		const HRESULT createResult = ::CoCreateInstance(
			::CLSID_FileOpenDialog,
			nullptr,
			CLSCTX_ALL,
			IID_PPV_ARGS(&myFileDialog));

		if (FAILED(createResult))
			throw std::runtime_error("Could not create dialog.");

		AddOptions(::FOS_FORCEFILESYSTEM | ::FOS_PICKFOLDERS);
	}

	FolderBrowserDialog::~FolderBrowserDialog()
	{
		myFileDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
	}

	std::optional<std::filesystem::path> FolderBrowserDialog::GetSingle()
	{
		RemoveOptions(::FOS_ALLOWMULTISELECT);

		HRESULT dialogStatus = myFileDialog->Show(nullptr);
		if (SUCCEEDED(dialogStatus))
		{
			ComPtr<IShellItem> resultItem;
			if (FAILED(myFileDialog->GetResult(resultItem.ReleaseAndGetAddressOf())))
				throw std::runtime_error("Could not get shell item from dialog.");

			LPWSTR selectedPath;
			if (FAILED(resultItem->GetDisplayName(::SIGDN_FILESYSPATH, &selectedPath)))
				throw std::runtime_error("Could not get file path from file dialog.");

			return selectedPath;
		}
		else if (dialogStatus == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			return { };
		}
		else
		{
			throw std::runtime_error("Failed to show file dialog.");
		}
	}

	std::vector<std::filesystem::path> FolderBrowserDialog::GetMultiple()
	{
		AddOptions(::FOS_ALLOWMULTISELECT);

		ComPtr<IFileOpenDialog> openDialog;
		myFileDialog.As<IFileOpenDialog>(&openDialog);

		HRESULT dialogStatus = openDialog->Show(nullptr);
		if (SUCCEEDED(dialogStatus))
		{
			ComPtr<IShellItemArray> resultItems;
			if (FAILED(openDialog->GetResults(resultItems.ReleaseAndGetAddressOf())))
				throw std::runtime_error("Could not get shell items from dialog.");

			DWORD numItems;
			resultItems->GetCount(&numItems);

			std::vector<std::filesystem::path> selectedPaths;
			for (DWORD i = 0; i < numItems; ++i)
			{
				ComPtr<IShellItem> resultItem;
				resultItems->GetItemAt(i, resultItem.ReleaseAndGetAddressOf());
				LPWSTR selectedPath;
				if (FAILED(resultItem->GetDisplayName(::SIGDN_FILESYSPATH, &selectedPath)))
					throw std::runtime_error("Could not get file path from file dialog.");

				selectedPaths.push_back(selectedPath);
			}

			return selectedPaths;
		}
		else if (dialogStatus == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			return { };
		}
		else
		{
			throw std::runtime_error("Failed to show file dialog.");
		}
	}
}
#endif
