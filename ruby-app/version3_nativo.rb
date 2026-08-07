require 'sinatra'
require 'numbers_and_words'

# Configurar idioma español
I18n.default_locale = :es

set :port, 4569

get '/nativo' do
  numero = params[:n] || "10"
  
  begin
    numero_entero = numero.to_i
    resultado = numero_entero.to_words
    
    resultado
    
  rescue => e
    # Fallback si falla la conversión
    mock = { "1" => "uno", "2" => "dos", "10" => "diez", "20" => "veinte" }
    mock[numero] || "número"
  end
end

puts "Servidor Ruby nativo en http://localhost:4569"
puts "Prueba: http://localhost:4569/nativo?n=10"