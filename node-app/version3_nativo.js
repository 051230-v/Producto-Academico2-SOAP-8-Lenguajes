const express = require('express');
const app = express();

const numeros = { '1': 'uno', '2': 'dos', '3': 'tres', '4': 'cuatro', '5': 'cinco', '6': 'seis', '7': 'siete', '8': 'ocho', '9': 'nueve', '10': 'diez', '20': 'veinte' };

app.get('/v3', (req, res) => {
    const numero = req.query.n || '10';
    const resultado = numeros[numero] || 'número fuera de rango';
    res.send(`<h1>Node.js V3 (Nativo)</h1><p style='color:blue'><strong>${numero} = ${resultado}</strong></p>`);
});

app.listen(3002, () => console.log('Servidor Node V3 en http://localhost:3002/v3?n=10'));