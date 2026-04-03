const elPuertaEstado = document.getElementById('puerta-estado');
const elPuertaHora = document.getElementById('puerta-hora');
const elDistanciaValor = document.getElementById('distancia-valor');
const elDistanciaEstado = document.getElementById('distancia-estado');
const elListaEventos = document.getElementById('lista-eventos');
const elApiIndicator = document.getElementById('api-indicator');
const elApiText = document.getElementById('api-text');

const URL_API = 'http://localhost:3000/api/datos';

// CARGAR DATOS Y ACTUALIZAR UI
async function refrescarDatos() {
    try {
        const respuesta = await fetch(URL_API);
        const datos = await respuesta.json();
        
        if (datos.length > 0) {
            // ACTUALIZAR INDICADOR DE CONEXION
            elApiIndicator.className = "dot connected";
            elApiText.innerText = "Conexion Establecida (En vivo)";
            
            // ACTUALIZAR TARJETA CON EL EVENTO MAS RECIENTE
            const actual = datos[0];
            
            // Actualizar estado de puerta
            if (actual.tipo === "puerta") {
                const estado = actual.estado === "abierta" ? "ABIERTA" : "CERRADA";
                elPuertaEstado.innerText = estado;
                elPuertaEstado.className = `status-indicator ${actual.estado === "abierta" ? 'abierta' : 'cerrada'}`;
            } else if (actual.tipo === "intento") {
                const estado = actual.resultado === "exitoso" ? "ACCESO PERMITIDO" : "ACCESO DENEGADO";
                elPuertaEstado.innerText = estado;
                elPuertaEstado.className = `status-indicator ${actual.resultado === "exitoso" ? 'abierta' : 'cerrada'}`;
            }
            
            // ACTUALIZAR HORA
            if (actual.createdAt) {
                const fecha = new Date(actual.createdAt);
                const hora = fecha.toLocaleTimeString('es-ES');
                elPuertaHora.innerText = hora;
            } else {
                elPuertaHora.innerText = '--:--:--';
            }
            
            // ACTUALIZAR SENSOR DE DISTANCIA
            if (actual.distancia_cm !== undefined) {
                elDistanciaValor.innerText = actual.distancia_cm + " cm";
                elDistanciaEstado.innerText = actual.distancia_cm < 30 ? "OBSTACULO DETECTADO" : "ZONA DESPEJADA";
            }
            
            // ACTUALIZAR TABLA DE HISTORIAL
            elListaEventos.innerHTML = "";
            datos.forEach(registro => {
                const fila = document.createElement('tr');
                
                // Formatear fecha
                let fechaLimpia = '--:--:--';
                if (registro.createdAt) {
                    const fecha = new Date(registro.createdAt);
                    fechaLimpia = fecha.toLocaleString('es-ES');
                } else {
                    fechaLimpia = new Date().toLocaleString('es-ES');
                }
                
                // Determinar tipo de evento
                const tipoEvento = registro.tipo === "intento" ? "INTENTO" : "PUERTA";
                
                // Determinar resultado/estado y clase CSS
                let resultado = '';
                let claseRes = '';
                
                if (registro.tipo === "intento") {
                    resultado = registro.resultado ? registro.resultado.toUpperCase() : '--';
                    claseRes = registro.resultado === "exitoso" ? "res-exitoso" : "res-fallido";
                } else if (registro.tipo === "puerta") {
                    resultado = registro.estado ? registro.estado.toUpperCase() : '--';
                    claseRes = registro.estado === "abierta" ? "abierta" : "cerrada";
                }
                
                fila.innerHTML = `
                    <td>${fechaLimpia}</td>
                    <td>${tipoEvento}</td>
                    <td class="${claseRes}">${resultado}</td>
                    <td>${registro.distancia_cm || '--'} cm</td>
                `;
                
                elListaEventos.appendChild(fila);
            });
        }
    } catch (error) {
        elApiIndicator.className = "dot disconnected";
        elApiText.innerText = "Error: Servidor Desconectado";
        console.error("Error cargando datos:", error);
    }
}

// INICIAR CICLO DE POLLING (cada 2 segundos)
refrescarDatos(); // Una vez al cargar
setInterval(refrescarDatos, 2000);
