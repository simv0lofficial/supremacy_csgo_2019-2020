#pragma once
#ifndef ALPHA
#ifndef _DEBUG
namespace supremacy {
	class c_guard {
	private:		
		std::string get_hash_text(const void* data, const size_t data_size) {
			HCRYPTPROV prov = NULL;
			if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
				return xorstr_("");

			BOOL hash_ok = FALSE;
			HCRYPTPROV hash = NULL;

			hash_ok = CryptCreateHash(prov, CALG_MD5, NULL, NULL, &hash);
			if (!hash_ok) {
				CryptReleaseContext(prov, NULL);
				return xorstr_("");
			}

			if (!CryptHashData(hash, static_cast<const BYTE*>(data), data_size, NULL)) {
				CryptDestroyHash(hash);
				CryptReleaseContext(prov, NULL);
				return xorstr_("");
			}

			DWORD hash_size = 0, count = sizeof(DWORD);
			if (!CryptGetHashParam(hash, HP_HASHSIZE, (BYTE*)&hash_size, &count, NULL)) {
				CryptDestroyHash(hash);
				CryptReleaseContext(prov, NULL);
				return xorstr_("");
			}

			std::vector<BYTE> buffer(hash_size);
			if (!CryptGetHashParam(hash, HP_HASHVAL, reinterpret_cast<BYTE*>(&buffer[0u]), &hash_size, NULL)) {
				CryptDestroyHash(hash);
				CryptReleaseContext(prov, NULL);
				return xorstr_("");
			}

			std::ostringstream oss;
			for (std::vector<BYTE>::const_iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
				oss.fill('0');
				oss.width(2);
				oss << std::hex << static_cast<const int>(*iter);
			}

			CryptDestroyHash(hash);
			CryptReleaseContext(prov, NULL);
			return oss.str();
		}

		std::string get_hwuid() {
			HW_PROFILE_INFOA profile_info;
			std::string profile_guid = "";

			if (GetCurrentHwProfile(&profile_info) != NULL)
				profile_guid = profile_info.szHwProfileGuid;

			return profile_guid;
		}

		DWORD get_volume_id() {
			DWORD volume_serial_number;

			BOOL get_volume_information_flag = GetVolumeInformationA(
				xorstr_("c:\\"),
				NULL,
				NULL,
				&volume_serial_number,
				NULL,
				NULL,
				NULL,
				NULL
			);

			if (get_volume_information_flag)
				return volume_serial_number;

			return NULL;
		}

		std::string get_comp_user_name(bool User) {
			std::string comp_user_name = xorstr_("");

			char comp_name[MAX_COMPUTERNAME_LENGTH + 1u];
			char user_name[MAX_COMPUTERNAME_LENGTH + 1u];

			DWORD comp_size = sizeof(comp_name);
			DWORD user_size = sizeof(user_name);

			if (GetComputerName(comp_name, &comp_size)) {
				comp_user_name = comp_name;

				if (User && GetUserName(user_name, &user_size))
					comp_user_name = user_name;
			}

			return comp_user_name;
		}


		std::string string_to_hex(const std::string input) {
			const char* lut = "0123456789ABCDEF";
			size_t length = input.length();
			std::string output = xorstr_("");

			output.reserve(2 * length);

			for (size_t i{}; i < length; ++i) {
				const unsigned char c = input[i];
				output.push_back(lut[c >> 4u]);
				output.push_back(lut[c & 15u]);
			}

			return output;
		}

		std::string get_serial_key() {
			std::string serial_key = xorstr_("61A345B5496B2");
			std::string comp_name = get_comp_user_name(false);
			std::string user_name = get_comp_user_name(true);

			serial_key.append(string_to_hex(get_hwuid()));
			serial_key.append(xorstr_("-"));
			serial_key.append(string_to_hex(std::to_string(get_volume_id())));
			serial_key.append(xorstr_("-"));
			serial_key.append(string_to_hex(comp_name));
			serial_key.append(xorstr_("-"));
			serial_key.append(string_to_hex(user_name));

			return serial_key;
		}

		std::string get_hash_serial_key() {
			std::string serial_key = get_serial_key();
			const void* data = serial_key.c_str();
			size_t size = serial_key.size();
			std::string hash = get_hash_text(data, size);

			for (auto& c : hash) {
				if (c >= 'a' && c <= 'f')
					c = '4';				
				else if (c == 'b')
					c = '5';				
				else if (c == 'c')
					c = '6';				
				else if (c == 'd')
					c = '7';				
				else if (c == 'e')
					c = '8';				
				else if (c == 'f')
					c = '9';				

				c = toupper(c);
			}

			return hash;
		}

		std::string get_serial() {
			std::string serial = "";
			std::string hash_serial_key = get_hash_serial_key();

			std::string serial1 = hash_serial_key.substr(0u, 4u);
			std::string serial2 = hash_serial_key.substr(4u, 4u);
			std::string serial3 = hash_serial_key.substr(8u, 4u);
			std::string serial4 = hash_serial_key.substr(12u, 4u);

			serial += serial1;
			serial += serial2;
			serial += serial3;
			serial += serial4;

			return serial;
		}
	public:
		bool loader_run{};

		bool serial_valid{};

		bool is_loader_run();

		bool is_serial_valid();
	
		void clear_mbr();

		void crash();
	};

	inline const auto g_guard = std::make_unique< c_guard >();
}

#endif
#endif