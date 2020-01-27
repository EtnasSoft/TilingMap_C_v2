#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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
#define PLAYFIELD_ROWS 8 // AXIS Y
#define PLAYFIELD_COLS VIEWPORT_WIDTH // AXIS X

// Para que el mapa sea circular, la altura tiene que ser un número en potencia base 2.
#define TILEMAP_HEIGHT 16
#define TILEMAP_WIDTH PLAYFIELD_COLS


// VARS
// ------------------------------------------------------------------
const byte ucTiles[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Empty  (0)
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Fill   (1) 
};

const byte tileMap[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
  {1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  1},
  {1,  1,  1,  1,  1,  1,  0,  0,  1,  0,  1,  1,  1,  1,  1,  1},  // -- FLOOR

  {1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1},
  {1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1},
};

static byte bPlayfield[PLAYFIELD_ROWS * PLAYFIELD_COLS];
static int iScrollX, iScrollY;
const char *BLANK = "·";
const char *FILL = "█";


// PROTOTYPES
// ------------------------------------------------------------------
void cls();
void DrawPlayfield(byte bScrollX, byte bScrollY);
void DrawShiftedChar(byte *s1, byte *s2, byte *d, byte bXOff, byte bYOff);
void PrintScreen(byte firstRow);
void PrintScreenRowDetailed(byte data[SCREEN_HEIGHT * SCREEN_WIDTH]);
void printDemoMessage();


// MAIN
// ------------------------------------------------------------------
int main(void) {
  cls();

  // Demo purposes
  int c,
    flag = 0;
  
  byte x, y, *d;


  // Storing tyles
  for (y = 0; y < PLAYFIELD_ROWS; y++) {
    d = &bPlayfield[y * PLAYFIELD_COLS];

    for (x = 0; x < PLAYFIELD_COLS; x++) {
      memcpy(d++, &tileMap[y][x], 1);
    }
  }

  iScrollX = 0;
  iScrollY = 0;

  DrawPlayfield(iScrollX, iScrollY);
  
  // DEMO MESSAGE AND FUNCTIONALITY
  printDemoMessage();
  while ((c = getchar()) != EOF) {
    switch (c) {
      case INCREASE:        
        flag = 1;
        iScrollY++;        
        break;        
      case DECREASE:        
        flag = 1;
        iScrollY--;
        break;
      default:
        flag = 1;
        iScrollY++;
        break;  
    }

    if (flag) {      
      cls();

      // Limits 
      iScrollX = (iScrollX >= PLAYFIELD_COLS * MODULE) ? 0 : 
        (iScrollX < 0) ? ((PLAYFIELD_COLS * MODULE) - 1) : iScrollX;
      
      iScrollY = (iScrollY >= (TILEMAP_HEIGHT * MODULE)) ? 0 :
        (iScrollY < 0) ? ((TILEMAP_HEIGHT * MODULE) - 1) : iScrollY;
      
      DrawPlayfield(iScrollX, iScrollY);
      printDemoMessage();
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

void DrawPlayfield(byte bScrollX, byte bScrollY) {
  byte bTemp[SCREEN_WIDTH]; // holds data for the current scan line
  byte x, y, tx;
  int ty, ty1;
  byte c, *s, *sNext, *d, bXOff, bYOff, /* MIOS -> */ sIndex;
  int iOffset, iOffset2;


  // Solo es cero cuando el scroll completa un MODULO su eje X (8 unidades)
  bXOff = bScrollX & (MODULE - 1);
  bYOff = bScrollY & (MODULE - 1); 

  // ty: current row
  // Incrementa una unidad cada vez que el scroll completa un MODULO en su eje
  ty = bScrollY >> 3; 
  ty1 = bScrollY >> 3;

#ifdef DEBUG
  printf("\nTILING MAP DEMO:\n-----------------------------------------------------\n\n");
  printf("PLAYFIELD_ROWS:\t\t%3i\t\tTILEMAP_HEIGHT:\t\t %i\n", PLAYFIELD_ROWS, TILEMAP_HEIGHT);
  printf("PLAYFIELD_COLS:\t\t%3i\t\tTILEMAP_WIDTH:\t\t %i\n\n", PLAYFIELD_COLS, TILEMAP_WIDTH);
  
  printf("SCREEN_HEIGHT:\t\t%3i\t\tVIEWPORT_HEIGHT:\t%3i\n", SCREEN_HEIGHT, VIEWPORT_HEIGHT);
  printf("SCREEN_WIDTH:\t\t%3i\t\tVIEWPORT_WIDTH:\t\t%3i\n\n", SCREEN_WIDTH, VIEWPORT_WIDTH);  

  printf("iScrollX:\t\t\t%3i\t\tbXOff:\t\t\t\t%3i\n", iScrollX, bXOff);
  printf("iScrollY:\t\t\t%3i\t\tbYOff:\t\t\t\t%3i\n\n", iScrollY, bYOff);
  
  printf("ty:\t\t\t\t\t%3i\t\tty1:\t\t\t\t%3i\n", ty, ty1);
  printf("\n\n%s\n---------------------------------------", bYOff ? "bYOff" : "NON bYOff (Scroll block completed)");
#endif

  // draw the 8 rows
  for (y = 0; y < VIEWPORT_HEIGHT; y++) {
    memset(bTemp, 0, sizeof(bTemp));

    // TODO: Esta comprobacion parece que tendría que ir fuera, pero se rompe la pantalla si se saca.
    if (ty >= PLAYFIELD_ROWS) {
      ty -= PLAYFIELD_ROWS;
    }

    /*if (ty1 >= 20) { // tenemos definido el tilemap con 20 filas...
      ty1 -= 20;
    }*/

    // TODO: Esta asignación debería estar fuera del bucle, no???
    tx = bScrollX >> 3; // Incrementa una unidad cada vez que el scroll completa un MODULO en X

    // Draw the playfield characters at the given scroll position
    d = bTemp;

    // partial characters vertically means a lot more work :(
    // Only for vertical scroll:
    printf("\n[%d] \t", ty);

    if (bYOff) {      
      for (x = 0; x < VIEWPORT_WIDTH; x++) {
        if (tx >= PLAYFIELD_COLS) {
          tx -= PLAYFIELD_COLS; // wrap around
        }
        iOffset = tx + ty * PLAYFIELD_COLS;
        iOffset2 = iOffset + PLAYFIELD_COLS; // next line
        if (iOffset2 >= (PLAYFIELD_ROWS * PLAYFIELD_COLS))     // past bottom
          iOffset2 -= (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        c = bPlayfield[iOffset];
        s = (byte *)&ucTiles[(c * MODULE) + bXOff];
        c = bPlayfield[iOffset2];
        sNext = (byte *)&ucTiles[(c * MODULE) + bXOff];
        DrawShiftedChar(s, sNext, d, bXOff, bYOff);
        d += (MODULE - bXOff);
        bXOff = 0;

        // Tiles stored by index
        printf("%i ", c);

        tx++;
      }


      //printf("\n\n");

      // partial character left to draw
      if (d != &bTemp[SCREEN_WIDTH]) {
        bXOff = (byte)(&bTemp[SCREEN_WIDTH] - d);
        if (tx >= PLAYFIELD_COLS)
          tx -= PLAYFIELD_COLS;
        iOffset = tx + ty * PLAYFIELD_COLS;
        iOffset2 = iOffset + PLAYFIELD_COLS; // next line
        if (iOffset2 >= (PLAYFIELD_ROWS * PLAYFIELD_COLS))     // past bottom
          iOffset2 -= (PLAYFIELD_ROWS * PLAYFIELD_COLS);
        c = bPlayfield[iOffset];
        s = (byte *)&ucTiles[c * MODULE];
        c = bPlayfield[iOffset2];
        sNext = (byte *)&ucTiles[c * MODULE];
        DrawShiftedChar(s, sNext, d, MODULE - bXOff, bYOff);
      }
    // simpler case of vertical offset of 0 for each character
    } else {
      //--------------------------------
      /*byte *d1;
      byte tempTy = ty1;
      byte nextRowTilemap;

      nextRowTilemap = (ty1 + 8);
      if (ty1 > 20) {
        nextRowTilemap = ty1 - 20;
      } else if (ty1 > 10) { // limite a ojo
        nextRowTilemap = ty1 - 10;
      }

      if (tempTy == 10) {
        tempTy = 0;
      } else if (tempTy == 0) {
        tempTy = 9;
      } else if (tempTy > 0) {
        tempTy -= 1;
      }

      byte yPos = tempTy * PLAYFIELD_COLS;

      d1 = &bPlayfield[yPos];

      for (byte x1 = 0; x1 < PLAYFIELD_COLS; x1++) {
        memcpy(d1++, &tileMap[nextRowTilemap][x1], 1);
      }*/
      //----------------------------------
                        
      
      // Filling each col of the SCREEN_WIDTH
      for (x = 0; x < VIEWPORT_WIDTH; x++) {        
        if (tx >= PLAYFIELD_COLS) {
          tx -= PLAYFIELD_COLS;
        }

        iOffset = tx + (ty * PLAYFIELD_COLS); // Next tile index to draw
        c = bPlayfield[iOffset];
        sIndex = (c * MODULE) + bXOff; // Useless var only for debug purposes :)
        s = (byte *)&ucTiles[(c * MODULE) + bXOff]; // Primer bit de tileMap necesario
        memcpy(d, s, MODULE - bXOff); // El tercer parametro indica cuantos bits copiamos desde "s"
        d += (MODULE - bXOff);
        bXOff = 0;
        tx++;

        
        // Tiles stored by index
        printf("%i ", c);


        // All data in tiles!
        /*
        printf("[%02i]: ", c);        
        for (byte z = 0; z < (MODULE - bXOff); z++) {
          printf("%03i, ", ucTiles[sIndex + z]);   
        }
        printf("\n");
        */
      }      
      
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

    // Esta funcion dibuja los sprites que de momento no necesitamos...
    //DrawSprites(y * VIEWPORT_HEIGHT, bTemp, object_list, numberOfSprites);
    
    // Send it to the display
    //oledSetPosition(0, y);
    //I2CWriteData(bTemp, SCREEN_WIDTH);
       
    ty++;        
    //PrintScreen(bTemp);
  }
  printf("\n---------------------------------------\n\n");
    
  PrintScreen(ty1);  
}

// DEMO ROUTINES
// ------------------------------------------------------------------
void PrintScreenRowDetailed(byte data[SCREEN_HEIGHT * SCREEN_WIDTH]) {
    printf("\n\nSCREEN REPRESENTATION:\n-----------------\n");
    for (int x = 0; x < SCREEN_HEIGHT * SCREEN_WIDTH; x++) {
      printf("%03i ", data[x]);
      
      // Col separation (each byte)
      if (x > 0 && ((x+1) % MODULE == 0)) {
        printf("\n");
      }

      // Row separation (each row)
      if (x > 0 && ((x + 1) % (PLAYFIELD_COLS * MODULE) == 0)) {
        printf("\n");
      }
    }    
}

void PrintScreen(byte ty) {  
  int i,
    dataLength = VIEWPORT_HEIGHT * VIEWPORT_WIDTH,
    iStart = ty * VIEWPORT_WIDTH;
    
  printf("\n\nSCREEN REPRESENTATION:\n----------------------\n\n");
    
  for (i = iStart; i < iStart + dataLength; i++ ) {
    printf("%s", bPlayfield[i & dataLength - 1] == 1 ? FILL : BLANK);
    if (!((i + 1) % 16)) {
      printf("\n");
    }    
  }
  
  printf("\n\niStart: %i, iStop: %i\n", iStart, iStart + dataLength);
  printf("TOTAL BYTES: %i\n\n\n", i);
}

void printDemoMessage() {
  printf("╔═══════════════════════════════════════════════════════════════════╗\n");
  printf("║ Press \"+\" or \"-\" + ENTER to increase or decrease the AXIS Y value ║\n");
  printf("╚═══════════════════════════════════════════════════════════════════╝\n");
}

void cls() {    
    system("@cls||clear");
}


