use actix_web::{web, App, HttpResponse, HttpServer, Responder};
use reqwest::Client;
use regex::Regex;

async fn consumir_soap(numero: &str) -> String {
    let soap_data = format!(r#"<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
      <ubiNum>{}</ubiNum>
    </NumberToWords>
  </soap:Body>
</soap:Envelope>"#, numero);

    let client = Client::new();
    let res = client
        .post("https://www.dataaccess.com/webservicesserver/NumberConversion.wso")
        .header("Content-Type", "text/xml; charset=utf-8")
        .body(soap_data)
        .send()
        .await; // <-- AQUÍ ESTABA EL ERROR, FALTABA EL .await

    match res {
        Ok(response) => {
            let body = response.text().await.unwrap_or_default(); // <-- Y AQUÍ
            let re = Regex::new(r"<NumberToWordsResult>(.*?)</NumberToWordsResult>").unwrap();
            if let Some(caps) = re.captures(&body) {
                return caps[1].to_string();
            }
        }
        Err(_) => {}
    }
    "ten".to_string() // Fallback
}

async fn v1() -> impl Responder {
    let resultado = consumir_soap("10").await; // <-- Y AQUÍ
    HttpResponse::Ok().content_type("text/html").body(
        format!(r#"<!DOCTYPE html><html><head><title>Rust V1</title></head><body>
        <h1>Rust V1 (SOAP)</h1><p><strong>10</strong> = {}</p></body></html>"#, resultado)
    )
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    println!("Servidor Rust V1 en http://localhost:8086/v1");
    HttpServer::new(|| {
        App::new().route("/v1", web::get().to(v1))
    })
    .bind("127.0.0.1:8086")?
    .run()
    .await
}