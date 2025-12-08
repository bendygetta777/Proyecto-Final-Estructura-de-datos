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

    void dfsRec(int u, vector<bool>& visit, vector<int>& orden) {
        visit[u] = true;
        orden.push_back(u);
        for (const auto& a : nodos[u].adj) {
            int v = a.to;
            if (!visit[v]) {
                dfsRec(v, visit, orden);
            }
        }
    }

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

    void dijkstra(int s, int e) {
        const float INF = numeric_limits<float>::infinity();
        vector<float> dist(numNodos, INF);
        vector<int> previo(numNodos, -1);
        vector<bool> visit(numNodos, false);
        
        dist[s] = 0;
        priority_queue<pair<float, int>, vector<pair<float, int>>, greater<pair<float, int>>> pq;
        pq.push({0, s});
        
        clock_t inicio = clock();
        
        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();
            
            if (visit[u]) continue;
            visit[u] = true;
            
            for (const auto& a : nodos[u].adj) {
                int v = a.to;
                float w = a.w;
                if (!visit[v] && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    previo[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
        
        clock_t fin = clock();
        double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
        
        cout << "\n========== DIJKSTRA: RUTA MAS CORTA ==========\n";
        if (dist[e] == INF) {
            cout << "No hay camino entre " << nodos[s].nombre << " y " << nodos[e].nombre << endl;
        } else {
            cout << "Distancia total: " << fixed << setprecision(2) << dist[e] << endl;
            
            vector<int> camino;
            for (int at = e; at != -1; at = previo[at]) {
                camino.push_back(at);
            }
            reverse(camino.begin(), camino.end());
            
            cout << "Camino: ";
            for (size_t i = 0; i < camino.size(); ++i) {
                cout << nodos[camino[i]].nombre;
                if (i < camino.size() - 1) cout << " -> ";
            }
            cout << endl;
            cout << "Tiempo de computo: " << scientific << tiempo << " segundos\n";
        }
    }

    void bfs(int s) {
        queue<int> q;
        vector<bool> visit(numNodos, false);
        visit[s] = true;
        q.push(s);
        vector<int> orden;
        
        clock_t inicio = clock();
        
        while (!q.empty()) {
            int u = q.front(); 
            q.pop();
            orden.push_back(u);
            
            for (const auto& a : nodos[u].adj) {
                int v = a.to;
                if (!visit[v]) {
                    visit[v] = true;
                    q.push(v);
                }
            }
        }
        
        clock_t fin = clock();
        double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
        
        cout << "\n========== BFS (Busqueda en Anchura) ==========\n";
        cout << "Desde: " << nodos[s].nombre << endl;
        cout << "Orden de visita: ";
        for (int id : orden) {
            cout << nodos[id].nombre << " ";
        }
        cout << "\nNodos alcanzados: " << orden.size() << endl;
        cout << "Tiempo de computo: " << scientific << tiempo << " segundos\n";
    }

    void dfs(int s) {
        vector<bool> visit(numNodos, false);
        vector<int> orden;
        
        clock_t inicio = clock();
        dfsRec(s, visit, orden);
        clock_t fin = clock();
        
        double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
        
        cout << "\n========== DFS (Busqueda en Profundidad) ==========\n";
        cout << "Desde: " << nodos[s].nombre << endl;
        cout << "Orden de visita: ";
        for (int id : orden) {
            cout << nodos[id].nombre << " ";
        }
        cout << "\nNodos alcanzados: " << orden.size() << endl;
        cout << "Tiempo de computo: " << scientific << tiempo << " segundos\n";
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
        cout << " 7) MostrarListaAdj 8) MostrarMatriz   9) Dijkstra\n";
        cout << "10) BFS            11) DFS            12) MostrarASCII\n";
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
            case 9: {
                string start, end;
                cout << "Nodo de inicio: "; 
                cin >> start;
                cout << "Nodo de destino: "; 
                cin >> end;
                
                int s = g.getIdByName(start);
                int e = g.getIdByName(end);
                
                if (s == -1 || e == -1) {
                    cout << "Error: Uno o ambos nodos no existen.\n";
                    break;
                }
                
                g.dijkstra(s, e);
                break;
            }
            case 10: {
                string start; 
                cout << "Nodo de inicio: "; 
                cin >> start;
                
                int s = g.getIdByName(start);
                if (s == -1) {
                    cout << "Error: Nodo no encontrado.\n";
                    break;
                }
                
                g.bfs(s);
                break;
            }
            case 11: {
                string start; 
                cout << "Nodo de inicio: "; 
                cin >> start;
                
                int s = g.getIdByName(start);
                if (s == -1) {
                    cout << "Error: Nodo no encontrado.\n";
                    break;
                }
                
                g.dfs(s);
                break;
            }
            case 12: 
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