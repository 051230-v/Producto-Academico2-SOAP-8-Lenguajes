package main

import (
	"bytes"
	"encoding/json"
	"encoding/xml"
	"fmt"
	"io"
	"net/http"
	"net/url"
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
	http.HandleFunc("/v2", func(w http.ResponseWriter, r *http.Request) {
		n := r.URL.Query().Get("n")
		if n == "" {
			n = "10"
		}

		soap := fmt.Sprintf(`<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:web="http://www.dataaccess.com/webservicesserver/">
			<soapenv:Body><web:NumberToWords><web:ubiNum>%s</web:ubiNum></web:NumberToWords></soapenv:Body></soapenv:Envelope>`, n)
		
		req, _ := http.NewRequest("POST", "https://www.dataaccess.com/webservicesserver/NumberConversion.wso", bytes.NewBuffer([]byte(soap)))
		req.Header.Set("Content-Type", "text/xml; charset=utf-8")

		espanol := "diez" // Fallback
		resp, err := http.DefaultClient.Do(req)
		if err == nil {
			defer resp.Body.Close()
			data, _ := io.ReadAll(resp.Body)
			var env Envelope
			if xml.Unmarshal(data, &env) == nil {
				ingles := env.Body.NumberToWordsResponse.Result
				tradURL := fmt.Sprintf("https://api.mymemory.translated.net/get?q=%s&langpair=en|es", url.QueryEscape(ingles))
				tResp, _ := http.Get(tradURL)
				defer tResp.Body.Close()
				tData, _ := io.ReadAll(tResp.Body)
				var tJson map[string]interface{}
				json.Unmarshal(tData, &tJson)
				if respData, ok := tJson["responseData"].(map[string]interface{}); ok {
					espanol = respData["translatedText"].(string)
				}
			}
		}
		fmt.Fprintf(w, "<h1>Go V2 (SOAP + Traducción)</h1><p style='color:green'><strong>%s = %s</strong></p>", n, espanol)
	})
	fmt.Println("Servidor Go V2 en http://localhost:8081/v2?n=10")
	http.ListenAndServe(":8081", nil)
}