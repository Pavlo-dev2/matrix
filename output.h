#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdint.h>

int retfdfb0();
//returns file deckriptor fb0;

int getinfos(int fbo);
//gets infos(finfo, vinfo);

long* retscreensize();
//returns screen size in pixels([x, y, fbolength]);

int setsize(long hight, long wigth, char type);
//set screen size;
//type 0 - hight*wight;
//type 1 - all block have same hight and wigth, higth depandse on how much elements with this wigth will fit on the screen; 
//type 2 - all block have same hight and wigth, wigth depandse on how much elements with this height will fit on the screen; 

long* retblockinfo();
//returns block info;
//[0] - x - number of blocks
//[1] - y - number of blocks
//[2] - block - hight
//[3] - block - wigth

int definefeld(long wigth, long hight, char x_pos, char y_pos);
//creates feld;
//height - number of blocks;
//wigth - number of blocks;
//x_pos - l/m/r(left, midle, right);
//y_pos - u/m/d(up, midle, down);

long* retfeldinfo();
//returns feld info;
//[0] - x - feld wigth in blocks;
//[1] - y - feld hight in  blocks;
//[2] - feld x start posision in pixel;
//[3] - feld y start posision in pyxel;
//[4] - feld x end posision in pixel;
//[5] - feld y end posision in pyxel;

uint8_t *retfbp(int fb0);
//return pointer;

int closefbp(uint8_t *fbp);
//close fb0 pointer;

int drawblock(uint8_t *fbp, float x, float y, int r, int g, int b);
//draws block on cord x:y;
//if x or y == -1, draw the whole line;
//if x == -1 and y == -1 draws the whole feld;

int clearframebuffer(uint8_t *fbp);
//clear the framebuffer(full with 0);

#endif
