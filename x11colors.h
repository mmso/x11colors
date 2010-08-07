#ifndef x11colors_h
#define x11colors_h

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/Intrinsic.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define sign(x) (( x > 0 ) - ( x < 0 ))

#define COLOR(x) "[38;5;"x"m"
#define ENDCOLOR "[m"

#define ARGB
#define COLOR_AMOUNT 16
#define BOX_WIDTH 80
#define BOX_HEIGHT 10
#define DISPLACE 4
#define DEFAULT_FONT "nu:pixelsize=11"
#define TEXT_OFFSET 0
#define WINDOW_CLASS_TITLE "x11colors"

struct display {
	Display *display;
	int depth;
	int screen;
	int display_width;
	int display_height;

	Visual *visual;
	Colormap colormap;

	Window root;
};

struct window {
	bool running;

	Window window;
	Window drawable;
	XSetWindowAttributes wa;

	GC gc;

	XftDraw *xftdraw;
	XftColor xftcolors[COLOR_AMOUNT];
	XftFont *xftfont;
};

struct config {
	char title[32];
	char fontname[32];
	char xcolors[COLOR_AMOUNT][8];

	int x,y;
	unsigned int width,height;

	int textx,texty;
	int rectanglex,rectangley;
	unsigned int rectanglew,rectangleh;

	int displace;
};

void render(struct window *, struct display *, struct config *);
void init_colors(struct window *, struct display *, struct config *);
void init_xdefaults(struct config *);
void init_x11colors(struct config *);

#endif
