
#include <stdint.h>

namespace Tetris {
    struct XY { char x; char y; };
    struct Kick { XY cw; XY ccw; };

    constexpr char fieldWidth = 10;
    constexpr char fieldHeight = 22;

    namespace Tetromino {
        constexpr char I = 0;
        constexpr char J = 1;
        constexpr char L = 2;
        constexpr char O = 3;
        constexpr char S = 4;
        constexpr char T = 5;
        constexpr char Z = 6;


        struct Piece {
            char type;
            uint8_t rot; // cw: 0,1,2,3
            XY pos[4];
        };

        // 3x3 tetromino cw rotation data
        constexpr XY Rot3[3 /* y */][3 /* x */] = {
            { {+2,0}, {+1,+1}, {0,+2} },
            { {+1,-1}, {0,0}, {-1,+1} },
            { {0,-2}, {-1,-1}, {-2,0} }
        };
        // 4x4 tetromino cw rotation data
        constexpr XY Rot4[4 /* y */][4 /* x */] = {
            { {+3,0}, {+2,+1}, {+1,+2}, {0,+3} },
            { {+2,-1}, {+1,0}, {0,+1}, {-1,+2} },
            { {+1,-2}, {0,-1}, {-1,0}, {-2,+1} },
            { {0,-3}, {-1,-2}, {-2,-1}, {-3,0} }
        };

        // Normal tetromino wall kick data
        constexpr Kick NKick[4 /* rotation */][5 /* test */] = {
            { {{0,0}, {0,0}}, {{-1,0}, {1,0}}, {{-1,1}, {1,-1}}, {{0,-2}, {0,2}}, {{-1,-2}, {1,2}} },
            { {{0,0}, {0,0}}, {{1,0}, {-1,0}}, {{1,-1}, {-1,1}}, {{0,2}, {0,-2}}, {{1,2}, {-1,-2}} },
            { {{0,0}, {0,0}}, {{1,0}, {-1,0}}, {{1,1}, {-1,-1}}, {{0,-2}, {0,2}}, {{1,-2}, {-1,2}} },
            { {{0,0}, {0,0}}, {{-1,0}, {1,0}}, {{-1,-1}, {1,1}}, {{0,2}, {0,-2}}, {{-1,2}, {1,-2}} }
        };
        // I tetromino wall kick data
        constexpr Kick IKick[4 /* rotation */][5 /* test */] = {
            { {{0,0}, {0,0}}, {{-2,0}, {2,0}}, {{1,0}, {-1,0}}, {{-2,-1}, {2,1}}, {{1,2}, {-1,-2}} },
            { {{0,0}, {0,0}}, {{-1,0}, {1,0}}, {{2,0}, {-2,0}}, {{-1,2}, {1,-2}}, {{2,-1}, {-2,1}} },
            { {{0,0}, {0,0}}, {{2,0}, {-2,0}}, {{-1,0}, {1,0}}, {{2,1}, {-2,-1}}, {{-1,-2}, {1,2}} },
            { {{0,0}, {0,0}}, {{1,0}, {-1,0}}, {{-2,0}, {2,0}}, {{1,-2}, {-1,2}}, {{-2,1}, {2,-1}} }
        };

        // Tetromino initial state
        constexpr Piece Init[/* tetromino */] = {
            { 0, 0, {{0,1}, {1,1}, {2,1}, {3,1}} }, // I
            { 1, 0, {{0,0}, {0,1}, {1,1}, {2,1}} }, // J
            { 2, 0, {{2,0}, {0,1}, {1,1}, {2,1}} }, // L
            { 3, 0, {{0,0}, {1,0}, {0,1}, {1,1}} }, // O
            { 4, 0, {{1,0}, {2,0}, {0,1}, {1,1}} }, // S
            { 5, 0, {{1,0}, {0,1}, {1,1}, {2,1}} }, // T
            { 6, 0, {{0,0}, {1,0}, {1,1}, {2,1}} }, // Z
        };

    }

    // State ===============================================
    
    int score = 0;
    char field[fieldHeight][fieldWidth] = {};
    Tetromino::Piece piece[4] = {}; // piece[0] = current piece

    // Random ==============================================
    
    uint32_t prng = 0;
    uint32_t rand() { // xorshift32
        prng ^= prng << 13;
        prng ^= prng >> 17;
        prng ^= prng << 5;
        return prng;
    }

    // Piece operations ====================================

    bool _valid(Tetromino::Piece p) {
        for(char i = 0; i < 4; i++) {
            char x = p.pos[i].x;
            char y = p.pos[i].y;
            if(0 > x || x >= fieldWidth) return false;
            if(y >= fieldHeight) return false;
            if(y >= 0 && field[y][x]) return false;
        }
        return true;
    }

    XY _rotIndex(char type, char pos, char rot) {
        XY idx = Tetromino::Init[type].pos[pos];
        for(char r = 0; r < rot; r++) {
            if(type == Tetromino::I) {
                idx = {
                    static_cast<char>(idx.x + Tetromino::Rot4[idx.y][idx.x].x),
                    static_cast<char>(idx.y + Tetromino::Rot4[idx.y][idx.x].y)
                };
            } else {
                idx = {
                    static_cast<char>(idx.x + Tetromino::Rot3[idx.y][idx.x].x),
                    static_cast<char>(idx.y + Tetromino::Rot3[idx.y][idx.x].y)
                };
            }
        }
        return idx;
    }

    Tetromino::Piece _cw(Tetromino::Piece p, char kick) {
        if(p.type == Tetromino::O || kick >= 5) return p;
        auto n = p;
        for(char i = 0; i < 4; i++) {
            XY idx = _rotIndex(n.type, i, n.rot);
            if(n.type == Tetromino::I) {
                n.pos[i].x += Tetromino::Rot4[idx.y][idx.x].x + Tetromino::IKick[n.rot][kick].cw.x;
                n.pos[i].y += Tetromino::Rot4[idx.y][idx.x].y + Tetromino::IKick[n.rot][kick].cw.y;
            } else {
                n.pos[i].x += Tetromino::Rot3[idx.y][idx.x].x + Tetromino::NKick[n.rot][kick].cw.x;
                n.pos[i].y += Tetromino::Rot3[idx.y][idx.x].y + Tetromino::NKick[n.rot][kick].cw.y;
            }
        }
        n.rot += 1; n.rot %= 4;
        return _valid(n) ? n : _cw(p, kick + 1);
    }

    Tetromino::Piece _ccw(Tetromino::Piece p, char kick) {
        if(p.type == Tetromino::O || kick >= 5) return p;
        auto n = p;
        for(char i = 0; i < 4; i++) {
            XY idx = _rotIndex(n.type, i, n.rot + 1); // Use rot+1 index for CCW rotation
            if(n.type == Tetromino::I) {
                n.pos[i].x += Tetromino::Rot4[idx.y][idx.x].x + Tetromino::IKick[n.rot][kick].ccw.x;
                n.pos[i].y += Tetromino::Rot4[idx.y][idx.x].y + Tetromino::IKick[n.rot][kick].ccw.y;
            } else {
                n.pos[i].x += Tetromino::Rot3[idx.y][idx.x].x + Tetromino::NKick[n.rot][kick].ccw.x;
                n.pos[i].y += Tetromino::Rot3[idx.y][idx.x].y + Tetromino::NKick[n.rot][kick].ccw.y;
            }
        }
        n.rot -= 1; n.rot %= 4;
        return _valid(n) ? n : _cw(p, kick + 1);
    }

    Tetromino::Piece _left(Tetromino::Piece p) {
        auto n = p;
        for(char i = 0; i < 4; i++) n.pos[i].x -= 1;
        if(_valid(n)) return n;
        return p;
    }

    Tetromino::Piece _right(Tetromino::Piece p) {
        auto n = p;
        for(char i = 0; i < 4; i++) n.pos[i].x += 1;
        if(_valid(n)) return n;
        return p;
    }

    bool _drop() {
        auto n = piece[0];
        for(char i = 0; i < 4; i++) n.pos[i].y += 1;
        if(_valid(n)) {
            piece[0] = n;
            return true;
        }
        return false;
    }

    // Game operations =====================================

    void _spawn() {
        char y = -2, x = 3; // Spawn offset
        if(piece[0].type == Tetromino::O) x = 4;
        for(char i = 0; i < 4; i++) {
            piece[0].pos[i].x += x;
            piece[0].pos[i].y += y;
        }
    }

    bool _tetris(char y) {
        bool tetris = true;
        for(char x = 0; x < fieldWidth; x++) {
            if(!field[y][x]) { tetris = false; break; }
        }
        return tetris;
    }

    bool _lockdown() {
        // Set current piece on field
        for(char i = 0; i < 4; i++) {
            XY p = piece[0].pos[i];
            if(p.y < 0) return false; // Game Over
            field[p.y][p.x] = 1;
        }
        // Handle tetris
        for(char y = fieldHeight - 1; y >= 0; y--) {
            while(_tetris(y)) {
                score++;
                for(char y2 = y; y2 > 0; y2--) {
                    for(char x = 0; x < fieldWidth; x++) field[y2][x] = field[y2-1][x];
                }
            }
        }
        // Update upcoming pieces
        for(char i = 0; i < 3; i++) piece[i] = piece[i+1];
        piece[3] = Tetromino::Init[rand() % 7];
        _spawn();
        return true;
    }

    void init(uint32_t seed) {
        score = 0;
        prng = seed;
        // Clear play field
        for(char y = 0; y < fieldHeight; y++) {
            for(char x = 0; x < fieldWidth; x++) {
                field[y][x] = 0;
            }
        }
        // Init upcoming pieces
        for(char i = 0; i < 4; i++) piece[i] = Tetromino::Init[rand() % 7];
        _spawn();
    }

    bool tick() { // Tick or soft drop
        if(!_drop()) {
            return _lockdown();
        }
        return true;
    }

    bool drop() { // Hard drop
        while(_drop());
        return _lockdown();
    }

    void rotate(bool ccw) {
        piece[0] = ccw ? _ccw(piece[0], 0) : _cw(piece[0], 0);
    }

    void slide(bool left) {
        piece[0] = left ? _left(piece[0]) : _right(piece[0]);
    }

}
