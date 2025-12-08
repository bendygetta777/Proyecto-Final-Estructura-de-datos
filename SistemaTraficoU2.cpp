#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <iomanip>
#include <ctime>
#include <unordered_map>
#include <algorithm>
#include <cmath>

using namespace std;

// Estructuras para el Grafo
struct Arista {
    int to;
    float w;
};

struct Nodo {
    int id;
    string nombre;
    vector<Arista> adj;
};

class Grafo {
private:
    vector<Nodo> nodos;
    unordered_map<string, int> nombreToId;
    int numNodos;

public:
    Grafo() : numNodos(0) {}

    void altaNodo(int id, string nombre) {
        if (id >= static_cast<int>(nodos.size())) nodos.resize(id + 1);
        nodos[id].id = id;
        nodos[id].nombre = nombre;
        nombreToId[nombre] = id;
        numNodos = max(numNodos, id + 1);
    }

    void bajaNodo(int id) {
        if (id >= 0 && id < static_cast<int>(nodos.size())) {
            nodos[id].adj.clear();
            for (auto& n : nodos) {
                n.adj.erase(remove_if(n.adj.begin(), n.adj.end(), 
                    [id](const Arista& a){ return a.to == id; }), n.adj.end());
            }
            nombreToId.erase(nodos[id].nombre);
            nodos[id].nombre = "";
        }
    }

    void altaArista(int from, int to, float w) {
        if (from >= 0 && from < static_cast<int>(nodos.size()) && to >= 0 && to < static_cast<int>(nodos.size()) && w >= 0) {
            nodos[from].adj.push_back({to, w});
        } else if (w < 0) {
            cout << "Advertencia: No se permiten pesos negativos.\n";
        }
    }

    void bajaArista(int from, int to) {
        if (from >= 0 && from < static_cast<int>(nodos.size())) {
            nodos[from].adj.erase(remove_if(nodos[from].adj.begin(), nodos[from].adj.end(), 
                [to](const Arista& a){ return a.to == to; }), nodos[from].adj.end());
        }
    }

    void mostrarListaAdj() {
        cout << "\n========== LISTA DE ADYACENCIA ==========\n";
        for (const auto& n : nodos) {
            if (!n.nombre.empty()) {
                cout << n.nombre << " (" << n.id << "): ";
                for (const auto& a : n.adj) {
                    cout << "-> " << nodos[a.to].nombre << "(" << a.w << ") ";
                }
                cout << endl;
            }
        }
    }

    void mostrarMatriz() {
        const float INF = numeric_limits<float>::infinity();
        vector<vector<float>> mat(numNodos, vector<float>(numNodos, INF));
        
        for (int i = 0; i < numNodos; ++i) {
            for (const auto& a : nodos[i].adj) {
                mat[i][a.to] = a.w;
            }
        }
        
        cout << "\n========== MATRIZ DE ADYACENCIA ==========\n";
        cout << setw(12) << " ";
        for (int i = 0; i < numNodos; ++i) {
            if (!nodos[i].nombre.empty()) 
                cout << setw(10) << nodos[i].nombre.substr(0, 9);
        }
        cout << endl;
        
        for (int i = 0; i < numNodos; ++i) {
            if (!nodos[i].nombre.empty()) {
                cout << setw(12) << nodos[i].nombre.substr(0, 11);
                for (int j = 0; j < numNodos; ++j) {
                    if (!nodos[j].nombre.empty()) {
                        if (mat[i][j] == INF) 
                            cout << setw(10) << "INF";
                        else 
                            cout << setw(10) << fixed << setprecision(1) << mat[i][j];
                    }
                }
                cout << endl;
            }
        }
    }

    void mostrarASCII() {
        cout << "\n========== MAPA ASCII DE LA RED ==========\n";
        if (numNodos == 0) {
            cout << "Red vacia.\n";
            return;
        }
        
        // Crear una rejilla simple basada en los nodos existentes
        int filas = (int)sqrt(numNodos) + 1;
        int cols = (int)sqrt(numNodos) + 2;
        vector<vector<string>> grid(filas * 2 + 1, vector<string>(cols * 3, "  "));
        
        int idx = 0;
        for (int i = 0; i < numNodos && idx < filas * cols; ++i) {
            if (!nodos[i].nombre.empty()) {
                int fila = (idx / cols) * 2;
                int col = (idx % cols) * 3;
                string label = nodos[i].nombre.substr(0, 3);
                grid[fila][col] = "[" + label.substr(0, 1) + "]";
                idx++;
            }
        }
        
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                cout << cell;
            }
            cout << endl;
        }
    }

    void cargar(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: No se pudo abrir el archivo " << filename << endl;
            return;
        }
        
        string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
            
            stringstream ss(line);
            string token;
            getline(ss, token, ';');
            string type = token;
            
            if (type == "N") {
                getline(ss, token, ';'); 
                int id = stoi(token);
                getline(ss, token, ';'); 
                string nombre = token;
                altaNodo(id, nombre);
            } else if (type == "E") {
                getline(ss, token, ';'); 
                int from = stoi(token);
                getline(ss, token, ';'); 
                int to = stoi(token);
                getline(ss, token, ';'); 
                float w = stof(token);
                altaArista(from, to, w);
            }
        }
        file.close();
        cout << "Red cargada exitosamente desde " << filename << endl;
    }

    void guardar(const string& filename) {
        ofstream file(filename);
        file << "# NODOS\n";
        for (const auto& n : nodos) {
            if (!n.nombre.empty()) {
                file << "N;" << n.id << ";" << n.nombre << "\n";
            }
        }
        file << "# ARISTAS\n";
        for (int i = 0; i < numNodos; ++i) {
            for (const auto& a : nodos[i].adj) {
                file << "E;" << i << ";" << a.to << ";" << a.w << "\n";
            }
        }
        file.close();
        cout << "Red guardada exitosamente en " << filename << endl;
    }

    int getIdByName(const string& name) { 
        if (nombreToId.find(name) != nombreToId.end())
            return nombreToId[name];
        return -1;
    }
    
    string getNameById(int id) { 
        if (id >= 0 && id < static_cast<int>(nodos.size()))
            return nodos[id].nombre;
        return "";
    }
    
    int getNumNodos() { return numNodos; }
};

int main() {
    Grafo g;
    int opcion;
    
    cout << "\n================================================\n";
    cout << "   SIMULADOR DE TRAFICO URBANO - VERSION 1.0\n";
    cout << "================================================\n";
    
    do {
        cout << "\n========== MENU PRINCIPAL ==========\n";
        cout << " 1) CargarRed       2) GuardarRed      3) AltaNodo\n";
        cout << " 4) AltaArista      5) BajaNodo        6) BajaArista\n";
        cout << " 7) MostrarListaAdj 8) MostrarMatriz   9) MostrarASCII\n";
        cout << " 0) Salir\n";
        cout << "====================================\n";
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore();
        
        switch (opcion) {
            case 1: {
                string file; 
                cout << "Archivo de red: "; 
                getline(cin, file);
                g.cargar(file);
                break;
            }
            case 2: {
                string file; 
                cout << "Archivo de red: "; 
                getline(cin, file);
                g.guardar(file);
                break;
            }
            case 3: {
                int id; 
                string nombre;
                cout << "ID del nodo: "; 
                cin >> id;
                cin.ignore();
                cout << "Nombre: "; 
                getline(cin, nombre);
                g.altaNodo(id, nombre);
                cout << "Nodo agregado exitosamente.\n";
                break;
            }
            case 4: {
                int from, to; 
                float w;
                cout << "Origen (ID): "; 
                cin >> from;
                cout << "Destino (ID): "; 
                cin >> to;
                cout << "Peso (distancia/tiempo): "; 
                cin >> w;
                g.altaArista(from, to, w);
                cout << "Arista agregada exitosamente.\n";
                break;
            }
            case 5: {
                int id; 
                cout << "ID del nodo a eliminar: "; 
                cin >> id;
                g.bajaNodo(id);
                cout << "Nodo eliminado exitosamente.\n";
                break;
            }
            case 6: {
                int from, to;
                cout << "Origen (ID): "; 
                cin >> from;
                cout << "Destino (ID): "; 
                cin >> to;
                g.bajaArista(from, to);
                cout << "Arista eliminada exitosamente.\n";
                break;
            }
            case 7: 
                g.mostrarListaAdj(); 
                break;
            case 8: 
                g.mostrarMatriz(); 
                break;
            case 9: 
                g.mostrarASCII(); 
                break;
            case 0: 
                cout << "\nCerrando simulador...\n"; 
                cout << "Gracias por usar el sistema!\n";
                break;
            default: 
                cout << "Opcion invalida. Intente nuevamente.\n";
        }
    } while (opcion != 0);
    
    return 0;
}