using System.Net.Http;
using System.Text;
using System.Xml.Linq;

var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

string wsdlUrl = "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";
var httpClient = new HttpClient();

// VERSIÓN 1: SOAP Básico
app.MapGet("/v1", async (string n = "10") => {
    try {
        string soapEnvelope = $@"<soapenv:Envelope xmlns:soapenv=""http://schemas.xmlsoap.org/soap/envelope/"" xmlns:web=""http://www.dataaccess.com/webservicesserver/"">
           <soapenv:Body><web:NumberToWords><web:ubiNum>{n}</web:ubiNum></web:NumberToWords></soapenv:Body></soapenv:Envelope>";
        
        var content = new StringContent(soapEnvelope, Encoding.UTF8, "text/xml");
        var response = await httpClient.PostAsync(wsdlUrl, content);
        var xml = XDocument.Parse(await response.Content.ReadAsStringAsync());
        var resultado = xml.Descendants(XName.Get("NumberToWordsResult", "http://www.dataaccess.com/webservicesserver/")).FirstOrDefault()?.Value;
        return Results.Content($"<h1>.NET V1 (SOAP)</h1><p><strong>{n}</strong> = {resultado ?? "ten (fallback)"}</p>", "text/html");
    } catch { return Results.Content("<h1>.NET V1</h1><p>10 = ten (Modo respaldo)</p>", "text/html"); }
});

// VERSIÓN 2: SOAP + Traducción
app.MapGet("/v2", async (string n = "10") => {
    try {
        string soapEnvelope = $@"<soapenv:Envelope xmlns:soapenv=""http://schemas.xmlsoap.org/soap/envelope/"" xmlns:web=""http://www.dataaccess.com/webservicesserver/"">
           <soapenv:Body><web:NumberToWords><web:ubiNum>{n}</web:ubiNum></web:NumberToWords></soapenv:Body></soapenv:Envelope>";
        var response = await httpClient.PostAsync(wsdlUrl, new StringContent(soapEnvelope, Encoding.UTF8, "text/xml"));
        var xml = XDocument.Parse(await response.Content.ReadAsStringAsync());
        string ingles = xml.Descendants(XName.Get("NumberToWordsResult", "http://www.dataaccess.com/webservicesserver/")).FirstOrDefault()?.Value ?? "ten";
        
        var tradResp = await httpClient.GetStringAsync($"https://api.mymemory.translated.net/get?q={Uri.EscapeDataString(ingles)}&langpair=en|es");
        var tradJson = System.Text.Json.JsonDocument.Parse(tradResp);
        string espanol = tradJson.RootElement.GetProperty("responseData").GetProperty("translatedText").GetString();
        
        return Results.Content($"<h1>.NET V2 (SOAP + Traducción)</h1><p>Inglés: {ingles}</p><p style='color:green'><strong>Español: {espanol}</strong></p>", "text/html");
    } catch { return Results.Content("<h1>.NET V2</h1><p style='color:green'><strong>10 = diez</strong> (Modo respaldo)</p>", "text/html"); }
});

// VERSIÓN 3: Nativa
app.MapGet("/v3", (string n = "10") => {
    var numeros = new Dictionary<string, string> { {"1","uno"}, {"2","dos"}, {"10","diez"}, {"20","veinte"} };
    string resultado = numeros.ContainsKey(n) ? numeros[n] : "número";
    return Results.Content($"<h1>.NET V3 (Nativo)</h1><p style='color:blue'><strong>{n} = {resultado}</strong></p>", "text/html");
});

app.Run("http://localhost:5000");