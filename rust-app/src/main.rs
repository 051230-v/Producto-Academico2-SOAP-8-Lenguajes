use actix_web::{web, App, HttpResponse, HttpServer, Responder};
use std::collections::HashMap;

fn numero_a_letras(numero: &str) -> String {
    let mut numeros = HashMap::new();
    numeros.insert("1", "uno");
    numeros.insert("2", "dos");
    numeros.insert("10", "diez");
    numeros.insert("20", "veinte");
    
    numeros.get(numero).unwrap_or(&"número").to_string()
}

async fn v3() -> impl Responder {
    let resultado = numero_a_letras("10");
    HttpResponse::Ok().content_type("text/html").body(
        format!(r#"<!DOCTYPE html><html><head><title>Rust V3</title></head><body>
        <h1>Rust V3 (Nativo)</h1><p style='color:blue'><strong>10</strong> = {}</p></body></html>"#, resultado)
    )
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    println!("Servidor Rust V3 en http://localhost:8088/v3");
    HttpServer::new(|| {
        App::new().route("/v3", web::get().to(v3))
    })
    .bind("127.0.0.1:8088")?
    .run()
    .await
}