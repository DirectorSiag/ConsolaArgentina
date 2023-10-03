const net = require('net');
//const { parentPort } = require('worker_threads');

let socket_port = "2020";
var filas= 4096;
var columnas= 8183;
var matrizPuntos = [];
var newMatrix = [];

for(var i=0; i<filas; i++) {
    matrizPuntos[i] = new Array(columnas);
}


const server = net.createServer(socket => {
  console.log('Cliente C++ conectado.');
  
  let jsonString = '';
  let jsonStrings = [];

  socket.on('data', data => {     
    
    jsonString += data.toString();
    data="";
   // Buffer.from('');

    let primeraLlave = jsonString.indexOf("{");
    let ultimaLlave =jsonString.lastIndexOf("}"); 

    // Dividir la cadena en objetos JSON individuales
    let jsonMatches = jsonString.match(/\{[^}]+\}/g);

    // Eliminar las cadenas JSON procesadas de jsonString
    jsonString= jsonString.slice( ultimaLlave - primeraLlave + 1);
   // jsonStrings.splice(0, jsonMatches.length);
   

    let cadena = "{fila:1, columna:2, color:3}{fila:1, columna:2, color:3}{fil";
     primeraLlave = cadena.indexOf("{");
     ultimaLlave =cadena.lastIndexOf("}"); 
     cadena= cadena.slice(ultimaLlave - primeraLlave +1);


    //jsonString = "";//JSON.stringify(jsonStrings);
   // jsonStrings = [];

    if (jsonMatches) {
      jsonMatches.forEach((jsonStr) => {
        const jsonObj = JSON.parse(jsonStr);

        // Procesar el objeto JSON
        postMessage(jsonObj);
        //console.log(jsonObj);
      });

    };
    jsonMatches="";
    
  });    

  socket.on('end', () => {
    console.log('Cliente C++ desconectado.');
  });
});




// const server = net.createServer(socket => {
//     console.log('Cliente C++ conectado.');
    
//     let jsonString = '';
//     const jsonStrings = [];

//     socket.on('data', data => {     
//       // let objJSON = JSON.parse(data.toString());
//       // let dataString = data.toString();

//       jsonString += data.toString();
     
//       Buffer.from('');
//       // Dividir la cadena en objetos JSON individuales
//       const jsonMatches = jsonString.match(/\{[^}]+\}/g);

//       // Eliminar las cadenas JSON procesadas de jsonString
//       jsonStrings.splice(0, jsonMatches.length);


//       if (jsonMatches) {
//         jsonMatches.forEach((jsonStr) => {
//           const jsonObj = JSON.parse(jsonStr);

//           // Procesar el objeto JSON
//           postMessage(jsonObj);
//           console.log(jsonObj);
//         });

//       };
      
//     });
//     // let jsonString;
//     // socket.on('data', data => {     
//     //   // let objJSON = JSON.parse(data.toString());
//     //   // let dataString = data.toString();

//     //   jsonString += data.toString();
//     //   //Buffer.fill();
//     //     // Dividir la cadena en objetos JSON individuales
//     //   const jsonStrings = jsonString.match(/\{[^}]+\}/g);

//     //   // Analizar cada objeto JSON
//     //   jsonStrings.forEach((jsonStr) => {
//     //       const jsonObj = JSON.parse(jsonStr);

//     //       postMessage(jsonObj);
//     //       //console.log(jsonObj);
//     //   });

      
//       // let angulo = objJSON.fila;
//       // let distancia = objJSON.columna;
//       // let color = objJSON.color;
      
        

//         // if (angulo != "H"){
//         //   matrizPuntos[angulo][distancia] = color;
//         // }

//        // postMessage(objJSON);
//        // postMessage(matrizPuntos);
     
    
  
//     socket.on('end', () => {
//       console.log('Cliente C++ desconectado.');
//     });
//   });
  
 // const port = 5500;
  server.listen(socket_port, () => {
    console.log('Server listening on port', socket_port);
  });