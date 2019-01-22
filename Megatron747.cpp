#include<bits/stdc++.h>
using namespace std;

typedef pair<int,int> PII;
typedef vector< string >VS;
typedef vector< VS >VVS;

struct Cell {
    int x, y;
    Cell():x(0), y(0) { }
    Cell(int x, int y):x(x), y(y) {}
    bool operator==(const Cell& c) const {
        return x==c.x&&y==c.y;
    }
    bool operator<(const Cell& c) const {
        if (x==c.x) return y < c.y;
        return x < c.x;
    }
};

Cell operator+(const Cell &p1, const Cell &p2)
{
    return Cell(p1.x+p2.x, p1.y+p2.y);
}

struct Move {
    Cell c;
    int strength;
    bool operator < (const Move& m) const {
        return strength > m.strength;
    }
};

vector< Cell >dir = {Cell(1, 0), Cell(-1, 0), Cell(0, 1), Cell(0, -1)};
const int DIM = 8;
const int MAX_SCORE = 10000;

class Grid {
    const int MAX_LOOP = 1000;

    VVS s;
    queue< Cell >unstable;
    int cnt[2];
    /// 0 for R
    /// 1 for G

    int charToInt(char c) {
        return c=='G';
    }

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

    void update() {
        int loop = 0;
        while (!unstable.empty() && !gameEnded()) {
            loop++;
            if (loop > MAX_LOOP) {
                cout << "Loop more than " << MAX_LOOP << endl;
                break;
            }

            assert(unstable.size() < DIM*DIM);
            Cell p = unstable.front();
            unstable.pop();

            char cur = s[p.x][p.y][0];
            int atom = s[p.x][p.y][1]-'0';
            int react = neighbourCount(p);
            assert(react <= atom);

            if (react==atom) {
                s[p.x][p.y] = "No";
                cnt[charToInt(cur)]--;
            } else {
                int tmp = atom-react;
                if (tmp >= react) unstable.push(p);
                s[p.x][p.y][1] = tmp+'0';
            }

            for (Cell d : dir) {
                d = d+p;
                if (!isValidCell(d)) continue;
                addAtom(d, cur, true);
            }
        }
        while (unstable.size()) unstable.pop();
    }


public:
    Grid(VVS s) : s(s) {
        clearCount();
    }

    void clearCount() {
        cnt[0] = cnt[1] = 0;
    }

    void addAtom(Cell p, char cur, bool fromUpdate = false) {
        int player = charToInt(cur);
        if (s[p.x][p.y]=="No") {
            s[p.x][p.y][0] = cur;
            s[p.x][p.y][1] = '1';
            cnt[player]++;
            return;
        }
        if (s[p.x][p.y][0]!=cur) {
            cnt[player^1]--;
            cnt[player]++;
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
        if (s[i][j]!="No") cnt[charToInt(s[i][j][0])]++;
    }

    int scoreX(char player) {
        int cp[2];
        int xp[2];
        cp[0] = cp[1] = xp[0] = xp[1] = 0;

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (s[i][j]=="No") continue;
                char c = s[i][j][0];
                cp[player==c]++;
                if (s[i][j][1]-'0'==neighbourCount(Cell(i, j))-1) {
                    xp[player==c]++;
                }
            }
        }

        if (cp[1] > 0 && cp[0]==0) return MAX_SCORE;
        if (cp[0] > 0 && cp[1]==0) return -MAX_SCORE;
        return cp[1]-cp[0]+xp[1]-xp[0];
    }

    int score1(char player) {
        /// calculate score for bot1

        int cp[2];
        cp[0] = cp[1] = 0;

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (s[i][j]=="No") continue;
                char c = s[i][j][0];
                cp[player==c] += s[i][j][1]-'0';
            }
        }

        if (cp[1] > 0 && cp[0]==0) return MAX_SCORE;
        if (cp[0] > 0 && cp[1]==0) return -MAX_SCORE;
        return cp[1];
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

    bool gameEnded() {
        return cnt[0]==0||cnt[1]==0;
    }
};

const int MIN_DEPTH = 2;
const int MAX_DEPTH = 5;
const int MAX_KILL = 10;

char otherPlayer(char now)
{
    if (now=='R') return 'G';
    return 'R';
}

class Megatron747 {
    int botVersion;
    char me, other;
    Grid gr;

    vector<Cell>killerMoves[MAX_DEPTH+2];
    vector<int>efficiency[MAX_DEPTH+2];

    void updateKill(Cell c, int depth) {
//        cout << "killer " << c.x << " " << c.y << " at " << depth << endl;
        for (int i = 0; i < killerMoves[depth].size(); i++) {
            if (killerMoves[depth][i]==c) {
                efficiency[depth][i]++;
                return;
            }
        }

        if (killerMoves[depth].size() < MAX_KILL) {
            killerMoves[depth].push_back(c);
            efficiency[depth].push_back(1);
            return;
        }

        int mnid = 0;
        for (int i = 1; i < killerMoves[depth].size(); i++) {
            if (efficiency[depth][i] < efficiency[depth][mnid]) {
                mnid = i;
            }
        }
        killerMoves[depth][mnid] = c;
        efficiency[depth][mnid] = 1;
    }

    void clearKill() {
        for (int i = 0; i <= MAX_DEPTH; i++) {
            killerMoves[i].clear();
            efficiency[i].clear();
        }
    }

    bool readFile() {
        ifstream in;
        in.open("shared_file.txt");
        char now;
        in >> now;
        if (now!=me) {
            in.close();
            return false;
        }
        gr.clearCount();
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


    chrono::time_point<chrono::system_clock> start_time = chrono::high_resolution_clock::now();
    const int TIME_LIMIT = 2500;
    bool time_out;
    int allowedDepth;

//    int evaluate(Grid& gr, int depth, int alpha, int beta) {
//
//        for (int i = 0; i < )
//
//    }

    int goDeeperMin(Grid& gr, int depth, int alpha, int beta) {
        /// greedily make move to the cell which
        /// maximizes score2
//        assert(depth!=0);
        auto current_time = chrono::high_resolution_clock::now();
        int elapsed = chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (depth==allowedDepth||gr.gameEnded()||elapsed>TIME_LIMIT) {
            if (elapsed > TIME_LIMIT) time_out = true;
            return gr.score1(me);
        }

        set<Cell>done;
        for (int i = 0; i < killerMoves[depth].size(); i++) {
            Cell k = killerMoves[depth][i];
            if (gr.isEmpty(k.x, k.y)||gr.getPlayer(k.x, k.y)==other) {
                done.insert(k);

                Grid tmp(gr);
                tmp.addAtom(k, other);

                int score = goDeeperMax(tmp, depth+1, alpha, beta);

                if (score < beta) beta = score;
                if (alpha >= beta) {
                    efficiency[depth][i]++;
                    return beta;
                } else {
                    efficiency[depth][i]--;
                }
            }
        }

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==other) {
                    Cell now = Cell(i, j);
                    if (done.count(now) > 0) continue;

                    Grid tmp(gr);
                    tmp.addAtom(now, other);

                    int score = goDeeperMax(tmp, depth+1, alpha, beta);

                    if (score < beta) beta = score;
                    if (alpha >= beta) {
                        updateKill(now, depth);
                        return beta;
                    }
                }
            }
        }
        return beta;
    }


    int goDeeperMax(Grid& gr, int depth, int alpha, int beta) {
        /// greedily make move to the cell which
        /// maximizes score2
        auto current_time = chrono::high_resolution_clock::now();
        int elapsed = chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (depth==allowedDepth||gr.gameEnded()||elapsed>TIME_LIMIT) {
            if (elapsed > TIME_LIMIT) time_out = true;
            return gr.score1(me);
        }

        set<Cell>done;
        for (int i = 0; i < killerMoves[depth].size(); i++) {
            Cell k = killerMoves[depth][i];
            if (gr.isEmpty(k.x, k.y)||gr.getPlayer(k.x, k.y)==me) {
                done.insert(k);

                Grid tmp(gr);
                tmp.addAtom(k, me);

                int score = goDeeperMin(tmp, depth+1, alpha, beta);

                if (score > alpha) alpha = score;
                if (alpha >= beta) {
                    efficiency[depth][i]++;
                    return alpha;
                } else {
                    efficiency[depth][i]--;
                }
            }
        }


        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Cell now = Cell(i, j);
                    if (done.count(now) > 0) continue;

                    Grid tmp(gr);
                    tmp.addAtom(now, me);

                    int score = goDeeperMin(tmp, depth+1, alpha, beta);

                    if (score > alpha) alpha = score;
                    if (alpha >= beta) {
                        updateKill(now, depth);
                        return alpha;
                    }
                }
            }
        }
        return alpha;
    }

    typedef pair<Cell,int>PCI;
    PCI bot21(int depth = MIN_DEPTH) {
        /// greedily make move to the cell which
        /// maximizes score2
        if (depth==MIN_DEPTH) {
            ///direct call to bot21
            time_out = false;
            start_time = chrono::high_resolution_clock::now();
        }
        allowedDepth = depth;
        vector<Cell>vc;
        int mx = -MAX_SCORE*2;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j), me);
                    int score = goDeeperMin(tmp, 0, -MAX_SCORE, MAX_SCORE);
                    if (score > mx) {
                        mx = score;
                        vc.clear();
                    }
                    if (mx==score) vc.emplace_back(i, j);
                }
            }
        }

        cout << "depth " << depth << " max score " << mx << " found "
            << vc.size() << " cells" << endl;

        assert(vc.size() > 0);
        int idx = rand()%((int)vc.size());
//        cout << idx << endl;

        return PCI(vc[idx], mx);
    }

    Cell bot31() {
        /// iterative deepening of bot21
        time_out = false;
        start_time = chrono::high_resolution_clock::now();

        int depth = MIN_DEPTH;
        PCI p = bot21(depth);
        Cell bestMove = p.first;
        int mx = p.second;

        while (time_out == false && ++depth <= MAX_DEPTH) {
            p = bot21(depth);
            if (time_out) break;
            cout << "went " << depth << " steps deeper!" << endl;
            bestMove = p.first;
            mx = p.second;
        }

        auto current_time = chrono::high_resolution_clock::now();
        int elapsed = chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        cout << "made move after " << elapsed << " milis" << endl;

        clearKill();
        if (mx==-MAX_SCORE) return bot0();
        return bestMove;
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
                    tmp.addAtom(Cell(i, j), me);
                    int score1 = tmp.score1(me);
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
        if (botVersion==21) return bot21().first;
        if (botVersion==31) return bot31();
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
    Megatron747(char me, int botVersion):gr(VVS(DIM, VS(DIM))) {
        this->me = me;
        this->botVersion = botVersion;
        this->other = otherPlayer(me);
        run();
    }
};

const int DEFAULT_BOT = 31;
int main(int argc, char *argv[])
{
    srand(time(0));

    int v = DEFAULT_BOT;
    if (argc > 2) v = atoi(argv[2]);
    Megatron747 mt747(argv[1][0], v);

    return 0;
}
