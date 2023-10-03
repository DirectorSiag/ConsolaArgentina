#ifndef CONSTANTESRADARGUI_H_INCLUDED
#define CONSTANTESRADARGUI_H_INCLUDED
//NOTA: CUALQUIER CAMBIO A ESTE ARCHIVO NECESITA SER COMPILADO PREVIO A CORRER EL PROGRAMA.
float freqM=16.666;
int nBI=2049;
int nTRG=2037;
int AnchoPulso =0;
int VelGiro =2;
int nfilas = 4096;
int ncolumnas = 8183; //8183

//----------------------------------------------
//Modo DEBUG
int muestra_min = 0;
// maximo de muestra recibido
int muestra_max = 16000;    //Valor minimo de muestra recibido

//----------------------------------------------

//Configuro los limites dependiendo de muestra_min y muestra_max
int xmin = 0;// definir como constantes poner afuera como global, las que no son variables!!!! (por ej en linea 354 quedaría dividido por una constante)!!!! (hacer en un .h)
int xmax = 8040;
int ymin = 0;
int ymax = 5; //Cantidad de niveles a representar (colores)


	//interpolacion lineal para poder discriminar la asignacion de colores
float a = (float)(ymin-ymax)/(float)(xmin-xmax);
float b = ymax - a*xmax;


#endif // CONSTANTESRADARGUI_H_INCLUDED
