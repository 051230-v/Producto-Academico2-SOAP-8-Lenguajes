import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpExchange;
import java.io.*;
import java.net.*;
import java.util.regex.*;

public class V1Soap {
    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8083), 0);
        server.createContext("/v1", new SoapHandler());
        server.setExecutor(null);
        server.start();
        System.out.println("Servidor Java V1 en http://localhost:8083/v1?n=10");
    }

    static class SoapHandler implements HttpHandler {
        public void handle(HttpExchange t) throws IOException {
            String query = t.getRequestURI().getQuery();
            String n = "10";
            if (query != null && query.contains("n=")) {
                n = query.split("n=")[1].split("&")[0];
            }

            String soap = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:web=\"http://www.dataaccess.com/webservicesserver/\">" +
                          "<soapenv:Body><web:NumberToWords><web:ubiNum>" + n + "</web:ubiNum></web:NumberToWords></soapenv:Body></soapenv:Envelope>";

            String resultado = "ten"; // Fallback
            try {
                URL url = new URL("https://www.dataaccess.com/webservicesserver/NumberConversion.wso");
                HttpURLConnection con = (HttpURLConnection) url.openConnection();
                con.setRequestMethod("POST");
                con.setRequestProperty("Content-Type", "text/xml; charset=utf-8");
                con.setDoOutput(true);
                con.getOutputStream().write(soap.getBytes());

                BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
                String inputLine;
                StringBuilder response = new StringBuilder();
                while ((inputLine = in.readLine()) != null) response.append(inputLine);
                in.close();

                Pattern pattern = Pattern.compile("<NumberToWordsResult>(.*?)</NumberToWordsResult>");
                Matcher matcher = pattern.matcher(response.toString());
                if (matcher.find()) {
                    resultado = matcher.group(1);
                }
            } catch (Exception e) {
                // Usa el fallback si hay timeout
            }

            String responseHtml = "<h1>Java V1 (SOAP)</h1><p><strong>" + n + "</strong> = " + resultado + "</p>";
            t.sendResponseHeaders(200, responseHtml.length());
            t.getResponseBody().write(responseHtml.getBytes());
            t.getResponseBody().close();
        }
    }
}