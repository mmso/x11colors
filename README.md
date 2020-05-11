x11colors displays terminal colors in a transparent X11 window.
=========

![screenshot](https://raw.githubusercontent.com/mmso/x11colors/master/x11colors-screenshot.png)

It uses either real transparency with ARGB, or fake transparency with the background as pixmap. x11colors requires libxft for drawing text.

## How to use

### Help
	Usage:  [-?hc] [-f fontstring] [-x textoffset] [-W box-width] [-H box-height] [-d displace]

	Options:
	  -?,-h		: this help
	  -c		: draw text with background or foregrund color (default: off)
	  -f fontstring	: xft fontstring (default: nu:pixelsize=11)
	  -x textoffset	: horizontal text offset (default: 0)
	  -W box-width	: height of colorbox (default: 80)
	  -H box-width	: width of colorbox (default: 10)
	  -d displace	: vertical box displace (default: 4)

### Keybinds
	q	quit
	u	refresh xcolors
