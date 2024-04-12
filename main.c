#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>

#define KIT_IMPL
#include "kit.h"

#define TETROMINOS 7
#define ROTATIONS 4
#define ELEMENTS 16 // 4 rows * 4 columns
#define RGB_COMPONENTS 3

// Define the tetrominos with linear arrays for each rotation
int tetrominos[TETROMINOS][ROTATIONS][ELEMENTS] = {
    {// I piece
     {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
     {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}},

    {// O piece
     {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}},
    {// T piece
     {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}},
    {// S piece
     {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}},
    {// Z piece
     {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0}},
    {// J piece
     {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}},
    {// L piece
     {0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
     {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}}};

// Tetromino colors matching the tetromino array index
int tetrominoColors[TETROMINOS][RGB_COMPONENTS] = {
    {0, 255, 255}, // I piece (Cyan)
    {255, 255, 0}, // O piece (Yellow)
    {220, 0, 220}, // T piece (Purple)
    {0, 200, 0},   // S piece (Green)
    {255, 60, 60}, // Z piece (Red)
    {70, 70, 255}, // J piece (Blue)
    {255, 165, 0}  // L piece (Orange)
};

int fieldw = 10;
int fieldh = 18;
int blocksz = 25;
unsigned int *field;

int screenw = 0;
int screenh = 0;

int currentPiece = 0;
int nextPiece = 0;
int holdPiece = -1;  // Initially, no piece is held.
bool canHold = true; // Player is allowed to hold when a new piece starts.
int currentRotation = 0;
int currentX = 0;
int currentY = 0;
int gameSpeed = 1200; // Time (in ms) before the piece moves down automatically
int lastTime = 0;
int score = 0; // Initialize score
int level = 1; // Current level

int bag[7] = {0};
int bagIndex = 0;
bool gameover = true;
bool pause = false;
bool shouldClose = false;

void shuffle_bag()
{
    srand(time(NULL));

    for (int i = 6; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }
}

void fill_and_shuffle_bag()
{
    for (int i = 0; i < 7; i++)
        bag[i] = i;

    shuffle_bag();
}

int get_next_piece_from_bag()
{
    if (bagIndex >= 7)
    { // Assuming bag is always initialized and full
        fill_and_shuffle_bag();
        bagIndex = 0;
    }
    int pieceIndex = bag[bagIndex];
    bagIndex++;
    return pieceIndex;
}

bool does_piece_fit(int piece, int rotation, int x, int y)
{
    for (int py = 0; py < 4; py++)
    {
        for (int px = 0; px < 4; px++)
        {
            int pi = py * 4 + px;
            int fx = x + px;
            int fy = y + py;
            if (tetrominos[piece][rotation][pi] == 1)
            {
                if (fx < 0 || fx >= fieldw || fy >= fieldh || field[fy * fieldw + fx] != 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

int calculate_ghost_position()
{
    int ghostY = currentY;
    while (does_piece_fit(currentPiece, currentRotation, currentX, ghostY + 1))
    {
        ghostY++;
    }
    return ghostY;
}

void update_level_and_speed()
{
    const int levelThreshold = 1200; // Every 1000 points, increase the level
    int newLevel = score / levelThreshold + 1;

    if (newLevel > level)
    {
        level = newLevel;
        gameSpeed = 1200 - (level - 1) * 50;             // Decrease speed by 50 ms per level
        gameSpeed = (gameSpeed > 200) ? gameSpeed : 300; // Don't go below 300 ms
    }
}

void remove_complete_lines()
{
    int row, col, moveRow;
    int linesCleared = 0;
    bool isRowComplete;

    for (row = fieldh - 1; row >= 0; row--)
    {
        isRowComplete = true;
        for (col = 0; col < fieldw; col++)
        {
            if (field[row * fieldw + col] == 0)
            {
                isRowComplete = false;
                break;
            }
        }

        if (isRowComplete)
        {
            for (moveRow = row; moveRow > 0; moveRow--)
            {
                for (col = 0; col < fieldw; col++)
                {
                    field[moveRow * fieldw + col] = field[(moveRow - 1) * fieldw + col];
                }
            }
            for (col = 0; col < fieldw; col++)
            {
                field[col] = 0; // Clear the top row after shifting down
            }
            row++; // Recheck the same row index because it now contains new data
            linesCleared++;
        }
    }

    if (linesCleared > 0)
    {
        switch (linesCleared)
        {
        case 1:
            score += 100; // Single line
            break;
        case 2:
            score += 300; // Double
            break;
        case 3:
            score += 500; // Triple
            break;
        case 4:
            score += 800; // Tetris
            break;
        }

        update_level_and_speed(); // Update the level and game speed based on score
    }
}

void lock_piece()
{
    int py, px;
    for (py = 0; py < 4; py++)
    {
        for (px = 0; px < 4; px++)
        {
            if (tetrominos[currentPiece][currentRotation][py * 4 + px] == 1)
            {
                field[(currentY + py) * fieldw + (currentX + px)] = 1; // Mark the cell as occupied
            }
        }
    }

    remove_complete_lines();
}

void advance_to_next_piece()
{
    lock_piece();
    currentPiece = nextPiece;
    nextPiece = get_next_piece_from_bag();
    currentRotation = 0;
    currentX = fieldw / 2 - 2;
    currentY = 0;
    canHold = true; // Allow holding again

    if (!does_piece_fit(currentPiece, currentRotation, currentX, currentY))
    {
        gameover = true;
    }
}

bool handle_input(kit_Context *ctx)
{
    if (kit_key_pressed(ctx, VK_ESCAPE))
    {
        pause = true;
    }
    else if (kit_key_pressed(ctx, VK_RIGHT) || kit_key_pressed(ctx, 0x44)) // D
    {
        if (does_piece_fit(currentPiece, currentRotation, currentX + 1, currentY))
        {
            currentX++;
        }
    }
    else if (kit_key_pressed(ctx, VK_LEFT) || kit_key_pressed(ctx, 0x41)) // A
    {
        if (does_piece_fit(currentPiece, currentRotation, currentX - 1, currentY))
        {
            currentX--;
        }
    }
    else if (kit_key_pressed(ctx, VK_DOWN) || kit_key_pressed(ctx, 0x53)) // S
    {
        if (does_piece_fit(currentPiece, currentRotation, currentX, currentY + 1))
        {
            currentY++;
        }
    }
    else if (kit_key_pressed(ctx, VK_UP) || kit_key_pressed(ctx, 0x57)) // W
    {
        if (does_piece_fit(currentPiece, (currentRotation + 1) % 4, currentX, currentY))
        {
            currentRotation = (currentRotation + 1) % 4;
        }
    }
    else if (kit_key_pressed(ctx, VK_SPACE))
    {
        if (does_piece_fit(currentPiece, currentRotation, currentX, currentY))
        {
            int ghostY = calculate_ghost_position();
            currentY = ghostY;
            advance_to_next_piece();
        }
    }
    else if (kit_key_pressed(ctx, VK_SHIFT))
    {
        if (canHold)
        {
            if (holdPiece < 0)
            {
                holdPiece = currentPiece;
                currentPiece = nextPiece;
                nextPiece = get_next_piece_from_bag();
            }
            else
            {
                int tmp = currentPiece;
                currentPiece = holdPiece;
                holdPiece = tmp;
            }
            currentRotation = 0;
            currentX = floor(fieldw / 2) - 2;
            currentY = 0;
            canHold = false;
        }
    }

    return false;
}

void update_game(double dt)
{
    static double elapsedTime = 0; // ensure this is not reset unintentionally anywhere else
    elapsedTime += dt;

    // Convert gameSpeed from milliseconds to seconds for comparison
    double gameSpeedInSeconds = gameSpeed / 1000.0;

    if (elapsedTime >= gameSpeedInSeconds)
    {
        if (does_piece_fit(currentPiece, currentRotation, currentX, currentY + 1))
        {
            currentY++;
        }
        else
        {
            advance_to_next_piece();
        }
        elapsedTime = 0;
    }
}

void init_game()
{
    fill_and_shuffle_bag();
    currentPiece = get_next_piece_from_bag();
    nextPiece = get_next_piece_from_bag();
    holdPiece = -1;
    currentRotation = 0;
    currentX = floor(fieldw / 2 - 2);
    currentY = 0;
    score = 0;
    canHold = true;
    memset(field, 0, fieldw * fieldh * sizeof(int));
}

void show_info(kit_Context *ctx)
{
    kit_clear(ctx, KIT_BLACK);

    if (score > 0)
    {
        int x = screenw / 2 - (kit_text_width(ctx->font, "Score:") / 2);
        int y = screenh / 2 - 80;
        kit_draw_text(ctx, KIT_WHITE, "Score:", x, y);

        char scoretxt[128] = {0};
        sprintf(scoretxt, "%d", score);
        x = screenw / 2 - (kit_text_width(ctx->font, scoretxt) / 2);
        y = screenh / 2 - 60;
        kit_draw_text(ctx, KIT_WHITE, scoretxt, x, y);
    }
    else
    {
        int x = screenw / 2 - (kit_text_width(ctx->font, "=== Freetris ===") / 2);
        int y = screenh / 2 - 80;
        kit_draw_text(ctx, KIT_WHITE, "=== Freetris ===", x, y);
    }

    int x = 130;
    int y = screenh / 2 - 30;
    if (pause)
    {
        kit_draw_text(ctx, KIT_WHITE, "- ENTER To Resume", x, y);
    }
    else
    {
        if (score <= 0)
            kit_draw_text(ctx, KIT_WHITE, "- ENTER To Start", x, y);
        else
            kit_draw_text(ctx, KIT_WHITE, "- ENTER To Restart", x, y);
    }

    y = screenh / 2;
    kit_draw_text(ctx, KIT_WHITE, "- ESC To Quit / Pause", x, y);

    y = screenh / 2 + 40;
    kit_draw_text(ctx, KIT_WHITE, "HELP:", x, y);
    y = screenh / 2 + 60;
    kit_draw_text(ctx, KIT_WHITE, "- WASD or Arrow Keys To Move", x, y);
    y = screenh / 2 + 80;
    kit_draw_text(ctx, KIT_WHITE, "- UP or W To Rotate", x, y);
    y = screenh / 2 + 100;
    kit_draw_text(ctx, KIT_WHITE, "- Shift To Hold", x, y);
    y = screenh / 2 + 120;
    kit_draw_text(ctx, KIT_WHITE, "- Space To Drop", x, y);

    if (kit_key_pressed(ctx, VK_RETURN))
    {
        if (gameover)
        {
            gameover = false;
            pause = false;
            init_game();
        }
        else if (pause)
        {
            gameover = false;
            pause = false;
        }
    }

    if (kit_key_pressed(ctx, VK_ESCAPE))
    {
        shouldClose = true;
    }
}

void draw_piece(kit_Context *ctx, int pieceIndex, int rotation, int x, int y, int a)
{
    int py, px;
    int r, g, b;

    // Extract RGB values from the tetrominoColors array
    r = tetrominoColors[pieceIndex][0];
    g = tetrominoColors[pieceIndex][1];
    b = tetrominoColors[pieceIndex][2];

    for (py = 0; py < 4; py++)
    {
        for (px = 0; px < 4; px++)
        {
            int cell = tetrominos[pieceIndex][rotation][py * 4 + px];
            if (cell == 1)
            {
                // Draw block border in black
                kit_Rect borderRect = {(x + px) * blocksz, (y + py) * blocksz, blocksz, blocksz};
                kit_draw_rect(ctx, KIT_BLACK, borderRect);

                // Draw the actual block with specified color and alpha
                kit_Rect innerRect = (kit_Rect){(x + px) * blocksz + 1, (y + py) * blocksz + 1, blocksz - 2, blocksz - 2};
                kit_draw_rect(ctx, kit_rgba(r, g, b, a), innerRect);
            }
        }
    }
}

void draw_level(kit_Context *ctx, int y)
{
    char buffer[128];

    sprintf(buffer, "Level: ");
    kit_draw_text(ctx, KIT_WHITE, buffer, blocksz * fieldw + blocksz * 3, blocksz * y);

    sprintf(buffer, "%d", level);
    kit_draw_text(ctx, KIT_WHITE, buffer, blocksz * fieldw + blocksz * 3, blocksz * (y + 1));
}

void draw_score(kit_Context *ctx, int y)
{
    char buffer[50];

    sprintf(buffer, "Score: ");
    kit_draw_text(ctx, KIT_WHITE, buffer, blocksz * fieldw + blocksz * 3, blocksz * y);

    sprintf(buffer, "%d", score);
    kit_draw_text(ctx, KIT_WHITE, buffer, blocksz * fieldw + blocksz * 3, blocksz * (y + 1));
}

void draw_next_piece(kit_Context *ctx, int y)
{
    char buffer[50];
    int previewX = fieldw + 2;
    int previewY = y + 1;

    sprintf(buffer, "Next:");
    kit_draw_text(ctx, KIT_WHITE, buffer, blocksz * fieldw + blocksz * 3, blocksz * y);

    draw_piece(ctx, nextPiece, 0, previewX, previewY, 255);
}

void draw_held_piece(kit_Context *ctx, int y)
{
    char buffer[50];
    int previewX = fieldw + 2;
    int previewY = y + 1;

    sprintf(buffer, "Hold:");
    kit_draw_text(ctx, KIT_WHITE, buffer, blocksz * fieldw + blocksz * 3, blocksz * y);

    if (holdPiece != -1)
    { // Assume -1 represents no piece held
        draw_piece(ctx, holdPiece, 0, previewX, previewY, 255);
    }
}

void draw_game(kit_Context *ctx)
{
    kit_Color black = kit_rgba(0, 0, 0, 35);
    kit_Color white = kit_rgba(255, 255, 255, 35);
    kit_Rect rect;

    // Clear the entire canvas
    kit_clear(ctx, black);

    // Draw the playfield
    int x, y;
    for (y = 0; y < fieldh; y++)
    {
        for (x = 0; x < fieldw; x++)
        {
            rect = (kit_Rect){x * blocksz, y * blocksz, blocksz, blocksz};
            if (field[y * fieldw + x] != 0)
            {
                kit_draw_rect(ctx, black, rect);
                rect = (kit_Rect){x * blocksz + 1, y * blocksz + 1, blocksz - 2, blocksz - 2};
                kit_draw_rect(ctx, white, rect);
            }
        }
    }

    // Calculate and draw ghost piece
    int ghostY = calculate_ghost_position();
    float alpha = 120;
    draw_piece(ctx, currentPiece, currentRotation, currentX, ghostY, alpha);

    // Draw the current piece
    draw_piece(ctx, currentPiece, currentRotation, currentX, currentY, 255);

    // Draw UI elements
    draw_level(ctx, 1);
    draw_score(ctx, 4);
    draw_next_piece(ctx, 7);
    draw_held_piece(ctx, 13);

    kit_Color gray = kit_rgba(200, 200, 200, 80);
    kit_draw_line(ctx, gray, 0, 0, 0, fieldh * blocksz);
    kit_draw_line(ctx, gray, fieldw * blocksz, 0, fieldw * blocksz, fieldh * blocksz);
}

int main(int argc, char **argv)
{
    // Setup ===============================
    screenw = fieldw * blocksz + blocksz * 7;
    screenh = fieldh * blocksz;

    field = (unsigned int *)calloc(fieldw * fieldh, sizeof(int));

    kit_Context *ctx = kit_create("Freetris", screenw, screenh, KIT_SCALE2X);
    double dt;

    // Main Loop ===============================
    while (kit_step(ctx, &dt))
    {
        if (shouldClose)
        {
            break;
        }

        if (gameover || pause)
        {
            show_info(ctx);
        }
        else
        {
            handle_input(ctx);
            update_game(dt);
            draw_game(ctx);
        }
    }

    free(field);
    kit_destroy(ctx);
    return 0;
}