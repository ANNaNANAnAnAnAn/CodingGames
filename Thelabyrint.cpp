#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct Node {
    int x, y;
    Node(int _x, int _y): x(_x), y(_y) {}
};

int main() {
    int r, c, a;
    cin >> r >> c >> a; cin.ignore();

    vector<vector<char>> knownMap(r, vector<char>(c, '?'));

    //direction vectors 
    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};
    string dirNames[4] = {"UP", "DOWN", "LEFT", "RIGHT"};
    bool foundC = false;
    int tx = -1; 
    int ty = -1;

    //lambda helper exploration function 
    //finds the shortest path to the C if it in the vision, 
    auto bfsShortestPath = [&](int startX, int startY, vector<vector<bool>> &blocked, 
                               function<bool(int,int)> isTarget) -> vector<Node> {
        vector<vector<bool>> visited(r, vector<bool>(c, false));// to track which nodes were visited 
        vector<vector<Node>> parent(r, vector<Node>(c, {-1, -1}));// for later path reconstruction
        
        //bfs algorithm 
        queue<Node> q;
        q.push({startX, startY});
        visited[startX][startY] = true; 

        Node targetNode(-1, -1);
        bool found = false;

        while (!q.empty() && !found) {
            Node cur = q.front(); q.pop();

            if (isTarget(cur.x, cur.y)) {
                targetNode = cur;
                found = true;
                break;
            }

            for (int i = 0; i < 4; i++) {
                int nx = cur.x + dx[i];
                int ny = cur.y + dy[i];

                if (nx >= 0 && nx < r && ny >= 0 && ny < c &&
                    !visited[nx][ny] && !blocked[nx][ny] && knownMap[nx][ny] != '#') {
                    visited[nx][ny] = true;
                    parent[nx][ny] = cur;
                    q.push({nx, ny});
                }
            }
        }

        

        if (!found) return {}; // no path found

        // reconstruct path from target to start
        vector<Node> path;
        Node cur = targetNode;
        while (!(cur.x == startX && cur.y == startY)) {
            path.push_back(cur);
            cur = parent[cur.x][cur.y];
        }
        reverse(path.begin(), path.end());
        return path;
    };

    while (true) {
        int kr, kc;
        cin >> kr >> kc; cin.ignore();

        for (int i = 0; i < r; i++) {
            string row; cin >> row; cin.ignore();
            for (int j = 0; j < c; j++) {
                if (row[j] != '?') { // update only known cells
                    knownMap[i][j] = row[j];
                    if (row[j] == 'T') { // record T position
                     tx = i; ty = j;
                    }
                }
            }
        }

        // update known map by visibility range 
        // write only the ones that got uncovered in the move 
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                int ni = kr + i, nj = kc + j;
                if (ni >= 0 && ni < r && nj >= 0 && nj < c) {
                    knownMap[ni][nj] = knownMap[ni][nj]; // already updated above from input, making sure
                }
            }
        }

        // i use the separated bool data, since i do not want to get into the wall for sure 
        // to make sure that bfs explores only the cells that it can go to
        vector<vector<bool>> blocked(r, vector<bool>(c, false));
        for (int i = 0; i < r; i++){
            for (int j = 0; j < c; j++){
                if (knownMap[i][j] == '#'){
                    blocked[i][j] = true;
                }
            }
        }
        // flag if c was visited, then go back to T 
        if (knownMap[kr][kc] == 'C'){
            foundC = true;
        }

        bool moved = false; // track if we already output a move

        if (!foundC){
            // trying to find the shortest path to the control room 
            // if the C is available in the vision 
            vector<Node> pathToC = bfsShortestPath(kr, kc, blocked,
                [&](int x, int y) { return knownMap[x][y] == 'C'; });

            // if the C is found within the vision range, move towards 
            if (!pathToC.empty()) {
                // move 1 step towards control room
                Node nextMove = pathToC[0];
                // determine direction
                for (int i = 0; i < 4; i++) {
                    if (kr + dx[i] == nextMove.x && kc + dy[i] == nextMove.y) {
                        cout << dirNames[i] << endl;
                        break;
                    }
                }
                
                continue;
            }

            // if C is not found move to the nearest ? 
            vector<Node> pathToUnknown = bfsShortestPath(kr, kc, blocked,
                [&](int x, int y) { return knownMap[x][y] == '?'; });

            if (!pathToUnknown.empty()) {
                Node nextMove = pathToUnknown[0];
                for (int i = 0; i < 4; i++) {
                    if (kr + dx[i] == nextMove.x && kc + dy[i] == nextMove.y) {
                        cout << dirNames[i] << endl;
                        break;
                    }
                }
                continue;
            }

            // if all ? cells are explored go to the nearest . 
            vector<Node> pathToDot = bfsShortestPath(kr, kc, blocked,
                [&](int x, int y) { return knownMap[x][y] == '.'; });

            if (!pathToDot.empty()) {
                Node nextMove = pathToDot[0];
                for (int i = 0; i < 4; i++) {
                    if (kr + dx[i] == nextMove.x && kc + dy[i] == nextMove.y) {
                        cout << dirNames[i] << endl;
                        break;
                    }
                }
                continue;
            }
        }else{
                if (tx != -1 && ty != -1) {
                vector<Node> pathToT = bfsShortestPath(kr, kc, blocked,
                    [&](int x, int y) { return x == tx && y == ty; });

                if (!pathToT.empty()) {
                    Node nextMove = pathToT[0];
                    for (int i = 0; i < 4; i++) {
                        if (kr + dx[i] == nextMove.x && kc + dy[i] == nextMove.y) {
                            cout << dirNames[i] << endl;
                            moved = true;
                            break;
                        }
                    }

                    if (moved) continue; 
                }
            }
        }


        // if all fails, just moves up 
        cout << "UP" << endl;
    }

    return 0;
}
