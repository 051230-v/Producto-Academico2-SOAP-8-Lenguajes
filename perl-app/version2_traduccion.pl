#!/usr/bin/perl
use strict;
use warnings;
use SOAP::Lite;
use HTTP::Daemon;
use LWP::UserAgent;
use JSON;
use URI::Escape;

my $client = SOAP::Lite->uri('http://www.dataaccess.com/webservicesserver/')->proxy('https://www.dataaccess.com/webservicesserver/NumberConversion.wso');
my $server = HTTP::Daemon->new(LocalAddr => 'localhost', LocalPort => 5001) or die "Error: $!";

print "Servidor Perl con traducción en http://localhost:5001\n";

while (my $conn = $server->accept) {
    my $request = $conn->get_request;
    my $numero = 10;
    $numero = $1 if $request->uri =~ /n=(\d+)/;
    
    my $resultado_espanol = "número";
    eval {
        my $respuesta = $client->call(NumberToWords => SOAP::Data->name(ubiNum => $numero));
        my $resultado_ingles = $respuesta->result;
        
        my $ua = LWP::UserAgent->new;
        my $api_url = "https://api.mymemory.translated.net/get?q=" . uri_escape($resultado_ingles) . "&langpair=en|es";
        my $res = $ua->get($api_url);
        my $json = decode_json($res->decoded_content);
        $resultado_espanol = $json->{responseData}->{translatedText} if $json->{responseData};
    };
    
    my %mock = (1 => "uno", 2 => "dos", 10 => "diez", 20 => "veinte");
    $resultado_espanol = $mock{$numero} if $resultado_espanol eq "número" || !$resultado_espanol;
    
    $conn->send_response(HTTP::Response->new(200, 'OK', ['Content-Type' => 'text/plain'], $resultado_espanol));
    $conn->close;
}