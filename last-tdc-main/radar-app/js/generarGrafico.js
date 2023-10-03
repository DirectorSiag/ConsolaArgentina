const { Socket, dataEmitter } = require('./js/Socket.js');
//const { dataEmitter } = require('./receiveData');
//const plotly = require('plotly.js-dist');

dataEmitter.on('dataProcessed', mensaje => {

    alert(mensaje);
        // for (var i = 0; i < matrizPuntos.length; i++) {
        //     for (var j = 0; j < matrizPuntos[i].length; j++) {
              
        //         let angulo = i; //fila
        //         let distancia = j; //columna
        //         let color = matrizPuntos[i][j]; //color
        
        //         let escala = document.querySelector('input[name="escala"]:checked').value;
               
        //         if (angulo != "H"){       
        //          angulo = 90 - (angulo * 360/4096);
        //          distancia = (distancia * 80) / 8183;
        //         }
                 
        //          //console.log('dis:' + distancia + " ang: " + angulo + " color: " + data.toString().split(",")[2]);
                  
        //         graficarPuntoPolar(distancia, angulo, color, escala);     
        //     }
        // }


//   // Generar gráfico utilizando los datos en la matriz de dos dimensiones
//   const traces = dataMatrix.map((data, index) => ({
//     x: data.map(entry => entry[0]), // Suponemos que la primera columna es el eje X
//     y: data.map(entry => entry[1]), // Suponemos que la segunda columna es el eje Y
//     type: 'scatter',
//     mode: 'lines+markers',
//     name: `Serie ${index + 1}`
//   }));

//   const layout = {
//     title: 'Gráfico de Datos Procesados',
//     xaxis: { title: 'Eje X' },
//     yaxis: { title: 'Eje Y' }
//   };

//   const graphOptions = { filename: 'grafico-datos-procesados', fileopt: 'overwrite' };

//   plotly.plot(traces, layout, graphOptions, function (err, msg) {
//     if (err) return console.error(err);
//     console.log('Grafico generado:', msg.url);
//   });
});

function graficarPuntoPolar(rango, anguloEnGrados, color, escala) {
    // Elemento canvas (esta fc sólo se utiliza para crudo, sino enviar el canvas a obtener)
    const canvas = document.getElementById('canvas_crudo');
    const ctx = canvas.getContext('2d');

    if (anguloEnGrados != "H"){

        // Coordenadas del centro del canvas
        const centroX = canvas.width / 2;
        const centroY = canvas.height / 2;
    
        let radio = 0;
        let angulo = 0;

        // Valores de entrada
        radio = parseFloat(rango); 
        angulo = parseFloat(anguloEnGrados);// * (Math.PI / 180); // Convierte grados a radianes

        radio = radio.toFixed(2);
        angulo = angulo.toFixed(2);
        console.log("radio: " + radio + " angulo: " + angulo);

        //resto 3 del margen al crear el radar en radarwidget
       // const radioEscalado = (radio * ((canvas.width / 2) - 3) ) / escala

        const x = radio * Math.cos(angulo);
        const y = radio * Math.sin(angulo);

        let puntoX = (centroX + x);
        let puntoY = (centroY - y); // Restar y porque en el sistema de coordenadas del canvas, el eje y aumenta hacia abajo
        
        let puntoXdec = puntoX.toFixed(3);
        let puntoYdec = puntoY.toFixed(3);

        //console.log("puntoXdec: ",puntoXdec," puntoYdec: ",puntoYdec);
        // Cálculo de coordenadas cartesianas a coordenadas polares
       // const x = centroX + radio * Math.cos(angulo);
       // const y = centroY - radio * Math.sin(angulo); 
        
        ctx.beginPath();
        ctx.arc(puntoXdec, puntoYdec, 8, 0, 2 * Math.PI);
        ctx.fillStyle = ESCALA_COLOR[4]; 
        ctx.fill();
        ctx.closePath();
        // Dibujo del punto polar en el gráfico
        ctx.beginPath();
        ctx.arc(puntoX, puntoY, 4, 0, 2 * Math.PI);
        ctx.fillStyle = ESCALA_COLOR[color]; 
        ctx.fill();
        ctx.closePath();

        
    }
    else{
        let w = canvas.width;
        let h = canvas.height;
     //   ctx.clearRect(0, 0, w, h);
     //   crudo.graficarRadar(4);
    }
   
}
