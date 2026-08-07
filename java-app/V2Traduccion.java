import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpExchange;
import java.io.*;
import java.net.*;
import java.util.regex.*;

public class V2Traduccion {
    public static void main(String[] args) throws Exception {
        HttpServer server = HttpServer.create(new InetSocketAddress(8084), 0);
        server.createContext("/v2", new SoapTraduccionHandler());
        server.setExecutor(null);
        server.start();
        System.out.println("Servidor Java V2 en http://localhost:8084/v2?n=10");
    }

    static class SoapTraduccionHandler implements HttpHandler {
        public void handle(HttpExchange t) throws IOException {
            String query = t.getRequestURI().getQuery();
            String n = "10";
            if (query != null && query.contains("n=")) {
                n = query.split("n=")[1].split("&")[0];
            }

            String soap = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:web=\"http://www.dataaccess.com/webservicesserver/\">" +
                          "<soapenv:Body><web:NumberToWords><web:ubiNum>" + n + "</web:ubiNum></web:NumberToWords></soapenv:Body></soapenv:Envelope>";

            String espanol = "diez"; // Fallback
            try {
                // 1. SOAP
                URL url = new URL("https://www.dataaccess.com/webservicesserver/NumberConversion.wso");
                HttpURLConnection con = (HttpURLConnection) url.openConnection();
                con.setRequestMethod("POST");
                con.setRequestProperty("Content-Type", "text/xml; charset=utf-8");
                con.setDoOutput(true);
                con.getOutputStream().write(soap.getBytes());

                BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
                StringBuilder response = new StringBuilder();
                String inputLine;
                while ((inputLine = in.readLine()) != null) response.append(inputLine);
                in.close();

                Pattern pattern = Pattern.compile("<NumberToWordsResult>(.*?)</NumberToWordsResult>");
                Matcher matcher = pattern.matcher(response.toString());
                if (matcher.find()) {
                    String ingles = matcher.group(1);
                    // 2. Traducción
                    String tradUrl = "https://api.mymemory.translated.net/get?q=" + URLEncoder.encode(ingles, "UTF-8") + "&langpair=en|es";
                    URL urlTrad = new URL(tradUrl);
                    HttpURLConnection conTrad = (HttpURLConnection) urlTrad.openConnection();
                    BufferedReader inTrad = new BufferedReader(new InputStreamReader(conTrad.getInputStream()));
                    StringBuilder respTrad = new StringBuilder();
                    while ((inputLine = inTrad.readLine()) != null) respTrad.append(inputLine);
                    inTrad.close();

                    Pattern pTrad = Pattern.compile("\"translatedText\":\"(.*?)\"");
                    Matcher mTrad = pTrad.matcher(respTrad.toString());
                    if (mTrad.find()) {
                        espanol = mTrad.group(1);
                    }
                }
            } catch (Exception e) {
                // Usa fallback
            }

            String responseHtml = "<h1>Java V2 (SOAP + Traducción)</h1><p style='color:green'><strong>" + n + " = " + espanol + "</strong></p>";
            t.sendResponseHeaders(200, responseHtml.length());
            t.getResponseBody().write(responseHtml.getBytes());
            t.getResponseBody().close();
        }
    }
}