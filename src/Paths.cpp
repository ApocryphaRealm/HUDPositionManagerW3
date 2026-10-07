#include "Paths.h"

namespace paths
{
	namespace
	{
		std::filesystem::path ModulePath(HMODULE a_module)
		{
			wchar_t buf[MAX_PATH * 2]{};
			const DWORD n = ::GetModuleFileNameW(a_module, buf, static_cast<DWORD>(std::size(buf)));
			return (n == 0 || n >= std::size(buf)) ? std::filesystem::path() : std::filesystem::path(buf);
		}
	}

	std::filesystem::path ModuleFolder()
	{
		HMODULE self = nullptr;
		::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCWSTR>(&ModuleFolder), &self);
		const auto p = self ? ModulePath(self) : std::filesystem::path();
		return p.empty() ? std::filesystem::path(L".") : p.parent_path();
	}

	std::filesystem::path DataFolder()
	{
		const auto dir = ModuleFolder() / L"HUDPositionManager";
		std::error_code ec;
		std::filesystem::create_directories(dir, ec);
		return dir;
	}

	std::filesystem::path GameRoot()
	{
		// witcher3.exe is <game>\bin\x64_dx12\witcher3.exe
		const auto exe = ModulePath(nullptr);
		return exe.empty() ? ModuleFolder().parent_path().parent_path() : exe.parent_path().parent_path().parent_path();
	}

	std::filesystem::path TranslationsFolder() { return ModuleFolder() / L"AMF" / L"Translations"; }
}
