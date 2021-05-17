Vars:

- `Tetris::fieldWidth` = 10
- `Tetris::fieldHeight` = 22
- `Tetris::field[y][x]` non-zero = occupied
- `Tetris::piece[4]` current piece followed by 3 upcoming pieces

Funs:

- `Tetris::init(uint32_t randomSeed)`
- `bool Tetris::tick()` Game tick or soft drop, return `false` on game over
- `bool Tetris::drop()` Hard drop, return `false` on game over
- `Tetris::shift(bool left)` Shift current piece
- `Tetris::rotate(bool ccw)` Rotate current piece

Compile:
```
$ g++ -std=c++17 -lncurses main.cpp
```
