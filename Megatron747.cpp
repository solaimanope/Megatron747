#include<bits/stdc++.h>
using namespace std;

typedef pair<int,int> PII;
typedef vector< string >VS;
typedef vector< VS >VVS;

struct Cell {
    int x, y;
    Cell():x(0), y(0) { }
    Cell(int x, int y):x(x), y(y) {}
};

Cell operator+(const Cell &p1, const Cell &p2)
{
    return Cell(p1.x+p2.x, p1.y+p2.y);
}

vector< Cell >dir = {Cell(1, 0), Cell(-1, 0), Cell(0, 1), Cell(0, -1)};
const int DIM = 8;
const int MAX_SCORE = 10000;

class Grid {
    const int MAX_LOOP = 1000;

    VVS s;
    queue< Cell >unstable;
    char cur, other;
    int vs[DIM][DIM];

    bool isValidCell(const Cell& p) {
        return 0 <= p.x && p.x < DIM && 0 <= p.y && p.y < DIM;
    }

    int neighbourCount(const Cell& p) {
        int ans = 0;
        for (Cell c : dir) {
            c = c+p;
            ans += isValidCell(c);
        }
        return ans;
    }

    bool enemyGone() {
        for (VS v : s) {
            for (string p : v) {
                if (p=="No") continue;
                if (p[0]!=cur) return false;
            }
        }
        return true;
    }
    bool meGone() {
        for (VS v : s) {
            for (string p : v) {
                if (p[0]==cur) return false;
            }
        }
        return true;
    }

    void update() {
        int loop = 0;
        while (!unstable.empty() && !enemyGone()) {
            loop++;
            if (loop > MAX_LOOP) {
                cout << "Loop more than " << MAX_LOOP << endl;
                return;
            }
            assert(unstable.size() < DIM*DIM);
            Cell p = unstable.front();
            unstable.pop();

            int atom = s[p.x][p.y][1]-'0';
            int react = neighbourCount(p);
            assert(react <= atom);

            if (react==atom) {
                s[p.x][p.y] = "No";
            } else {
                int tmp = atom-react;
                if (tmp >= react) unstable.push(p);
                s[p.x][p.y][1] = tmp+'0';
            }

            for (Cell d : dir) {
                d = d+p;
                if (!isValidCell(d)) continue;
                addAtom(d, true);
            }
        }
    }

    bool flag;
    int dfs(Cell c) {
        vs[c.x][c.y] = 1;
        int ans = 1;
        for (Cell d : dir) {
            d = d+c;
            if (isValidCell(d)==false) continue;
            if (s[d.x][d.y]=="No") continue;
            if (s[d.x][d.y][0]!=cur) {
                if (neighbourCount(d)-1==s[d.x][d.y][1]-'0') flag = true;
                continue;
            }
            if (vs[d.x][d.y]) continue;
            if (neighbourCount(d)-1==s[d.x][d.y][1]-'0')
                ans += dfs(d);
        }
        return ans;
    }

    int chainLengthSum(int version) {
        int ans = 0;
        memset(vs, 0, sizeof vs);
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (s[i][j][0]!=cur) continue;
                if (vs[i][j]) continue;
                Cell now(i, j);
                if (neighbourCount(now)-1==s[i][j][1]-'0') {
                    flag = false;
                    int l = dfs(now);
                    if (l > 1) {
                        if (flag) ans += l*2*version;
                        else ans += l*2;
                    }
                }
            }
        }
        return ans;
    }



public:
    Grid(VVS s, char cur) : s(s), cur(cur) {
        if (cur=='R') other = 'G';
        else other = 'R';
    }

    void addAtom(Cell p, bool fromUpdate = false) {
        if (s[p.x][p.y]=="No") {
            s[p.x][p.y][0] = cur;
            s[p.x][p.y][1] = '1';
            return;
        }
        s[p.x][p.y][0] = cur;
        s[p.x][p.y][1]++;
        if (neighbourCount(p)==s[p.x][p.y][1]-'0') {
            unstable.push(p);
            if (!fromUpdate) update();
        }
    }

    void setCell(int i, int j, string str) {
        s[i][j] = str;
    }

    int score1() {
        /// calculate score for bot1
        if (enemyGone()) return 10000;
        if (meGone()) return -10000;
        int ans = 0;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (s[i][j][0]!=cur) continue;
                int cnt = s[i][j][1]-'0';
                assert(0 <= cnt);
                ans += cnt;
            }
        }
        return ans;
    }

    int score2(int version) {
        /// calculate score for bot1
        int mine = 0;
        int enemies = 0;
        int ans = 0;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (s[i][j]=="No") continue;
                if (s[i][j][0]!=cur) {
                    enemies += s[i][j][1]-'0';
                    continue;
                }
                mine += s[i][j][1]-'0';
                bool flag = true;
                Cell now(i, j);
                int ng = neighbourCount(now);
                for (Cell d : dir) {
                    d = d + now;
                    if (isValidCell(d)==false) continue;
                    if (s[d.x][d.y][0]==other&&s[d.x][d.y][1]-'0'==neighbourCount(d)-1) {
                        ans -= 5-ng;
                        flag = false;
                    }
                }
                if (flag) {
                    if (ng==3) ans += 2;
                    if (ng==2) ans += 3;
                    if (s[i][j][1]-'0'==ng-1) ans += 2;
                }
            }
        }
        ans += mine;
        if (enemies == 0 && mine > 1) return MAX_SCORE;
        if (mine == 0 && enemies > 1) return -MAX_SCORE;
        ans += chainLengthSum(version);
        return ans;
    }

    bool isEmpty(int i, int j) {
        return s[i][j]=="No";
    }
    char getPlayer(int i, int j) {
        return s[i][j][0];
    }
    int getAtoms(int i, int j) {
        return s[i][j][1]-'0';
    }
};

const int MAX_DEPTH = 3;
class Megatron747 {
    int botVersion;
    char me, other;
    Grid gr;

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
                gr.setCell(i, j, str);
            }
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

    typedef pair<Cell,int>PCI;

    PCI goDeeperMin(Grid& gr, int level) {
        /// greedily make move to the cell which
        /// minimizes score2
        vector<Cell>vc;
        int mn = MAX_SCORE*2;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j));

                    int score2;
                    if (level==MAX_DEPTH) score2 = tmp.score1();
                    else score2 = goDeeperMax(tmp, level+1).second;

                    if (score2 < mn) {
                        mn = score2;
                        vc.clear();
                    }
                    if (mn==score2) vc.emplace_back(i, j);
                }
            }
        }

        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
        return PCI(vc[idx], mn);
    }

    PCI goDeeperMax(Grid& gr, int level) {
        /// greedily make move to the cell which
        /// maximizes score2
        vector<Cell>vc;
        int mx = -MAX_SCORE*2;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j));

                    int score2;
                    if (level==MAX_DEPTH) score2 = tmp.score1();
                    else score2 = goDeeperMin(tmp, level+1).second;

                    if (score2 > mx) {
                        mx = score2;
                        vc.clear();
                    }
                    if (mx==score2) vc.emplace_back(i, j);
                }
            }
        }

        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
        return PCI(vc[idx], mx);
    }

    Cell bot21() {
        /// greedily make move to the cell which
        /// maximizes score2
//        int possible = 0;
//        for (int i = 0; i < DIM; i++) {
//            for (int j = 0; j < DIM; j++) {
//                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me)
//                    possible++;
//            }
//        }
//        if (possible > 45) return bot12();
//        if (possible > 15) return bot13();
        cout << "go deeper!!" << endl;
        Grid tmp(gr);
        return goDeeperMax(tmp, 0).first;
    }

    Cell bot13() {
        /// greedily make move to the cell which
        /// maximizes score2
        vector<Cell>vc;
        int mx = -MAX_SCORE*2;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j));
                    int score2 = tmp.score2(0);
                    if (score2 > mx) {
                        mx = score2;
                        vc.clear();
                    }
                    if (mx==score2) vc.emplace_back(i, j);
                }
            }
        }

        cout << "max score " << mx << " found "
            << vc.size() << " cells" << endl;

        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
        cout << idx << endl;
        return vc[idx];
    }

    Cell bot12() {
        /// greedily make move to the cell which
        /// maximizes score2
        vector<Cell>vc;
        int mx = -MAX_SCORE*2;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j));
                    int score2 = tmp.score2(1);
                    if (score2 > mx) {
                        mx = score2;
                        vc.clear();
                    }
                    if (mx==score2) vc.emplace_back(i, j);
                }
            }
        }

        cout << "max score " << mx << " found "
            << vc.size() << " cells" << endl;

        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
        cout << idx << endl;
        return vc[idx];
    }

    Cell bot11() {
        /// greedily make move to the cell which
        /// maximizes my color
        vector<Cell>vc;
        int mx = -MAX_SCORE*2;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j));
                    int score1 = tmp.score1();
//                    cout << score1 << " for " << i << " " << j << endl;
                    if (score1 > mx) {
                        mx = score1;
                        vc.clear();
                    }
                    if (mx==score1) vc.emplace_back(i, j);
                }
            }
        }

        cout << "max score " << mx << " found "
            << vc.size() << " cells" << endl;

        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
        return vc[idx];
    }

    Cell bot0() {
        /// randomized move
        vector<Cell>vc;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me)
                    vc.emplace_back(i, j);
            }
        }
        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
        return vc[idx];
    }

    Cell makeMove() {
        if (botVersion==0) return bot0();
        if (botVersion==11) return bot11();
        if (botVersion==12) return bot12();
        if (botVersion==13) return bot13();
        if (botVersion==21) return bot21();
        assert(false);
    }

    void run() {
        while (true) {
            if (!readFile()) continue;
            Cell mv = makeMove();
            writeFile(mv);
        }
    }

public:
    Megatron747(char me, int botVersion):gr(VVS(DIM, VS(DIM)), me) {
        this->me = me;
        this->botVersion = botVersion;
        if (me=='R') other = 'G';
        else other = 'R';
        run();
    }
};

int main(int argc, char *argv[])
{
    srand(time(0));

    int v = 0;
    if (argc > 2) v = atoi(argv[2]);
    Megatron747 mt747(argv[1][0], v);

    return 0;
}
