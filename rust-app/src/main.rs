use actix_web::{web, App, HttpResponse, HttpServer, Responder};
use reqwest::Client;
use regex::Regex;

async fn consumir_y_traducir(numero: &str) -> String {
    let soap_data = format!(r#"<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
      <ubiNum>{}</ubiNum>
    </NumberToWords>
  </soap:Body>
</soap:Envelope>"#, numero);

    let client = Client::new();
    
    // 1. SOAP
    let res = client.post("https://www.dataaccess.com/webservicesserver/NumberConversion.wso")
        .header("Content-Type", "text/xml; charset=utf-8")
        .body(soap_data)
        .send().await;

    if let Ok(response) = res {
        let body = response.text().await.unwrap_or_default();
        let re = Regex::new(r"<NumberToWordsResult>(.*?)</NumberToWordsResult>").unwrap();
        if let Some(caps) = re.captures(&body) {
            let ingles = caps[1].to_string();
            
            // 2. Traducción
            let url_trad = format!("https://api.mymemory.translated.net/get?q={}&langpair=en|es", urlencoding::encode(&ingles));
            if let Ok(trad_res) = client.get(&url_trad).send().await {
                if let Ok(trad_body) = trad_res.text().await {
                    let re_trad = Regex::new(r#""translatedText":"(.*?)""#).unwrap();
                    if let Some(caps_trad) = re_trad.captures(&trad_body) {
                        return caps_trad[1].to_string();
                    }
                }
            }
        }
    }
    "diez".to_string() // Fallback
}

async fn v2() -> impl Responder {
    let resultado = consumir_y_traducir("10").await;
    HttpResponse::Ok().content_type("text/html").body(
        format!(r#"<!DOCTYPE html><html><head><title>Rust V2</title></head><body>
        <h1>Rust V2 (SOAP + Traducción)</h1><p style='color:green'><strong>10</strong> = {}</p></body></html>"#, resultado)
    )
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    println!("Servidor Rust V2 en http://localhost:8087/v2");
    HttpServer::new(|| App::new().route("/v2", web::get().to(v2)))
        .bind("127.0.0.1:8087")?.run().await
}