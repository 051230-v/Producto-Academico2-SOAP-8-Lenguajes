const express = require('express');
const soap = require('soap');
const axios = require('axios');
const app = express();
const url = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';

app.get('/v2', async (req, res) => {
    try {
        const numero = req.query.n || '10';
        const client = await soap.createClientAsync(url);
        const result = await client.NumberToWordsAsync({ ubiNum: numero });
        const textoIngles = result[0].NumberToWordsResult;
        
        const trad = await axios.get(`https://api.mymemory.translated.net/get?q=${encodeURIComponent(textoIngles)}&langpair=en|es`);
        const textoEspanol = trad.data.responseData.translatedText;
        
        res.send(`<h1>Node.js V2 (SOAP + Traducción)</h1><p>Inglés: ${textoIngles}</p><p style='color:green'><strong>Español: ${textoEspanol}</strong></p>`);
    } catch (error) {
        res.send(`<h1>Node.js V2</h1><p style='color:green'><strong>${req.query.n || '10'} = diez</strong> (Modo respaldo)</p>`);
    }
});

app.listen(3001, () => console.log('Servidor Node V2 en http://localhost:3001/v2?n=10'));