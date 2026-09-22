#include <iostream>
#include <cstdio>
using namespace std;

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
void pedirventa(const char* Archivo); // pide la venta
bool valido(int cod,const char* Archivo);// verifica el mozo
bool password(char pass[], char mpass[]); // verifica la contraseña del mozo
void ordenar(const char* Archivo); // ordena la planilla por mozo
void planilla(int dia, int mes, int anio, char nombre[]); // crea la planilla del día
void ordenamientoBurbuja(Comanda vec[], int len); // ordena el vector de comandas por mozo
bool validarFecha(int dia, int mes, int anio); // valida la fecha ingresada


int main(){
	// funcion fecha 
	int dia, mes, anio; // Pide la fecha del día
	cout << "Ingrese la fecha (DD MM AAAA): ";
	cin >> dia >> mes >> anio;
	bool fecha = validarFecha(dia, mes, anio);
	while (!fecha) {
		cout << "fecha incorrecta, ingresela nuevamente" << endl;
		cin >> dia >> mes >> anio; 
		fecha = validarFecha(dia, mes, anio);
	}
	char planillaDia[30];
	planilla(dia, mes, anio, planillaDia); // crea la planilla del día
	pedirventa(planillaDia	); 

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
					cout << "stock agotado" << endl;// si no hay mas stock
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
	Comanda c;
	while (n != 0) {

		cout << "Ingrese el código de mozo: ";
		cin >> c.idMozo;
		bool esValido = valido(c.idMozo, "mozos.dat"); // FUNCION verifica el mozo 
		while (!esValido) {
			cout << "Código de mozo inválido" << endl;
			cout << "ingrese un codigo de mozo valido" << endl;
			cin >> c.idMozo;
			esValido = valido(c.idMozo, "mozos.dat");
		}
		cout << "Ingrese código del producto: ";
		cin >> c.codigoProducto;
		cout << "Ingrese cantidad vendida: ";
		cin >> c.cantidad;
		while (c.cantidad <= 0) {
			cout << "ingesaste una cantidad incorrectar, vuelve a ingresarla " << endl;
			cin >> c.cantidad;
		}
		float p = precio("inventario.dat", c.codigoProducto);
		while (p == -1) {
			cout << "Producto inexistente" << endl;
			cout << "Ingrese un código de producto válido: ";
			cin >> c.codigoProducto;
			p = precio("inventario.dat", c.codigoProducto);
		}
		c.comision = p * c.cantidad * TASA_COMISION;
		fwrite(&c, sizeof(Comanda), 1, f);
		actualizar("inventario.dat", c.codigoProducto, c.cantidad);
		cout << "Ingrese 0 para terminar o 1 para continuar: ";
		cin >> n;
	
	}	
	fclose(f); // cierro el archivo para poder usar ordenar 
	ordenar(Archivo);
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
		if (cod == m.idMozo) {
			cout << "Ingrese password: ";
			cin >> pass;
			fclose(f);
			return password(pass, m.password);
		}
		leido = fread(&m, sizeof(Mozo), 1, f);
	}
	fclose(f);
	cout << "Mozo no encontrado." << endl;
	return false;
}

bool password(char pass [], char mpass[]) {
	int k = 8;
	int i = 0;
	while (pass[i] != '\0' && mpass[i] != '\0') {
		if (pass[i] + k != mpass[i]) {
			cout << "contraseña incorrecta" << endl;
			return false;
		}
		i++;
	}
	if (pass[i] != '\0' || mpass[i] != '\0'){
		cout << "contraseña incorrecta" << endl;
		return false;
	}
	return true;
}

void ordenar(const char* Archivo) { // funcion que pasa el archivo a un array y lo ordena por mozo
	FILE* f = fopen(Archivo, "rb");
	if (f == NULL) {
		cout << "Error al abrir el archivo." << endl;
		return;
	}
	Comanda vec[900];
	int len = 0;
	Comanda c;
	int leido = fread(&c, sizeof(Comanda), 1, f);
	while (leido == 1) { // paso el archivo a un array 
		if (len < 900) {
			vec[len] = c;
			len++;
		}
		if (len >= 900) {
			cout << "error" << endl;
			return;
		}
		leido = fread(&c, sizeof(Comanda), 1, f);
	}

	fclose(f);
	ordenamientoBurbuja(vec, len); // ordeno el array por mozo
	FILE* f2 = fopen(Archivo, "wb");
	if (f2 == NULL) {
		cout << "Error al abrir el archivo." << endl;
		return;
	}
	for (int i = 0; i < len; i++) {
		fwrite(&vec[i], sizeof(Comanda), 1, f2); // paso el array a un archivo otra vez
	}
	
	fclose(f2);
}

void planilla(int dia, int mes, int anio, char nombre[]) {
// Abre o crea la planilla de ese día 
   sprintf(nombre, "comandas_%d-%d-%d.dat", dia,mes,anio);
	FILE* f = fopen(nombre, "ab+");
	if (f == NULL) {
		cout << "Error al abrir el archivo de comandas." << endl;
		return;
	}
	fclose(f);
}

void ordenamientoBurbuja(Comanda vec[], int len) {
	for (int i = 0; i < len - 1; i++) {
		for (int j = 0; j < len - i - 1; j++) {
			if (vec[j].idMozo > vec[j + 1].idMozo) {
				Comanda temp = vec[j];
				vec[j]= vec[j + 1];
				vec[j + 1] = temp;
			}
		}
	}
}
bool validarFecha(int dia, int mes, int anio) {
	if (dia < 1 || dia > 31) {
		return false;
	}
	if (mes < 1 || mes > 12) {
		return false;
	}
	if (anio < 2026) {
		return false;
	}
	switch (mes) {
	case 1:
	case 3: 
	case 5: 
	case 7:
	case 8:
	case 10:
	case 12:
		return dia <= 31;

	case 4:
	case 6:
	case 9:
	case 11:
		return dia <= 30;

	case 2:
		return dia <= 29;
	}
	return false;
}
