#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "conio/conio.h"

//#include <stdlib.h>

// DEFS
// ------------------------------------------------------------------
typedef uint8_t byte;
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// CONSTS
// ------------------------------------------------------------------
#define DEBUG
#define INCREASE 43 // ASCII CODE for +
#define DECREASE 45 // ASCII CODE for -

// Screen resolution 128x64px
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define MODULE 8 // Module for tiles (bits) used for screen division / pages
#define EDGES 2 // Used only for pseudo buffer screen purposes

#define VIEWPORT_WIDTH SCREEN_WIDTH / MODULE
#define VIEWPORT_HEIGHT SCREEN_HEIGHT / MODULE

// MAXIMUM: 16rows x 18cols => 288 byte + 128 byte (SSD1306 page)
#define PLAYFIELD_ROWS (VIEWPORT_HEIGHT + EDGES) // AXIS Y
#define PLAYFIELD_COLS (VIEWPORT_WIDTH + EDGES) // AXIS X

// Para que el mapa sea circular, la altura tiene que ser un número en potencia base 2.
#define TILEMAP_HEIGHT 29
#define TILEMAP_WIDTH PLAYFIELD_COLS


// VARS
// ------------------------------------------------------------------
const byte ucTiles[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Empty  (0)
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Fill   (1) 
};

const byte tileMap[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
  {0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  0},  // -- FLOOR  
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0},

  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0},  
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
  {0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0},
};

static byte bPlayfield[PLAYFIELD_ROWS * PLAYFIELD_COLS];
static int iScrollX, iScrollY;
const char *BLANK = "·";
const char *FILL = "█";


// PROTOTYPES
// ------------------------------------------------------------------
void cls();
void DrawPlayfield(int bScrollX, int bScrollY);
void DrawShiftedChar(byte *s1, byte *s2, byte *d, byte bXOff, byte bYOff);
void PrintDemoMessage();
void PrintRow(byte data[SCREEN_WIDTH], byte y);
void PrintPlayFieldRow(byte row);
void PrintPlayField();

// MAIN
// ------------------------------------------------------------------
int main(void) {
  cls();

  // Demo purposes
  int c,
    flag = 0;
  
  byte x, y, *d;
  
  iScrollX = 0;
  iScrollY = 0;

  // Storing tyles according to initial iScroll
  int iStart = iScrollY >> 3;
  for (y = 0; y < PLAYFIELD_ROWS; y++) {
    d = &bPlayfield[y * PLAYFIELD_COLS];

    for (x = 0; x < PLAYFIELD_COLS; x++) {
      memcpy(d++, &tileMap[(iStart + y) % TILEMAP_HEIGHT][x], 1);
    }
  }

  DrawPlayfield(iScrollX, iScrollY);
  PrintDemoMessage();
  
  // DEMO FUNCTIONALITY  
  char ch = 0, k = 0;
  while (1) {  
    ch = c_getch();
    if (ch == 27)                  
      k = 1;

    if (ch == 91 && k == 1)
      k = 2;

    if (ch == 65 && k == 2) { // UP
      iScrollY++;
      flag = 1;
    }

    if (ch == 66 && k == 2) { // DOWN
      iScrollY--;
      flag = 1;
    }

    if (ch == 67 && k == 2) { // RIGHT
      iScrollX++;
      flag = 1;
    }

    if (ch == 68 && k == 2) { // LEFT
      iScrollX--;
      flag = 1;
    }

    if (ch != 27 && ch != 91)
      k = 0;

    if (flag) {            
      // Limits 
      iScrollX = (iScrollX >= PLAYFIELD_COLS * MODULE) ? 0 : 
        (iScrollX < 0) ? ((PLAYFIELD_COLS * MODULE) - 1) : iScrollX;
      
      iScrollY = (iScrollY >= 240) ? 0 :
        (iScrollY < 0) ? ((TILEMAP_HEIGHT * MODULE) - 1) : iScrollY;
      
      cls();
      DrawPlayfield(iScrollX, iScrollY);
      PrintDemoMessage();
    }
  }
      
  return 0;
}

void DrawShiftedChar(byte *s1, byte *s2, byte *d, byte bXOff, byte bYOff) {
  byte c, c2, z;  
  // s1 -> ucTiles
  // s2 -> ucTiles

  for (z = 0; z < (8 - bXOff); z++) {
    c = *s1++;//pgm_read_byte(s1++);
    c >>= bYOff; // shift over
    c2 = *s2++; //pgm_read_byte(s2++);
    c2 <<= (8 - bYOff);
    *d++ = (c | c2);
  }
}

void DrawPlayfield(int bScrollX, int bScrollY) {  
  byte bTemp[SCREEN_WIDTH]; // holds data for the current scan line
  byte x, y, tx;
  int ty, ty1, bXOff, bYOff;
  byte c, *s, *sNext, *d;
  int iOffset, iOffset2;

  // Solo es cero cuando el scroll completa un MODULO su eje X (8 unidades)
  bXOff = bScrollX & (MODULE - 1);
  bYOff = bScrollY & (MODULE - 1);

  // ty: current row
  // Incrementa una unidad cada vez que el scroll completa un MODULO en Y
  ty = (bScrollY >> 3) + (EDGES / 2);
  ty1 = (bScrollY >> 3) + (EDGES / 2);

#ifdef DEBUG
  printf("TILING MAP DEMO:\n-----------------------------------------------------\n\n");
  printf("PLAYFIELD_ROWS:\t\t%3i\t\tTILEMAP_HEIGHT:\t\t %i\n", PLAYFIELD_ROWS, TILEMAP_HEIGHT);
  printf("PLAYFIELD_COLS:\t\t%3i\t\tTILEMAP_WIDTH:\t\t %i\n\n", PLAYFIELD_COLS, TILEMAP_WIDTH);
  
  printf("SCREEN_HEIGHT:\t\t%3i\t\tVIEWPORT_HEIGHT:\t%3i\n", SCREEN_HEIGHT, VIEWPORT_HEIGHT);
  printf("SCREEN_WIDTH:\t\t%3i\t\tVIEWPORT_WIDTH:\t\t%3i\n\n", SCREEN_WIDTH, VIEWPORT_WIDTH);  

  printf("iScrollX:\t\t\t%3i\t\tbXOff:\t\t\t\t%3i\t\t\tty: \t%3i\n", 
  iScrollX, bXOff, (ty >= PLAYFIELD_COLS ? ty - PLAYFIELD_COLS : ty));
  printf("iScrollY:\t\t\t%3i\t\tbYOff:\t\t\t\t%3i\t\t\tty1: \t%3i\n", iScrollY, bYOff, ty1);
      
  printf("\n%s\n", bYOff ? "bYOff Mode (step between block complete)" : "NON bYOff (Scroll block completed)");
  printf("\nSCREEN REPRESENTATION:\n----------------------\n");
#endif

  byte *d1;

  int playFieldLength = PLAYFIELD_ROWS * PLAYFIELD_COLS,
    nextPlayfieldBit = (ty1 + VIEWPORT_HEIGHT) * PLAYFIELD_COLS,
    cNextPlayfieldBit = (nextPlayfieldBit % playFieldLength),
    cNextRow = ((ty1 + VIEWPORT_HEIGHT) % TILEMAP_HEIGHT),
    cIndex, cIndex2;

  d1 = &bPlayfield[cNextPlayfieldBit];

  for (byte x1 = 0; x1 < PLAYFIELD_COLS; x1++) {
    memcpy(d1 + x1, &tileMap[cNextRow][x1], 1);
  }

  // -----------------------------------------


  // draw the 8 rows
  for (y = 0; y < VIEWPORT_HEIGHT; y++) {
    memset(bTemp, 0, sizeof(bTemp));
    
    if (ty >= PLAYFIELD_ROWS) {
      ty -= PLAYFIELD_ROWS;
    }
        
    tx = (bScrollX >> 3) + (EDGES / 2);

    // Draw the playfield characters at the given scroll position
    d = bTemp;

    
    if (bYOff) {            
      for (x = 0; x < VIEWPORT_WIDTH; x++) {
        if (tx >= PLAYFIELD_COLS) {
          tx -= PLAYFIELD_COLS; // wrap around
        }
        
        iOffset = tx + (ty * PLAYFIELD_COLS);
        iOffset2 = (iOffset + PLAYFIELD_COLS); // next line

        if (iOffset2 >= (PLAYFIELD_ROWS * PLAYFIELD_COLS)) {    // past bottom
          iOffset2 -= (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        }

        cIndex = iOffset % (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        c = bPlayfield[cIndex];
        s = (byte *)&ucTiles[(c * MODULE) + bXOff];

        cIndex2 = iOffset2 % (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        c = bPlayfield[cIndex2];
        sNext = (byte *)&ucTiles[(c * MODULE) + bXOff];
        
        /*printf("\niScrollY: %i, iOffset: %03i, iOffset2: %03i, cIndex: %03i, cIndex2: %03i, s: %03i, sNext: %03i", 
          iScrollY, iOffset, iOffset2, cIndex, cIndex2, *s, *sNext);*/

        DrawShiftedChar(s, sNext, d, bXOff, bYOff);

        d += (MODULE - bXOff);
        bXOff = 0;
        tx++;
      }     

      // partial character left to draw
      // De momento no pasamos por aqui
      /*if (d != &bTemp[SCREEN_WIDTH]) {       
        bXOff = (byte)(&bTemp[SCREEN_WIDTH] - d);
   
        if (tx >= PLAYFIELD_COLS) {
          tx -= PLAYFIELD_COLS;
        }
        
        iOffset = tx + ty * PLAYFIELD_COLS;
        iOffset2 = iOffset + PLAYFIELD_COLS; // next line
        
        if (iOffset2 >= (PLAYFIELD_ROWS * PLAYFIELD_COLS)) {    // past bottom
          iOffset2 -= (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        }
        
        c = bPlayfield[iOffset];
        s = (byte *)&ucTiles[c * MODULE];
        c = bPlayfield[iOffset2];
        sNext = (byte *)&ucTiles[c * MODULE];
        DrawShiftedChar(s, sNext, d, MODULE - bXOff, bYOff);
      }*/
    // simpler case of vertical offset of 0 for each character
    } else {
      //-----------------------------------
      // NON BYOFF; SCREEN BYTE COMPLETED!      
      //-----------------------------------
      
      // Filling each col of the SCREEN_WIDTH
      for (x = 0; x < VIEWPORT_WIDTH; x++) {
        if (tx >= PLAYFIELD_COLS) {
          tx -= PLAYFIELD_COLS;
        }

        iOffset = tx + (ty * PLAYFIELD_COLS);
        cIndex = iOffset % (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        c = bPlayfield[cIndex];
        s = (byte *)&ucTiles[(c * MODULE) + bXOff];
        memcpy(d, s, MODULE - bXOff);
        d += (MODULE - bXOff);
        bXOff = 0;
        tx++;
      }

      //printf("\n");
      
      // partial character left to draw
      // De momento el codigo no pasa por aqui.
      /*if (d != &bTemp[SCREEN_WIDTH]) {
        printf("\n\nPARTIAL CHARACTER LEFT \n\n");
        bXOff = (byte)(&bTemp[SCREEN_WIDTH] - d);
        
        if (tx >= PLAYFIELD_COLS) {
          tx -= PLAYFIELD_COLS;
        }
        
        iOffset = tx + ty * PLAYFIELD_COLS;
        c = bPlayfield[iOffset];
        s = (byte *)&ucTiles[c * MODULE];
        memcpy(d, s, bXOff);
      }*/
    }

    
    //printf("\n");
        
    // Send it to the display
    //oledSetPosition(0, y);
    //I2CWriteData(bTemp, SCREEN_WIDTH);
       
    ty++;        
        
    PrintRow(bTemp, y);    
  }

    
  printf("\n\n\n");

  //PrintPlayFieldRow(1);
  //PrintScreen(ty1);  
  
  PrintPlayField();    
}




// DEMO ROUTINES
// ------------------------------------------------------------------
void PrintRow(byte data[SCREEN_WIDTH], byte y) {
  byte i;
  printf("\n[%i]\t ", y);
  
  for (i = 0; i <= SCREEN_WIDTH; i += MODULE ) {    
    printf("%s", data[i] == 255 ? FILL : BLANK);
  }  
}

void PrintPlayFieldRow(byte row) {
  int i,
    dataLength = PLAYFIELD_COLS * PLAYFIELD_ROWS,
    iStart = row * PLAYFIELD_COLS,
    iStop = iStart + PLAYFIELD_COLS;

  //printf("iStart: %i, iStop: %i\n", iStart, iStop);
  printf("[%i]\t [", row);

  for (i = iStart; i < iStop; i++ ) {
    printf("%s", bPlayfield[i] == 1 ? FILL : BLANK);      
  }

  printf("]\n");
}

void PrintPlayField() {
  byte i;

  printf("PLAYFIELD VISUALIZATION (%i bytes):\n------------------------------------\n\n", 
    PLAYFIELD_COLS * PLAYFIELD_ROWS);  

  for (i = 0; i < PLAYFIELD_ROWS; i++) {
    PrintPlayFieldRow(i);
  }

  printf("\n\n");
}

void PrintDemoMessage() {
  printf("╔═══════════════════════════════════════════════════════════════════╗\n");
  printf("║ Press the arrow keys to scroll the screen                         ║\n");
  printf("╚═══════════════════════════════════════════════════════════════════╝");
}

void cls() {    
    c_clrscr();
}


