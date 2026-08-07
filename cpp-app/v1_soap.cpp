#include <iostream>
#include <string>
#include <windows.h>
#include <winhttp.h>
#include <fstream>

using namespace std;

string consumirSOAP(string numero) {
    string soapData = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
        "  <soap:Body>\n"
        "    <NumberToWords xmlns=\"http://www.dataaccess.com/webservicesserver/\">\n"
        "      <ubiNum>" + numero + "</ubiNum>\n"
        "    </NumberToWords>\n"
        "  </soap:Body>\n"
        "</soap:Envelope>";

    HINTERNET hSession = WinHttpOpen(L"C++ SOAP Client", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
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
            return responseBody.substr(pos + 23, endPos - pos - 23);
        }
    }
    return "ten";
}

int main() {
    string numero = "10";
    string resultado = consumirSOAP(numero);
    
    // Crear archivo HTML
    ofstream htmlFile("resultado_v1.html");
    htmlFile << "<!DOCTYPE html><html><head><title>C++ V1 SOAP</title></head><body>";
    htmlFile << "<h1>C++ V1 (SOAP)</h1>";
    htmlFile << "<p><strong>" << numero << "</strong> = " << resultado << "</p>";
    htmlFile << "</body></html>";
    htmlFile.close();
    
    // Abrir en el navegador
    ShellExecute(NULL, "open", "resultado_v1.html", NULL, NULL, SW_SHOWNORMAL);
    
    cout << "Resultado: " << numero << " = " << resultado << endl;
    cout << "Abriendo navegador..." << endl;
    
    return 0;
}