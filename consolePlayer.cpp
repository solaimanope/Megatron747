#include<bits/stdc++.h>
using namespace std;

struct Cell {
    int x, y;
    Cell():x(0), y(0) { }
    Cell(int x, int y):x(x), y(y) {}
};

const int DIM = 8;
char me;
string gr[DIM][DIM];

bool readFile() {
    ifstream in;
    in.open("shared_file.txt");
    char now;
    in >> now;
    if (now!=me) {
        in.close();
        return false;
    }
    cout << "reading" << endl;
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            string str;
            in >> str;
            cout << str << " ";
            gr[i][j] = str;
        }
        cout << endl;
    }
    in.close();
    cout << "read done" << endl;
    return true;
}

void writeFile(Cell mv) {
    cout << "move: " << mv.x << " " << mv.y << "\n";
    ofstream out;
    out.open("shared_file.txt");
    out << 0 << "\n";
    out << mv.x << " " << mv.y << "\n";
    out.close();
}

bool isValidCell(const Cell& p) {
    return 0 <= p.x && p.x < DIM && 0 <= p.y && p.y < DIM;
}

int main(int argc, char* argv[])
{
    me = argv[1][0];
    char other;
    if (me=='R') other = 'G';
    else other = 'R';

    while (true) {
        if (readFile()==false) continue;
        cout << "Input x y: ";
        Cell c;
        while (true) {
            cin >> c.x >> c.y;
            if (isValidCell(c)==false || gr[c.x][c.y][0]==other) {
                cout << "Invalid move!!" << endl;
            } else {
                break;
            }
        }
        writeFile(c);
    }


    return 0;
}
