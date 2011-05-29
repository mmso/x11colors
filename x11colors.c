#include "x11colors.h"

char colors[][8] = {
	{ "black" },
	{ "red" } ,
	{ "green" },
	{ "yellow" },
	{ "blue" },
	{ "magenta" },
	{ "cyan" },
	{ "white" }
};

void clear(struct window *win, struct display *in) {
	for(int i=0; i<COLOR_AMOUNT; i++) {
		XftColorFree(in->display, in->visual, in->colormap, &win->xftcolors[i]);
	}

	XftFontClose(in->display, win->xftfont);
	XftDrawDestroy(win->xftdraw);

	XFreeGC(in->display, win->gc);
#ifdef ARGB
	XFreeColormap(in->display, in->colormap);
#endif

	XUnmapWindow(in->display, win->window);
	XDestroyWindow(in->display, win->window);

	XCloseDisplay(in->display);
}

void drawcolors(struct window *win, struct config *cfg) {
	int tx,ty;
	int fh, fa;

	tx = cfg->textx;
	ty = cfg->texty;

	fa = win->xftfont->ascent;
	fh = win->xftfont->ascent + win->xftfont->descent;

	for(int i=0,y=cfg->rectangley; i<COLOR_AMOUNT/2; i++) {
		ty = y + cfg->rectangleh - fh/2 + fa;
		XftDrawStringUtf8(win->xftdraw, &win->xftcolors[i+cfg->textcoloroffset], win->xftfont, tx, ty, (const XftChar8*) colors[i], strlen(colors[i]));

		for(int d=0; d<=8; d+=8) {
			XftDrawRect(win->xftdraw, &win->xftcolors[i+d], cfg->rectanglex, y, cfg->rectanglew, cfg->rectangleh);
			y += cfg->rectangleh + sign(d)*cfg->displace;
		}
	}
}

void render(struct window *win, struct display *in, struct config *cfg) {
	XClearArea(in->display, win->drawable, cfg->x, cfg->y, cfg->width, cfg->height, False);

	drawcolors(win, cfg);
	
	XFlush(in->display);
}

void keypress(XEvent *e, struct window *win, struct display *in, struct config *cfg) {
	XKeyEvent *ev;

	ev = &e->xkey;

	switch(XKeycodeToKeysym(in->display, (KeyCode)ev->keycode, 0)) {
		case XK_q:
			fprintf(stderr, "freeing resources\n");
			win->running = false;
			break;
		case XK_u:
			fprintf(stderr, "refreshing xcolors\n");
			init_xdefaults(in, cfg);
			init_colors(win, in, cfg);
			render(win, in, cfg);
			break;
	}
}

void expose(XEvent *e, struct window *win, struct display *in, struct config *cfg) {
	render(win,in,cfg);
}

static void (*handler[LASTEvent]) (XEvent *, struct window *, struct display *in, struct config *cfg) = {
	[KeyPress] = keypress,
	[Expose] = expose,
#ifndef ARGB
	[ConfigureNotify] = expose,
#endif

};

void loop(struct window *win, struct display *in, struct config *cfg) {
	XEvent ev;

	while(win->running) {
		XNextEvent(in->display, &ev);

		if(handler[ev.type]) {
			handler[ev.type](&ev, win, in, cfg);
		}
	}
}

void init_colors(struct window *win, struct display *in, struct config *cfg) {
	for(int i=0; i<COLOR_AMOUNT; i++) {
		XftColorFree(in->display, in->visual, in->colormap, &win->xftcolors[i]); /*for reload*/
		XftColorAllocName(in->display, in->visual, in->colormap, (const char*)cfg->xcolors[i], &win->xftcolors[i]);
	}
}

void init_xdefaults(struct display *in, struct config *cfg) {
	char *type[16];
	char buffer[8];
	XrmDatabase rdb;
	XrmValue value;

	rdb = XrmGetDatabase(in->display);

	for(int i=0; i<COLOR_AMOUNT; i++) {
		snprintf(buffer, sizeof(buffer), "color%d", i);

		XrmGetResource(rdb, buffer, buffer, type, &value);
		
		strncpy(cfg->xcolors[i], value.addr, (int)value.size);
		fprintf(stderr, "%s: "COLOR("%d")"%s"ENDCOLOR"\n", buffer, i, cfg->xcolors[i]);
	}
}

void init_xft(struct window *win, struct display *in, struct config *cfg) {
	win->xftdraw = XftDrawCreate(in->display, win->drawable, in->visual, in->colormap);
	win->xftfont = XftFontOpenName(in->display, in->screen, cfg->fontname);

	fprintf(stderr, "loading '%s'\n", cfg->fontname);

	if (!win->xftfont) {
		fprintf(stderr, "error loading font: '%s'\n", cfg->fontname);
		win->xftfont = XftFontOpenName(in->display, in->screen, "sans-10");
	}
}

void init_win(struct window *win, struct display *in, struct config *cfg) {
	XSetWindowAttributes attrs = { ParentRelative, 0L, 0, 0L, 0, 0, Always, 0L, 0L, False, StructureNotifyMask | ExposureMask | KeyPressMask, 0L, True, 0, 0 };
	XClassHint classhint = { cfg->title, cfg->title };
	unsigned long valuemask;
	unsigned int class;

#ifndef ARGB
	class = CopyFromParent;
	valuemask = CWBackPixmap | CWEventMask;
#else
	attrs.colormap = in->colormap;
	class = InputOutput;
	valuemask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
#endif
	win->window = XCreateWindow(in->display, in->root, cfg->x, cfg->y, cfg->width, cfg->height, 0, in->depth, class, in->visual, valuemask, &attrs);

	XSetClassHint(in->display, win->window, &classhint);
	XStoreName(in->display, win->window, cfg->title);
	XMapWindow(in->display, win->window);

	win->drawable = win->window; /* could add double buffering here */
	win->gc = XCreateGC(in->display, win->drawable, 0, NULL);

	win->running = true;
}

#ifdef ARGB
static int get_argb_visual(struct display *in) {
	/* code from gtk project, gdk_screen_get_rgba_visual */
	int nxvisuals, ret;
	XVisualInfo visual_template;
	XVisualInfo *visual_list;
	
	ret = 0;
	visual_template.screen = in->screen;
	visual_list = XGetVisualInfo (in->display, VisualScreenMask, &visual_template, &nxvisuals);

	for (int i = 0; i < nxvisuals; i++) {
		if (visual_list[i].depth == 32 &&
		 (visual_list[i].red_mask   == 0xff0000 &&
			  visual_list[i].green_mask == 0x00ff00 &&
			  visual_list[i].blue_mask  == 0x0000ff)) {
			in->visual = visual_list[i].visual;
			in->depth = visual_list[i].depth;
			in->colormap = XCreateColormap(in->display, in->root, in->visual, AllocNone);

			ret = 1;
			break;;
		}
	}

	XFree(visual_list);
	return ret;
}
#endif

void init_display(struct display *in) {
	in->display = XOpenDisplay(NULL);
	in->screen = DefaultScreen(in->display);
	in->depth = DefaultDepth(in->display, in->screen);
	in->visual = DefaultVisual(in->display, in->screen);
	in->colormap = DefaultColormap(in->display, in->screen);
	in->root = RootWindow(in->display, in->screen);
	in->display_width = DisplayWidth(in->display, in->screen);
	in->display_height = DisplayHeight(in->display, in->screen);

#ifdef ARGB
	if(!get_argb_visual(in)) {
		fprintf(stderr, "error no argb visual found\nrecompile without argb\n");
		XCloseDisplay(in->display);

		exit(-1);
	}
#endif
}

void init_x11colors(struct config *cfg) {
	struct display in;
	struct window win;

	init_display(&in);
	init_win(&win, &in, cfg);
	init_xft(&win, &in, cfg);

	init_xdefaults(&in, cfg);
	init_colors(&win, &in, cfg);

	loop(&win, &in, cfg);

	clear(&win, &in);
}
