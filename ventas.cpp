#include <iostream>
#include <cstdio>
using namespace std;
/*Pide la fecha del día una sola vez al empezar y abre (o crea) la 
planilla de ese día. Después, por cada venta: pide el mozo y su 
clave y los valida, busca el precio, calcula la comisión y agrega la 
venta a la planilla del día. Al terminar, la deja ordenada por mozo.

mozos.dat, inventario.dat → comandas_dd-mm-aaaa.dat (crea/agrega), inventario.dat*/

struct ComandaHistorica {
	char  fecha[11];         // "DD-MM-AAAA" 
	char  nombreMozo[50];   // el nombre completo, repetido en cada venta 
	int codigoProducto;
	int cantidad;  
	float comision;
};

struct Producto {
	int   codigo;  
	char descripcion[50];  
	float precio;   
	int stockActual;
};
struct Mozo { 
	int idMozo; 
	char nombre[50]; 
	char password[20]; 
	float totalComision; 
};
struct Comanda { 
	int idMozo; 
	int codigoProducto; 
	int cantidad; 
	float comision; 
};

const float TASA_COMISION = 0.10f;   // la comision de cada venta es el 10% de lo vendido

float precio(const char* Archivo, int cod); // busca el pecio
void actualizar(const char* Archivo, int cod, int cantidadVendida); // actualiza el stock
void planilla(int dia, int mes, int anio); // crea la planilla del día
void pedirventa(const char* Archivo); // pide la venta
bool valido(int cod,const char* Archivo);// verifica el mozo
void ordenar(const char* Archivo); // ordena la planilla por mozo
bool password(const char* Archivo, int cod); // verifica la contraseña del mozo


int main(){
	// funcion fecha 
	int dia, mes, anio; // Pide la fecha del día
	cout << "Ingrese la fecha (DD MM AAAA): ";
	cin >> dia >> mes >> anio;
		while (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 2026 ) {    // valida que la fecha sea correcta
			cout << "Fecha inválida. Ingrese la fecha nuevamente (DD MM AAAA): ";
			cin >> dia >> mes >> anio;
		}
		
		const char* archivo = planilla(dia, mes, anio); // crea la planilla del día
	

	// Abre o crea la planilla de ese día 
		/*falta asignarle el nombre al archivo*/
	char nombreArchivo[30];
	sprintf(nombreArchivo, "comandas_%s.dat", fecha);// ver como se usa
	FILE* archivoComandas = fopen(nombreArchivo, "ab+");
	if (archivoComandas == NULL) {
		cout << "Error al abrir el archivo de comandas." << endl;
		return 1;
	}



	return 0;
}

float precio(const char* Archivo, int cod) {

	float precio;
	Producto prod;
	FILE* f = fopen(Archivo, "rb"); // abro el archivo
	if (f == NULL) {
		cout << "Error al abrir el archivo de inventario." << endl;
		return -1;
	}
	int leido = fread (&prod, sizeof(Producto), 1, f); // leo el codigo del producto
	while (leido == 1) {
		if (cod == prod.codigo) {
			precio = prod.precio; // si el codigo coincide 
			fclose(f);// cierro el archivo 
			return precio; // retorno el precio
		}
		else if (prod.codigo > cod) { // si el codigo del producto es mayor al buscado
			cout << "No se pudo encontrar el producto" << endl;
			fclose(f);
			return -1; // retorno para que no se ejecute el resto de la funcion
		}
			leido = fread(&prod, sizeof(Producto), 1, f); // leo el siguiente producto
	}
		fclose(f);// cierro el archivo 
		cout << "Producto no encontrado." << endl;
		return -1; // retorno -1 si no se encuentra el producto
	
}


void actualizar(const char* Archivo, int cod, int cantidadVendida) {
		FILE* f = fopen(Archivo, "rb+"); // abro el archivo
		if (f == NULL) {
			cout << "Error al abrir el archivo de inventario." << endl; // si no se puede abrir el archivo
			return; // para que no se ejecute el resto de la funcion
		}
		Producto prod;
		int leido = fread(&prod, sizeof(Producto), 1, f); // leo el codigo del producto
		while (leido == 1) {
			if (cod == prod.codigo) {
				prod.stockActual -= cantidadVendida; // resto la cantidad vendida
				fseek(f, -sizeof(Producto), SEEK_CUR); // vuelvo al producto a modificar
				fwrite(&prod, sizeof(Producto), 1, f); // escribo el producto actualizado
				cout << "Stock actual: " << prod.stockActual << endl; // muestro el stock actual
				if (prod.stockActual <= 0) {
					cout << "Ya no hay stock suficiente" << endl;// si no hay mas stock
				}
				fclose(f);// cierro el archivo 
				cout << "Stock actualizado con éxito" << endl;
				return; // retorno para que no se ejecute el resto de la funcion
				
			}
			else if (prod.codigo > cod) { // si el codigo del producto es mayor al buscado
				cout << "No se pudo actualizar el producto" << endl;
				fclose(f);
				return; // retorno para que no se ejecute el resto de la funcion
			}
				leido = fread(&prod, sizeof(Producto), 1, f); // leo el siguiente producto
		}
		fclose(f);// cierro el archivo
		cout << "No se pudo actualizar el producto" << endl;
		
}


void pedirventa(const char* Archivo) {
	FILE* f = fopen(Archivo, "ab+");
	if (f == NULL) {
		cout << "Error al abrir el archivo de comandas." << endl;
		return;
	}
	int n = 1;
	comanda c;
	while (n != 0) {

		cout << "Ingrese el código de mozo: ";
		cin >> c.idMozo;
		bool esValido = valido(c.idMozo, "mozos.dat"); // FUNCION verifica el mozo 
		while (!esValido) {
			cout << "Código de mozo inválido" << endl;
			cout << "ingrese un codigo de mozo valido" << endl;
			cin >> c.idMozo;
		}
		cout << "Ingrese código del producto: ";
		cin >> c.codigoProducto;
		cout << "Ingrese cantidad vendida: ";
		cin >> c.cantidad;
		float p = precio("inventario.dat", c.codigoProducto);
		while (p == -1) {
			cout << "Producto inexistente" << endl;
			cout << "Ingrese un código de producto válido: ";
			cin >> c.codigoProducto;
			p = precio("inventario.dat", c.codigoProducto);
		}
		c.comision = p * c.cantidad * TASA_COMISION;
		fwrite(&c, sizeof(comanda), 1, f);
		actualizar("inventario.dat", c.codigoProducto, c.cantidad);
		cout << "Ingrese 0 para terminar o 1 para continuar: ";
		cin >> n;
	
	}	
	ordenar(Archivo);
	fclose(f);
}
bool valido(int cod,const char* Archivo) {
	FILE* f = fopen(Archivo, "rb");
	if (f == NULL) {
		cout << "Error al abrir el archivo de mozos." << endl;
		return false;
	}
	Mozo m;
	char pass[20];
	int leido = fread(&m, sizeof(Mozo), 1, f);
	while (leido == 1) {
		if (cod == m.codigo) {
			cout << "Ingrese password: ";
			cin >> pass;
			return password(pass, m.password);
		}
		leido = fread(&m, sizeof(Mozo), 1, f);
	}
	fclose(f);
	cout << "Mozo no encontrado." << endl;
	return false;
}
bool password(char pass [], char mpass[]) {
	if (pass == mpass) {
		cout << "Mozo encontrado y password correcto." << endl;
		return true;
	}
	else {
		cout << "Password incorrecto." << endl;
		return false;
	}
}