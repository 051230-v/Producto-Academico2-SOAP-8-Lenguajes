package main

import (
	"fmt"
	"net/http"
)

func main() {
	numeros := map[string]string{"1": "uno", "2": "dos", "10": "diez", "20": "veinte"}
	
	http.HandleFunc("/v3", func(w http.ResponseWriter, r *http.Request) {
		n := r.URL.Query().Get("n")
		if n == "" { n = "10" }
		resultado := numeros[n]
		if resultado == "" { resultado = "número" }
		fmt.Fprintf(w, "<h1>Go V3 (Nativo)</h1><p style='color:blue'><strong>%s = %s</strong></p>", n, resultado)
	})
	fmt.Println("Servidor Go V3 en http://localhost:8082/v3?n=10")
	http.ListenAndServe(":8082", nil)
}