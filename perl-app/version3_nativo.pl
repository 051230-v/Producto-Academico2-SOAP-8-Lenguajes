#!/usr/bin/perl
use strict;
use warnings;
use HTTP::Daemon;

my %numeros = (
    1 => 'uno', 2 => 'dos', 3 => 'tres', 4 => 'cuatro', 5 => 'cinco',
    6 => 'seis', 7 => 'siete', 8 => 'ocho', 9 => 'nueve', 10 => 'diez',
    11 => 'once', 12 => 'doce', 13 => 'trece', 14 => 'catorce', 15 => 'quince',
    16 => 'dieciséis', 17 => 'diecisiete', 18 => 'dieciocho', 19 => 'diecinueve', 20 => 'veinte'
);

my $server = HTTP::Daemon->new(LocalAddr => 'localhost', LocalPort => 5002) or die "Error: $!";
print "Servidor Perl nativo en http://localhost:5002\n";

while (my $conn = $server->accept) {
    my $request = $conn->get_request;
    my $numero = 10;
    $numero = $1 if $request->uri =~ /n=(\d+)/;
    
    my $resultado = $numeros{$numero} || 'número fuera de rango';
    
    $conn->send_response(HTTP::Response->new(200, 'OK', ['Content-Type' => 'text/plain'], $resultado));
    $conn->close;
}