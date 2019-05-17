#include <iostream>
#include <shlwapi.h>
#include <wininet.h>
#include <string>
#include "HTTPURL.h"
#include<QDebug>
#include <tchar.h>


#pragma comment (lib, "WinInet.lib")




std::string postRequest(std::string post)
{

	try {
		//Retrieve default http user agent
		char httpUseragent[512];
		DWORD szhttpUserAgent = sizeof(httpUseragent);
		//ObtainUserAgentString( 0, httpUseragent, &szhttpUserAgent );

		char m[5];


		HINTERNET internet = InternetOpen(_T(" Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		//HINTERNET internet = InternetOpen(_T(" Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36"), INTERNET_OPEN_TYPE_PROXY, L"SOCKS=127.0.0.1:9050", NULL, 0);
		if (internet == NULL)
		{
			qInfo() << "InternetOpen failed";
		}
		if (internet != NULL)
		{

			HINTERNET connect = InternetConnect(internet, _T("testwebspace12122.000webhostapp.com"), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
			if (connect == NULL)
			{
				qInfo() << "InternetConnect failed";
			}
			if (connect != NULL)
			{

				HINTERNET request = HttpOpenRequest(connect, _T("POST"), _T("/validate.php"), _T("HTTP/1.1"), NULL, NULL,
					INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
					INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
					INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
					INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
					INTERNET_FLAG_NO_AUTH |
					INTERNET_FLAG_NO_CACHE_WRITE |
					INTERNET_FLAG_NO_UI |
					INTERNET_FLAG_PRAGMA_NOCACHE |
					INTERNET_FLAG_RELOAD, NULL);
				if (request == NULL)
				{
					
					QMessageBox msgBoxfailed;
					msgBoxfailed.setText("DarkRat License");
					msgBoxfailed.setInformativeText("Check your Internet we have no connection, or the License server is Offline conntact me!");
					msgBoxfailed.setStandardButtons(QMessageBox::Cancel);
					msgBoxfailed.setDefaultButton(QMessageBox::Cancel);
					int ret = msgBoxfailed.exec();
					
				}
				if (request != NULL)
				{
					int datalen = 0;
					if (post.c_str() != NULL) datalen = post.length();
					LPCWSTR RH = _T("Connection: keep-Alive\r\n");
					HttpAddRequestHeaders(request, RH, _tcslen(RH), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
					LPCWSTR RH1 = _T("Content-Type: application/x-www-form-urlencoded\r\n");
					HttpAddRequestHeaders(request, RH1, _tcslen(RH1), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
					LPCWSTR RH3 = _T("Accept-Language: en-US,en;q=0.8\r\n");
					HttpAddRequestHeaders(request, RH3, _tcslen(RH3), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
					LPCWSTR RH4 = _T("Accept: */*\r\n");
					HttpAddRequestHeaders(request, RH4, _tcslen(RH4), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);
					LPCWSTR RH5 = _T("Accept-Encoding: gzip,deflate,sdch\r\n");
					HttpAddRequestHeaders(request, RH5, _tcslen(RH5), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);

					HttpSendRequestA(request, NULL, 0, (LPVOID)post.c_str(), datalen);
					DWORD rSize;
					char tmp[1024 + 1];
					string szBuffer;

					while (InternetReadFile(request, tmp, 1024, &rSize) && rSize > 0)
					{
						tmp[rSize] = '\0';
						szBuffer += (string)tmp;
					}
					qInfo() << QString::fromStdString(szBuffer);


					DWORD StatusCode = 0;
					DWORD StatusCodeLen = sizeof(StatusCode);
					HttpQueryInfo(request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &StatusCode, &StatusCodeLen, NULL);
					if (StatusCode == 200)
					{

						qInfo() << "Sucess=200";
					}
					else
					{
						qInfo() << "failed=" << StatusCode;
					}
					
					if (szBuffer == "access") {
						return "access";
					}
					else {
						return "false";
					}
					

					InternetCloseHandle(request);
				}
			}
			InternetCloseHandle(connect);
		}
		InternetCloseHandle(internet);
	}
	catch (...) {}

}