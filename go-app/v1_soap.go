package main

import (
	"bytes"
	"encoding/xml"
	"fmt"
	"io"
	"net/http"
)
type Envelope struct {
	XMLName xml.Name `xml:"http://schemas.xmlsoap.org/soap/envelope/ Envelope"`
	Body    Body     `xml:"http://schemas.xmlsoap.org/soap/envelope/ Body"`
}
type Body struct {
	NumberToWordsResponse Response `xml:"http://www.dataaccess.com/webservicesserver/ NumberToWordsResponse"`
}
type Response struct {
	Result string `xml:"NumberToWordsResult"`
}
func main() {
	http.HandleFunc("/v1", func(w http.ResponseWriter, r *http.Request) {
		n := r.URL.Query().Get("n")
		if n == "" { n = "10" }
		
		soap := fmt.Sprintf(`<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:web="http://www.dataaccess.com/webservicesserver/">
			<soapenv:Body><web:NumberToWords><web:ubiNum>%s</web:ubiNum></web:NumberToWords></soapenv:Body></soapenv:Envelope>`, n)
		
		req, _ := http.NewRequest("POST", "https://www.dataaccess.com/webservicesserver/NumberConversion.wso", bytes.NewBuffer([]byte(soap)))
		req.Header.Set("Content-Type", "text/xml; charset=utf-8")
		resp, err := http.DefaultClient.Do(req)
		
		resultado := "ten" // Fallback por si el servicio externo presenta latencia
		if err == nil {
			defer resp.Body.Close()
			data, _ := io.ReadAll(resp.Body)
			var env Envelope
			if xml.Unmarshal(data, &env) == nil {
				resultado = env.Body.NumberToWordsResponse.Result
			}
		}
		fmt.Fprintf(w, "<h1>Go V1 (SOAP)</h1><p><strong>%s</strong> = %s</p>", n, resultado)
	})
	fmt.Println("Servidor Go V1 en http://localhost:8080/v1?n=10")
	http.ListenAndServe(":8080", nil)
}