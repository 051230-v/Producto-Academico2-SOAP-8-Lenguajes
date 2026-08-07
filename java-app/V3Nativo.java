import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpExchange;
import java.io.*;
import java.net.*;
import java.util.*;

public class V3Nativo {
    public static void main(String[] args) throws Exception {
        Map<String, String> numeros = new HashMap<>();
        numeros.put("1", "uno"); numeros.put("2", "dos"); numeros.put("10", "diez"); numeros.put("20", "veinte");

        HttpServer server = HttpServer.create(new InetSocketAddress(8085), 0);
        server.createContext("/v3", new NativoHandler(numeros));
        server.setExecutor(null);
        server.start();
        System.out.println("Servidor Java V3 en http://localhost:8085/v3?n=10");
    }

    static class NativoHandler implements HttpHandler {
        private Map<String, String> map;
        public NativoHandler(Map<String, String> map) { this.map = map; }

        public void handle(HttpExchange t) throws IOException {
            String query = t.getRequestURI().getQuery();
            String n = "10";
            if (query != null && query.contains("n=")) {
                n = query.split("n=")[1].split("&")[0];
            }
            String resultado = map.getOrDefault(n, "número");
            String responseHtml = "<h1>Java V3 (Nativo)</h1><p style='color:blue'><strong>" + n + " = " + resultado + "</strong></p>";
            
            t.sendResponseHeaders(200, responseHtml.length());
            t.getResponseBody().write(responseHtml.getBytes());
            t.getResponseBody().close();
        }
    }
}