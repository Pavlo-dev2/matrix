#include <stdio.h>
#include <stdlib.h>
#include "timing.h"
#include "input.h"
#include "output.h"
#define ARRLEN 210
#define XSIDE 14
#define YSIDE 14
#define TIME 0.05

int drawfild();
int prepfeld(int x, int y);
char* getfeld(FILE *fp);
int drawfild(char *fild);
int drawplayer();

//player cords
float player_x = 0;
float player_y = 0;

//fbo and fraimbuffer pointer
int fb0;
uint8_t *fbp;

int main()
{
	FILE *fp = fopen("map1.txt", "r");
	char *feld = getfeld(fp);
	printf("%s", feld);
	clearframebuffer(fbp);
	prepfeld(XSIDE, YSIDE);
	while (1)
	{
		long double new_time = rettime();
		clearframebuffer(fbp);
		drawfild(feld);
		player_x += 0.5;
		player_y += 0.5;
		drawplayer();
		printf("Time: %Lf;\n", timediff(rettime(), new_time));
		printf("Lol\n");
		sleepsec(TIME - timediff(rettime(), new_time));
	}
	return 0;
}

char* getfeld(FILE *fp)
{
	char *feld = calloc(ARRLEN, sizeof(char));
	char nc;
	int i;
	for (i = 0; i < ARRLEN && (feld[i] = nc = fgetc(fp)) != '\0'; i++);
	printf("%d\n", i);
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
