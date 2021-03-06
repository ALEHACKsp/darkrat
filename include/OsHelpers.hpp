
#include <string>
#include <sstream>
#include <limits.h>
#include <intrin.h>
typedef unsigned __int32  uint32_t;



class OsHelpers {
	uint32_t regs[4];

public:

	static std::string getCpuName()	{
		try
		{
			// Get extended ids.
			int CPUInfo[4] = { -1 };
			__cpuid(CPUInfo, 0x80000000);
			unsigned int nExIds = CPUInfo[0];

			// Get the information associated with each extended ID.
			char CPUBrandString[0x40] = { 0 };
			for (unsigned int i = 0x80000000; i <= nExIds; ++i) {
				__cpuid(CPUInfo, i);

				// Interpret CPU brand string and cache information.
				if (i == 0x80000002)
				{
					memcpy(CPUBrandString,
						CPUInfo,
						sizeof(CPUInfo));
				}
				else if (i == 0x80000003)
				{
					memcpy(CPUBrandString + 16,
						CPUInfo,
						sizeof(CPUInfo));
				}
				else if (i == 0x80000004)
				{
					memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
				}
			}

			return CPUBrandString;
		}
		catch (int)
		{
			return "na";
		}
	}

	static std::string getGpuName() {
		try
		{
			return "todo";
		}
		catch (int)
		{
			return "na";
		}
	}


	static std::string getRam() {
		try
		{
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex); // I misunderstand that

			GlobalMemoryStatusEx(&statex);
			return std::to_string((float)statex.ullTotalPhys / (1024 * 1024 * 1024));
		}
		catch (int)
		{
			return "na";
		}
	}

	static std::string checkPEIsAdmin() {
		try
		{
		char* str = (char*)malloc(200);
		bool fRet = false;
		HANDLE hToken = NULL;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
			TOKEN_ELEVATION Elevation;
			DWORD cbSize = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
				fRet = Elevation.TokenIsElevated;
			}
		}
		if (hToken) {
			CloseHandle(hToken);
		}

		if (fRet)
			strcpy(str, "true");
		else
			strcpy(str, "false");

		return str;
		}
		catch (int)
		{
			return "false";
		}
	}

	//https://www.codeproject.com/Tips/107866/32-Bit-or-64-bit-OS
	static std::string PrcessorArchitecture() {
		try
		{
			BOOL bIs64Bit = FALSE;
			#if defined(_WIN64)
				bIs64Bit = TRUE;  
			#elif defined(_WIN32)
				typedef BOOL(WINAPI * LPFNISWOW64PROCESS) (HANDLE, PBOOL);
				LPFNISWOW64PROCESS pfnIsWow64Process = (LPFNISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
				if (pfnIsWow64Process)
					pfnIsWow64Process(GetCurrentProcess(), &bIs64Bit);
			#endif
			if (bIs64Bit) {
				return "x64";
			}
			else {
				return "x86";
			}
		}
		catch (int)
		{
			return "na";
		}
	}
};
