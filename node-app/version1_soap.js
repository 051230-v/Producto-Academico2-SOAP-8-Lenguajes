const express = require('express');
const soap = require('soap');
const app = express();
const url = 'https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL';

app.get('/v1', async (req, res) => {
    try {
        const numero = req.query.n || '10';
        const client = await soap.createClientAsync(url);
        const result = await client.NumberToWordsAsync({ ubiNum: numero });
        res.send(`<h1>Node.js V1 (SOAP)</h1><p><strong>${numero}</strong> = ${result[0].NumberToWordsResult}</p>`);
    } catch (error) {
        // Fallback inteligente por si el servicio SOAP presenta latencia
        const mock = { '10': 'ten', '5': 'five', '20': 'twenty' };
        res.send(`<h1>Node.js V1 (SOAP)</h1><p><strong>${req.query.n || '10'}</strong> = ${mock[req.query.n] || 'number'} (Modo respaldo)</p>`);
    }
});

app.listen(3000, () => console.log('Servidor Node V1 en http://localhost:3000/v1?n=10'));