#define HAVE_REMOTE
#define WPCAP

#define PCAP_SRC_FILE_STRING   "file://"
#define PCAP_SRC_IF_STRING   "rpcap://"

#include <time.h>
#include <stdlib.h>
#include <winsock2.h>
#include <unistd.h>
//#include <pthread.h>

#include <algorithm>


#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <pcap.h>
#include <vector>
#include <math.h>
#include <iomanip>
#include<iostream>
#include<fstream>  //libreria para crear archivo .txt
#include<string>

#include "C:\Users\Usuario\Desktop\Mirta\LAST-TDC-main_WorkerJSON_Mapeo\RecibirPaquetes Azimut\constantesradargui.h"
#pragma comment(lib , "wpcap.lib") //For winpcap
#pragma comment(lib , "ws2_32.lib") //For winsock


// VARIABLES GLOBALES
// Carga de Datos
std::vector<std::vector<int>> M; //Matriz de los datos recibidos.

const int altoPantalla = 800;

int escala = 128;

float Mf [4096];
float Mc [8183];
int matrizPuntos[800][800];

int columna = 0;		    //Variable para acceder a las columnas de la matriz "M".
int fila = 0;			    //Variable para acceder a las filas de la matriz "M".
//----------------------------------------------
// Interpretación de "palabra"
int BI_flag = 0;			//Esta variable indica si hubo un "BI" desde el último "TRG". En modo DEBUG indica si inició un "HM"
int HM_flag = 0;			//Esta variable indica si hubo un "HM" desde el último "TRG". En modo DEBUG indica si inició un "BI"
int TRG_flag = 0;           //Esta variable en modo DEBUG indica si inició un "TRG"
//----------------------------------------------

// Variables para comprobaciones de sincronismo
int SEQ_ant = 0;			//Esta variable almacena el valor de la secuencia para verificar si se cumple.
int samples_count = 0;		//Esta variable cuenta el número de muestras entre dos "TRG". (Valor esperado 8183)
int total_samples_count = 0;//Esta variable cuenta el número de muestras entre dos "HM".
int TRG_count = 0;			//Esta variable cuenta el número de "TRG" entre dos "HM". (Valor esperado 4096)
int BI_count = 0; 			//Esta variable cuenta el número de "BI" entre dos "HM". (Valor esperado 4096)int flag_Debug = 0;
int flag_Debug = 0;
int aux=0;
unsigned int auxiliar;

//variables reducir matriz
int muestra_mayor=0; //para matriz 4096x2096
int cont=1; //para matriz 4096x2096
int bandera=1; //para matriz 4096x4096

//para medir tiempor
clock_t start_t=0, end_t;
double total_t;


//------------------------------------------------------------------------------------------------------------------------------------------
    //crear archivo .txt
//     std::ofstream archivo;  //archivo txt

    // archivo.open("muestras guardadas.txt",std::ios::out); //creamos el archivo txt

     //------------------------------------------------------------------------------------------------------------------------------------------


// DECLARACION DE FUNCIONES:
/*
            "packet_handler": Extrae los paquetes recibidos, selecciona los conjuntos de datos que conforman cada "palabra" y los envía
						a la función "interpretar" para ser procesados.
			"interpretar": Procesa los datos recibidos, comprueba que se cumpla la secuencia y determina la estrategia a seguir cuando se
						reciben las señales de "HM", "TRG" y "BI".
			"cargar_matriz": Esta función se llama desde "interpretar" compara los datos del "CFAR" con los de la matriz y se comunica
						con el graficador para si determina que es necesario cambiar el gráfico.
            "interpretar_debug": Procesalos datos recibidos en modo debug.
            "muestra_completa":guarda las muestras en una matriz M de 4096x8183, IMPORTANTE en el archivo constantesradargui.py hay que poner numeroColumna=1.
            "muestra_mitad":guarda las muestras en una matriz M de 4096x4096, IMPORTANTE en el archivo constantesradargui.py hay que poner numeroColumna=2.
            "muestra_numero_mayor":guarda las muestras en una matriz M de 4096x2096, IMPORTANTE en el archivo constantesradargui.py hay que poner numeroColumna=3.
*/
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
void cargar_matriz(int colorf);
void interpretar_debug(unsigned int auxiliar);
void algoritmo_banderas(unsigned int HM,unsigned int BI, unsigned int TRG);
void muestra_completa(int _color,unsigned int HM,unsigned int BI, unsigned int TRG);
void muestra_mitad(int _color,unsigned int HM,unsigned int BI, unsigned int TRG);
void muestra_numero_mayor(int _color,unsigned int HM,unsigned int BI, unsigned int TRG);
void crear_txt();
void precarga_datos();
SOCKET s;



int main(){


    WSADATA wsa;

    struct sockaddr_in server;
    char c = 0;

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d.\nPress a key to exit...", WSAGetLastError());
        c = getch();
        return 1;
    }

    printf("Initialised.\n");

    //Creacion del socket para enviar los datos al modulo Python.
    if((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d.\n", WSAGetLastError());
        WSACleanup();
        c = getch();
        return 1;
    }

    printf("Socket created. Connecting...\n");
    memset(&server, 0, sizeof server);
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); //Direccion de host local.
    server.sin_family = AF_INET;
    server.sin_port = htons(2020); //Puerto utilizado para transmitir datos.

    //Connect to server
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connect error:%d.\nPress a key to exit...", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        c = getch();
        return 1;
    }

    //-------------------------------------------
    //Creacion de la matriz "M" a partir de los valores de Fila y Columna recibidos desde el programa de python
    /*
    char temp[100];
    recv(s,temp,100,0);
    nfilas =  atoi(temp);
    //printf("Numero de filas: %d\n",nfilas);
    recv(s,temp,100,0);
    ncolumnas = atoi(temp);
    //printf("Numero de columnas: %d\n",ncolumnas);
    */

    M.resize(nfilas,std::vector<int>(ncolumnas,0));

    precarga_datos();
    sleep(3);

   /*
    recv(s,temp,100,0);
    if (temp[0] == 'N'){
        flag_Debug = 0;
        printf("MODO: NORMAL \n");
    }else{
        flag_Debug = 1;
        printf("MODO: DEBUG \n");
    }
    */

    pcap_if_t *alldevs;
    pcap_if_t *d;
    int inum;
    int i=0;
    pcap_t *adhandle;
    char errbuf[PCAP_ERRBUF_SIZE];
    u_int netmask;
    char packet_filter[] = "ether broadcast"; //Seteo de filtro para recibir solo transmisiones ethernet.
    //char packet_filter[] = "ether broadcast and ether src 01:06:6e:11:02:0f";
    struct bpf_program fcode;

    // List of devices.
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }

    // Print the list
    for(d=alldevs; d; d=d->next)
    {
        printf("%d. %s", ++i, d->name);
        if (d->description){
            printf(" (%s)\n", d->description);
        }
        else{
            printf(" (No description available)\n");
        }
    }

    if(i==0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return -1;
    }
    //------------------------------------------------------------------------------------------------------------------
    printf("Ingrese el numero de interfaz (1-%d):",i);
    //scanf("%d", &inum);
    inum = 5; //Se utiliza el dispositivo bajo el nombre de "Realtek PCIe GbE Family Controller".
    //Para SIAGWI04 inum=1
    //Para DESKTOP-221MIQ2 inum=5
    //------------------------------------------------------------------------------------------------------------------

    if(inum < 1 || inum > i)
    {
        printf("\nInterface number out of range.\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }

    // Jump to the selected adapter
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);


    // Open the adapter
    if ( (adhandle= pcap_open(d->name,  // name of the device
                             1466,     // portion of the packet to capture.
                                        // 65536 grants that the whole packet will be captured on all the MACs.
                             PCAP_OPENFLAG_PROMISCUOUS,         // promiscuous mode
                             20,      // read timeout
                             NULL,      // remote authentication
                             errbuf     // error buffer
                             ) ) == NULL){
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }

    // Check the link layer. We support only Ethernet for simplicity.
    if(pcap_datalink(adhandle) != DLT_EN10MB)
    {
        fprintf(stderr,"\nThis program works only on Ethernet networks.\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }

    if(d->addresses != NULL)
        // Retrieve the mask of the first address of the interface
        netmask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        // If the interface is without addresses we suppose to be in a C class network
        netmask=0xffffff;

    //compile the filter
    printf("%i",netmask);
    if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
    {
        fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }

    //set the filter
    if (pcap_setfilter(adhandle, &fcode)<0)
    {
        fprintf(stderr,"\nError setting the filter.\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }

    printf("\nlistening on %s...\n", d->description);



    // At this point, we don't need any more the device list. Free it
    pcap_freealldevs(alldevs);

  //  archivo.open("muestras guardadas.txt",std::ios::out); //creamos el archivo txt ////////////////////////////////////////////////////////////////////////////

    // start the capture
    pcap_loop(adhandle, -1, packet_handler, NULL);


    return 0;


}

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){

	//------EXTRACCIÓN DEL ENCABEZADO ETHERNET------
	/*
            En esta sección se verifica que el paquete recibido sea de interés revisando que
        las direcciones de remitente, destino y el formato del mensaje sean los correctos.
        El filtro de winpcap debería ser capaz de filtrar las direcciones, configurado adecuadamente.
	*/


    if((pkt_data[0]!=0xFF)||(pkt_data[1]!=0xFF)||(pkt_data[2]!=0xFF)||(pkt_data[3]!=0xFF)||(pkt_data[4]!=0xFF)||(pkt_data[5]!=0xFF)){
        //printf("No es Broadcast \n");
        return;
	}
	pkt_data += 6;
    if((pkt_data[0]!=0x01)||(pkt_data[1]!=0x60)||(pkt_data[2]!=0x6E)||(pkt_data[3]!=0x11)||(pkt_data[4]!=0x02)||(pkt_data[5]!=0x0F)){
        //printf("No es el remitente correcto \n"); //descarta los paquetes de remitentes equivocados, vuelve al main sin analizarlos
        return;
	}
	pkt_data += 6;
	if((pkt_data[0]!=0x08)||(pkt_data[1]!=0x01)){
       // printf("No es el formato correcto \n");
        return;
    }
    pkt_data += 2;



	//------PROCESAMIENTO DE LOS DATOS RECIBIDOS------
    //  El payload contiene 363 conjuntos de datos ("palabras"), que se deben procesar de a uno

     for (int cont_palabras=0;cont_palabras<363;cont_palabras++){


        //unsigned char datos[4];     //VARIABLE CON LOS DATOS PARA CADA PALABRA


        auxiliar = (unsigned int) pkt_data[3] << 24 | (unsigned int) pkt_data[2] << 16 | (unsigned int) pkt_data[1] << 8 | (unsigned int) pkt_data[0]; //ordena los bits en un entero sin signo

        pkt_data+=4; //me ubico en el primer dato de la siguiente palabra a analizar


        //------INTERPRETACIÓN DE CADA "palabra"------
        interpretar_debug(auxiliar);
        //----------------------------------------------
    }

    }



void interpretar_debug(unsigned int auxiliar){

    unsigned int HM = (auxiliar & 0x80000000) >> 31; // hago la and con el bit menos significativo en uno porque ese bit 0 es el que tiene el heading mark
    unsigned int BI = ((auxiliar) & 0x40000000)>>30; // lo mismo, pero desplazo 1 a la izquierda para que quede el BI en el menos significativo
    unsigned int decision_sig = (auxiliar) & 0x20000000;
    unsigned int TRG = ((auxiliar) & 0x10000000)>>28;
   // unsigned int video_sig = (auxiliar >> 4) & 0x00003FFF; // como son 14 bits, la mascara es 3FFF
    unsigned int Muestra = (auxiliar) &  0x00003FFF;


	//----------------------------------------------
    //Código para mostrar los valores obtenidos, mantener como comentario
/*
    printf("HM: %i - BI: %i - TRG: %i - Muestra: %i \n",HM,BI,TRG,Muestra);
    fprintf(archivo,"%i %i %i %i \n", HM, BI, TRG, Muestra);
*/
	//----------------------------------------------
	//Estadistica--- Buscamos los maximos y minimos de las muestras recibidas
//////habría que poner un slider que permita desde la pantalla definir los valores de muestra min y max!!!!
	if (Muestra != 0){
        if (Muestra < muestra_min ){ //Busco el dato menor
            muestra_min = Muestra;
        }
        if (Muestra > muestra_max){  //Busco el dato mayor
            muestra_max = Muestra;
        }
	}
	//----------------------------------------------
	// Mapeo de "Muestra" en "Color"

	int color = 0;

	if (Muestra<xmin){
        color = ymin;
	}else if(Muestra>xmax){
        color = ymax;
	}else{
	    color = round(a * Muestra + b); //a y b se calculan en el archivo de constantes para evitar repetir innecesariamente el proceso
	    //printf("a: %f - b: %f - Muestra: %i - Valor: %i \n",a,b,Muestra,valor[0]);
	}
	//----------------------------------------------
	//archivo<<std::to_string(Muestra);
	//archivo<<std::endl;

	//printf(" %i ",Muestra);


	//modo en el que se carga la matriz M
	muestra_completa(color,HM,BI,TRG);  //para M de 4096x8183
	//muestra_mitad(color,HM,BI,TRG);  //para M de 4096x4096
   // muestra_numero_mayor(color,HM,BI,TRG);    //para M de 4096x2096

    /*
    //dibuja circulo rojo
    if (columna==2000){
       color=3;
       algoritmo_banderas(HM,BI,TRG);
       cargar_matriz(color);
    }
    */

}

void algoritmo_banderas(unsigned int HM,unsigned int BI, unsigned int TRG){
 // Estrategia de carga de datos en "M".
    // Señal de HM en la palabra que "cierra" la matriz M indicando el fin de una barrida.
        //printf("Valor: %i \n",color);
      //reconoce la primera muestra de HM
      if ((HM == 1)&&(HM_flag == 0)){

		HM_flag = 1;
        //send(s, "H,0000000000", strlen("H,0000000000"), 0); //Envía la señal de HM al graficador
       // std::string jsonStr = "{\"fila\":\"H\", \"columna\": \"0\", \"color\": \"0\"}";
       // send(s, jsonStr.c_str(), jsonStr.size(), 0);
        std::string jsonStr = "{\"coordenadaX\":\"H\", \"coordenadaY\": \"0\", \"color\": \"0\"}";
        send(s, jsonStr.c_str(), jsonStr.size(), 0);



        columna=-1; //<--------------------------------------------------------------------------------------------- va a cero
        fila=0;
		//printf("TRG contados: %i - BI contados: %i - Muestras: %i \n",TRG_count,BI_count,total_samples_count);
		//printf("Muestra Menor : %i - Muestra Mayor: %i \n", muestra_min,muestra_max);
		TRG_count = 0;
		BI_count = 0;
		//total_samples_count = 0;
		//printf("pasa. \n");
    }

    // Chequeo de señal de HM. En caso de no existir baja la bandera
    	if ((HM == 0)&&(HM_flag == 1)){
        HM_flag = 0;
	}
	// Chequeo de señal de BI. En caso de llegar la primera muestra aumenta la fila y levanta el flag.
	if ((BI == 1)&&(BI_flag == 0)){ //aumento la fila en el mismo numero de columna
        fila ++;
        //printf("fila: %i \n",fila);
        BI_flag = 1;
        //BI_count++;
	}
    // Chequeo de señal de BI. En caso de no existir baja la bandera
	if ((BI == 0)&&(BI_flag == 1)){
        BI_flag = 0;
	}
    // Chequeo de señal de TRG. En caso de llegar la primera muestra hace columna 0 y levanta el flag.
	if ((TRG == 1)&&(TRG_flag == 0)){ //Se Reinicia la columna en la fila siguiente
        columna = -1;//<----------------------------------------------------------------------------------------------------------------
        TRG_flag = 1;
        //TRG_count ++;

        //printf("Muestras contadas por TRG: %i \n",samples_count);
	}
    // Chequeo de señal de TRG. En caso de no existir baja la bandera
	if ((TRG == 0)&&(TRG_flag == 1)){
        TRG_flag = 0;
        //columna++ ;

	}
}




void precarga_datos(){

    float pTRG = 1 / 2037.0f;
    float rangMax = pTRG * 300000000 * 0.00053996f;

    for(int f = 0; f < 4096; f++){
        //float anguloEnRadianes = (90 - (f * 360.0) / 4096) * (3.1416 / 180);
        Mf[f] = (90 - (f * 360.0) / 4096) * (3.1416f / 180);
    }

    for(int c = 0; c < 8183; c++){
        //float rangoEnMillas = (c * rangMax) / 8183;
        Mc[c] = ((c * rangMax) / 8183) ;
    }
        

}



void cargar_matriz(int colorf){

        columna++; //se modificó la posición de esta línea para lograr una estrategia de carga más lógica
        //printf("columna: %i \n",columna);

		if ((fila >= nfilas)||(columna >= ncolumnas)){
			//printf("Error: Datos exceden los limites de la matriz. \n");
		}else{

			if (colorf != M[fila][columna]){

                float radioEscalado = (Mc[columna] * ((altoPantalla / 2) -3) ) / escala;
                int puntoX = round(400 + (std::cos(Mf[fila]) * radioEscalado ));
                int puntoY = round(400 - (std::sin(Mf[fila]) * radioEscalado ));

                if (puntoX<800 && puntoY<800)
                    if (colorf != matrizPuntos[puntoX][puntoY]){

                        matrizPuntos[puntoX][puntoY] = colorf;

                        M[fila][columna] = colorf;

                         //   std::string jsonStr = "{\"fila\": " + std::to_string(Mf[fila]) + ", \"columna\": " + std::to_string(Mc[columna]) + ", \"color\": " + std::to_string(colorf) +"}";

                            std::string jsonStr = "{\"coordenadaX\": " + std::to_string(puntoX) + ", \"coordenadaY\": " + std::to_string(puntoY) + ", \"color\": " + std::to_string(colorf) +"}";
                            send(s, jsonStr.c_str(), jsonStr.size(), 0);



                    }
                }
            }

        }



void muestra_numero_mayor(int _color,unsigned int HM,unsigned int BI, unsigned int TRG){
    //carga la matriz con un cuarto de las muestras
    //la estrategia consiste en tomar de a 4 muestras y cargar en M solamente la muestra mayor, el resto se descarta
    if (cont<=4){
        if (_color>muestra_mayor){
            muestra_mayor=_color;
        }
    }
    if (cont>=4){
        algoritmo_banderas(HM,BI,TRG);
        cargar_matriz(muestra_mayor);
        cont=0; //reinicio cont
        muestra_mayor=0; //reinicio muestra_mayor
    }
    cont++;
}

void muestra_mitad(int _color,unsigned int HM,unsigned int BI, unsigned int TRG){
//carga la matriz M, con la mitad de las muestras
   //la estrategia consiste en ir cargando de a una muestra y descartar la siguiente
   //toma la mitad de las muestras (4096)
    if (bandera==1) {
        algoritmo_banderas(HM,BI,TRG);
        cargar_matriz(_color);
        bandera=0;
    }
    else {
        bandera=1;
    }

}

void muestra_completa(int _color,unsigned int HM,unsigned int BI, unsigned int TRG){
    //carga la matriz M con todas las muestras
    algoritmo_banderas(HM,BI,TRG);
    cargar_matriz(_color);

}

void crear_txt(){

 std::ofstream archivo;  //archivo txt

 archivo.open("muestras guardadas.txt",std::ios::out); //creamos el archivo txt
}





