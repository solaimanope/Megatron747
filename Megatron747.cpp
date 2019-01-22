#include<bits/stdc++.h>
using namespace std;

typedef pair<int,int> PII;
typedef vector< string >VS;
typedef vector< VS >VVS;

struct Cell {
    int x, y;
    Cell():x(-1), y(-1) { }
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
//const int MAX_HASH = 100003;
const int MAX_HASH = 10007;

class Grid {
    const int MAX_LOOP = 1000;
    const int PRIME_FACTOR = 997;

    VVS s;
    queue< Cell >unstable;

    int cnt[2];
    /// R -> 0
    /// G -> 1
    int charToInt(char c) {
        assert(c=='R'||c=='G');
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

    void addAtom(Cell p, char cur, bool fromUpdate) {
        int player = charToInt(cur);
        if (s[p.x][p.y]=="No") {
            s[p.x][p.y][0] = cur;
            s[p.x][p.y][1] = '1';
            cnt[player]++;
            return;
        }
        if (s[p.x][p.y][0]!=cur) {
            assert(fromUpdate);
            cnt[player]++;
            cnt[player^1]--;
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

    int getHashValue() {
        int ans = 0;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                for (char c : s[i][j]) {
                    ans += (ans*PRIME_FACTOR)%MAX_HASH+c;
                    ans %= MAX_HASH;
                }
            }
        }
        return ans;
    }
};

const int MIN_DEPTH = 2;
const int MAX_DEPTH = 4;
const int MAX_KILL = 10;

char otherPlayer(char now)
{
    if (now=='R') return 'G';
    return 'R';
}

class Timer {
    const int TIME_LIMIT = 2500;
    chrono::time_point<chrono::system_clock> start_time, current_time;

public:
    Timer() {}

    void startTimer() {
        start_time = chrono::high_resolution_clock::now();
    }

    int elapsedMilis() {
        current_time = chrono::high_resolution_clock::now();
        return chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
    }

    bool timesUp() {
        return elapsedMilis() > TIME_LIMIT;
    }
};

//struct Info {
//    Cell bestMove;
//    int score, alpha, beta;
//    Info() {}
//    Info(Cell bestMove, int score, int alpha, int beta) :
//        bestMove(bestMove), score(score), alpha(alpha), beta(beta) {
//
//    }
//};

class Megatron747 {
    const int HASH_OFFSET = 2;

    Timer timer;

    int botVersion;
    char me, other;
    Grid gr;

    vector<Cell>killerMoves[MAX_DEPTH];
    vector<int>efficiency[MAX_DEPTH];

    Cell ttable[2][MAX_DEPTH][MAX_HASH];

    int times[MAX_DEPTH], calc[MAX_DEPTH];

    void addKill(Cell c, int depth) {
//        cout << "killer " << c.x << " " << c.y << " at " << depth << endl;
        for (int i = 0; i < killerMoves[depth].size(); i++) {
            if (killerMoves[depth][i]==c) {
                efficiency[depth][i]++;

                /// push the more efficient killer move to the front
                for (int j = i-1; j >= 0; j--) {
                    if (efficiency[depth][j] < efficiency[depth][j+1]) {
                        swap(killerMoves[depth][j+1], killerMoves[depth][j]);
                        swap(efficiency[depth][j+1], efficiency[depth][j]);
                    } else {
                        break;
                    }
                }

                return;
            }
        }

        if (killerMoves[depth].size() < MAX_KILL) {
            killerMoves[depth].push_back(c);
            efficiency[depth].push_back(1);
        }
        /// since all the killer moves have efficiency > 0
        /// c is not considered as killer move
    }

    void removeKill(Cell c, int depth) {
//        cout << "killer " << c.x << " " << c.y << " at " << depth << endl;
        for (int i = 0; i < killerMoves[depth].size(); i++) {
            if (killerMoves[depth][i]==c) {
                efficiency[depth][i]--;
                /// push the less efficient killer move to the back
                for (int j = i+1; j < killerMoves[depth].size(); j++) {
                    if (efficiency[depth][j-1] < efficiency[depth][j]) {
                        swap(killerMoves[depth][j-1], killerMoves[depth][j]);
                        swap(efficiency[depth][j-1], efficiency[depth][j]);
                    } else {
                        break;
                    }
                }
                if (efficiency[depth].back()==0) {
                    killerMoves[depth].pop_back();
                    efficiency[depth].pop_back();
                }
            }
        }
    }

    int killerScore(Cell c, int depth) {
        for (int i = 0; i < killerMoves[depth].size(); i++) {
            if (killerMoves[depth][i]==c) {
                return MAX_KILL-i;
            }
        }
        return 0;
    }


    void clearShit() {
        for (int i = 0; i < MAX_DEPTH; i++) {
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

    int allowedDepth;

    void getMoves(Grid &gr, int boardHash, int depth, vector<Move> &moves) {
        int _v = depth&1;
        char currentPlayer;

        if (_v) currentPlayer = me;
        else currentPlayer = other;

        int height = allowedDepth-1-depth;

        Cell tt;
        for (int idx = MAX_DEPTH-1; idx >= height; idx--) {
            Cell in = ttable[_v][idx][boardHash];
            if (in.x==-1) continue;
            tt = in;
            break;
        }

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==currentPlayer) {
                    Move mv;
                    mv.c = Cell(i, j);
                    mv.strength = 0;

                    mv.strength += killerScore(mv.c, depth);
                    if (i==tt.x&&j==tt.y) mv.strength += MAX_KILL*2;

                    moves.push_back(mv);
                }
            }
        }

        sort(moves.begin(), moves.end());
    }

    vector< Cell >stck;
    int heuristic(Grid& gr, int depth, int alpha, int beta) {
        /// greedily make move to the cell which
        /// maximizes score1
        int _v = depth&1;

        char currentPlayer;
        if (_v) currentPlayer = me;
        else currentPlayer = other;

        if (depth==allowedDepth||gr.gameEnded()||timer.timesUp()) {
            return gr.score1(me);
        }

        int boardHash = gr.getHashValue();
        int height = allowedDepth-1-depth;

        vector<Move>moves;
        getMoves(gr, boardHash, depth, moves);

        Cell bestMove;
        int mx = -MAX_SCORE, mn = MAX_SCORE;

        for (Move m : moves) {
            Cell now = m.c;
            Grid tmp(gr);
            tmp.addAtom(now, currentPlayer, false);

            stck.push_back(now);
            int score = heuristic(tmp, depth+1, alpha, beta);
            stck.pop_back();

            if (currentPlayer==me&&score>mx) {
                mx = score;
                bestMove = now;
            }
            if (currentPlayer!=me&&score<mn) {
                mn = score;
                bestMove = now;
            }

            if (currentPlayer==me&&score>alpha) alpha = score;
            if (currentPlayer!=me&&score<beta)  beta  = score;

            if (alpha >= beta) {
                addKill(now, depth);
                break;
            } else {
                removeKill(now, depth);
            }
        }

        if (_v)  {
            ttable[1][height][boardHash] = bestMove;
            return alpha;
        } else {
            ttable[0][height][boardHash] = bestMove;
            return beta;
        }
    }

    Cell bot21(int depth = MIN_DEPTH) {
        /// greedily make move to the cell which
        /// maximizes score2
        allowedDepth = depth;
        vector<Cell>vc;
        int mx = -MAX_SCORE;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (gr.isEmpty(i, j)||gr.getPlayer(i, j)==me) {
                    Grid tmp(gr);
                    tmp.addAtom(Cell(i, j), me, false);
                    stck.push_back(Cell(i, j));
                    int score = heuristic(tmp, 0, -MAX_SCORE, MAX_SCORE);
                    stck.pop_back();
                    if (score > mx) {
                        mx = score;
                        vc.clear();
                    }
                    if (mx==score) vc.emplace_back(i, j);
                }
            }
        }

        cout << "depth " << depth << " max score " << mx << " found " << vc.size()
                << " cells using " << timer.elapsedMilis() << " milis" << endl;

        times[depth] += timer.elapsedMilis();
        calc[depth]++;

        if (vc.size()==0) vc.push_back(bot0());
        int idx = rand()%((int)vc.size());
//        cout << idx << endl;

        return vc[idx];
    }

    Cell bot31() {
        /// iterative deepening of bot21

        int depth = 0;
        Cell bestMove = bot21(depth);

        while (!timer.timesUp() && ++depth < MAX_DEPTH) {
            Cell tmp = bot21(depth);
            if (timer.timesUp()) break;
            cout << "went " << depth << " steps deeper!" << endl;
            bestMove = tmp;
        }

        for (int i = 0; i < depth; i++) {
            if (calc[i]==0) continue;
            cout << i << "->" << fixed << setprecision(2) << 1.0*times[i]/calc[i] << " ";
        }
        cout << endl;

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
                    tmp.addAtom(Cell(i, j), me, false);
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
        timer.startTimer();
        clearShit();

        Cell mv;
        if (botVersion==0) mv = bot0();
        else if (botVersion==11) mv = bot11();
        else if (botVersion==21) mv = bot21();
        else if (botVersion==31) mv = bot31();
        else assert(false);

        cout << "made move after " << timer.elapsedMilis() << " milis" << endl;

        return mv;
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
        if (me=='R') other = 'G';
        else other = 'R';
        run();
    }
};

const int DEFAULT_BOT = 31;
int main(int argc, char *argv[])
{
    srand(time(0));
    cout << argv[1][0] << " started!" << endl;

    int v = DEFAULT_BOT;
    if (argc > 2) v = atoi(argv[2]);
    Megatron747 mt747(argv[1][0], v);

    return 0;
}
