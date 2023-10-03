# ConsolaArgentina

# `C++`

## recibir_paquetes.cpp
Programa en C++ que recibe y decodifica los paquetes de la FPGA.

- ### void precarga_datos()
    Función que para cada fila guarda en un arreglo (Mf) el valor de ángulo que le corresponde en radianes y para cada columna guarda en un arreglo (Mc) el valor de rango que le corresponde. 

- ### void cargar_matriz(int colorf)
    *colorf:* número entero entre 0 y 5. Es la amplitud del dato que se mapea a una escala de color para su representación en el gráfico.
  
    Función que escala y envía los puntos al programa en javascript de acuerdo a un canvas de 800x800px. Envía sólo las diferencias.


# `Javascript`

## wkrSocket.js
Web worker cuya función es crear el socket de conexión con la aplicación en C++ recibirPaquetes.cpp y recibir los datos a graficar.

- ### socket.on('data', data => {  
    Evento que se emite cuando se reciben datos. Acumula los datos que recibe en una variable para que no se pierdan ni se sobrescriban con nuevos valores.  De esta variable, se obtienen los objetos JSON por orden de llegada y se los envía al hilo principal para graficar.

## script.js
Hilo principal que crea un web worker para recibir datos y enviarlos a graficar.

- ### function graficarPunto(data)
    Función que recibe un objeto JSON con la coordenada x, la coordenada y el color de un punto, y lo grafica utilizando el elemento <canvas>.

