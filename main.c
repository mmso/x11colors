#include "x11colors.h"

void init_config(struct config *cfg) {
	strcpy(cfg->title, WINDOW_CLASS_TITLE);

	if(*cfg->fontname == '\0')
		strcpy(cfg->fontname, DEFAULT_FONT);

	cfg->textx = cfg->textx?cfg->textx:TEXT_OFFSET;
	cfg->texty = 0; //not used
	cfg->rectanglex = 60;
	cfg->rectangley = 0;
	cfg->rectangleh = cfg->rectangleh?cfg->rectangleh:BOX_HEIGHT;
	cfg->rectanglew = cfg->rectanglew?cfg->rectanglew:BOX_WIDTH;
	cfg->displace = cfg->displace?cfg->displace:DISPLACE;

	cfg->x = 0;
	cfg->y = 0;
	cfg->width = cfg->textx + cfg->rectanglex + cfg->rectanglew ;
	cfg->height = cfg->rectangley + (COLOR_AMOUNT/2) * cfg->rectangleh * 2 + (COLOR_AMOUNT/2 - 1)*cfg->displace;
}

static void usage(void) {
	fprintf(stderr,
	"Usage:  [-?hc] [-f fontstring] [-x textoffset] [-W box-width] [-H box-height] [-d displace]\n"
	"\n"
	"Options:\n"
	"  -?,-h\t\t: this help\n"
	"  -c\t\t: draw text with background or foregrund color (default: off)\n"
	"  -f fontstring\t: xft fontstring (default: %s)\n"
	"  -x textoffset\t: horizontal text offset (default: %d)\n"
	"  -W box-width\t: height of colorbox (default: %u)\n"
	"  -H box-width\t: width of colorbox (default: %u)\n"
	"  -d displace\t: vertical box displace (default: %d)\n"
	"\n"
	, DEFAULT_FONT, TEXT_OFFSET, BOX_WIDTH, BOX_HEIGHT, DISPLACE);
}

int main(int argc, char* argv[]) {
	char ch;
	struct config cfg;

	bzero((struct config *)&cfg, sizeof(struct config));

	while ((ch = getopt(argc, argv, "?hcf:x:W:H:d:")) != -1) {
		switch (ch) {
			case '?': 
			case 'h': usage(); exit(0);
			case 'c': cfg.textcoloroffset = 8; break;
			case 'f': sscanf(optarg, "%s", cfg.fontname); break;
			case 'x': sscanf(optarg, "%d", &cfg.textx); break;
			case 'W': sscanf(optarg, "%u", &cfg.rectanglew); break;
			case 'H': sscanf(optarg, "%u", &cfg.rectangleh); break;
			case 'd': sscanf(optarg, "%d", &cfg.displace); break; 
		}
	}

	init_config(&cfg);
	init_x11colors(&cfg);

	return 0;
}
