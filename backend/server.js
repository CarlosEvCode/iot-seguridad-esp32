const express = require('express');
const { MongoClient } = require('mongodb');
const cors = require('cors');

const app = express();
app.use(cors());
app.use(express.json());

const uri = "mongodb://localhost:27017"; 
const nombreBaseDatos = "puerta_inteligente"; 
const nombreColeccion = "eventos_puerta"; 

let db;

// INICIO DE CONEXION A MONGODB
MongoClient.connect(uri)
    .then(client => {
        console.log("[EXITO] Conectado exitosamente a MongoDB");
        db = client.db(nombreBaseDatos);
    })
    .catch(error => console.error("[ERROR] Error conectando a Mongo:", error));

// ENDPOINT PARA OBTENER DATOS (Polling)
app.get('/api/datos', async (req, res) => {
    try {
        const coleccion = db.collection(nombreColeccion);
        const datos = await coleccion.find().sort({ createdAt: -1 }).limit(10).toArray();
        res.json(datos);
    } catch (error) {
        res.status(500).json({ error: "Error interno leyendo la base de datos" });
    }
});

// --- ENCENDER EL SERVIDOR ---
app.listen(3000, () => {
    console.log("[SISTEMA] Servidor corriendo en el puerto 3000");
});
