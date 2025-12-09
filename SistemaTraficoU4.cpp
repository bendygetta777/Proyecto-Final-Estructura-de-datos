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

    int componentesConexas() {
        vector<bool> visit(numNodos, false);
        int count = 0;
        
        cout << "\n========== COMPONENTES CONEXAS ==========\n";
        
        for (int i = 0; i < numNodos; ++i) {
            if (!visit[i] && !nodos[i].nombre.empty()) {
                count++;
                cout << "Componente " << count << ": ";
                
                queue<int> q;
                visit[i] = true;
                q.push(i);
                
                while (!q.empty()) {
                    int u = q.front(); 
                    q.pop();
                    cout << nodos[u].nombre << " ";
                    
                    for (const auto& a : nodos[u].adj) {
                        int v = a.to;
                        if (!visit[v]) {
                            visit[v] = true;
                            q.push(v);
                        }
                    }
                }
                cout << endl;
            }
        }
        
        cout << "Total de componentes: " << count << endl;
        return count;
    }

    void detectarCuellos() {
        cout << "\n========== CUELLOS DE BOTELLA ==========\n";
        cout << "(Nodos con alto grado de conexion)\n\n";
        
        vector<pair<int, int>> grados;
        
        for (int i = 0; i < numNodos; ++i) {
            if (!nodos[i].nombre.empty()) {
                int gradoSalida = static_cast<int>(nodos[i].adj.size());
                int gradoEntrada = 0;
                
                for (int j = 0; j < numNodos; ++j) {
                    for (const auto& a : nodos[j].adj) {
                        if (a.to == i) gradoEntrada++;
                    }
                }
                
                int gradoTotal = gradoSalida + gradoEntrada;
                grados.push_back({gradoTotal, i});
            }
        }
        
        sort(grados.rbegin(), grados.rend());
        
        cout << setw(20) << "Nodo" << setw(15) << "Grado Total" 
            << setw(12) << "Entrada" << setw(12) << "Salida" << endl;
        cout << string(60, '-') << endl;
        
        for (const auto& par : grados) {
            int id = par.second;
            int total = par.first;
            int salida = static_cast<int>(nodos[id].adj.size());
            int entrada = total - salida;
            
            cout << setw(20) << nodos[id].nombre 
                << setw(15) << total 
                << setw(12) << entrada 
                << setw(12) << salida << endl;
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

// Estructura para Vehículo
struct Vehiculo {
    string id, tipo, placa;
    int origen, destino;
    string horaEntrada;
};

// Tabla Hash con encadenamiento
class HashTable {
private:
    vector<list<pair<string, Vehiculo>>> table;
    int size;
    int count;

    int hashFunc(const string& key) {
        int hash = 0;
        for (char c : key) {
            hash = (hash * 31 + c) % size;
        }
        return hash;
    }

public:
    HashTable(int sz = 100) : size(sz), count(0) { 
        table.resize(size); 
    }

    void insert(const string& key, const Vehiculo& v) {
        int idx = hashFunc(key);
        
        for (auto& p : table[idx]) {
            if (p.first == key) {
                p.second = v;
                return;
            }
        }
        
        table[idx].push_back({key, v});
        count++;
    }

    Vehiculo* search(const string& key) {
        int idx = hashFunc(key);
        for (auto& p : table[idx]) {
            if (p.first == key) return &p.second;
        }
        return nullptr;
    }

    void remove(const string& key) {
        int idx = hashFunc(key);
        auto it = table[idx].begin();
        while (it != table[idx].end()) {
            if (it->first == key) {
                table[idx].erase(it);
                count--;
                return;
            }
            ++it;
        }
    }

    void info() {
        cout << "\n========== INFORMACION TABLA HASH ==========\n";
        cout << "Funcion hash: Modular (h(k) = k mod " << size << ")\n";
        cout << "Resolucion de colisiones: Encadenamiento\n";
        cout << "Tamaño de tabla: " << size << endl;
        cout << "Vehiculos registrados: " << count << endl;
        cout << "Factor de carga: " << fixed << setprecision(3) 
             << (float)count / size << endl;
        
        int buckets_usados = 0;
        int max_cadena = 0;
        
        for (const auto& bucket : table) {
            if (!bucket.empty()) {
                buckets_usados++;
                max_cadena = max(max_cadena, static_cast<int>(bucket.size()));
            }
        }
        
        cout << "Buckets ocupados: " << buckets_usados << " / " << size 
             << " (" << fixed << setprecision(1) 
             << (100.0 * buckets_usados / size) << "%)\n";
        cout << "Cadena mas larga: " << max_cadena << " elementos\n";
    }

    void cargar(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Advertencia: No se pudo abrir " << filename << endl;
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
            
            if (type == "V") {
                Vehiculo v;
                getline(ss, token, ';'); v.id = token;
                getline(ss, token, ';'); v.tipo = token;
                getline(ss, token, ';'); v.placa = token;
                getline(ss, token, ';'); v.origen = stoi(token);
                getline(ss, token, ';'); v.destino = stoi(token);
                getline(ss, token, ';'); v.horaEntrada = token;
                insert(v.id, v);
            }
        }
        file.close();
        cout << "Vehiculos cargados desde " << filename << endl;
    }

    void guardar(const string& filename) {
        ofstream file(filename);
        file << "# idVehiculo;tipo;placa;origen;destino;horaEntrada\n";
        for (const auto& lst : table) {
            for (const auto& p : lst) {
                const Vehiculo& v = p.second;
                file << "V;" << v.id << ";" << v.tipo << ";" << v.placa << ";" 
                     << v.origen << ";" << v.destino << ";" << v.horaEntrada << "\n";
            }
        }
        file.close();
        cout << "Vehiculos guardados en " << filename << endl;
    }
};

int main() {
    Grafo g;
    HashTable ht;
    int opcion;
    
    cout << "\n================================================\n";
    cout << "   SIMULADOR DE TRAFICO URBANO - VERSION 1.0\n";
    cout << "================================================\n";
    
    do {
        cout << "\n========== MENU PRINCIPAL ==========\n";
        cout << " 1) CargarRed       2) GuardarRed      3) AltaNodo\n";
        cout << " 4) AltaArista      5) BajaNodo        6) BajaArista\n";
        cout << " 7) MostrarListaAdj 8) MostrarMatriz   9) Dijkstra\n";
        cout << "10) BFS            11) DFS            12) AltaVeh\n";
        cout << "13) BuscarVeh      14) BajaVeh        15) InfoHash\n";
        cout << "16) MostrarASCII   17) Componentes    18) CuellosBotella\n";
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
                ht.cargar("vehiculos.csv");
                break;
            }
            case 2: {
                string file; 
                cout << "Archivo de red: "; 
                getline(cin, file);
                g.guardar(file);
                ht.guardar("vehiculos.csv");
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
            case 12: {
                Vehiculo v;
                cout << "ID del vehiculo: "; 
                cin >> v.id;
                cout << "Tipo (Particular/Transporte/Emergencia): "; 
                cin >> v.tipo;
                cout << "Placa: "; 
                cin >> v.placa;
                cout << "Origen (ID nodo): "; 
                cin >> v.origen;
                cout << "Destino (ID nodo): "; 
                cin >> v.destino;
                cin.ignore();
                cout << "Hora de entrada (HH:MM): "; 
                getline(cin, v.horaEntrada);
                
                ht.insert(v.id, v);
                cout << "Vehiculo registrado exitosamente.\n";
                break;
            }
            case 13: {
                string id; 
                cout << "ID del vehiculo: "; 
                cin >> id;
                
                auto* v = ht.search(id);
                if (v) {
                    cout << "\n========== INFORMACION DEL VEHICULO ==========\n";
                    cout << "ID: " << v->id << endl;
                    cout << "Tipo: " << v->tipo << endl;
                    cout << "Placa: " << v->placa << endl;
                    cout << "Origen: " << v->origen << endl;
                    cout << "Destino: " << v->destino << endl;
                    cout << "Hora de entrada: " << v->horaEntrada << endl;
                } else {
                    cout << "Vehiculo no encontrado.\n";
                }
                break;
            }
            case 14: {
                string id; 
                cout << "ID del vehiculo: "; 
                cin >> id;
                ht.remove(id);
                cout << "Vehiculo eliminado exitosamente.\n";
                break;
            }
            case 15: 
                ht.info(); 
                break;
            case 16: 
                g.mostrarASCII(); 
                break;
            case 17: 
                g.componentesConexas(); 
                break;
            case 18:
                g.detectarCuellos();
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





/*
Solo lo escribire esta vez, con la poca esperanza de que alguien lo lea.
Son las 3 de la mañana y llevo 5 horas programando este simulador
Necesite mucha investigacion de parte de mi equipo para poder entenderlo
La Ia no ha sido de gran ayuda, Mas que nada porque si le pedia ayuda me daba algo completamente distinto
a lo que yo queria, y al final tuve que adaptarlo.
Ni chat, ni Gemini, nisiquiera cualquier IA me ayudo a pensar en la logica del programa.
Lo juro por dios, Como odie la clase de Matematicas Discretas.
Al final, despues de tanto tiempo, consegui hacer un simulador decente.
Que probablemente se perdera junto los demas codigos que alguna vez programare. 
Como sea. aqui una foto mia en ascii porque, why not?
solo haz pequeño el texto para verlo bien








xxxXXXx:..::::::::::::::::::::::::::::.................................................................................................................................................................:+xxxxxxx
XxxxXXx;..:::::::::::::::::::::::::::::::..............................................................................................................................................................:+xxxxxxx
XXxxXXX;..::::::::::::::::::::::::::::.................................................................................................................................................................:+xxxxxxx
XxxxxXX;..::::::::::::::::::.:..::::::.................................................................................................................................................................:++xxxxxx
xxxxXXX+..:::::::::::::::::..::.:::::..................................................................................................................................................................:+xxxxxxx
XxxxXXX+...:::::..........:.:::........................................................................................................................................................................:+xxxxxxx
XxxXxXXx:..:..::..............::.:.....................................................................................................................................................................:+xxxxxxx
xXXXXXXx:..:::::.....::..:....::..:....................................................................................................................................................................:+xxxxxxx
xXXxxXXx:...::::...:::::::.............................................................................................................................................................................:+xxxxxxx
XXxxxXXx;..:::.....::::................................................................................................................................................................................:+xxxxxxx
xxxxxxXX;..::::::::::..................................................................................................................................................................................:+xxxxxxx
XXXxxXXX;...::::::.....................................................................................................................................................................................:+xxxxxxx
xxxxXXXX+...::::::.....::::................................................................................................................................................:::;;;;;;;;;;;;;::::::::::;;;+xxxxxxx
XxxXxxXX+:..:::....::.....................................................................................................................................................::::::;;;;;;;;;;;::::::::::;;;+xxxxxxx
XXxxxXXXx:.............::...................................................................................................................................................:::.::::;;;;;;;::::;:::::;;;+xxxxxxx
XXxXXXXXx:..::::.......................................................................................................................:::......................................::..:...;...:...::::::;+xxxxxxxx
XXxXXXXXx;..::::::.............................................................................................::::::::::::::..::::::;;;;;::..:::...........................................::..:...:..;+xxxxxxx
XxxXXXXXX;....::::........................................................................::::::::::::;;;;;;;;;;;;;;:;;;;;;;;;+++xxxxXXXXXxxXXXXx:..............................:...........::.::...:..;+xxxxxxx
XxXXXXXXX;..::::::::::::::::::...::::............::::::::::..:::................:::::::::::;;;;;;+++++xxxxXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;::........................ ..........::::::::::..::.:;xxxxxxxx
XXXXxXxXX+::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;;;;;;;;++xxxxXXXXX$$$$$$$$$$$$$$$$$$$XXXXXXXXXXXXXXXXxxxxxxxxx+++:::::xx;;:.......................  ..........:..:::::.:::::::;+xxxxxxx
XXXXXXXXX+::::::::::::::::::;;:;;;;;;;;:;;;;;;;;;;+++xxxx+xxxxXXXXXXXXX$$$$$$$$$$$$$$$$$$$$$$$$$$X$XXXXXXXXXxxxxxxxx+++++++++++++++;;;;;;;;;;::xx;;:...................................:..:::::::::::::;xxxxxxxx
XXXXXXXXXx::;;;;+++++xxxxxxxxxxXXXXXXXXXXXXXXX$X$XX$$$$$$$$$$$$$$$$X$$$XXX$X$XXXXXXXXXXXXXXxxxxxxx+++++++++;++++++++++++++++++++++++;;;;;;;;;;:xx+;:.......................................::::::::::;;+xxxxxxxx
XXxXXXXXXX;:xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX$$$$$$$$$$$$$XXXXXXXXxxxxxxxxxx++++++++++++++++++++++++++++++++++++++++++++++++++++;;;;;;;;::xx+;:....................:.::....::..........:...:::::::+xxxxxxxx
XxxxXXXXXX;:+XXXXXXXXXXXXXXXX$$$$$$$$$$$$$XXXXXXXXxxxxxxxxxxxxxxxx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;;;;;;;:::xx+;:....................:.::::::::::.:::::::::::;;;::::;+xxxxxxx
XXXXXXXXXX;:+X$$X$$XXXXXXXXXxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++xx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;+++;;;;;;;:::+x+::....................:.:::::::::::::::::::::::::::::;+xxxxxxx
XXXXXXXXXX+:;+xxxxx+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++x++++++++++++;;;++++++++++++++++++++++++++++++++++++++++++++++++++++++;++;+;;;;;;:::xX+::....................:::::::;;;:;:::::.:;;;;;;;;;;;++xxxxxxxx
XXXXXXXXXX+::+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+++++xx++++++++++++;;;;++++++++++++++++++++++++++++++++++;+++++++++++++++++++;++;;;;;;;;;::xX+::....................::;;;;:::;;;;;;;:.::;;;;;+++++++xxxxxxxx
XXXXXXXXXXx::+xxxxx++xxxxxxxxxxxxxxxxxxxx+xxxxxxx+++++++xxx+++++++++++;;;;;;++++++++++++++++++++++++++++++++;:;+++++++++++++++++++++;;;;;;;;;::xx+::....................::;;;;;;;:::;;;;::;::::;;+++++++xxxxxxxx
XXXXXXXXXXx::+xxxxxxxxxxxxxxxxxxxxxxxxxxxxx++++++++++++++xx+++++++++++++;;;;+++++++++++++++++++++++++++++++++;+++++++++++++++++++++++;;;;;;;;::+++;:....................::++++;;;;;;;;;;::;;;;;::;;+++++xxxxxxxx
XXXXXXXXX$X;:+xxxxxxxxxxxxxxxxxxxxxxxx+x+xxxxx++++++++++++xx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;;;;;;;;;;;;;;;;::xx+;:....................::+++++++++;;;;;::;+++++;;;;;+++xxxxxxxx
XXXXXXXXXXX;:;+xxxxxxxxxxxxxxxxxxxxxx+++++xxx++++++++++++++x++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++x++++;;;:::;;;;;;;;;;::+x+;:.....................;++++++++++++++:::::::::::::::;xxxxxxxx
XXXXXXXXXXX;:;+xxxxxxxxxxxxxxxxxxxxxxxx++++xx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++xxxxxxxxx++++;;;;;;;;;;;;;;;;::+x+;:.....................;+++++++++++++++++++;;;;;;;;;;+xxxxxxxx
XXXXXXXXXXX+:;+xxxxxxxxxxxxxxxxxxxxxxxx++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++xxxxxxxxxxxxx++++;;;;;+;;;;;;+++;;;:+x+;:.....................:+++++++++++++;++++++++++++++++xxxxxxxx
XXXXXXXXXXX+:;+xxxxxxxxxxxxxxxxxxxxxxx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++xxxxx+xxxxxxxxx++++++;;;;;;;;;+++++++;;:;+;;:....................:::::::::::;;;;;+++xxx++++++++++xxxxxxxx
XXXXXXXXXX$x:;+xxxxxxxxxxxxxxxxxxxxxxxxx++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++xxxxxx++xxxxxxxxxxxx+++;;;;;;;;;;;;;;;;+++;+++;:............................:;;:..:;;;;;;;;;;;;;;;;+xxxxxxxx
XXXXXXXXXX$x:;+xxxxxxxxxxxxxxxxxxxxxxxxx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++xxxxx+xx+++++++xxxxx+++++++++++;;;;;;;;;;+++++x++;;............................::..::::::.::......:+xxxxxxxxxxx
XXXXXXXXXX$X::+xxxxxxxxxxxxxxxxxxxxxxxx+++++++++++++++++++++++++++++++x++xx++++++++++++++++++++xxxxx++;+;;++++x+xxxxxx+;+x++++++;;;;;;;;;++xxxxxx++;::..........................:;:::.............:+xxxxxxxxxxxX
XXXXXXXXXX$X;:+xxxxxxxxxxxxxxxxxxxxxxxxx+++++++++++++++++++++++++++++++++++++++++++++++++++++xxxxxxx++++;;+++xxxxxxxx+;xxxxxxxx+++++++++++xxxXXXxx++;;:.........................:;;;..............++xxxxxxxxxxxx
XXXXXXXXXX$X;:;+xxxxxxx+xxxxxxxxxxxxxxxxx+x+++++++++++++++++++++++xxxxxxXXXXXXXXxxxxxx+++++++xxXXxXxxxx+++++xx+xxxxxx+xxxxxxxxxxxxxx++xxxxxxxxxxxx+;;;;::......................::.::;;;:.......::;+++++xxxxxxxxx
XXXXXXXXXXXX+:;xxxxxxxx+xxxxx+xxxxxxxxxxx+xxxxx++++++++++++xxxXXXXXXXXXXXXXXXXXXXXXXXXXXXXxxxxxXXXXXXXXxxxxxxxxxxxxx+xxxxXxxXxXxxxxxxXxXxXxxxxxxxx+;;;;+;:....................::......:;;;;;::;+;;::;:;+xxxxxxxx
XXXXXXXXXXXXx:;+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+x++++xxxXXXXXXxxxxxxxxxxxxxxxxxxxxxxxxxxXx+xXXXXXXXXXXXXXXxxxxxxxxxxxxXXXXxXxXxxxxxxXXxxXxxxxXXxx+;;;+++;:..................::............::::::::::::;xxxxxxxx
XXXXXXXXXXXXx::+xxxxxx+++xxxx+xxxxxxxxxxxxxxxx++++xxXXXXXxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxXXXXXXXXXXXXXXXxXxxxxxx+xxxXXxxXXXXXxxxxxXx++xxxxxXXXXx++;;+x+;:................::............:...:::......:xxxxxxxx
XXXXXXXXXXXXX::+xxxxxxx++xxxxxxxxxxxxxxxxx+++++xXXXXXxxxxxxxxxxxxxx++++++++++x++++xxx++xxxXXXXXXXXxXXXXXXXXXxxxXxxx+XXXXXxXXXXXXXxxxxXx;;+xxxxxxXXXXxx+++++;:...............:.................:;:..::::;xxxxxxxx
XXXXXXXXXXX$X;:+xxxxxxxxxxxxxxxxxxxxxxxxx++xxXXXXxxxxxxxxxxx++x+x+++++++++++++++++++++xxxXXXXX$XXXXXXXXXXXXXxxXXxxx+xXXXXXXXXXXXXxxxxX+;;;++;;;++xxXXXXxx+++;::............::............:....:::::::::;xxxxxxxx
XXXXXXXXXXXXX;:+xxxxxxxxxxxxxxxxxxxxxx++xxXXXXXxxxxxxxxx+++++++++++++;::..;++++++++++xxxXXXXXXXXXXX$$XXXXXXXXXXXXxxxxXXXXXXXXXXXXXxxxx+;;;;++;;;++xxXXXXx+++++;:...........::....:;::::.::..::::::;;;::;xxxxxxxx
XXXXXXXXXXXX$;:+xxxxxxxxxxxxxxxxxxxx++xXXXXxxxxxxxx+++xx+++++++++++;::::::.:::::::::;+XXXX$$$$XXXxXXXXXXXXXXXXXXXxxxxxXXXXXXXXXXXxxxxx+;;;;;;+;;+++xXXXXXxxx+;+;::.........:::...;;;;;;;:::::...::;++;:;xxxxxxxx
XXXXXXXXXXXX$+:;xxxxxxxxxxxxxxxxx+xXXX$Xxx+++++++;;;;;;:::::::::::::;++;::::......::+xXXX$$$$X$XXxXXXXX$XXXXXXXXXXXXXxXXXXXxXXXXXx+xxx;;;;;;;;;;;+++xXXXXXXXx+;;;:::.......::....+x++x;;;:::::...:;;:::;xxxxxxxX
XXXXXXXXXXXX$x:;xxxxxxxxxxx++xxxXXXXXXxxxx+;::::::::::::::::;;;::;;;++xx++;++++++++xXXXXXXXXXX$$XXXXX$$$XXXX$XXXXXXXXXXXXxxXXXXXXx+xxx;;;::::;;;;;++xXX$XXXXXx+;;;:::......::...:++++;xx+:::::..::;::::;xxxxxxxx
XXXXXXXXXXXX$x:;xxxxx;xxxxxXXXXXXXXXXxxxxx+::++++++++++++++++;::;++++xxx++++++++++xXXXXXXXXX$X$XXXX$XXXXXX$XXXXXXXXXXXXXxxXXXXXXXxx+x+;;;:::::;;;;++xXXXXX$XXx+;:;::.:......::..:+x+;;;;:........::::::;xxxxxxxx
XXXXXXXXXXXX$X::+xxxxxxXXXXXXXXXxxxxxxxxxx+;:++++++++++++++;::;++++xxx++++++++++xXXXXXXXXXXXXXXX$$$$$$$$$$$$XxXXXXXXXXXXXxXXXXXXXxx+x+;;;:::::;;;++xxXX$$$XXXX+;::::........::...:;;;;::::....::....:::;xxxxxxxx
XXXXXXXXXXXX$X;:+xxxXxxXXXXxxxxxxxxxxxxxx++;:+++++++++++;:::;+++++xx++++++++++;+XXXXXXXXXXXXX$X$$$$$$$$$$$XXXXXXXXXXXXXXXXXXXXXXXxxxx++;;;::::;;;++xxXX$$$$$XXx+:::::........::.::::.........;;;:::::::+xxxxxxxx
XXXXXXXXXXXXXX;:+xxxXxxXXxxxxxxxxxxxxxxxx++;.+++++++++;:::;++++++++;:...;;:...;xXXXXXX$$$X$$$XXX$XX$$$$$$XXXXXXXXXXXxxXXXXXXXXXXxxxxx+++;;;;;;;;+++xXX$$$$$$XXx+;::::..:.....:::::::..................:;xxxxxxxx
XXXXXXXXXXXX$$+:+xxxxxxXXXxxxxxxxxxxxxxxx++;.++++++;:.:;;+++++++++:.....:xx+::;xXXXXX$$$$$$$$$$$$$$$$$XXxXXXXXXXXXXXXXXXXXXXXXxxxxx+++++++++++++++xxXX$$$$$$XXx+;::.::.:.....::::::.........:..........:xxxxxXxx
XXXXXXXXXXXXX$x:;xxxxxxXXXxxx+++xxxxxx++:.....;++:..::;+++xxx+++:........:;xXXXXXXXX$$$$$$$$$$$$$$$$XXXXXXXXXXXXXXXXXXXXXXXXXXXXxx++;++++x++++++++xxXX$$$$$XXXxx;::.:.::..:;++++;;:........++;.........:xxxxxXxx
XXXXXXXXXXXXX$x:+xxxxxxXx+;::....:+++;::..........::;;+++x++++;:...........:x+;:::X$X$$$$$$$$$$$$XXXXXXXXXXXXXXXXXXXxxXXXXXXxxxxx++;+xxxxxxxxxx++xxXX$$$$$XXXxXx+;:..:::..;;;;;;;:::...................;xxxXxXxX
XXXXXXXXXXXXX$X:;xxxx+;::;;;;::....:::::........::;;+++++++++:..............:::::.:X$$$$$$$$$$$$XXXXXXXXXXXxX$XXXXXXXXXXXXXXxxxxx+++xxxxXXXXXXXXXXXXX$$$$$$XXXxx+;:..:.:.......::;:::::::.............:;xxxxxxxx
XXXXXXXXXXXXX$X;:x+;;;;;;++;;;;::...:;::.......::;;+++++++;;;++++++++++;::::;;;;;;;$$$$$$$$$$$$XXXXXX+xXXXXXXXXXXXXXXXXXXXXXXxxXx++xxXXXxxxxxxXXXXXXX$$$$$$XXXxxx;:..:::........:::::::..::....::::::::;xxxXXXxX
XXXXXXXXXXXXX$X;:;;;++xxx+xx++;;;::::::::......;;+++++++;;:;++++++++;:::;;+++xx+++XXXX$$$$$$$XXxxxxXx;xXXXxXXXXXXXXXXXXXXXXXXXXx;::;x$xxxxxxxxxxxxXX$$$$$$XXXXXxx+:..:.::......::;;;;::::::::::::::::::;xxxxxXXX
XXXXXXXXXXXXX$$+:;+++++xXXXXXXx++++++;:::::...:+++x++++;+++++++++;:::;;+xxxxxxxxxXX$$$$$$$$&$xx+++++xxxxxXxxxxxxXXXXXXXXXXXXXXx++;;;xXXXxxxxxxxxxXXXX$$$$$$XXXXXx+:..::::......:;;::::;;;;:::::::;;;;::+xxxXxxXX
XXXXXXXXXXXXX$$+:++xxxxxxxxXXXxxxxxxxx+;;;::::+++++++;+++++++;::::;+++++xxxxxxxX$$$X$XXXX$&&$x++++++++xxxxxxxxxxxXxxxxxxxxxXXxx+;;::xXX$$XXxxxxxxxxX$$$$$$$XXXXXx;:..::::::::::;;;;;;;;;;;;;;+;;;;;;;;;+xxxxXXXX
XXXXXXXXXXXXX$$x:+xxxxxxXxxxXXXXxxxx+++++;;;:++++++++++++;;;:::;+++xxxxxxxx++++XX$$$$X+xxX$&X+++;;;;;+++xxxxxxxxxxxxxxxxxXXXxx++;::;XXXXX$$$XXXxxxx$$$$XX$$XXXXXx;;..:::::..:::;;:::::::;;;;;;;;;;;;;;;+xxxXXXXX
XXXXXXXXXXXXX$$x:;xx;;;xXXXXXXXXXxx+;;;;;;;;;+++++++;;;;;++;;;++xxxxxxxx+:;::::+XXX$$$x++xX&X+;;;;;;;;+++xxxxxxxxxxxxxxxxXxx++;;::::xXxxXXXXX$XXxxX$$$$$$$$XXXXXx+;:....::....:;;:::;;;;:;;;;;;;;;;;;;:;xxXxXXXX
XXXXXXXXXXXXX$$X:;+xxxxXXXXXXXXXXxx;;;:::;;::..:;+++++++++++++++xxxxxx;:.:.:;.:xXX$$$$&$++x$x;;;;;;;;;;+++++xxxxxxxxxxxxxxx++;::::::+xx+xxxxxxxxxx$$$$$$$X$$XXXx++;:.:::::....:;;::::::::::;;;;;;;;;;;:;xxXXXXXX
XXXXXXXXXXXXXX$X;;xxxxxxxxXXXXXXXXx+;;::;++x++;;;+++++++xxxxxxxxxxx++++++++++++xXX$$$$$&$$+++;;;;;;;;;;;+++++++xxxxxxxxxx+++;;::::::;xx+++xxxxxxxXX$$$XXX++XXXx+;;::.::..:....:;:::::::::;;;;;;;;;;;;:.;xxXXXXXX
XXXXXXXXXXXXXX$X;:+xxxxxxxxxxxxxxxxxxxx+xxxxxx+;;++++++xxxxxxxxx+++++++++++++++xXX$$$$$$&$+;;;;;;;;;;;;;++++++++++++xxx++++++++;;;::;++x+++++xxxx$XXXXxxx:+XXX+;:::..:..:.....:;:;::::;;;;;;;;;;;;;;:..:xxXXXXXX
XXXXXXXXXXXXX$$$+:+xxxxxxxxxxxxxXxxxxxxxXxXXxxx;;+xx+++xxxxx+;;;;+;;;;;:::::::::++X$$$&&&$;;;;;;;;;;;;;;+++++++++++xxxx+xxXxx++++++;++++xx++++++xXxXx+;::;XXXx+;:;:..:........:;::::::;;;;;;;;;;;;;:...:xxXXXXXX
XXXXXXXXXXXXXX$$x:+xxxxxxxxxxxxxxxxxxxxxxxXXxx+;;x++++xxx+;::::::;;;;;;;+++++xxxXX$$$$&&&$;;;;;;;;;;;;;;++++++++++++xxxxxxxxxxxxxxx+++++++xxxx++xXx+;:::;xXxx;::;:..:.........::::::;;;;;;;;;;;;;;:....:xxXXXXXX
XXXXXXXXXXXXXX$$x:;xxxxxxxxxxxxxxxxxxxxxxXxxxx+;:+++xxxxxxxxxxxxxxxxxxxxxxxxxxxXX$$$$$&&&X;;;;;;;;;;;;;+++++++++++++++xxxxxxxxxxXx++++++++++++++XX++::::;+X++:::::::..........:::::::::;;;;;;;;::......:xxXXXXXX
XXXXXXXXXXXXXX$$X:;xxxxxxxxxxxxxxxxxxxxxxXxxxx+;:xxxxxxxxxxxxxxxx+xxxxxxxxxxxxxxXXX$$$&&&x;;;;;;;;;;;;++++++++++++++++x++++xxxxxx+++;+++++++++++XX++::::;x++:::::::..........:;:::::;;;;;;;;::.........:xxXXXXXX
XXXXXXXXXXXXXX$$X:;+xxxxxxxxxxxx+;;;++;xxXXXXx+;;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxXX$$$&$$+;;;;;;;;;;;;+++++++++++++++++xxxxxxxxxx+++++++++++++xxXX++;::+x+;::::..............:;;;;;;:;;;;;:............:xxXXXXXX
XXXXXXXXXXXXXX$$$;:+xxxx+xxxx+::...:::;xxXXXXXxx+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx$$&&$$;;;;;;;;;;;;+++++++xxxXXxxxxxXxxxxxxxx++++++++++++++xxxXXxxx++;:::..................;;;;;;;;;;;;..............:xxXXXXXX
XXXXXXXXXXXXXXX$$;:+xxxxx+;+;::...::::+xxxXXXXx+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxX$&$$x;;;;;;;;;;;;+++++++++xxxxxxXXXXXxxxxxxx++++++++++++++++XXxx+::::...................:;;;;;;;;;;:...............;xXXXXXXX
XXXXXXXXXXXXXXX$$+:+xxxxxx++;:::::;:;++xxxXXxxx+xxxxxxxxxxxxxxxxx+xxxxxxxxxxxxxxxxxxxX$$+;;;;+++;;;++++++++++++++xxxxxxxXXXXXXXXxxx+x++++;;xxxx+XX++;:......................:;;;;;;;;;:................;xxXXXXXX
XXXXXXXXXXXXXXX$$x:;x+++++++;;;;;+++xxxxxXXXxx+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+;;;;;+++++;++++++++++++++++xxxxxxxxxxxxxxxxx++++;++xxx++XX++;:......................:;;;;;;;;:.................;xXXXXXXX
XXXXXXXXXXXXXXX$$x:;+++++++++++++x+xXxxxxxxxxx+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+;;;;;+++++++++++++++++x++++++++++++++++++;+;++++;:;++xx++XX++;:......................:;;;;;;;...................;xXXXXXXX
XXXXXXXXXXXXXXX$$X::+xxxxxxxxxxxxxxxXxxxxxxxx++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;;;;;;+++++xxx++++xX$$$$$$$XX$XXXX$X$$$XXXXXXXXXXXXXx+++x++XXx+;:.......................::;;;::...................;xXXXXXXX
XXXXXXXXXXXXXXX$$X::xxxxxxxxxxxxxxxxXxxxXXxxx++xxxxxxxxxxxx+++xxxxxxxxxxxxxxxxxxxxx;;;;;;;+++++xxxxxx+X$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$XXXXx++x+XXx++:.................................................;xXXXXXXX
XXXXXXXXXXXXXXX$$$;:xxxxxxxxxXxxxxXxXXXXXXxxx++xxxxxxxxxxxx+:::+xxxxxxxxxxxxxxxx+;;;;;;;;;+++++xxxxxxxX$$$$$$&$$$$$$$$$$$$$$$$$$$$$$$$$$$XXX++++xXx++:.................................................;xXXXXXXX
XXXXXXXXXXXXXXXX$$+;xxXXXxXXXXXXXXXXXXxxXXxxxxxxxxxxxxxxxxx+:::xxxxxxxxxxxxxx++;;++;;;;;;++++++xxxxxxxX$&&&&&$$$$$$$$$$$$$$$$$$$$$&$$$$$$$XX++++XXxx+:.................................................;xXXXXXXX
XXXXXXXXXXXXXXXX$$+;xxXXxXXXXXXxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+;;;;+++;;;+++;;;;;+++++++xxxxxxxxX$&&$$$$$&$$$$$$$$$$$$$$$$$&$$$$$$$XXX++x+XXx++:.................................................;xXXXXXXX
XXXXXXXXXXXXXXXX$$x;xxxxxxxxxxxxxxxxxxxxxxxxxxx+xxxxxxxxxxxxxxxx++;;;;;;;;;++++++;;;;;++++++++xxxxxxxxX$&&$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$XX++++xXxx+:.................................................;xXXXXXXX
XXXXXXXXXXX$XX$X$$x;xxxxxXXXXXxxxxxxxxxxxxxx++xxxxxxxxx++x+++;;;;;;;;;;;;;;+++++++;+++++++++++xxxxxxxxX$&&&$$$$&&&$$$$$$$$$$$$$$$$$$$$$$$$XX+x++XXxx+:.................................................;xXXXXXXX
XXXXXXXXXX$$XX$X$$X;xxxxXXXXXXxxxxxxxxxxxxx+++xx++;;;;;;;:::;;;;;;;;;;++++++++++++++++++++++xxxxxxxxxxx$&&&$$$$&&&$$$$$$$$$$$$$$$$$$$$$$$$$X+xxxXXxx+:.................................................;xXXXXXXX
XXXXXXXXXXX$XX$XX$X;xxxxXXXXXx+;:.....................::::::;;;;;;;;+++++;;;++++++++++++++xxxxxxxxxxxxx$&&&$$$$$$$&$$$$$$$$$$$$$$$$$$$$$$$$X+xxxx$xx+;.................................................;xXXXXXXX
XXXXXX$XXXXXXX$$X$$;+xxxxXXX;:::;;::::................::::;;;;;;;;;;;;+++++;+;++++++++++++xxxxxxxxxxxxx$&&&&$$$$&&&&$$$$$$$$$$$$$$$$$$$$$$$X+xx+x$xxx;.................................................;xXXXXXXX
XXXXXXXXXXX$XX$XX$$++xxxxXx;:::...:::::::..............::::;;;;;;;;;;+;;++++++++++++++++++xxxxxxxxxxxxx$&&&&&$$&&&&&&&$$$$$xxxX$$$$$$$$$$$$X+xxxx$xxx;:::..............................................;xXXXXXXX
XXXXXXXXXXXXX$$$$$$+;xxx+::::;;;:::...:::::...........:::;;;;;;;;;;;;;+;;;;+++;++++++++++++xxxxxxxxxxxx$&&&&&&&&&&&&&&&$$$$$$$$$$$$$$$$X$$$X;.:;xXxxx;:::..............................................;xXXXXXXX
XXXXXX$$XXX$XXXXX$$x:+;..::;;;;;;;;;::..::::.........:::::;;;;;;;;;;;++++++;;+++++++++++++++xxxxxxxxxxx$&&&&&&&&&&&&&&&&$$$$$$$$$$$$$$$$$$$X;....:;++;::...............................................;xXXXXXXX
XXXXXXX$XXXX$$XXX$$x:;::.........::;;;::.::::....:...:::::;;;;;;;;;;++++;++;;;;+++++++++++++++++xxxxx+x$&&&&&&$&&&&&&&$&$&$$$$$$$$$$$$$$$$$x;...........::.::.....::...................................;xXXXXXXX
XXXXXXX$XXXXXXXX$$$X:;:::::::....::..:;;:.::::..::::::::::;;;;;;;+;;;;++;+++;;;++++++++++++++++++++xx+x$&&&&&&&&$&&&&&$&&&$$$$$$$$$$$$$$$$$X;.............:::.....:::::::::............::::::.........:;xXXXXXXX
XXXXXXX$XXXX$XXX$$$X;;;;:.:;;;::...:::::::.:::...::::::::;;;;+;;;+++;;;++;+;;++;++++++++++++++++++++x+x$&&&&&&&&&&&&&&&&$&$$$$$$$$$$$$$$$$$X;...............::....::::::::::::::..:::.::::::::....:::::;xXXXXXXX
XXXXXXX$XXXXXXX$X$$$;;;;:..:;;++;:....:::::::::.:::::::::;;;;+++++++++;;;;;+++++;;+;++++++++++++++++++x$&&&&&&$$$$$$$$&&&&$$$$$$$$$$$$$$$$$X:...............:::.:::::::::::::::::::::::::::::::.:::::::;xXXXXXXX
XXXXXXX$XX$$$X$$X$$$;;;;:..::::;++;:..::;:::::::::::::::;;;;;+++++++++++;++;;;++++;+++++++x+++++++++++x$&&&&&$$$XxX$$$$&&&$$$$$$$$$$$$$$$$$x:................::::::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXX$$XX$$XX$$X$$+;;;:.:::::::+++;:..:;::;::::::::::;;;;;;++++++++++++;;+;+++++++;+;;+++++xx+++++++x$&&&&$XX+++++xX$$&$&&&$$$$$$$$$$$$$$x;:.....:......:...:::::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXX$$$XX$XX$$X$$+:+;::::::::::++++;:.:::;:::::::::::;;;;++++++++++++++;;;;;+++++++++++++++++++++x+x$&&&&&&Xxx++++++x$$&&&&$$$$$$$$$$$$$xx;:....:......::..:::::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXXXXXX$XX$$$$$x;;;::::::::::;++x+;:..:+;:::::::::;;;;;;++++++++++++++;++++++++++++++++++++++++++x$&&&&&&&Xxxxxxx+++X$$$$$$$$$$$$$$$$$xx+::..:::...:.:....::::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXX$XXXXXXX$$$$X;;;;:.:;;:::::;+xx++::.:;::::::::;;;;;++++++++;++++++++++++++++++++++++++++++++++x$$&&&&&&&$xxxxxxx+++X$$$$$$$$$$$$$$$x;x:::..:::.:::::...::::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXX$$XX$XX$$X$$X;;;:::::;;:::::;x++x+:::;::::::;;;;;;;+++++++++++++++++++;+++++++++++;;;+++++++++x$&&&&&&&&&$Xxxxxx+++++X$$$$$$$$$$$$$X:;;::...:::..:::....:::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXX$$XX$XX$$$$$$;;;:::.::;;:::::+++x++;:;:::::;;;;;;;;+++++++++;++++++++++;;++++++++++;;;++++++++x$&&&&&&&&&&&$Xxxx+++++++X$$$$$$$$$$$X;:;:::..::::::::.....::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXX$$$X$$X$$XX$$;;;;::.::;;;:::::+xx+++;:;::::;;;;;;;++++++++++++++++++++++;;++++++++++;;;++++++++$&&&&&&&&&&&&&$xxxx+++++++$$$$$$$$$$X;:::::::::::::::..::.::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXXXX$X$$XX$$$$$+:::::..::;;;:::::;+x+++;;:::;;;;;+++++++++++++++++++++x+++++;+++++++++++;;;;+++++$&&&&&&&&&&&&&&$Xxxx+++++++x$$$$$$$$X;::::::::::::::...::..:::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXX$$$X$$$X$$X$$x::::::..:;;;;::::::+x++;+:::;;;;;++++++++++++++++++++xxxxx+++;;+++++++++;;;;;;+++X&&&&&&&&&&&&&&&&$xxxxx+xx+++X$$$$$$X;:+:::::::::::.::::...:::::::::::::::::::::::::::::::::::::;XXXXXXX$
XXXXXXX$X$$XX$XX$$X$$x::::::...::;;;;:::::;++++;;;;;;;;++;+++++++++++++++++++xxxxx++;;+++++++++;;;:;;++$&&$&&&&&&&&&&&&&&$Xxxxxx++;::;X$$$$$;:+;:::::::::::::::..::::::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXXXX$XXX$XX$X$$X;;::::....:;;;;;::::::;++;;;;;;;+++++++++++++x++++++++++xxxx+++;;;;;++++++;;++++++x$&&&&&&&&&&&&&&$&&Xxxxx+;;::::;X$$X;:+;:::::::::::::::..::.:::::::::::::::::::::::::::::::::::;xXXXXXXX
XXXXXXXXXX$XXX$X$$$$$X;;;;::::...::;+++;::::;;+;;;;;;;+++++++++++++xxx++++++++xxxxxxx+;;;;;;;++++++x+++++++xX$&&&&&&&&&&&&&&$Xxxx++;;::::+$X;:x;:::;;:::::::::.:::::.:::::::::::::::::::::::::::::::::::xXXXXXXX
$XXXXXX$XX$$XX$$X$$$$X;;::::.....:::;+++;;;::;+;;;;;;;+++++++++++++xxxx+++++++xxxxxxx++;;;+;;+++++xx++xxx++++++xX$&&&&&&&&&&$$Xxxxx++;:::::;::;;::::;::::::::..:::::..::::::::::::::::::::::::::::::::::xXXXXXXX
XXXXX$XXXXX$XX$$X$$$$$;:.:::::::..:::;++++;;;;+;;;;;;+++++++++++++++xxxxx+++++++xxxxxxx+;;;+;;+++++++xxxx++++++++++x$&&&&&&&&&&$xxxxx+;;::::...:::::::::::::...:::::..::::::::::::::::::::::::::::::::::xXXXXXXX
X$XX$XX$XX$$XX$$X$$$$$+:::::::.::...::;+++++;;+++;;++++xxx++++++++++xxxxx++++++++xxxxxxx++;+++;+++;:::+xx++++++++++++++X&&&&&&&&&xxxxxx+;:::...:::::::::::::::::::.....:::::::::::::::::::::::::::::::::+XXXXXXX
$$$XXXXXXXX$XXXXX$$$$$+:;;:::::.:....::;+++++;+;;+;+++++xxx++++++++++xxxxxx++++++++xxxxxxx+;;;;;;;;;++x$$Xxx+++xxxxx++++++xX$&&$xxxxx+xxx+;::::::::::::::..::::::::::::.::::::::::::::::::::::::::::::::+XXXXXXX
XXXX$X$$XXX$XXX$$$$$$$x:+;;:::::.:...:::;++;;;+++++++++xxxxx++++++++++xxxxxx+++++++++xxxxxx+++++;+++++xxX$&$Xxxxxxxxxx+++;;::;++++++++++xx++;;:::::::::::::::....:::::::::::::::::::::::::::::::::::::::xXXXXXXX
XX$$$$$X$X$XXX$$$$$$$$X:++;;;;:::.::..:::;;;;;+++++++++xxxxxx+++++++++xxxxxxxx++++++++xxxxxx++;++;+++++++xX$&&$Xxxxxxxxx++;::::::;;;+++++xxx++++;:::::::::::::::::;+xxxx+:::::::::::::::::::::::::::::::xXXXXXXX
$XX$$$X$$$$$$XX$$$$$$$X;+x+;;;;::::::..:::;;;;+++++++++xxxxxxx+++++++++xxxxxxx++++;;;++xxxxxx+;;;;;;++++++++xX$&&&$Xxxxxxx++;;:::::::;+++xxxxxx++;:::::::::::::::::;xx+:::::::::::::::::::::::::::::::::xXXXXXXX
$$X$$$$$XX$X$X$$$$$$$$$+++++;;;;;:::::.:::::;;+++++++++xxxxxxxx+++++++++xxxxxxx++;;;;;++xxxxxx+::;;:;++++++++++xX$&&&$Xxxxxxxx+;::::::::++xxxxx++;:::::::::.......::;:::::::::::::::::::::::::::::::::::xXXXXXXX
XXX$$$$$$$$X$XXX$$$$$$$+;++++;;;;;::::::.::::;+++++++++xxxxxxxxxx++++++++xxxxxxx++;;;;+++xxxxxx+::::;:;+++++++++++xxX$&&&XXxxxxxx+;::::::;++xxxxx++;::::::::......::::::::::::::::::::::::::::::::::::::xXXXXXXX
XXXX$$XXX$$XX$XX$$$$$$$x;+++++;;;;;::::::::::++++++++++++xxxxxxxxx+++++++xxxxxxxx+;;;;;++++xxxxx+;:::::+Xx+++xxxxx++++xxX$$XXxxxxxx++;;:::+++xxxx+++;;:::::::......:::::::::::::::::::::::::::::::::::::xXXXXXXX
X$X$$$$$$$XX$$$$$$$$$$$x;:;;+++;;;;;;;;:::::;++++x+x+++++xxxxxxxxx++++;+++xxxxxxxx++;;;+++++xxxxx+;::;;+$&$xxxxxxxxx++++++xxxxxxxxxxxxx+++++++xxxxx++;;;::::::.....:::::::::::::::::::::::::::::::::::::xXXXXXXX
XXXX$XXX$X$X$$X$$$$$$$$X;:::;;;;::;;;;;;:;;;;+++++x+++++++xxxxxxxxxx+++;;+++xxxxxxx+++;;;;;;+xxxxxx;:::;X$&&$XXxxxxxxxx+++++++xxxxxxxxxxxxx++++xxxxxx++;;::::::...::::::::::::::::::::::::::::::::::::::xXXXXXXX
XX$X$$$$$$X$$$$$$$$$$$$X;;:::;;;:::;;;;;;;;;++xxxxxx++++++xxxxxxxxxxx++;;;;+xxxxxxxxx+++;;;;:+xxxxxx+:::;+X$$$$$$XXXxxxxxxx++++xxxxxxXxxxxxxx++++xxxxxx+;;::::::::::::::::::::::::::::::::::::::::::::::xXXXXXXX
XXX$$$$$$$$$$$$$$$$$$$$$+;:::;;;;:;;;+;;+;;;++xxxx+++++++++xxxxxxxxxx++;;;;;+xxxxxxxxx++;;;:::+xxxxxx+;::;;;;++++;xXXXXxxxxxxxx++xxxxxxxxxxxxxxxxxxxxxxx+;;;::::::::::::::::::::::::::::::::::::::::::::xXXXXXXX
XXXXXX$$X$$X$$$$$$$$$$$$+;::::;;;;:;;;+;++;++xxxxxxxx++++++xxxxxxxxxxx++;;:;;+xxxxxxxxx++;;::::+xxxxxxx;:::;;;;+;;xxXXXXXxxxxxxxxx+xxxxxxxxxxxxxxxxxxxxxx++;;;;:::::::::::::::::::::::::::::::::::::::::xXXXXXXX
+++;;++XX$$X$$$$$$$$$$$$+;;;;;;;;;;;;++++;;xxxxxxxxxx+++++++xxxxxxxxxxxx+;:::;+xxxxxxxxxx+;;::::;xxxxxxx;::::;;;+;+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++;;;;::::::::::::::::::::::::::::::::::::::::+XXXXXXX
....:;+X$X$$X$$$$$$$$$$$x;;;;;;;+++x+++;;;+xxxxxxxxxx+++++++xxxxxxxxxxxx+;;:::;+xxxxxxxxxx+;;;::::+xxxxxx+::::;+++++xxxxxxxxxxxxxXXXXXxXxxxxxxxxxxxxxxxxx++x++;;;;;:::::::::::::::::::::::::::::::::::::+XXXXXXX
;.:;;;+X$$XXX$$$$$$$$$$$X+;;;;+++xx++;;;;+xxxxxxxxxxx++++++++xxxxxxxxxxxx+;:::::+xxxxxxxxxxx+;;::::+xxxxxx+;:::;++++++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++;;;;;;:::::::::::::::::::::::::::::::::::+XXXXXXX
;..;;;+X$X$$$$$$$$$$$$$$X+;+++xxxx+;;;+++xxxxxxxxxxxxxx++++++xxxxxxxxXxxxx+;:::::+xxxxxxxxxxx+;;::::+xxxxxx+;:::;++++++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++;;;;;;;:::::::::::::::::::::::::::::::::+XXXXXXX
;:::;;+X$$X$$X$$$$$$$$$$X+++xxxxxxxxxxxxxxxxxxxxxxxxxxx++++++++xxxxxXxxXxxx+;:::::+xxxxxxxxxxxx+;::::;xxxxxxx+::::;++++++xxxxxxxxxxxxxxxxxxxxxXxxxxxxxxxxxxxxxx++++;;;;;::::::::::::::::::::::::::::::::+xXXXXXX
;:.:;;+xXXX$$$$$$$$$$$$$$++xxxxxxxxxXxxxxxXxxxxxxxxxxxxx+++++++xxxxxxxxxxxxx+;:::::+xxxxxxxxxxxx+;;:::;+xxxxxx+;:::;;+++++++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+++++;;;:::::::::::::::::::::::::::::::+xXXXXXX
;:.:;;;xXX$XX$$$$$$$$$$$$++xxxxxXXxxxxxXxxxxxxxxxxxxxxxx+++++++xxxxxxxxxxxxxx+;:::::;+xxxxxxxxxxxx+;::::+xxxxxx+;::::;;+++++++++xxXXXXXxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++;;;::::::::::::::::::::::::::::::+XXXXXXX
;:.:;;;+X$XXXX$$$XX$$$$$$++xXXXXxxxxxxxxxxxxxxxxxxxxxxxx+++++++++xxxxxxxxxxxxx+;:::.:;+xxxxxxxxxxxx+;::::;+xxxxxx+;:::::;;;;;;+;;;+xXXXXXXXXxxxxxxxxxxxxxxxxxxxxxxxxx++++;;:::::::::::::::::::::::::::::+XXXXXXX
;:::;;;+XXXXXXXXXXXXXX$$$x+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+++++++++xxxxxxxxxxxxx+;::::::++xxxxxxxxxxxx+;::::+xxxxxx+;::::::;;;;;;;;;;;;xxXXXxxxXXXXXxxxxxxxxxxxxxxxxxxxx+++;;::::::::::::::::::::::::::::+XXXXXXX
;;:::;;+XXXXXXXXXXXXXX$$$x+x+++xxxxxxxxxxxxxxxxxxxxxxxxxxx+++++;;;+xxxxxxxxxxxxx+;;:::::;+xxxxxxxxxxxx+;;:::;xxxxxxx+;::::::::::::::::::;++xxxxxXXXXxxxxxxxxxxxxxxxxxxxx++;;;;::::::::::::::::::::::::::+XXXXXXX
+;:::;;+XXx++;:;xXXX$$$$$x+xx++++xxxxxxxxxxxxxxxxxxxxxxxxx+++++;;;;++++xxxxxxxxxx++;::::::++xxxxxxxxxxxx+;::::+xxxxxx+;::::::::::::::::::::;;;;;;++xxxxxxxxxxxxxxxxxxxxx++++;;;;::;+++xxxxxxxxxxxxxXXXXx+XXX$XXX
+;:::;;;+++;:::;+XX$$$$$$X+xX+++++xxxxxxxxxxxxxxxxxxxxxxxx+++++;;;;;+++++xxxxxxxxxx+;::::::;+xxxxxxxxxxxxx+;:::;xxxxxx++;:::::::::::::::::;:;;;;;;;+xxxxxxxxxxxxxxxxxxxxxxxx+++;;;;xXXXXXXXXXX$$$$$$&$&$xXXXXXXX
+;:::;;;+::.:;+xXX$$$$$$&X++xx+++x+xxxxxxxxxxxxxxxxxxxxxxx+++++;;;;;;++++xxxxxxxxxxx+;;:::::;++xxxxxxxxxxxx+;::::+xxxxxx+;:::::::::::::::::;;;;;;;+xxxxxxxXxxXXxxxxxxxxxxxxxxx+++;;+X$$$$$$$&&&&&&&&&&&$xXXXXXXX
;;:.:;;;;::;+xXX$$$$$$$$&X;;xxx++xxxxxxxxxxxxxxxxxxxxxxxxx+++;;;;;;;;;++++++xxxxxxxx++;:::::::;+xxxxxxxxxxxxx+;:::;+xxxxx+;:::::::::::::::::;;;;;++xXXxxxXXXXXXXXXXxxxxxxxxxxxxxx+++X$&&&&&&&&&$$$$$$$$XxXXXXXXX
;;:::;;;;;;xXX$$$$$$$$$$$$xxXXXx++xxxxxxxxxxxxxxxxxxxxxxxxx+++;;;;;;;:;++++++xxxxxxxxxx+;::::::;++xxxxxxxxxxxxx+;::;+++xxxx+;:::::::::::::::;;;++++xXXXxxxXXXXXXXXXxXXxxXxxxxxxxxxxxX$&$$XXXXXXXXXXXXXXxxXXXXXXX
;;:::;;;+++xX$$$$$$$$$$$$$X+xxXX++++xxxxxxxxxxxxxxxxxxxxxxx+++;;;;;;;;;;+++++++xxxxxxxxx+;;::::::;++xxxxxxxxxxxx+;:::++xxxxx+;;:::::::::::::;;+xxxxXXX$xxxxXXXXXXXXXXXXXXXxXXXxxxxxxXXXXXXX$$$$$&&&&&&&$xXXXXXXX
;+;::;;;+xxx$$$$$$$$$$$$&$X+xxXXX++x+xxxxxxxxxxxxxxxxxxxxxxx++;;;;;:;;::;++++++++xxxxxxxxx+;::::::;+xxxxxxxxxxxxxx+;::;+++xxx++;::::::::::::::;;++xXXX$xxxxXXXXXXXXXXXXXXXXXXXxXxXXXX$$$&&&&&&&&&&&&&$$$xX$$$XXX
;;;::;;;+XXxX$$$$$$$$$$$$$X+xXXXXx+xxx+xxxxxxxxxxxxxxxxxxxxxx++;;;;::::::;+++++++++xxxxxxxx+;:::::::++xxxxxxxxxxxxxx+;:;+++xx+;+;;:::::::::::::;;+xXxX$X+xxxXXXXXXXXXXXXXXXXXXXXxXXX$&&&&&&&&$$$$$$$$XXX+XXX$XX$
;;;::;;;+XXxX$$$$$$$$$$$$&$xXXXXXXx+++xxxxxxxxxxxxxxxxxxxxxxx++;;;::::::::;++++++++xxxxxxxxxx+;::::::;+xxxxxxxxxxxxxx+;::;+++++x++;::::::::::::;;;xXxX$X+xxxxXXXXXXXXXXXXXXXXXXXXXxX$$$$$$$$$$$$&&&&&&&$+XX$$$$X
;;;:::;;+xXxX$$$$$$$$$$$$&$xXXXXXX$x++++xx+xxxxxxxxxxxxxxxxxx++;;;;::::::::;+;++++++xxxxxxxxxx+;:::::::;+xxxxxxxxxxxxxx+;::+++++xx++;::::::;;;;;;+xXxX$X;+xxxxXXXXXXXXXXXXXXXXXXXxx$$&&&&&&&&&&$$$$$$$&$+x$$$$$$
;;;:::;;;x$xxX$$$$$$$$$$$$$XxXXXXX$$+++++++xxxxxxxxxxxxxxxxxxxx+;;;:::::::::;;++++++++xxxxxxxxx++;::::::;++xxxxxxxxxxxxxx+;:;++++x+++;;::::;++++xxXXx$$x::;xxxXXXXXXXXXXXXXXXXXXxxX$$$$$$$$$$$$$$$$$$$$$+xX$$$$$
;++:.:;;;xXxx$$$$$$$$$$$$$$XxXXXXX$$x+++++++xxxxxxxxxxxxxxxxxxx++;;;:::::::::;;+++++++++xxxxxxxxx+;;::::::;++xxxxxxxxxxxxxx;::;++xxx++;;:::::;+xxXXXx$$x:::;+xxXXXXXXXXXXXXXXXXXxX$$$$$$$$$$$$$$X$$$$XXX+xX$$$$$
;++;.:;;;+XxxX$$$$$$$$$$$&&XxXXXXX$$X++++++++x+xxxxxxxxxxxxxxxxx++;;::::::::::;++++++++++xxxxxxxxxx+;::::::;;;;++xxxxxxxxxxx+;:;+++++++;;;::::::;xxXx$$x::::;XXXXXXXXXXXXXXXXXXXX$$XXX$XXX$X$XXX$XXXXXXX+xX$$$$$
;++;.:;;;+XXxX$$$$$$$$$$$$$XxXXXXX$$$x+++++++++xxxxxxxxxxxxxxxxxx++;;::::::::::;;++++++++++xxxxxxxxxx+;;:::::;;;;;+xxxxxxxxxx++;:;++++++x++;;::::;xxx$$+;;X$$$$$$$XXXXXXXXXXXXX$$$$XxXXX$XX$XXXXXXXXXXXX+xX$$$$$
;;+;::;;;+XXxX$$$$$$$$$$$$$XxXXXXXX$$X+++++++++xxxxxxxxxxxxxxxxxx++;;;:::::::::::;;+++++++++xxxxxxxxxxx+;:::::;;;;;++xxxxxxxxxxx+;:++++xxxxxx+;:::;x$$$$$$$$$$$&&$&&$XXXXXXX$$$$$$$$XXXXXXXXXXXXXXXXXXXXxxX$$$$$
;;+;:.;;;+xX+xX$$$$$$$$$$$&$xXXXXXX$$X+x+++++++++xxxxxxxxxxxxxxxxx++;;::::::::::::;;+++++++++++xxxxxxxxxx+;::::;;;;;;++xxxxxxxxxx++;;++xxxxxxxxx+++X$$$$$&&&&&&&&&&&&&$$$$$$$$$$$$$$$$X$XXXXXXXXXXXXXXXXxXX$$$$$
;;+;:.:;;+xX+xX$$$$$$$$$$$$$XXXXXX$$$$x+x+++++++xxxxxxxxxxxxxxxxxxxx+;;;::::::::::::;++++++++x+xxxxxxxxxxx+;;::::;;;;;;++xxxxxxxxxx+;:;+xxxxxxxxxxxX$$&&&&&&&$$&&&&&&&&$$$$$$$$$$$$$$X$X$XXXXXXXXXXXXXXXxX$$$$$$
:;++:.:;;+xXxXX$$$$$$$$$$$&$xxXXXX$$$$X+xx++++++xxxxxxxxxxxxxxxxxxxx++;;;::::::::::::;++++++++xxxxxxxxxxxxxx+;:::::;;;;;;;++xxxxxxxx++;:;xxxxXXXxxx+$&&&&&&&$$$&&&&&&&&$$$$$$$$$$$$XXXXXXXXXX$$$$XXXXX$XXXX$$$$$
;;++;::;;;xXX+x$$$$$$$$$$$&$XXXXXX$$$$Xx+xx+x++++xxxxxxxxxxxxxxxxxxxx++;;;::::::::::::;++++++xxxxxxxxxxxxxxxxx+;:::::;;;;;;;+++xxxxxxxx+;:+xxxXXXxxxX&&&&&&$&&&&$$$&&&&&$$$$$$$$$$$$XXXXXXXXXXXX$$$$$$$XXXX$$$$$
:;+;;::;;;+XxxX$$$$$$$$$$$$$XXXXXXX$$$$X+xxx++++xxxxxxxxxxxxxxxxxxxxxx++;;;::::::::::::;;++++xxxxxxxxxxxxxxxxxxx+;::::;;;;;;;;;;+++xxxxx+;::+xxxxxxx$$&&&$++$&&$$$$$&&&&$$$$$$$$$$$$X$$$$$$$$$$$XXXXX$XXxXX$$$$$
;;++;::;;;+XXxX$$$$$$$$$$$&$XXXXXX$$$$$Xxxxxx+++++xxxxxxxxxxxxxxxxxxxxx++;;::::::::::::::;++xxxxxxxxxxxxxxxxxxxxxx+;:::::;;;;;;;;;;;++xxxx+;;::+xxxX&$$$x;++X$$$XXXX$$&$$$$$$&$$$$$$$$$$$$$$$$XXXXXXXX$XxX$$$$$$
;;;+;::;;;+XXxxX$$$$$$$$$$$$XXXXXX$$$$$XX+xxXx++++xxxxxxxxxxxxxxxxxxxxxx++;;;:::::::::::::;+++xxxxxxxxxxxxxxxxxxxxxxx+;::::;;;;;;;;;;;++xx+++;;:+++X&$$$X+x+xXXXXXXXX&&$$$&&&$$$$$$$$$$$$$$$$XXxxxXXXXXXxX$$$$$$
;;;+;::;;;+xXxXX$$$$$$$$$$$&XXXXXX$$$$$XXxxxXXx++xxx+xxxxxxxxxxxxxxxxxxxx++;;;:::::::::::::;;+++++x+++xxxxxxxxxxxxxxxxx+;::::;;;;;;;;;;;++xxx++;:;;;+X$$X+xxxXXXXXXXX$&&$&&&$$$$$$$$$$$XXXXXXXXxxxXxXX$$xxX$$$$$
;;;+;::;;;+x$x+x$$$$$$$$$$$&$XXXXX$$$$$XXX+xxxXXx+++xxxxxxxxxxxxxxxxxxxxxx++;;;;:::::::::::::;;++++++++xxxxxxxxxxxxxxxxxxx+;:::;;;;;;;;:;;;++++++++;;;+$X++x$$XXXXxXXX$&&&&&&$$$$$$$$$$XXXXXXXXXxxxXX$$$Xx$$$$$$
;;;++:::;;+xXxxX$$$$$$$$$$$&$XXXXX$$$$$$XXx+xxxXxx++xxxxxxxxxxxxxxxxxxxxxxx++;;;;;;:::::::::::::;;+++++++xxxxxxxxxxxxxxxxxxx+;:::;;;;;;;::;;;;+++xx+;;;;x++$&&XXXX$$$$$&&&&$$$$$$$$$$$$XXX$$$XXX$xXXX$$$Xx$$$$$$
;;;++;::;;+xXXxX$$$$$$$$$$$&$XXXXXXX$$$XXXX++xXxXxxx+xxxxxxxxxxxxxxxxxxxxxxx+++;;;:::::::::::::::::;;++++xxxxxxxxxxxxxxxxxxxxx+;;:::;;;;;;;::;;++x++++++;++$&&$X$$$$$$$$&&&&$$$$$$$$$$$XXXXXXXXXX$$$$$$&Xx$$$$$$
;;;++;::;;;xXX++$$$$$$$$$$$$$XXXXXXX$$$$XXXx++xxxxxxx+xxxxxxxxxxxxxxxxxxxxxxxx+;;;;:::::::::::::::::;;;++++xxxxxxxxxxxxxxxxxxxxxx+;;::::::::::;++++++;;;;++$&&&X$$$$$$$$$&$$$$$$$$$$$$$$XX$XXXX$$$$&&&&&x+$$$$$$
+;;++;::;;;+XXxX$$$$$$$$$$$$$XXXXXXX$$$XXXXX++xxxxxxxx++xxxxxxxxxxxxxxxxxxxxxxx++;;;::::::::::::::::::;;+++xxxxxxxxxxxxxxxxxxxxxxxx++;;::::::::;;++;;;;;+++$&&&$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$&&&&$$$$Xx+$$$$$$
+;;;+;::;;;+X$XxX$$$$$$$$$$$$$xXXXXX$$$$XXXXx++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+++;;;;::::::::::::::::;;++xxxxxx+xxxxxxxxxxxxxxxxxxx++;;:::;;;;;;;;;;+X++X&&&&$$$$$$$$$$&$$$$$$$$$$$$$$$$&&&&&&$$$$$$$x+X$$$$$
+;;;+;::;;;+XXxxX$$$$$$$$$$$&$XXXXXX$$$XXXXXX++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++++;;;;::::::::::::::::;;++++++++++++++xxxxxxxxxxxxxx+++;+++;;;;;;;+X$x+x$&&&$$$$$$$$$$$$$$$$$$$$$$$$$&&&&&&$$$$$$$&$x+$$$$$$
x;;;++::;;;+XXxX$$$$$$$$$$$$$$XXXXXX$$XXXXXXXX+xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+++;;;;::::::::::::::::;;;;;+;;;;:::;;;++++++xxxxxxxxxxxx+++++;++$xXx+x$&&&&$$$$$$$$$$$$$$$$$$$$&&&&&&$$$$$$$$$$$&$X+$$$$$$
x;;;++::;;;+x$X+x$$$$$$$$$$$$$XXXXXX$$$$XXXXXXx+xxxxxxxxxxxXxxxxxxxxxxxxxxxxxxxxxxxxxxx+++;;;;:::::::::::::::;;;;+++;;;;;:::::::::::::::::;+++++++++X$$X++x$&$$&&$$$$$$$$$$$$$$$$&$$&&&&&&$$$$$$$$$$&&&&X+X$$$$$
X;;;++;::;;+x$xxx$$$$$$$$$$$&$XXXXXX$$$XXXXXXXX++xxxxxxxxxxXXxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++;;;;:::::::::::::::;;;++++++++;;;:::::::::::::;+++++++x$$$$XXx$$$$$$$&$$$$$$$$&&&&$$&&&&&$$$&&&&$&&&&&&&&&&&$x$$$$$$
X;;;++;::;;+xXXxX$$$$$$$$$$$&$XXXXX$$$$$$XXXXXXx++x+xxxxxxxxXXXxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx++;;::::::::::::::::;;++++x+++++;;;:::::::;;+++x+xxx$&$$$$$xX$$$$$$$$$$&&&&&$$$$&&&$Xx++++x$$$$$&&&&&&&&&&$x$$$$$$
X+;;++;::;;+xXXx+X$$$$$$$$$$&$XxXX$$$$$$X$XXXXXXx++xxxxxxxxxxxxxxxxxxxxxxxxxxx+xxxxxxxxxxxxxxxxxxx+;;::::::::::::::::;;+++++++++++;;;;;;++xxxxxx+x&&$&$$$XxX$$$$$$&&&$$$$$$&&&&&$x+++xx+xX$$&&&&&&&&&&&&$X$$$$$$
X+;;++;::;;;+XX+X$$$$$$$$$$$$$XxXXXXX$$$XXXXXXXXX++++xxxxxxxxxxxxxxxxxxxx+x+++++xxxxxxxxxxxxxxxxxxxxx++;:::::::::::::::;;;+++xxx++++;;+++xxxxxx++X&&$$X$$$Xx$$$$$$$$$$$$&&&&&&&&&$X++;+++xX$XXXXXXXXXXXXXx$$$$$$
X+;;;++::;;;+X$XxX$$$$$$$$$$&&XxXXXXXXX$XXXXXXXXXX++x++xxxxxxxxxxxxxxxxx++++++;;;+xx+xxxxxxxxxxxxxxxxxxx+++;;::::::::::::;;;++xxxxxx+;;xxxxxxxx+x$&XXx$$$&$XX$$$$$$&&&&&&&$$$$&&&&&X+++xXXxxxxxxxxxxxxxxx+X$$$$$
$x;;;++::;;;+x$x+x$$$$$$$$$$$$XxXXXXXX$$XXXXXXXXXXx+++++++xxxxxxxxxxxxxxx++;;;;;;;:;;+++++++++;;;+x+++++++++;;;::::::::::::::;+++xxxxxx+xxxxxxxx$&XxX$$$$$$$X$$&&&&&$$$&&$$$$$$$$&&&&XX$X+++++xxxxxxx++;;+X$$$$$
$X;;;++;:;;;+x$XxX$$$$$$$$$$$&$XXXXXX$$$$$XXXXXXXXXXx+++++++++++++++xxxxxx+++;;;;;::::;:::;::;;:::::;;;::::;:::;:::::::::::::::::;;+xXXxxxxxxXXXXxx$&$X$$$$XxX&&&$XxxxxxX$&$$$$$$&&&&&&&$$$$$$$$$$$$$$$$$+X$$$$$
$X;;;++;::;;+x$XxxX$$$$$$$$$$$$XxX$$$$$$$XXXXX$XXXXXXx+++++++++++++++xxxxxxx++;;;;;;;:::::::::::::::::::::::::::::;:::::::::::::::::;+xXXxxxX$$XX$&&&&x$&$$$$xxXx++++xxxx$$$&$&&$&&&$$$$$$$$$$$$$$$$$$$$$x$$$$$$
$X;;;++;::;;+x$XxXX$$$$$$$$$$&$XxXXX$$$$$XXXXXXXXXXXXXXXxx+++++++++++++xxxxxxxx;;;;;;:::::::::::::::::::::::::::;;;;;;;;;;:;;;+xxxxxxxxX$$$$$&&$$&&&&&X$&$$$$$X+x+++xxxxx$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&$X$$$$$$
$X+;;++;::;;+xXXxXX$$$$$$$$$$&$XXXXX$$X$$XXXX$XXXXXXXXXXXXxx++++++xxxxxxxxxxxXXx++;;;;;:::::::::;;;;;:;++xxxxxxxx++xxxxxXXXx+xxxxxXX$$&&&$$$$$&$$&$&&X$$&&$$$$$X++++xxxxxX$$$$$$&&&&&&&&&$&&&&&&&&&&&&&&$XX$$$$$
$X+;;+++::;;+xXXx++$$$$$$$$$$$$XXXXX$$$$$XXXXX$XXXXXXXXXXXXXxxxxxxxxXXXXXXXX$$$$$$Xx++;;;;;;++++xxxXxxxxxxxxxxxxxXXXXXXXX$$$XxXX$$$$&&&&&$$$$$$&&&&$$$$$&&$$$$$$x+++++xxxX$$$$$$$&&&&&&&$$$$$$$$$$$$$$$$$xX$$$$$
+X+;;;++::;;++X$xX$$$$$$$$$$$&&$XXX$$$$$$XXX$$XXXXXXXXXXXXX$XXXxxxxxxxxxxXXXX$$&&&&&$Xx+++;;;;+xxx++++++++xxxxxxX$X$$$$&&&&&&&&&&&&&&&&&$$$&$$&$$$$&&&&X&&&&$$$Xx++++xxxxX$$$$$$$&&&&$$$$$$$$$$$$$$$$$$$$xX$$$$$
;Xx;;;++::;;;+X$XxxX$$$$$$$$$&&$XxXX$$$$$XXXX$$X$XXXXXXXX$XXXXXXXXXXXX$$$$X$$$$&&&&&&&&$XXxx+;;:::::;;+++++;;+X$&&&&&&&&&&&&&&&&&&&&&&$$$$$&&&&&&&&&&&$$&&&&$$$Xxx+x++xxxx$$$$$$$&&&&$$$$$$$$$$$$$$$$$$$$X$$$$$$
+Xx;;;++;::;++X$XxxX$$$$$$$$$$$$XXXX$$$$$XXX$$X$$XXXXX$$XXXXXXXXXXXXXX$$$$$$$$$&&&&&&&&&&&&$Xxx+;;;::::::;+X$&&&&&&&&&&&&&$$$&&$$$$&&&$$$&$$&&&&&&&&&&&$$&&$$$$XXx+++xxxxx$$$$$$$&$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
+Xx+;;++;::;;+x$XXX$$$$$$$$$$$&$XXXX$$$$$$X$$$$$$X$$$$$$$$$$$XXXXXXXXXX$$$$$$$&$&&$&&&&&&&&&&&&&$$$$$$$$$&&&&&&&$&$$$$$$$$XXXX$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$Xxx+++x+xxx$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
XXX+;;++;::;;+x$$x+x$$$$$$$$$$&$XXXX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$&$&&&&&&&&&&&&&&&&&&&&&&&&&$&$$$XXXXXXXXX$$&&&&&&&$$$&&&&&&&&&&&&&&&&&&&&&&$&&$$$Xxxx++xxxxxX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
+x$+;;+++::;++xXX+XX$$$$$$$$$$&$XxXX$$$XXXXXXXXXXXXXXXXXX$$X$$$$$$$$$$$$$$$$$$&$&&&&&&&&&&&&&&&&&&&&&&&&$$$$$&&&&&&&&&&&&&&&$xxX&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$Xxxxx+xxxxxX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$XX+;;+++::;++xX$XXXX$$$$$$$$$&$$XXX$$$$$$$$$$$$$$XXXXXXXXXXXXXXXXXXXXX$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$XxxX&&&&&&&&&&&&&&&&&&&&&&&&&&$&&&$&$$$XXxxxxxxxxxX$$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$$$$$
+XXx;;+++::;++xX$Xx;X$$$$$$$$$&&$XXXX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$XXX$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&&$$&$$$$XxxxxxxxxxX$$$$$$$$$$$$$$$$X$$$$$$$$$$$X$$$$$$$$
xX$x;;;++;:;+++X$XXX$$$$$$$$$$$$$xXXXXXX$XXX$$$$$XXXXXXX$$$$$XXXXXXXXX$$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$X$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$&$$$$XXxxxxxxxxx$$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$$$$$
XX$X+;;+x;:;;++X$XxxX$$$$$$$$$$&$XxXXXXXXXXXXXX$XXXXXXXXXXXXXXXXXXXXX$$$$$$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$$$$XXXXXXXxxxxx$$$$$$$$$$$$$$$$X$$$$$$$$$$$$$$$$$$$$
Xx$X+;;++;::;++x$XxxX$$$$$$$$$$$$XXXXXXXXXXXXXXXXX$XXXXXXXXXXXXXXXXX$$$$$$$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&&&&&&&&&&$$$$$$XXXXXXXXxxxx$$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$$$$$
Xx$X+;;+++::;++x$$XXX$$$$$$$$$$$$XXXXX$XXXXXXX$$XXXXXXXXXXXXXXXXXXXX$&$$$$$$&$$$&&&&&&&&&$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$$$XXXXXXXXxxxxX$$$$$$$$$$$$$$$$$$$$$$$$X$$X$$$$$$$$
$xXXx;;+x+::;++xX$xx+$$$$$$$$$$$$$XxxX$$XXXXXXXXXXXXXXXXXXXXXXXXXXXX$&$$$$$$$$&&$$$&&&&&&&&$&&&$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&&&&&&&&$$$&&&&&&&$$$$$X$XXXXXXxxxxX$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$$$$$
$X$$x;;+++;:;++x$$Xxx$$$$$$$$$$$$$X+XX$XXXXXXXXXX$XXXXXXXXXX$$XXXXXXX$&$$$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&$$$&&&&&&&$$$X$$XXXXXXxxxxX$$$$$$$$$$$$$$$$$$$$$$$$$$$X$$$$$$$$
$Xx$x;;+++;:;+++X$XXxX$$$$XXX$$$$$XxXXX$$XxxXX$XXXXXXXXXXXXXXXXXXXXXXX&$$$$$$$$$&$$&&&$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$&&&&&&$$$$&$$&&$$$$XX$$$XX$$XxxXXX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$xXXX+;;++;:;+++X$Xx+X$$XXXXX$$$$$XXxXX$$XxxxXXXXXXXXXXXXXXX$$XXXXXXX$$$$$$$$$$$&&&$$$&&$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&&&$&&&$&$&&&&&&&&$$$X$$X$$$$XXxxxX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$XX$X+;;+x;:;+++X$XxxX$XXXXX$$$$$$$xxXXXXXx+xXXXXXXXXXXXXXXXXXXXXXXXX$$$$$$$$$&$$$$$$&$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&$&&&&&$$$&$&$$&&&$$XX$$$$$$$$Xxxxx$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$XxXX+;;+++:;+++x$$XxxXXXXX$$$$$&$XXxXXXXXXxxXXXXXXXXXXXXX$$XXXX$XXX$$$$$$$$&$$$&&$$$$$$$$&$&&&&&&&&&&&&&&&&&&&&&&&&&&&$&&&&&&&&&&&&&&&&&$&&&&$&&&&&$$$$$$&$$$X$$$$$$XXXXXXX$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$XX$$x+;+x+::+++X$$x;xxxxX$$$$$$$$+++;;;:;;;;;;;;;;;+++xxxxxXXXXXXXX$$$$$$$$$$&&&&&$$$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$$$&&&&&&&&&&&&&$&&&&&&&&&&&&&$$$$$$$$XXX$$$$$$XXXXXX$$$$$$$$$$$$$$$$X$$$$$$$$$$$$$$$$$$$
$XxX$x+++++;:;++xXx++;.:x$$$$$$$$$x+++;;;;;;;;;;;:::::::;;;;;;;;:::;XX$$$$$$$$$$$&&&$$$&$$&&&&&&&&&&$$$&&&&&&&&&&$$$$$$$&&&&&&&&&&&&&&&&&$&&&&&&&&&&&&$$$$$$$XX$$$$$$$XXXXxX$$$$$$$$$$$$$$$$$$$X$$$$$$$$$$$$$$$$
$XxxXX+++++;:;++xxx++::+xX$$$$$$$$x;++;;;;;;;;;;;::::;;:;;;;;:;:::::;X$$$$$$$$$$&$$&&$$$&&&&&&&&$$$$$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&&&&&&&&&&&&$&&&$&$$$XX$$$$$$$XXXXxxX$$$$$$$$$$$$$$$$$$X$$$$$X$$$$$$$$$$
$XX$$Xx++++;:;++xxx+;;xxX$$$$$$$&$x;++;;;;;;;;;;::::::::;;::;;:::::::+X$$$$$$$&&&$&&&$$$$$$$&&&&$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$&$$$X$$$$$$$$XXXXXxX$$$$$$$$$$$$$$$$$$X$$$$$$$$$$$$$$$$
$$xxX$++;+x+:;++xx+::+xX$$$$$$$$&$X;++;;;;;;;;;;::::::::;;::::::::::::xX$$$$$$&&&&&&&&&$$$$$&&&&&&&&&&&&&&&&&&&&&&$$$&$&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&$$$$$$$$$$$$XXXXXXxx$$$$$$$$$$$$$$$$$$X$$$$$$$$$$$$$$$$
$$xX$$x+;+x+:;++xx::;+x$$$$$$$$$$$X;+++;;;;;;;;::::::;:;;:::::::::::::XXX$$$$$$$&&$&&&$$$&&&$&&&&&&&&&&&&&$$$$$$$$$$$$&&&&&&&&&&&&&&&&&&$$&&&&&&&&&&&&&$$$$$$$$$$$$$XXXXXXXxx$$$$$$$$$$$$$$$$$$X$$$$$$$$$$X$$$$$
$$XXx$X+++x+;;++++::;;x$$$$$$$$$$$X++++;;;;;;;:::::::;:::::::::::::::xXXX$$$$$$$&$&&&&$$$&&&&&&&&&&&&&&$$$$$$$$$$$$$$$&&&&&&&&&&&&&&&&&&$$&$&&$$&&&&&&&&$$$$$&$$$$$XX$$$XXXXx$$$$$$$$$$$$$$$$$$$$$$$$$$$$XX$$$$$
$$XXX$X+++x+;:+++x;;+xX$$$$$$$$$$$X+;++;;;;;;:::::::::::::::::::::::;X$$X$$$$$$$&$&&&&&$$$&&&&&&&&&&&$$$$$$$$$$$$$$$$$$&&&&&&&&&&&&&&&&$&$$&&&&&&&&&&&&&$$$&&$$$$$$$$$$$$XXXxX$$$$$$$$$$$$$$$$$$$$$$$$$$$XX$$$$$
$$XXX$X+++++;:;++x++xxx$$$$$$$$$$$$x;++;;;;;:::::;;;;;;;::::::::::::;$$$$$$$$$$&$$&&&$$$$$$$$&&&&&&&&&$$$$$$$$$$$$$$$$$&&&&&&&&&&&&&&&$&&&&&&&&&&&&&&&&&$$&&&$$$$$$$&$$X$$XXxx$$$$$$$$$$$$$$$$$$$$$$$$$$$XX$$$$$
$$XxxXXx++x+;:;+xxxX$XxX$$$$$$$$$$$+;;+;::::::::;;;;;;;;;;::::::::::;X$$$$$$$&&$&&&&$$$$$$$&$&&&&&&&&&$$$&&&&$&&&&&&&&&&&&$&&&&&&&&&&&&&&&&&$&$&&&&&&&&&$&&&&$&&&&&$$$$$$XXXXx$$$$$$$$$$$$$$$$$$$$$$$$$$$XX$$$$$
$$XX$$$x+++x+:;+xxXX$XxX$$$$$$$$$$$x;;+;;;:::::;;;;;;;;;;::::::::::::x$$$$$$$$$&$$&&&$$$$$$$&&$$$&&&&&&&&&&&&&&&&&&&$$$&&&&&&&&&&&&&&&$$&&&&&&$&&&&&&&&$$&&&&&&&&&$$$$$$$XXXXxX$$$$$$$$$$$$$$$$$$$$$$$$$$XX$$$$$















*/