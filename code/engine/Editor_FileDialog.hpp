// Filter "Editor"
#pragma once

#include <filesystem>
#include <span>
#include <string_view>
#include <vector>

#if _WIN32
#include <ShObjIdl.h>
#include <wrl.h>
#endif

namespace RoseGold::Editor
{
	using FileDialogFilter = std::pair<std::string_view, std::filesystem::path>;

	class FileDialog
	{
	public:
		void SetDefaultPath(const std::filesystem::path& aPath);
		void SetFileNameLabel(const std::string_view& aLabel);
		void SetFilter(const std::span<FileDialogFilter>& aFilterList);
		void SetOKButtonLabel(const std::string_view& aLabel);
		void SetTitle(const std::string_view& aLabel);

	protected:
#if _WIN32
		void AddOptions(FILEOPENDIALOGOPTIONS someOptions);
		void RemoveOptions(FILEOPENDIALOGOPTIONS someOptions);

		Microsoft::WRL::ComPtr<::IFileDialog> myFileDialog;
#endif
	};

	class OpenFileDialog : public FileDialog
	{
	public:
		OpenFileDialog();
		~OpenFileDialog();

		std::optional<std::filesystem::path> GetSingle();
		std::vector<std::filesystem::path> GetMultiple();
	};

	class SaveFileDialog : public FileDialog
	{
	public:
		SaveFileDialog();
		~SaveFileDialog();

		std::optional<std::filesystem::path> GetSingle();

		void SetDefaultName(const std::filesystem::path& aFilename);
		void SetDefaultExtension(const std::filesystem::path& anExtension);
	};

	class FolderBrowserDialog : public FileDialog
	{
	public:
		FolderBrowserDialog();
		~FolderBrowserDialog();

		std::optional<std::filesystem::path> GetSingle();
		std::vector<std::filesystem::path> GetMultiple();
	};
}
