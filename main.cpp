#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Estructuras de datos
struct Cliente {
    string nombre;
    string cedula;
    string direccion;
    string telefono;
    string correo;
    string ruc;
    string formaPago;
};

struct Producto {
    int id;
    string nombre;
    string descripcion;
    float precio;
    bool tieneIVA;
};

struct Establecimiento {
    string nombre;
    string direccion;
    string propietario;
    string ruc;
    string telefono;
};

// Variables globales
Establecimiento establecimiento;
bool establecimientoConfigurado = false;

// ────────────── Funciones Auxiliares ────────────────
void mostrarTitulo(const string& titulo) {
    cout << "\n=======================================\n";
    cout << "  " << titulo << "\n";
    cout << "=======================================\n";
}

string obtenerFechaActual() {
    time_t ahora = time(0);
    tm* tiempo = localtime(&ahora);
    return to_string(tiempo->tm_mday) + "/" + to_string(1 + tiempo->tm_mon) + "/" + to_string(1900 + tiempo->tm_year);
}

bool esNumero(const string& s) {
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return !s.empty();
}

string centrarTexto(const string& texto, int width) {
    int textoLength = (int)texto.length();
    if (textoLength >= width) return texto;

    int espaciosAntes = (width - textoLength) / 2;
    int espaciosDespues = width - textoLength - espaciosAntes;

    string resultado(espaciosAntes, ' ');
    resultado += texto;
    resultado += string(espaciosDespues, ' ');

    return resultado;
}

bool esDecimal(const string& s) {
    bool punto = false;
    for (char c : s) {
        if (c == '.') {
            if (punto) return false;
            punto = true;
        } else if (!isdigit(c)) {
            return false;
        }
    }
    return !s.empty();
}

// ────────────── Funciones JSON ────────────────
void guardarJSON(const string& archivo, const json& data) {
    ofstream f(archivo);
    if (f.is_open()) {
        f << data.dump(4);
        f.close();
    }
}

json cargarJSON(const string& archivo) {
    ifstream f(archivo);
    json data;
    if (f.is_open()) {
        f >> data;
        f.close();
    }
    return data;
}

// ────────────── Configuración ────────────────
void configurarEstablecimiento() {
    mostrarTitulo("CONFIGURAR ESTABLECIMIENTO");

    cout << "Nombre del Establecimiento: ";
    getline(cin, establecimiento.nombre);

    cout << "Dirección: ";
    getline(cin, establecimiento.direccion);

    cout << "Propietario del RUC: ";
    getline(cin, establecimiento.propietario);

    do {
        cout << "RUC (mínimo 10 dígitos): ";
        getline(cin, establecimiento.ruc);
    } while (establecimiento.ruc.length() < 10);

    cout << "Teléfono: ";
    getline(cin, establecimiento.telefono);

    json config;
    config["nombre"] = establecimiento.nombre.empty() ? "Sin nombre" : establecimiento.nombre;
    config["direccion"] = establecimiento.direccion.empty() ? "Sin dirección" : establecimiento.direccion;
    config["propietario"] = establecimiento.propietario.empty() ? "Sin propietario" : establecimiento.propietario;
    config["ruc"] = establecimiento.ruc.empty() ? "0000000000" : establecimiento.ruc;
    config["telefono"] = establecimiento.telefono.empty() ? "Sin teléfono" : establecimiento.telefono;

    guardarJSON("config.json", config);
    establecimientoConfigurado = true;

    cout << "\nConfiguración guardada!\nPresione Enter...";
    cin.get();
}

void cargarConfiguracion() {
    json config = cargarJSON("config.json");
    if (!config.empty()) {
        establecimiento.nombre = config.value("nombre", "");
        establecimiento.direccion = config.value("direccion", "");
        establecimiento.propietario = config.value("propietario", "");
        establecimiento.ruc = config.value("ruc", "");
        establecimiento.telefono = config.value("telefono", "");
        
        if (!establecimiento.nombre.empty() && establecimiento.ruc.length() >= 10) {
            establecimientoConfigurado = true;
        }
    }
}

// ────────────── Gestión de Clientes ────────────────
bool validarCedula(const string& cedula) {
    if (cedula.length() != 10) return false;
    for (char c : cedula) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool validarCorreo(const string& correo) {
    size_t at = correo.find('@');
    if (at == string::npos) return false;
    
    size_t dot = correo.find('.', at);
    return dot != string::npos && dot > at + 1 && dot < correo.length() - 1;
}

void guardarCliente() {
    Cliente c;
    mostrarTitulo("GUARDAR NUEVO CLIENTE");

    cout << "Nombre: ";
    getline(cin, c.nombre);

    do {
        cout << "Cédula (10 dígitos): ";
        getline(cin, c.cedula);
    } while (!validarCedula(c.cedula));

    cout << "Dirección: ";
    getline(cin, c.direccion);

    cout << "Teléfono: ";
    getline(cin, c.telefono);

    do {
        cout << "Correo: ";
        getline(cin, c.correo);
    } while (!validarCorreo(c.correo));

    cout << "RUC (opcional): ";
    getline(cin, c.ruc);

    cout << "Forma de Pago: ";
    getline(cin, c.formaPago);

    json clientes = cargarJSON("clientes.json");
    json nuevoCliente;
    nuevoCliente["nombre"] = c.nombre.empty() ? "Sin nombre" : c.nombre;
    nuevoCliente["cedula"] = c.cedula.empty() ? "0000000000" : c.cedula;
    nuevoCliente["direccion"] = c.direccion.empty() ? "Sin dirección" : c.direccion;
    nuevoCliente["telefono"] = c.telefono.empty() ? "Sin teléfono" : c.telefono;
    nuevoCliente["correo"] = c.correo.empty() ? "sin@correo.com" : c.correo;
    nuevoCliente["ruc"] = c.ruc.empty() ? "Sin RUC" : c.ruc;
    nuevoCliente["formaPago"] = c.formaPago.empty() ? "Sin especificar" : c.formaPago;

    clientes.push_back(nuevoCliente);
    guardarJSON("clientes.json", clientes);

    cout << "\nCliente guardado correctamente!\nPresione Enter...";
    cin.get();
}

void listarClientes() {
    mostrarTitulo("LISTA DE CLIENTES");
    json clientes = cargarJSON("clientes.json");
    
    if (clientes.empty()) {
        cout << "No hay clientes registrados.\n";
    } else {
        for (size_t i = 0; i < clientes.size(); i++) {
            cout << "Cliente #" << i+1 << ":\n";
            cout << "  Nombre: " << clientes[i].value("nombre", "[Sin nombre]") << "\n";
            cout << "  Cédula: " << clientes[i].value("cedula", "[Sin cédula]") << "\n";
            cout << "  Teléfono: " << clientes[i].value("telefono", "[Sin teléfono]") << "\n";
            cout << "------------------------\n";
        }
    }
    cout << "Presione Enter...";
    cin.get();
}

// ────────────── Gestión de Productos ────────────────
void guardarProducto() {
    Producto p;
    mostrarTitulo("GUARDAR NUEVO PRODUCTO");

    json productos = cargarJSON("productos.json");
    p.id = productos.empty() ? 1 : productos.back().value("id", 0) + 1;

    cout << "ID asignado: " << p.id << endl;
    cout << "Nombre del producto: ";
    getline(cin, p.nombre);

    cout << "Descripción: ";
    getline(cin, p.descripcion);

    string precioStr;
    do {
        cout << "Precio: ";
        getline(cin, precioStr);
    } while (!esDecimal(precioStr));
    p.precio = atof(precioStr.c_str());

    char respuestaIVA;
    do {
        cout << "¿Tiene IVA? (s/n): ";
        cin >> respuestaIVA;
        cin.ignore();
    } while (tolower(respuestaIVA) != 's' && tolower(respuestaIVA) != 'n');

    p.tieneIVA = (tolower(respuestaIVA) == 's');

    json nuevoProducto;
    nuevoProducto["id"] = p.id;
    nuevoProducto["nombre"] = p.nombre.empty() ? "Producto sin nombre" : p.nombre;
    nuevoProducto["descripcion"] = p.descripcion.empty() ? "Sin descripción" : p.descripcion;
    nuevoProducto["precio"] = p.precio;
    nuevoProducto["tieneIVA"] = p.tieneIVA;

    productos.push_back(nuevoProducto);
    guardarJSON("productos.json", productos);

    cout << "\nProducto guardado correctamente.\nPresione Enter...";
    cin.get();
}

void listarProductos() {
    mostrarTitulo("LISTA DE PRODUCTOS");
    json productos = cargarJSON("productos.json");
    
    if (productos.empty()) {
        cout << "No hay productos registrados.\n";
    } else {
        for (size_t i = 0; i < productos.size(); i++) {
            cout << "ID: " << productos[i].value("id", 0) << "\n";
            cout << "Nombre: " << productos[i].value("nombre", "[Sin nombre]") << "\n";
            cout << "Descripción: " << productos[i].value("descripcion", "[Sin descripción]") << "\n";
            cout << "Precio: $" << fixed << setprecision(2) << productos[i].value("precio", 0.0f) << "\n";
            cout << "IVA: " << (productos[i].value("tieneIVA", false) ? "SI" : "NO") << "\n";
            cout << "------------------------\n";
        }
    }
    cout << "Presione Enter...";
    cin.get();
}

// ────────────── Facturación ────────────────
void generarFactura() {
    if (!establecimientoConfigurado) {
        cout << "Primero configure el establecimiento\n";
        cout << "Presione Enter...";
        cin.get();
        return;
    }

    json clientes = cargarJSON("clientes.json");
    if (clientes.empty()) {
        cout << "No hay clientes registrados\n";
        cout << "Presione Enter...";
        cin.get();
        return;
    }

    json productos = cargarJSON("productos.json");
    if (productos.empty()) {
        cout << "No hay productos registrados\n";
        cout << "Presione Enter...";
        cin.get();
        return;
    }

    mostrarTitulo("GENERAR FACTURA");

    // Mostrar clientes
    cout << "Clientes disponibles:\n";
    for (size_t i = 0; i < clientes.size(); i++) {
        cout << i + 1 << ". " << clientes[i].value("nombre", "[Sin nombre]") 
             << " (" << clientes[i].value("cedula", "[Sin cédula]") << ")\n";
    }

    // Seleccionar cliente
    size_t clienteIdx;
    string input;
    do {
        cout << "Seleccione cliente (1-" << clientes.size() << "): ";
        getline(cin, input);
        if (esNumero(input)) {
            clienteIdx = stoi(input) - 1;
        }
        else {
            clienteIdx = clientes.size(); // Inválido
        }
    } while (clienteIdx >= clientes.size());

    json cliente = clientes[clienteIdx];

    // Seleccionar productos
    json factura;
    factura["cliente"] = cliente;
    factura["fecha"] = obtenerFechaActual();
    factura["productos"] = json::array();
    float subtotal = 0, iva = 0, total = 0;

    char continuar;
    do {
        // Mostrar productos
        cout << "\nProductos disponibles:\n";
        for (size_t i = 0; i < productos.size(); i++) {
            cout << productos[i].value("id", 0) << ". " << productos[i].value("nombre", "[Sin nombre]")
                << " - $" << fixed << setprecision(2) << productos[i].value("precio", 0.0f)
                << (productos[i].value("tieneIVA", false) ? " (con IVA)" : "") << "\n";

        }

        // Seleccionar producto
        int idProducto;
        bool encontrado = false;
        json productoSeleccionado;

        do {
            cout << "Ingrese ID del producto (0 para terminar): ";
            getline(cin, input);
            if (!esNumero(input)) {
                cout << "ID inválido\n";
                continue;
            }

            idProducto = stoi(input);
            if (idProducto == 0) break;

            for (size_t i = 0; i < productos.size(); i++) {
                if (productos[i].value("id", 0) == idProducto) {
                    productoSeleccionado = productos[i];
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado && idProducto != 0) {
                cout << "Producto no encontrado\n";
            }
        } while (!encontrado && idProducto != 0);

        if (idProducto == 0) break;

        // Agregar producto a factura
        factura["productos"].push_back(productoSeleccionado);

        // Calcular montos
        float precio = productoSeleccionado.value("precio", 0.0f);
        subtotal += precio;
        if (productoSeleccionado.value("tieneIVA", false)) {
            iva += precio * 0.15f;
        }

        cout << "¿Agregar otro producto? (s/n): ";
        cin >> continuar;
        cin.ignore();
    } while (tolower(continuar) == 's');

    total = subtotal + iva;

    // Formatear y guardar factura en archivo txt
    string nombreArchivo = "factura_" + cliente.value("cedula", "0000000000") + "_" + obtenerFechaActual() + ".txt";
    for (size_t pos = 0; (pos = nombreArchivo.find('/')) != string::npos; ) {
        nombreArchivo.replace(pos, 1, "-");
    }

    ofstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        const int anchoLinea = 50;
        archivo << centrarTexto("** " + establecimiento.nombre + " **", anchoLinea) << "\n";
        archivo << centrarTexto(establecimiento.direccion, anchoLinea) << "\n";
        archivo << centrarTexto("Tel: " + establecimiento.telefono, anchoLinea) << "\n";
        archivo << centrarTexto("RUC: " + establecimiento.ruc, anchoLinea) << "\n";
        archivo << string(anchoLinea, '=') << "\n";

        archivo << "FACTURA\n";
        archivo << "Fecha: " << factura["fecha"] << "\n";
        archivo << "Cliente: " << cliente.value("nombre", "[Sin nombre]") << "\n";
        archivo << "Cédula: " << cliente.value("cedula", "[Sin cédula]") << "\n";
        if (!cliente.value("ruc", "").empty()) {
            archivo << "RUC: " << cliente.value("ruc", "") << "\n";
        }
        archivo << string(anchoLinea, '-') << "\n";

        archivo << left << setw(30) << "Producto" << setw(10) << "Precio" << setw(10) << "IVA" << "\n";
        archivo << string(anchoLinea, '-') << "\n";

        for (size_t i = 0; i < factura["productos"].size(); i++) {
            json producto = factura["productos"][i];
            string nombre = producto.value("nombre", "[Producto sin nombre]");
            float precio = producto.value("precio", 0.0f);
            bool tieneIVA = producto.value("tieneIVA", false);
            
            string ivaStr = tieneIVA ? "15%" : "-";
            archivo << left << setw(30) << nombre
                    << "$" << setw(9) << fixed << setprecision(2) << precio
                    << setw(10) << ivaStr << "\n";
        }

        archivo << string(anchoLinea, '-') << "\n";
        archivo << setw(30) << "Subtotal:" << "$" << fixed << setprecision(2) << subtotal << "\n";
        archivo << setw(30) << "IVA (15%):" << "$" << fixed << setprecision(2) << iva << "\n";
        archivo << setw(30) << "TOTAL:" << "$" << fixed << setprecision(2) << total << "\n";

        archivo << string(anchoLinea, '=') << "\n";
        archivo << centrarTexto("¡Gracias por su compra!", anchoLinea) << "\n";

        archivo.close();
        cout << "\nFactura guardada en archivo: " << nombreArchivo << "\n";
    }
    else {
        cout << "Error al guardar la factura en archivo.\n";
    }

    cout << "Presione Enter...";
    cin.get();
}

// ────────────── Menú Principal ────────────────
int main() {
    // Mostrar ASCII art solo una vez al inicio
    static bool asciiShown = false;
    if (!asciiShown) {
        cout << "\n   ___  _      ___  _____         __    _               \n";
        cout << "  / __\\/_\\    / __\\/__   \\/\\ /\\  /__\\  /_\\    _     _   \n";
        cout << " / _\\ //_\\\\  / /     / /\\/ / \\ \\/ \\// //_\\\\ _| |_ _| |_ \n";
        cout << "/ /  /  _  \\/ /___  / /  \\ \\_/ / _  \\/  _  \\_   _|_   _|\n";
        cout << "\\/   \\_/ \\_/\\____/  \\/    \\___/\\/ \\_/\\_/ \\_/ |_|   |_|  \n\n";
        asciiShown = true;
    }

    cargarConfiguracion();
    int opcion;
    
    do {
        cout << "\n========================================\n";
        cout << "           SISTEMA DE FACTURACIÓN       \n";
        cout << "========================================\n";
        cout << " 1. Configurar Establecimiento\n";
        cout << " 2. Guardar Cliente\n";
        cout << " 3. Listar Clientes\n";
        cout << " 4. Guardar Producto\n";
        cout << " 5. Listar Productos\n";
        cout << " 6. Generar Factura\n";
        cout << " 7. Salir\n";
        cout << "========================================\n";

        if (establecimientoConfigurado) {
            cout << "Establecimiento: " << establecimiento.nombre << "\n";
        } else {
            cout << "[!] Establecimiento no configurado\n";
        }

        string input;
        do {
            cout << "\nSeleccione opción (1-7): ";
            getline(cin, input);
        } while (!esNumero(input) || (opcion = stoi(input)) < 1 || opcion > 7);

        switch(opcion) {
            case 1: configurarEstablecimiento(); break;
            case 2: guardarCliente(); break;
            case 3: listarClientes(); break;
            case 4: guardarProducto(); break;
            case 5: listarProductos(); break;
            case 6: generarFactura(); break;
            case 7: cout << "\n¡Hasta luego!\n"; break;
        }
    } while (opcion != 7);

    return 0;
}