#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
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
//[0] - x - number of blocks;
//[1] - y - number of blocks;
//[2] - block - hight;
//[3] - block - wigth;

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
//return fb0 pointer;

int closefbp(uint8_t *fbp);
//close fb0 pointer;

int drawblock(uint8_t *fbp, long x, long y, int r, int g, int b);
//draws block on cord x:y;
//if x or y == -1, draw the whole line;
//if x == -1 and y == -1 draws the whole feld;

//info
static struct fb_fix_screeninfo finfo;
static struct fb_var_screeninfo vinfo;
static long screen_hight;//screen hight in blocks
static long screen_wigth;//screen wigth in blocks
static long block_hight;//block height in pixels
static long block_wigth;//block wight in pixels
static long feld_hight;//feld height in blocks
static long feld_wigth;//feld wigth in blocks
static long feld_x_start;//feld x start posison in pixel
static long feld_x_end;//feld x start posison in pixel
static long feld_y_start;//feld y start posison in pixel
static long feld_y_end;//feld y start posison in pixel

int retfdfb0()
{
	int fb0 = open("/dev/fb0", O_RDWR);
}

int getinfos(int fb0)
{
	ioctl(fb0, FBIOGET_VSCREENINFO, &vinfo);
	ioctl(fb0, FBIOGET_FSCREENINFO, &finfo);
}

long *retscreensize()
{
	long *info = calloc(3, sizeof(long));
	
	info[0] = vinfo.xres;
	info[1] = vinfo.yres;
	info[2] = finfo.smem_len/(vinfo.bits_per_pixel/8);
	
	return info;
}

int setsize(long hight, long wigth, char type)
{
	if (type == 0)
	{
		screen_hight = hight;
		screen_wigth = wigth;
		block_hight = vinfo.yres/screen_hight;
		block_wigth = vinfo.xres/screen_wigth;
	}

	else if (type == 1)
	{
		screen_wigth = screen_hight = hight;
		block_wigth = block_hight = vinfo.xres/screen_wigth;
		screen_hight = vinfo.yres/block_wigth;
	}
	
	else if (type == 2)
	{
		screen_hight = screen_wigth = wigth;
		block_hight = block_wigth = vinfo.yres/screen_hight;
		screen_wigth = vinfo.xres/block_hight;
	}

	else
	{
		return 0;
	}
	return 0;
}

long* retblockinfo()
{
	long *info = calloc(4, sizeof(long));
	
	info[0] = screen_wigth;
	info[1] = screen_hight;
	info[2] = block_wigth;
	info[3] = block_hight;
	
	return info;
}

int definefeld(long wigth, long hight, char x_pos, char y_pos)
{
	if (hight <= screen_hight && wigth <= screen_wigth)
	{
		feld_hight = hight;
		feld_wigth = wigth;
	}
	else
	{
		return 1;
	}
	
	switch (x_pos)
	{
		case 'l':
			feld_x_start = 0;
			feld_x_end = feld_x_start + wigth;
			break;
		case 'r':
			feld_x_start = screen_wigth - 1 - wigth;
			feld_x_end = feld_x_start + wigth;
			break;
		case 'm':
			feld_x_start = (screen_wigth - 1 - wigth)/2;
			feld_x_end = feld_x_start + wigth;
			break;
	}
	switch (y_pos)
	{
		case 'u':
			feld_y_start = 0;
			feld_y_end = feld_y_start + hight;
			break;
		case 'd':
			feld_y_start = screen_hight - 1 - hight;
			feld_y_end = feld_y_start + hight;
			break;
		case 'm':
			feld_y_start = (screen_hight - 1 - hight)/2;
			feld_y_end = feld_y_start + hight;
			break;
	}

	if (hight%2 == 0)
	{	
		feld_y_start = feld_y_start * block_hight;// + 0.5*block_hight;//;
		feld_y_end = feld_y_end * block_hight;// + 0.5*block_hight;
	}
	else if (hight%2 == 1)
	{
		feld_y_start = feld_y_start * block_hight + 0.5*block_hight;
		feld_y_end = feld_y_end * block_hight + 0.5*block_hight;
	}
	
	if (wigth%2 == 0)
	{	
		feld_x_start = feld_x_start * block_wigth;// + 0.5*block_wigth;//;
		feld_x_end = feld_x_end * block_wigth;// + 0.5*block_wigth;
	}
	else if (wigth%2 == 1)
	{
		feld_x_start = feld_x_start * block_wigth + 0.5*block_wigth;
		feld_x_end = feld_x_end * block_wigth + 0.5*block_wigth;
	}

	return 0;

}

long* retfeldinfo()
{
	long *info = calloc(6, sizeof(long));
	
	info[0] = feld_wigth;
	info[1] = feld_hight;
	info[2] = feld_x_start;
	info[3] = feld_y_start;
	info[4] = feld_x_end;
	info[5] = feld_y_end;
	
	return info;
}

uint8_t *retfbp(int fb0)
{
	uint8_t *fbp = (uint8_t*)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb0, 0);
	if (fbp == MAP_FAILED)
	{
		perror("Mmap failed");
		close(fb0);
		return NULL;
	}
	return fbp;
}

int closefbp(uint8_t *fbp)
{
	munmap(fbp, finfo.smem_len);
}

int drawblock(uint8_t *fdp, long x, long y, int r, int g, int b)
{
	long xs, xe, ys, ye;

	if (y != -1)
	{
		ys = feld_y_start + block_hight*y;
		ye = ys + block_hight;
	}
	else
	{	
		ys = feld_y_start;
		ye = feld_y_end;
	}
	if (x != -1)
	{
		xs = feld_x_start + block_wigth*x;
		xe = xs + block_wigth;
	}
	else
	{	
		xs = feld_x_start;
		xe = feld_x_end;
	}

	for (long ay = ys; ay < ye; ay++)
	{
		for (long ax = xs; ax < xe; ax++)
		{
            		long location = (ax + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (ay + vinfo.yoffset) * finfo.line_length;
			
			if (vinfo.bits_per_pixel == 32)
			{
				fdp[location] = b;
				fdp[location+1] = g;
				fdp[location+2] = r;
				fdp[location+3] = 0;
			}
		
		}
	}

}

/*
int drawrect(unsigned char r, unsigned char g, unsigned char b, char x, char y, int un, int fb0);
int clearfeld(unsigned char r, unsigned char g, unsigned char b, int fb0);
int* info(int fb0);
*/
/*
//clear feld in the midle of the screan, size y_max*y_max
int clearfeld(unsigned char r, unsigned char g,  unsigned char b, int fb0)
{
	//info struckts
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
	//get infos
	if (ioctl(fb0, FBIOGET_VSCREENINFO, &vinfo) == -1 || ioctl(fb0, FBIOGET_FSCREENINFO, &finfo) == -1)
	{
		perror("Error while opening fb0");
		close(fb0);
		return 1;
	}

	//make fb0 a pointer
	long screensize = finfo.smem_len;
	uint8_t *fbp = (uint8_t*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb0, 0);
	if (fbp == MAP_FAILED)
	{
		perror("Mmap failed");
		close(fb0);
		return 1;
	}

	//feld cords, xstart, xend, ystart, yend, in the midle of the screan;
	long feldcords[4] = {0};
	feldcords[0] = (vinfo.xres - (float)vinfo.yres)/2.0;
	feldcords[1] = feldcords[0] + vinfo.yres;
	feldcords[2] = (vinfo.yres - (float)vinfo.yres)/2.0;
	feldcords[3] = feldcords[2] + vinfo.yres;
	
	//printf("XS0: %lu\nXE1: %lu\n", feldcords[0], feldcords[1]);
	//printf("YS2: %lu\nYE3: %lu\n", feldcords[2], feldcords[3]);


	//feld size in pixels
	float feld_heigth, feld_wigth;
	feld_wigth = feldcords[1] - feldcords[0];//x length
	feld_heigth = feldcords[3] - feldcords[2];//y length

	//printf("feld_wigth: %f;\nfeld_heigth: %f;\n", feld_wigth, feld_heigth);
	
	//sleep(1);

	//draw feld
	for (long ay = feldcords[2]; ay < feldcords[3]; ay++)
	{
		for (long ax = feldcords[0]; ax < feldcords[1]; ax++)
		{
            		long location = (ax + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (ay + vinfo.yoffset) * finfo.line_length;
			
			if (vinfo.bits_per_pixel == 32)
			{
				fbp[location] = b;
				fbp[location+1] = g;
				fbp[location+2] = r;
				fbp[location+3] = 0;
			}
		
		}
	}

	//end
	munmap(fbp, screensize);
}

//print fb0 info
int* info(int fb0)
{
	//info struckts
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
	//get infos
	if (ioctl(fb0, FBIOGET_VSCREENINFO, &vinfo) == -1 || ioctl(fb0, FBIOGET_FSCREENINFO, &finfo) == -1)
	{
		perror("Error while opening fb0");
		close(fb0);
		return NULL;
	}
	
	printf("vinfo.xres: %f\nvinfo.yres: %f\n", (float) vinfo.xres, (float) vinfo.yres);
	printf("finfo.smem_len: %f\n", (float) finfo.smem_len);
	printf("vinfo.xoffset: %f\nvinfo.yoffset: %f\n", (float) vinfo.xoffset, (float) vinfo.yoffset);
	printf("vinfo.xbits_per_pixel: %f\nvinfo.byts_per_pixel: %f\n", (float) vinfo.bits_per_pixel, (float) vinfo.bits_per_pixel/8);
	printf("finfo.line_length: %f\n\n", (float) finfo.line_length);
}

//funktion to draw a square
int drawrect(unsigned char r, unsigned char g, unsigned char b, char x, char y, int un, int fb0)
{
	//info struckts
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
	//get infos
	if (ioctl(fb0, FBIOGET_VSCREENINFO, &vinfo) == -1 || ioctl(fb0, FBIOGET_FSCREENINFO, &finfo) == -1)
	{
		perror("Error while opening fb0");
		close(fb0);
		return 1;
	}

	//make fb0 a pointer
	long screensize = finfo.smem_len;
	uint8_t *fbp = (uint8_t*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb0, 0);
	if (fbp == MAP_FAILED)
	{
		perror("Mmap failed");
		close(fb0);
		return 1;
	}
	
	//feld cords, xstart, xend, ystart, yend, in the midle of the screan;
	long feldcords[4] = {0};
	feldcords[0] = (vinfo.xres - (float)vinfo.yres)/2.0;
	feldcords[1] = feldcords[0] + vinfo.yres;
	feldcords[2] = (vinfo.yres - (float)vinfo.yres)/2.0;
	feldcords[3] = feldcords[2] + vinfo.yres;
	
	//printf("XS0: %lu\nXE1: %lu\n", feldcords[0], feldcords[1]);
	//printf("YS2: %lu\nYE3: %lu\n", feldcords[2], feldcords[3]);


	//feld size in pixels
	float feld_heigth, feld_wigth;
	feld_wigth = feldcords[1] - feldcords[0];//x length
	feld_heigth = feldcords[3] - feldcords[2];//y length

	//printf("feld_wigth: %f;\nfeld_heigth: %f;\n", feld_wigth, feld_heigth);
	
	//block size in pixel
	long block_wigth, block_heigth;
	block_wigth = feld_wigth/un;
	block_heigth = feld_heigth/un;
	
	//printf("block_wigth: %lu;\nblock_heigth: %lu;\n", block_wigth, block_heigth);

	//sleep(1);

	long xst = feldcords[0] + block_wigth*x;//x start
	long xed = xst+block_wigth;//x end
	long yst = feldcords[2] + block_heigth*y;//y start
	long yed = yst+block_heigth;//y end
	
	//draw block
	for (long ay = yst; ay < yed; ay++)
	{
		for (long ax = xst; ax < xed; ax++)
		{
            		long location = (ax + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (ay + vinfo.yoffset) * finfo.line_length;
			
			if (vinfo.bits_per_pixel == 32)
			{
				fbp[location] = b;
				fbp[location+1] = g;
				fbp[location+2] = r;
				fbp[location+3] = 0;
			}
		
		}
	}

	//end
	munmap(fbp, screensize);
}*/
