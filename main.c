#include <stdio.h>
#include <stdlib.h>
#include "timing.h"
#include "input.h"
#include "output.h"
#include <unistd.h>
#include <linux/input.h>
#include <math.h>
#define ARRLEN 272
#define XSIDE 16
#define YSIDE 16
#define TIME 0.1
#define TTGDI 0.6//time to get girection = TIME * TTGDI
#define PLAYERSTEP 0.5

int drawfild();
int prepfeld(int x, int y);
char* getfeld(FILE *fp);
int drawfild(char *fild);
int drawplayer();
int getdir(int fd);
int checkplayerpossision(char *feld);
//return 1 if player can't move 

//player cords
float player_x = 0;
float player_y = 0;
float player_nx = 0;
float player_ny = 0;
float playerstep_x = 0;
float playerstep_y = 0;
char *eloveddirections;//directions player can mave in
char dir = 'r';

//fbo and fraimbuffer pointer
int fb0;
uint8_t *fbp;
int fdie;//input file deskriptor
long double getdirtime;//time to get direction;

int main()
{
	FILE *fp = fopen("map1.txt", "r");
	char *feld = getfeld(fp);
	fdie = retfd(1, 2);
	prepfeld(XSIDE, YSIDE);
	clearframebuffer(fbp);
	getdirtime = TIME * TTGDI;
	char ndir;
	while (1)
	{
		long double new_time = rettime();
		
		//get direction
		ndir = getdir(fdie);
		
		//exite if e clicked
		if (ndir == 'E')
		{
			clearframebuffer(fbp);
			return 0;
		}

		switch(dir)
		{
			case 'u':
				player_ny -= PLAYERSTEP;
				break;
			case 'd':
				player_ny += PLAYERSTEP;
				break;
			case 'r':
				player_nx += PLAYERSTEP;
				break;
			case 'l':
				player_nx -= PLAYERSTEP;
				break;
		}
		
		//count player step
		int s = !checkplayerpossision(feld);
		if (dir == 'u' || dir == 'l')
		{
			playerstep_x += player_x - player_nx;
			playerstep_y += player_y - player_ny;
		}
		else if (dir == 'd' || dir == 'r')
		{
			playerstep_x += player_nx - player_x;
			playerstep_y += player_ny - player_y;
		}
		
		//if can move, move
		if ((s || ndir != dir) && fabs(playerstep_x - round(playerstep_x)) < 0.0001 && fabs(playerstep_y - round(playerstep_y)) < 0.0001)
		{
			dir = ndir;
			playerstep_y = playerstep_x = 0;
		}
		if (s)
		{
			player_x = player_nx;
			player_y = player_ny;
		}
		player_nx = player_x;
		player_ny = player_y;
		
		//draw
		clearframebuffer(fbp);
		drawfild(feld);
		drawplayer();
		//fflush(stdout);
		sleepsec(TIME - timediff(new_time, rettime()));
	}
	clearframebuffer(fbp);
	return 0;
}

char* getfeld(FILE *fp)
{
	char *feld = calloc(ARRLEN, sizeof(char));
	char nc;
	int i;
	for (i = 0; i < ARRLEN && (feld[i] = nc = fgetc(fp)) != '\0'; i++);
	//printf("%d\n", i);
	return feld;
}

int prepfeld(int x, int y)
{
	fb0 = retfdfb0();
	getinfos(fb0);
	setsize(x, y, 2);
	definefeld(x, y, 'm', 'm');
	fbp = retfbp(fb0);
	return 0;
}

int drawfild(char *fild)
{
	char nc = ' ';
	unsigned long c = 0;
	for (int y = 0; y < YSIDE; y++)
	{
		for (int x = 0; x < XSIDE; x++)
		{
			switch (fild[y*(XSIDE+1) + x])
			{
				case ' ':
					drawblock(fbp, x, y, 0, 0, 70);
					break;
				case 'S':
					drawblock(fbp, x, y, 0, 130, 20);
					break;
				case 'F':
					drawblock(fbp, x, y, 175, 0, 20);
					break;
				case '#':
					drawblock(fbp, x, y, 0, 145, 10);
					break;
			}
		}
	}
	return 0;
}

int drawplayer()
{
	drawblock(fbp, player_x, player_y, 0, 15, 0);
	return 0;
}

int getdir(int fd)
//get directory
{
	static char dir = 'r';
	static int events[] = {KEY_W, KEY_A, KEY_S, KEY_D, KEY_E};
	int code = ifeventscode(fd, events, 5, 1, getdirtime);
	switch (code)
	{
		case KEY_W:
			dir = 'u';
			break;
		case KEY_D:
			dir = 'r';
			break;
		case KEY_S:
			dir = 'd';
			break;
		case KEY_A:
			dir = 'l';
			break;
		case KEY_E:
			dir = 'E';
			break;
	}
	//printf("Function done\n");
	return dir;
}

int checkplayerpossision(char *feld)
{
	printf("Char: %c, Walue: %d\n%f, %f\n", feld[(int) (player_ny*(XSIDE+1) + player_nx)], (int) (player_ny*(XSIDE+1) + player_nx), player_nx, player_ny);
	fflush(stdout);
	if (player_nx < 0 || player_nx > XSIDE-1 || player_ny < 0 || player_ny > YSIDE-1)
	{
		return 1;
	}
	//feld[(int) (round(player_ny)*(XSIDE+1) + round(player_nx))] == '#'
	int inx, iny;
	switch(dir)
	{
		case 'u':
			if (fabs(player_ny - round(player_ny)) > 0.3)
			{
				iny = round(player_ny)+1;
			}
			else
			{
				iny = round(player_ny);
			}
			break;
		case 'd':
			if (fabs(player_ny - round(player_ny)) > 0.3)
			{
				iny = round(player_ny)+1;
			}
			else
			{
				iny = round(player_ny);
			}
			break;
		case 'r':
			if (fabs(player_nx - round(player_nx)) > 0.3)
			{
				iny = round(player_nx)+1;
			}
			else
			{
				iny = round(player_nx);
			}
			break;
		case 'l':
			if (fabs(player_nx - round(player_nx)) > 0.3)
			{
				iny = round(player_nx)-1;
			}
			else
			{
				iny = round(player_nx);
			}
			break;
		
	}
	if (feld[(int) (round(iny)*(XSIDE+1) + round(inx))] == '#')
	{
		return 1;
	}
	return 0;
}
