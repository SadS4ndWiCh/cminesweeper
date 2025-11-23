# cminesweeper

A simple minesweeper game written in C.

## tiles

Each tile is 1 byte long

```
  7   6   5   4   3   2   1   0   (bit)
+ - - - - - - - - - + - + - + - +
|     RESERVED      | F | O | B |
+ - - - - - - - - - + - + - + - +
          |           |   |   |-> Is bomb?
          |           |   |-> Is opened?
          |           |-> Is flagged?
          |-> Reserved
```

If a tile isn't a bomb and have bombs in neighbors, the `reserved` stores the 
total of bombs. The total of bombs uses only 4 bits.

```
  7   6   5   4   3   2   1   0   (bit)
+ - - - - - - - - - + - + - + - +
| R |  TOTAL BOMBS  | F | O | B |
+ - - - - - - - - - + - + - + - +
  |       |           |   |   |-> Is bomb?
  |       |           |   |-> Is opened?
  |       |           |-> Is flagged?
  |       |-> Total bombs
  |-> Reserved
```

## TODO

- [x] Hello World
- [x] Create grid
- [x] Render grid
- [x] Render tile
    - [x] Render tile based in it state
- [x] Implement floodfill
- [x] Get user input
- [x] Do floodfill in position
- [x] Toggle flag
- [ ] Handle all user inputs
- [ ] Show remaining bombs
- [ ] Implement win and lost
- [ ] Restart game
- [ ] Refactor to use vec2
- [ ] Refactor rendering
- [ ] Refactor UI with simple TUI
    - [ ] Move "cursor" with W A S D
    - [ ] Floodfill with ENTER
    - [ ] Toggle flag with F
    - [ ] Add time counter
- [ ] Save game
- [ ] Load game
