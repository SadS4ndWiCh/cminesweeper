#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 10
#define HEIGHT 10
#define TOTAL (WIDTH * HEIGHT)
#define INPUT_BUFFER 8

#define INDEX(x, y) (y * WIDTH + x)

typedef struct {
    size_t x;
    size_t y;
} vec2;

typedef struct {
    size_t count;
    vec2 items[8];
} neighbors_t;

//  8 bits
// [XXXXXXXX]
//  ||||||||-> Bomb
//  |||||||-> Opened
//  ||||||-> Flaged      
//  |||||-> Reserved    
//  ||||-> Reserved
//  |||-> Reserved
//  ||-> Reserved
//  |-> Reserved

#define TILE_DFLT 0
#define TILE_BOMB 1 << 0
#define TILE_OPEN 1 << 1
#define TILE_FLAG 1 << 2
#define TILE_METADATA (TILE_BOMB | TILE_OPEN | TILE_FLAG)

#define GET_TILE_RESERVED(tile) (tile >> 3)
#define SET_TILE_RESERVED(tile, value) ((tile & TILE_METADATA) | (value << 3))

#define ACTION_REVEAL 'O'
#define ACTION_TOGGLE_FLAG 'F'

typedef struct {
    size_t width;
    size_t height;
    char* items;
} grid_t;

void Input_getAction(size_t* x, size_t* y, char* action);

int Grid_new(grid_t* grid, size_t width, size_t height, size_t bombs);
void Grid_getNeighbors(grid_t grid, size_t x, size_t y, neighbors_t* neighbors);
void Grid_floodfill(grid_t grid, size_t x, size_t y);
void Grid_toggleFlag(grid_t grid, size_t x, size_t y);
void Grid_render(grid_t grid);

int Grid_new(grid_t* grid, size_t width, size_t height, size_t bombs) {
    grid->width = width;
    grid->height = height;
    grid->items = (char*) malloc(width * height * sizeof(int));

    if (grid->items == NULL) {
        return 1;
    }

    size_t size = width * height;
    for (size_t i = 0; i < size; ++i) {
        grid->items[i] = TILE_DFLT;
    }

    while (bombs > 0) {
        size_t bx = rand() % WIDTH;
        size_t by = rand() % HEIGHT;
        size_t index = INDEX(bx, by);

        if (!(grid->items[index] & TILE_BOMB)) {
            grid->items[index] |= TILE_BOMB;
            bombs--;

            neighbors_t neighbors = {0};
            Grid_getNeighbors(*grid, bx, by, &neighbors);

            for (size_t i = 0; i < neighbors.count; i++) {
                vec2 neighborPos = neighbors.items[i];
                int index = INDEX(neighborPos.x, neighborPos.y);

                int neighborTile = grid->items[index];
                if (neighborTile & TILE_BOMB) {
                    continue;
                }


                grid->items[index] = SET_TILE_RESERVED(neighborTile, (GET_TILE_RESERVED(neighborTile) + 1));
            }
        }
    }
    
    return 0;
}

void Grid_getNeighbors(grid_t grid, size_t x, size_t y, neighbors_t* neighbors) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= (int) grid.width || ny < 0 || ny >= (int) grid.height) {
                continue;
            }

            neighbors->items[neighbors->count++] = (vec2) { nx, ny };
        }
    }
}

void Input_getAction(size_t* x, size_t* y, char* action) {
    static char buf[INPUT_BUFFER] = {0};

    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%lu %lu %c", x, y, action);
}

void Grid_render(grid_t grid) {
    for (size_t y = 0; y < grid.height; ++y) {
        for (size_t x = 0; x < grid.width; ++x) {
            int tile = grid.items[INDEX(x, y)];

            if (!(tile & TILE_OPEN) && tile & TILE_FLAG) {
                printf("[F]");
            } else if (!(tile & TILE_OPEN)) {
                printf("[@]");
            } else if (tile & TILE_BOMB) {
                printf("[*]");
            } else {
                char reserved = GET_TILE_RESERVED(tile);
                if (reserved > 0) {
                    printf("[%d]", reserved);
                } else {
                    printf("[ ]");
                }
            }
        }

        printf("\n");
    }
}

void Grid_floodfill(grid_t grid, size_t x, size_t y) {
    size_t index = INDEX(x, y);
    int tile = grid.items[index];
    if (tile & TILE_OPEN || tile & TILE_FLAG) {
        return;
    }

    grid.items[index] |= TILE_OPEN;

    if (!(tile & TILE_BOMB) && GET_TILE_RESERVED(tile) == 0) {
        neighbors_t neighbors = {0};
        Grid_getNeighbors(grid, x, y, &neighbors);

        for (size_t i = 0; i < neighbors.count; i++) {
            vec2 neighborPos = neighbors.items[i];

            Grid_floodfill(grid, neighborPos.x, neighborPos.y);
        }
    }
}

void Grid_toggleFlag(grid_t grid, size_t x, size_t y) {
    char tile = grid.items[INDEX(x, y)];
    if (tile & TILE_OPEN) {
        return;
    }

     grid.items[INDEX(x, y)] ^= TILE_FLAG;
}

int main(void) {
    srand(time(NULL));

    size_t sx, sy;
    char action;

    grid_t grid = {0};
    if (Grid_new(&grid, 10, 10, 10) != 0) {
        fprintf(stderr, "ERROR: main: failed to alloc grid");
        exit(1);
    }

    do {
        Grid_render(grid);

        Input_getAction(&sx, &sy, &action);

        switch (action) {
            case ACTION_REVEAL:
                Grid_floodfill(grid, sx, sy);
                break;
            case ACTION_TOGGLE_FLAG:
                Grid_toggleFlag(grid, sx, sy);
                break;
        }
    } while (1);

    return 0;
}
