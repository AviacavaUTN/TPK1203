#include <iostream>
#include <cstdio>
using namespace std;


/* struct de archivos existente comandas_histroicas.dat */
struct ComandaHistorica{
	char fecha[11];  // "DD-MM-AAAA"
	char nombreMozo[50];
	int codigoProducto;
	int cantidad;
	float comision;
};


/* struct de archivo nuevo mozos.dat */
struct Mozo{
	int idMozo;
	char nombre[50];
	char password[20];
	float totalComision;
};


/* struct del nodo que relaciona a cada mozo con el siguiente */
struct NodoMozo{
	Mozo info;
	NodoMozo* sig;
};


/* struct que define la nueva forma en la que se registraran las comandas */
struct Comanda{
	int idMozo;
	int codigoProducto;
	int cantidad;
	float comision;
};


/* struct que define una lista de comandas realizadas en una fecha especifica. Es una
sublista de la lista "NodoFecha" */
struct NodoComanda{
	Comanda info;
	NodoComanda* sig;
};


/* struct que define una lista de nodos, donde cada nodo representa una fecha en la que
se realizaron comandas */
struct NodoFecha {
	char fecha[11];
	NodoComanda* listaComandas;
	NodoFecha* sig;
};


/* funcion que crea modifica la contraseña de los mozos para su posterior verificacion,
usando su ID como texto (char) y una constante K definida arbitrariamente*/
void encriptarContra (char* contrasenia, char* destino){
	int k=8; // K = 8 (valor arbitrario)
	int i=0;
	while (contrasenia[i] != '\0'){
		destino[i] = contrasenia[i]+k; // se suma 8 a cada carcter de la contraseña
		i++;
	}
	destino[i] = '\0';
}


/* funcion que lee el archivo existente "comandas_historicas.dat", identifica a todos los mozos y los guarda
de manera ordenada (por ID) en un nuevo archivo "mozos.dat" */
void guardarYordenarMozos (){
	FILE* f = fopen("datos/comandas_historicas.dat", "rb");
	if (f==NULL){
		cout<<"error, no se pudo abrir el archivo ´comandas_historicas.dat´"<<endl;
		return;
	}
	
	NodoMozo* lista = NULL; /* puntero a el primer nodo */
	int len = 0;
	
	ComandaHistorica comanda;
	while(fread(&comanda, sizeof(ComandaHistorica), 1, f)==1){
		
		NodoMozo* punteroAux = lista; /* puntero a el nodo de comapracion */
		NodoMozo* encontrado = NULL;
		
		// while que recorre y compara letra por letra
		while (punteroAux != NULL){
			int i=0;
			bool sonIguales = true;
			while (sonIguales && comanda.nombreMozo[i] != '\0' && punteroAux->info.nombre[i] != '\0'){
				if (comanda.nombreMozo[i] != punteroAux->info.nombre[i]){
					sonIguales = false;
				}
				i++;
			}
			if (sonIguales && comanda.nombreMozo[i] == '\0' && punteroAux->info.nombre[i] == '\0'){
				encontrado = punteroAux;
				break;
			}
			punteroAux = punteroAux->sig;
		}
		
		// accion consecuente 
		if (encontrado == NULL){ // cuando no existe, se crea un nuevo nodo (un nuevo mozo)
			NodoMozo* nuevoMozo = new NodoMozo();
			len++;
			
			// agregamos los datos pertenecientes al mozo
			nuevoMozo->info.idMozo = len; // agregar id
			
			int i=0; // agregar nombre
			while (comanda.nombreMozo[i] != '\0'){
				nuevoMozo->info.nombre[i] = comanda.nombreMozo[i];
				i++;
			}
			nuevoMozo->info.nombre[i] = '\0';
			
			// agregar contraseña encriptada ????? (agregar)
			char contraChar[20]; 
			sprintf(contraChar, "%d", nuevoMozo->info.idMozo); // Estructrua auxiliar! convierte dato int en dato char
			encriptarContra(contraChar, nuevoMozo->info.password); 
			
			nuevoMozo->info.totalComision = comanda.comision; // agregar comision
			
			// se enlaza el neuvo mozo/nodo a la lista (al principio)
			nuevoMozo->sig = lista; 
			lista = nuevoMozo;
		}
			
		else{ // cuando si existe, se suma la comision al mozo
			encontrado->info.totalComision += comanda.comision;
		}
	}
	fclose(f);
	
	/* recorrer la lista creada con mozos y guardar a cada uno (cada nodo) en un nuevo archivo llamado 
	"mozos.dat" . Hacer "delete" para cada "new" realizado */
	FILE* mozos = fopen("mozos.dat", "wb");
	if (mozos==NULL){
		cout<<"error, no se pudo crear el archivo ´mozos.dat´"<<endl;
		return;
	}
	
	NodoMozo* mozo = lista; // puntero que apunta al primer nodo (mozo) de la lista
	while (mozo != NULL){
		fwrite(&(mozo->info), sizeof(Mozo),1, mozos); // escribimos en el archivo solo los datos de la struct "mozo" (se encuentran en "info"
		NodoMozo* borrarPuntero = mozo; // guardamos puntero en un puntero auxiliar
		mozo = mozo->sig; // avanzar puntero valido para la funcion (siguiente nodo)
		delete borrarPuntero; // borrar/liberar memoria de puntero (nodo) ya usado
	}
	fclose(mozos);
	
}


// funcion para mostrar por pantalla el contenido gaurdado en "mozos.dat"
void leerYmostrarMozos() {
    FILE* f = fopen("mozos.dat", "rb"); // abrimos el archivo recién creado
    if (f == NULL) {
          cout << "error, no se pudo abrir mozos.dat" << endl;
          return;
    }

    cout << "CONTENIDO DEL ARCHIVO MOZOS.DAT " << endl;

    Mozo m;
    while (fread(&m, sizeof(Mozo), 1, f) == 1) { 
          cout<<"- Id: "<<m.idMozo<<endl;
          cout<<"- Nombre: "<<m.nombre<<endl;
          cout<<"- Contraseña encriptada: "<<m.password<<endl;
	     cout<<"- Comisiones acumuladas: "<<m.totalComision<<endl;
          cout<<" "<<endl;
    }

    fclose(f); 
}


int buscarIdMozo (char nombreMozo[]){
	FILE* f = fopen("mozos.dat", "rb");
	int encontrado = -1;
	if (f == NULL){
		cout<<"error, no se pudo abrir el archivo ´mozos.dat´"<<endl;
		return encontrado;
	}
	Mozo m;
	while (fread(&m, sizeof(Mozo), 1, f)==1){
		bool sonIguales = true;
		int i=0;
		while (sonIguales && nombreMozo[i] != '\0' && m.nombre[i] != '\0'){
			if (nombreMozo[i] != m.nombre[i]){
				sonIguales = false;
			}
			i++;
		}
		if (sonIguales && nombreMozo[i] == '\0' && m.nombre[i] == '\0'){
			encontrado = m.idMozo;
			break;
		}
	}
	fclose(f);
	return encontrado;
}


/* funcion que */
void separarComandasPorFecha (){
	FILE* f = fopen("datos/comandas_historicas.dat", "rb");
	if (f == NULL){
		cout<<"error, no se pudo abrir el archivo ´comandas_historicas.dat´"<<endl;
		return;
	}
	
	NodoFecha* listaFechas = NULL;
	ComandaHistorica comanda;
	
	while (fread(&comanda, sizeof(ComandaHistorica), 1, f) == 1){
		
		NodoFecha* fechaAux = listaFechas;
		NodoFecha* encontrado = NULL;
		
		while (fechaAux != NULL){ // busca si la fecha ya existe entre todos las que hay
			bool sonIguales = true;
			int i=0;
			while (sonIguales && comanda.fecha[i] != '\0' && fechaAux->fecha[i] != '\0'){
				if (comanda.fecha[i] != fechaAux->fecha[i]){
				sonIguales =false;
				}
				i++;
			}
			if (sonIguales && comanda.fecha[i] == '\0' && fechaAux->fecha[i] == '\0'){
				encontrado = fechaAux;
				break;
			}
			fechaAux = fechaAux->sig;
		}
		
		// accion consecuente
		if (encontrado == NULL){ // no existe la fecha todavia. Se agrega
			NodoFecha* nuevaFecha = new NodoFecha;
			
			int i=0;
			while (comanda.fecha[i] != '\0'){
				nuevaFecha->fecha[i] = comanda.fecha[i];
				i++;
			}
			nuevaFecha->fecha[i] = '\0';
			
			nuevaFecha->listaComandas = NULL;
			
			// se enlaza el nuevo nodo al inicio de la lista
			nuevaFecha->sig = listaFechas;
			listaFechas = nuevaFecha;
			encontrado = nuevaFecha;
		}
		
		// crear el nodo de la comanda
		NodoComanda* nuevaComanda = new NodoComanda;
		
		// cargar datos del neuvo nodo
		nuevaComanda->info.idMozo = buscarIdMozo(comanda.nombreMozo);
		nuevaComanda->info.codigoProducto = comanda.codigoProducto;
		nuevaComanda->info.cantidad = comanda.cantidad;
		nuevaComanda->info.comision = comanda.comision;
		
		// agregar la nueva comanda a la sub lista de la fecha en que se realizo (al inicio)
		nuevaComanda->sig = encontrado->listaComandas;
		encontrado->listaComandas = nuevaComanda;
	}
	fclose(f);
	
	// guardamos todos los datos pertenecientes a la lista en los archivos correspondientes
	NodoFecha* fechaActual = listaFechas;
	
	while (fechaActual != NULL){ // ciclo que recorre cada nodo (cada fecha)
		char nombreArchivo[50];
		sprintf(nombreArchivo, "comandas_%s.dat", fechaActual->fecha); // Estructura auxiliar!! 
		
		FILE* f = fopen(nombreArchivo, "wb");
		if (f == NULL){
			cout<<"error, no se pudo abrir un nuevo archivo"<<endl;
		}
		else{
			NodoComanda* comandaActual = fechaActual->listaComandas;
			while (comandaActual != NULL){ // ciclo que recorre cada nodo (cada comanda) de esta fecha
			     fwrite(&(comandaActual->info), sizeof(Comanda), 1, f);
			     NodoComanda* temp = comandaActual;
			     comandaActual = comandaActual->sig;
			     delete temp;
		     }
		     fclose(f);
		}	
		
		NodoFecha* temp = fechaActual;
		fechaActual = fechaActual->sig;
		delete temp;
	}
}


/* funcion auxiliar para leer las comandas realizadas en una fecha especifica y mostrarlo 
por pantalla */
void mostrarComandas(const char fecha[]) {
    char nombreArchivo[50];
    sprintf(nombreArchivo, "comandas_%s.dat", fecha);
    
    FILE* f = fopen(nombreArchivo, "rb");
    if(f == NULL) {
          cout<<"No existe el archivo "<<nombreArchivo<<endl;
          return;
    }
    
    cout<<"COMANDAS DEL DIA  "<<fecha<< endl;
    Comanda c;
    while(fread(&c, sizeof(Comanda), 1, f) == 1) {
          cout<<"Id mozo: "<< c.idMozo<<endl;
          cout<<"codigo producto: " << c.codigoProducto<<endl;
          cout<<"cantidad: "<< c.cantidad<<endl;
          cout<<"comision: "<<c.comision<<endl;
          cout<<" "<<endl;
    }
    fclose(f);
}


int main() {
    guardarYordenarMozos(); // creacion de mozos.dat
    cout<<"La operacion se realizo con exito"<<endl;
    cout<<" "<<endl;
    
    leerYmostrarMozos(); // mostrar contenido guardado en mozos.dat 
    
    separarComandasPorFecha();
    cout<<"La operacion se realizo con exito"<<endl; // creacion de varios archivos
    
    mostrarComandas("02-06-2025"); // mostar contenido guardado ena rchivo de la fecha indicada
    mostrarComandas("03-06-2025");
    mostrarComandas("04-06-2025");
    mostrarComandas("05-06-2025");
    mostrarComandas("06-06-2025");
    mostrarComandas("07-06-2025");

    return 0;
}