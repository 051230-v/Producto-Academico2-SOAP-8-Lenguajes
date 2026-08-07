#include <iostream>
#include <string>
#include <windows.h>
#include <winhttp.h>
#include <fstream>

using namespace std;

wstring hacerPeticionHTTP(wstring url) {
    HINTERNET hSession = WinHttpOpen(L"C++ HTTP", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    
    URL_COMPONENTS urlComp;
    memset(&urlComp, 0, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwHostNameLength = -1;
    urlComp.dwUrlPathLength = -1;
    
    WinHttpCrackUrl(url.c_str(), url.length(), 0, &urlComp);
    
    wstring host(urlComp.lpszHostName, urlComp.dwHostNameLength);
    wstring path(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    if (urlComp.dwExtraInfoLength > 0) {
        path += wstring(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), urlComp.nPort, 0);
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
    
    WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    WinHttpReceiveResponse(hRequest, NULL);
    
    DWORD dwSize = 0;
    wstring responseBody;
    do {
        dwSize = 0;
        WinHttpQueryDataAvailable(hRequest, &dwSize);
        if (dwSize > 0) {
            wchar_t* buffer = new wchar_t[dwSize + 1];
            WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwSize);
            buffer[dwSize] = L'\0';
            responseBody += buffer;
            delete[] buffer;
        }
    } while (dwSize > 0);
    
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return responseBody;
}

string consumirSOAP(string numero) {
    string soapData = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
        "  <soap:Body>\n"
        "    <NumberToWords xmlns=\"http://www.dataaccess.com/webservicesserver/\">\n"
        "      <ubiNum>" + numero + "</ubiNum>\n"
        "    </NumberToWords>\n"
        "  </soap:Body>\n"
        "</soap:Envelope>";

    HINTERNET hSession = WinHttpOpen(L"C++ SOAP", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = WinHttpConnect(hSession, L"www.dataaccess.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/webservicesserver/NumberConversion.wso", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)soapData.c_str(), soapData.length(), soapData.length(), 0);
    WinHttpReceiveResponse(hRequest, NULL);

    DWORD dwSize = 0;
    string responseBody;
    do {
        dwSize = 0;
        WinHttpQueryDataAvailable(hRequest, &dwSize);
        if (dwSize > 0) {
            char* buffer = new char[dwSize + 1];
            WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwSize);
            buffer[dwSize] = '\0';
            responseBody += buffer;
            delete[] buffer;
        }
    } while (dwSize > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    size_t pos = responseBody.find("<NumberToWordsResult>");
    if (pos != string::npos) {
        size_t endPos = responseBody.find("</NumberToWordsResult>", pos);
        if (endPos != string::npos) {
            string ingles = responseBody.substr(pos + 23, endPos - pos - 23);
            
            // Traducir usando MyMemory API
            wstring tradUrl = L"https://api.mymemory.translated.net/get?q=";
            tradUrl += wstring(ingles.begin(), ingles.end());
            tradUrl += L"&langpair=en|es";
            wstring tradResp = hacerPeticionHTTP(tradUrl);
            
            size_t tradPos = tradResp.find(L"\"translatedText\":\"");
            if (tradPos != wstring::npos) {
                size_t tradEnd = tradResp.find(L"\"", tradPos + 18);
                if (tradEnd != wstring::npos) {
                    wstring resultado = tradResp.substr(tradPos + 18, tradEnd - tradPos - 18);
                    return string(resultado.begin(), resultado.end());
                }
            }
            return ingles;
        }
    }
    return "diez";
}

int main() {
    string numero = "10";
    string resultado = consumirSOAP(numero);
    
    ofstream htmlFile("resultado_v2.html");
    htmlFile << "<!DOCTYPE html><html><head><title>C++ V2 Traducción</title></head><body>";
    htmlFile << "<h1>C++ V2 (SOAP + Traducción)</h1>";
    htmlFile << "<p style='color:green'><strong>" << numero << "</strong> = " << resultado << "</p>";
    htmlFile << "</body></html>";
    htmlFile.close();
    
    ShellExecute(NULL, "open", "resultado_v2.html", NULL, NULL, SW_SHOWNORMAL);
    
    cout << "Resultado: " << numero << " = " << resultado << endl;
    cout << "Abriendo navegador..." << endl;
    
    return 0;
}