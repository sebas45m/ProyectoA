/*****************************************************************************************************************
	UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
	FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

	Computadoras y programacion. 
	(c) Ponga su nombre y numero de cuenta aqui. 323176215 Martinez Navarrete sebbastian
	
	Quiso decir: Programa principal de la aplicacion de la distancia de Levenstein.
	
	
******************************************************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>
#include <cctype>
#include "corrector.h"
using namespace std;

#define TAMTOKEN 50


const char alfabeto[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    (char)0xF1, 
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    (char)0xE1, 
    (char)0xE9, 
    (char)0xED, 
    (char)0xF3, 
    (char)0xFA, 
    '\0'
};
const int TAM_ALFABETO = 32;

// ==================== FUNCIONES AUXILIARES ====================

// Convierte una cadena a minúsculas
void aMinusculas(char* cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = tolower(cadena[i]);
    }
}


bool esDelimitador(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || 
            c == ',' || c == '.' || c == ';' || c == '(' || c == ')');
}


void eliminarPuntuacion(char* palabra) {
    int len = strlen(palabra);
    while (len > 0 && (palabra[len-1] == ',' || palabra[len-1] == '.' || 
           palabra[len-1] == ';' || palabra[len-1] == '(' || palabra[len-1] == ')')) {
        palabra[len-1] = '\0';
        len--;
    }
}


int busquedaBinaria(char szPalabras[][TAMTOKEN], int numElementos, const char* palabra) {
    int izq = 0;
    int der = numElementos - 1;
    
    while (izq <= der) {
        int medio = (izq + der) / 2;
        int cmp = strcmp(szPalabras[medio], palabra);
        
        if (cmp == 0) {
            return medio; // Encontrada
        } else if (cmp < 0) {
            izq = medio + 1;
        } else {
            der = medio - 1;
        }
    }
    return -1; // No encontrada
}


void insertarOrdenado(char szPalabras[][TAMTOKEN], int iEstadisticas[], 
                      int& numElementos, const char* palabra) {
    // Buscar posición donde insertar
    int pos = 0;
    while (pos < numElementos && strcmp(szPalabras[pos], palabra) < 0) {
        pos++;
    }
    
    
    for (int i = numElementos; i > pos; i--) {
        strcpy_s(szPalabras[i], TAMTOKEN, szPalabras[i-1]);
        iEstadisticas[i] = iEstadisticas[i-1];
    }
    
    
    strcpy_s(szPalabras[pos], TAMTOKEN, palabra);
    iEstadisticas[pos] = 1;
    numElementos++;
}

// ==================== FUNCIÓN 1: DICCIONARIO ====================

void Diccionario(
    char* szNombre,
    char szPalabras[][TAMTOKEN],
    int iEstadisticas[],
    int& iNumElementos)
{
    iNumElementos = 0;
    
    // Abrir archivo
    ifstream archivo(szNombre);
    if (!archivo.is_open()) {
        return;
    }
    
    char palabra[TAMTOKEN];
    char c;
    int idx = 0;
    
    
    while (archivo.get(c)) {
        if (esDelimitador(c)) {
            if (idx > 0) {
                palabra[idx] = '\0';
                eliminarPuntuacion(palabra);
                aMinusculas(palabra);
                
                if (strlen(palabra) > 0) {
                    // Buscar si ya existe
                    int pos = busquedaBinaria(szPalabras, iNumElementos, palabra);
                    
                    if (pos != -1) {
                        // Ya existe, incrementar frecuencia
                        iEstadisticas[pos]++;
                    } else {
                        // No existe, insertar ordenado
                        insertarOrdenado(szPalabras, iEstadisticas, iNumElementos, palabra);
                    }
                }
                idx = 0;
            }
        } else {
            if (idx < TAMTOKEN - 1) {
                palabra[idx++] = c;
            }
        }
    }
    
    // Procesar última palabra si existe
    if (idx > 0) {
        palabra[idx] = '\0';
        eliminarPuntuacion(palabra);
        aMinusculas(palabra);
        
        if (strlen(palabra) > 0) {
            int pos = busquedaBinaria(szPalabras, iNumElementos, palabra);
            
            if (pos != -1) {
                iEstadisticas[pos]++;
            } else {
                insertarOrdenado(szPalabras, iEstadisticas, iNumElementos, palabra);
            }
        }
    }
    
    archivo.close();
}

// ==================== FUNCIÓN 2: CLONAR PALABRAS ====================

void ClonaPalabras(
    char* szPalabraLeida,
    char szPalabrasSugeridas[][TAMTOKEN],
    int& iNumSugeridas)
{
    iNumSugeridas = 0;
    int len = strlen(szPalabraLeida);
    
 
    strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, szPalabraLeida);
    
    // ===== 1. ELIMINACIÓN =====
    // Eliminar cada carácter de la palabra
    for (int i = 0; i < len; i++) {
        char temp[TAMTOKEN];
        int idx = 0;
        
        for (int j = 0; j < len; j++) {
            if (j != i) {
                temp[idx++] = szPalabraLeida[j];
            }
        }
        temp[idx] = '\0';
        
        if (strlen(temp) > 0) {
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temp);
        }
    }
    
  
    for (int i = 0; i < len - 1; i++) {
        char temp[TAMTOKEN];
        strcpy_s(temp, TAMTOKEN, szPalabraLeida);
        
        // Intercambiar temp[i] con temp[i+1]
        char aux = temp[i];
        temp[i] = temp[i+1];
        temp[i+1] = aux;
        
        strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temp);
    }
    
    // ===== 3. SUSTITUCIÓN =====
    // Sustituir cada carácter por cada letra del alfabeto
    // CORRECCIÓN: Usar TAM_ALFABETO en lugar de iterar hasta '\0'
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < TAM_ALFABETO; j++) {  // <=== CAMBIO CRÍTICO
            char temp[TAMTOKEN];
            strcpy_s(temp, TAMTOKEN, szPalabraLeida);
            temp[i] = alfabeto[j];
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temp);
        }
    }
    
    // ===== 4. INSERCIÓN =====
    // Insertar cada letra del alfabeto en cada posición
    // CORRECCIÓN: Usar TAM_ALFABETO en lugar de iterar hasta '\0'
    for (int i = 0; i <= len; i++) {
        for (int j = 0; j < TAM_ALFABETO; j++) {  // <=== CAMBIO CRÍTICO
            char temp[TAMTOKEN];
            int idx = 0;
            
            // Copiar caracteres antes de la posición i
            for (int k = 0; k < i; k++) {
                temp[idx++] = szPalabraLeida[k];
            }
            
            // Insertar letra del alfabeto
            temp[idx++] = alfabeto[j];
            
            // Copiar el resto de la palabra
            for (int k = i; k < len; k++) {
                temp[idx++] = szPalabraLeida[k];
            }
            temp[idx] = '\0';
            
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temp);
        }
    }
    
    // Ordenar las palabras sugeridas alfabéticamente
    // Ordenamiento burbuja simple
    for (int i = 0; i < iNumSugeridas - 1; i++) {
        for (int j = 0; j < iNumSugeridas - i - 1; j++) {
            if (strcmp(szPalabrasSugeridas[j], szPalabrasSugeridas[j+1]) > 0) {
                char temp[TAMTOKEN];
                strcpy_s(temp, TAMTOKEN, szPalabrasSugeridas[j]);
                strcpy_s(szPalabrasSugeridas[j], TAMTOKEN, szPalabrasSugeridas[j+1]);
                strcpy_s(szPalabrasSugeridas[j+1], TAMTOKEN, temp);
            }
        }
    }
}

// ==================== FUNCIÓN 3: LISTA CANDIDATAS ====================

void ListaCandidatas(
    char szPalabrasSugeridas[][TAMTOKEN],
    int iNumSugeridas,
    char szPalabras[][TAMTOKEN],
    int iEstadisticas[],
    int iNumElementos,
    char szListaFinal[][TAMTOKEN],
    int iPeso[],
    int& iNumLista)
{
    iNumLista = 0;
    
    for (int i = 0; i < iNumSugeridas; i++) {
        int pos = busquedaBinaria(szPalabras, iNumElementos, szPalabrasSugeridas[i]);
        
        if (pos != -1) {
            // La palabra existe en el diccionario
            // Verificar si ya está en la lista final (evitar duplicados)
            bool existe = false;
            for (int j = 0; j < iNumLista; j++) {
                if (strcmp(szListaFinal[j], szPalabras[pos]) == 0) {
                    existe = true;
                    break;
                }
            }
            
            if (!existe) {
                strcpy_s(szListaFinal[iNumLista], TAMTOKEN, szPalabras[pos]);
                iPeso[iNumLista] = iEstadisticas[pos];
                iNumLista++;
            }
        }
    }
    
    // Ordenar por peso (frecuencia) de mayor a menor
    for (int i = 0; i < iNumLista - 1; i++) {
        for (int j = 0; j < iNumLista - i - 1; j++) {
            if (iPeso[j] < iPeso[j+1]) {
                // Intercambiar pesos
                int tempPeso = iPeso[j];
                iPeso[j] = iPeso[j+1];
                iPeso[j+1] = tempPeso;
                
                // Intercambiar palabras
                char tempPalabra[TAMTOKEN];
                strcpy_s(tempPalabra, TAMTOKEN, szListaFinal[j]);
                strcpy_s(szListaFinal[j], TAMTOKEN, szListaFinal[j+1]);
                strcpy_s(szListaFinal[j+1], TAMTOKEN, tempPalabra);
            }
        }
    }
}

// ==================== PROGRAMA PRINCIPAL ====================

int main() {
    const int MAX_PALABRAS = 10000;
    char szPalabras[MAX_PALABRAS][TAMTOKEN];
    int iEstadisticas[MAX_PALABRAS];
    int iNumElementos = 0;
    

    cout << "=== CORRECTOR ORTOGRAFICO ===" << endl;
    cout << "Ingrese nombre del archivo diccionario: ";
    char nombreArchivo[100];
    cin >> nombreArchivo;
    
    Diccionario(nombreArchivo, szPalabras, iEstadisticas, iNumElementos);
    
    if (iNumElementos == 0) {
        cout << "Error: No se pudo cargar el diccionario." << endl;
        return 1;
    }
    
    cout << "Diccionario cargado: " << iNumElementos << " palabras" << endl;
    
    
    char palabraLeida[TAMTOKEN];
    while (true) {
        cout << "\nIngrese palabra a verificar (o 'salir' para terminar): ";
        cin >> palabraLeida;
        
        if (strcmp(palabraLeida, "salir") == 0) {
            break;
        }
        
        aMinusculas(palabraLeida);
        int pos = busquedaBinaria(szPalabras, iNumElementos, palabraLeida);
        
        if (pos != -1) {
            cout << "Palabra correcta" << endl;
        } else {
            cout << "Palabra incorrecta - Generando sugerencias..." << endl;
            
            char szPalabrasSugeridas[5000][TAMTOKEN];
            int iNumSugeridas = 0;
            ClonaPalabras(palabraLeida, szPalabrasSugeridas, iNumSugeridas);
            
            char szListaFinal[1000][TAMTOKEN];
            int iPeso[1000];
            int iNumLista = 0;
            ListaCandidatas(szPalabrasSugeridas, iNumSugeridas,
                           szPalabras, iEstadisticas, iNumElementos,
                           szListaFinal, iPeso, iNumLista);
            
            if (iNumLista > 0) {
                cout << "\nSugerencias:" << endl;
                int maxMostrar = (iNumLista < 10) ? iNumLista : 10;
                for (int i = 0; i < maxMostrar; i++) {
                    cout << "  " << i+1 << ". " << szListaFinal[i] 
                         << " (frecuencia: " << iPeso[i] << ")" << endl;
                }
            } else {
                cout << "No se encontraron sugerencias." << endl;
            }
        }
    }
    
    cout << "\nPrograma finalizado." << endl;
    return 0;
}
