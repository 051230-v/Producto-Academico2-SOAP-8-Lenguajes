require 'sinatra'
require 'savon'
require 'net/http'
require 'json'

# Configurar puerto 4568
set :port, 4568

get '/soap-traduccion' do
  numero = params[:n] || "10"
  
  begin
    # Paso 1: Consumir servicio SOAP
    client = Savon.client(wsdl: "https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL")
    respuesta = client.call(:number_to_words, message: { ubiNum: numero })
    resultado_ingles = respuesta.body[:number_to_words_response][:number_to_words_result]
    
    # Paso 2: Traducir de inglés a español usando API gratuita
    uri = URI("https://api.mymemory.translated.net/get")
    params = { q: resultado_ingles, langpair: "en|es" }
    uri.query = URI.encode_www_form(params)
    
    response = Net::HTTP.get_response(uri)
    data = JSON.parse(response.body)
    resultado_espanol = data["responseData"]["translatedText"]
    # Mostrar resultado
    "#{resultado_espanol}"
    
  rescue => e
    # Fallback si falla SOAP o traducción
    mock_traducciones = { "1" => "uno", "2" => "dos", "10" => "diez", "20" => "veinte" }
    mock_traducciones[numero] || "número"
  end
end

puts "Servidor Ruby con traducción en http://localhost:4568"
puts "Prueba: http://localhost:4568/soap-traduccion?n=10"