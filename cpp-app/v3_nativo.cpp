#include <iostream>
#include <string>
#include <map>

using namespace std;

int main() {
    map<string, string> numeros = {
        {"1", "uno"}, {"2", "dos"}, {"3", "tres"}, {"4", "cuatro"}, {"5", "cinco"},
        {"6", "seis"}, {"7", "siete"}, {"8", "ocho"}, {"9", "nueve"}, {"10", "diez"},
        {"20", "veinte"}
    };
    
    string numero = "10"; // Valor por defecto
    string resultado = numeros.count(numero) ? numeros[numero] : "número";
    
    cout << "Content-Type: text/html\n\n";
    cout << "<!DOCTYPE html><html><head><title>C++ V3</title></head><body>";
    cout << "<h1>C++ V3 (Nativo)</h1>";
    cout << "<p style='color:blue'><strong>" << numero << "</strong> = " << resultado << "</p>";
    cout << "</body></html>";
    
    return 0;
}