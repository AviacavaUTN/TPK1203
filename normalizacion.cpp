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
struct nodoMozo{
	Mozo info;
	nodoMozo* sig;
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
	
	nodoMozo* lista = NULL; /* puntero a el primer nodo */
	int len = 0;
	
	ComandaHistorica comanda;
	while(fread(&comanda, sizeof(ComandaHistorica), 1, f)==1){
		
		nodoMozo* punteroAux = lista; /* puntero a el nodo de comapracion */
		nodoMozo* encontrado = NULL;
		
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
			nodoMozo* nuevoMozo = new nodoMozo();
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
			sprintf(contraChar, "%d", nuevoMozo->info.idMozo); //convierte dato int en dato char
			encriptarContra(contraChar, nuevoMozo->info.password); 
			
			nuevoMozo->info.totalComision = comanda.comision; // agregar comision
			
			nuevoMozo->sig = NULL; // definir coneccion al siguiente nodo (null)
			
			// agregar al nuevo mozo como nodo en la "list"
			if (lista == NULL){ // si es un nuevo mozo y ademas el primero, entonces..
				lista = nuevoMozo;
			}
			else{ // si es solo un nuevo mozo
				nodoMozo* ultimo = lista;
				while (ultimo->sig != NULL){
					ultimo = ultimo->sig;
				}
				ultimo->sig = nuevoMozo;
			}
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
	
	nodoMozo* mozo = lista; // puntero que apunta al primer nodo (mozo) de la lista
	while (mozo != NULL){
		fwrite(&(mozo->info), sizeof(Mozo),1, mozos); // escribimos en el archivo solo los datos de la struct "mozo" (se encuentran en "info"
		nodoMozo* borrarPuntero = mozo; // guardamos puntero en un puntero auxiliar
		mozo = mozo->sig; // avanzar puntero valido para la funcion (siguiente nodo)
		delete borrarPuntero; // borrar/liberar memoria de puntero (nodo) ya usado
	}
	fclose(mozos);
	
}


void leerYMostrarMozos() {
    FILE* f = fopen("mozos.dat", "rb"); // abrimos el archivo recién creado
    if (f == NULL) {
        cout << "Error: no se pudo abrir mozos.dat" << endl;
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

int main() {
    guardarYordenarMozos(); // creacion de mozos.dat
    cout<<"La operacion se realizo con exito"<<endl;
    
    leerYMostrarMozos(); // mostrar contenido guardado en mozos.dat 

    return 0;
}