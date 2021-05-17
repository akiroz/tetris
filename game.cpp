
#include <stdint.h>

namespace Tetris {
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

        struct XY { char x; char y; };
        struct Kick { XY cw; XY ccw; };

        struct Piece {
            char type;
            char rot; // cw: 0,1,2,3
            XY pos[4];
        };

        // 3x3 tetromino cw rotation data
        constexpr XY Rot3[/* y */][/* x */] = {
            { {+2,0}, {+1,+1}, {0,+2} },
            { {+1,-1}, {0,0}, {-1,+1} },
            { {0,-2}, {-1,-1}, {-2,0} },
        };
        // 4x4 tetromino cw rotation data
        constexpr XY Rot4[/* y */][/* x */] = {
            { {+3,0}, {+2,+1}, {+1,+2}, {0,+3} },
            { {+2,-1}, {+1,0}, {0,+1}, {-1,+2} },
            { {+1,-2}, {0,-1}, {-1,0}, {-2,+1} },
            { {0,-3}, {-1,-2}, {-2,-1}, {-3,0} }
        };

        // Normal tetromino wall kick data
        constexpr Kick NKick[/* rotation */][/* test */] = {
            { {{0,0}, {0,0}}, {{-1,0}, {1,0}}, {{-1,1}, {1,-1}}, {{0,-2}, {0,2}}, {{-1,-2}, {1,2}} },
            { {{0,0}, {0,0}}, {{1,0}, {-1,0}}, {{1,-1}, {-1,1}}, {{0,2}, {0,-2}}, {{1,2}, {-1,-2}} },
            { {{0,0}, {0,0}}, {{1,0}, {-1,0}}, {{1,1}, {-1,-1}}, {{0,-2}, {0,2}}, {{1,-2}, {-1,2}} },
            { {{0,0}, {0,0}}, {{-1,0}, {1,0}}, {{-1,-1}, {1,1}}, {{0,2}, {0,-2}}, {{-1,2}, {1,-2}} }
        };
        // I tetromino wall kick data
        constexpr Kick IKick[/* rotation */][/* test */] = {
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

    Tetromino::Piece _cw(Tetromino::Piece p, char kick) {
        if(p.type == Tetromino::O || kick >= 5) return p;
        Piece n = p;
        for(char i = 0; i < 4; i++) {
            // Rotate
            // Kick
        }
        return _valid(n) ? n : _cw(p, kick + 1);
    }

    Tetromino::Piece _ccw(Tetromino::Piece p, char kick) {
        if(p.type == Tetromino::O || kick >= 5) return p;
        auto n = p;
        for(char i = 0; i < 4; i++) {
            // Rotate
            // Kick
        }
        return _valid(n) ? n : _cw(p, kick + 1);
    }

    Tetromino::Piece _left(Tetromino::Piece p) {
        auto n = p;
        for(char i = 0; i < 4; i++) n.pos[i].x -= 1;
        if(_valid(n)) return n
            return p;
    }

    Tetromino::Piece _right(Tetromino::Piece p) {
        auto n = p;
        for(char i = 0; i < 4; i++) n.pos[i].x += 1;
        if(_valid(n)) return n
            return p;
    }

    bool _drop() {
        auto n = p;
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

    void _lockdown() {
        // Set current piece on field
        for(char i = 0; i < 4; i++) {
            XY p = piece[0].pos[i];
            field[p.y][p.x] = 1;
        }
        // Update upcoming pieces
        for(char i = 0; i < 3; i++) piece[i] = piece[i+1];
        piece[3] = Tetromino::Init[rand() % 7];
        _spawn();
    }

    void init(uint32_t seed) {
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

    void tick() { // Tick or soft drop
        if(!_drop()) _lockdown();
    }

    void drop() { // Hard drop
        while(_drop());
        _lockdown();
    }

    void rotate(bool ccw) {
        piece[0] = ccw ? _ccw(piece[0], 0) : _cw(piece[0], 0);
    }

    void slide(bool left) {
        piece[0] = left ? _left(piece[0]) : _right(piece[0]);
    }

}
