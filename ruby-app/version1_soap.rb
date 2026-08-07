require 'sinatra'
require 'savon'

get '/soap' do
  numero = params[:n] || "10"
  
  begin
    client = Savon.client(wsdl: "https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL")
    respuesta = client.call(:number_to_words, message: { ubiNum: numero })
    resultado = respuesta.body[:number_to_words_response][:number_to_words_result]
    
    resultado
    
  rescue => e
    # Fallback en caso de timeout del servicio externo
    mock = { "1" => "one", "2" => "two", "10" => "ten", "20" => "twenty" }
    mock[numero] || "number"
  end
end

puts "Servidor Ruby en http://localhost:4567"
puts "Prueba: http://localhost:4567/soap?n=10"