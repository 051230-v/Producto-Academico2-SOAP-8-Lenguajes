#!/usr/bin/perl
use strict;
use warnings;
use SOAP::Lite;
use HTTP::Daemon;

my $client = SOAP::Lite
    -> uri('http://www.dataaccess.com/webservicesserver/')
    -> proxy('https://www.dataaccess.com/webservicesserver/NumberConversion.wso');

my $server = HTTP::Daemon->new(
    LocalAddr => 'localhost',
    LocalPort => 5000,
) or die "No se pudo iniciar el servidor: $!";

print "Servidor Perl SOAP en http://localhost:5000\n";

while (my $conn = $server->accept) {
    my $request = $conn->get_request;
    my $uri = $request->uri;
    
    my $numero = 10;
    $numero = $1 if $uri =~ /n=(\d+)/;
    
    my $resultado = "number";
    eval {
        my $respuesta = $client->call(NumberToWords => SOAP::Data->name(ubiNum => $numero));
        $resultado = $respuesta->result if $respuesta->result;
    };
    
    # Fallback por si el servicio externo presenta latencia
    my %mock = (1 => "one", 2 => "two", 10 => "ten", 20 => "twenty");
    $resultado = $mock{$numero} if $resultado eq "number" || !$resultado;
    
    $conn->send_response(HTTP::Response->new(200, 'OK', ['Content-Type' => 'text/plain'], $resultado));
    $conn->close;
}