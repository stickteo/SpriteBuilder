
// sprite builder by teod

// *** log ***
// 21-10-31
// - all basic functionality implemented
// - implement asm generation
// 21-10-30
// - reorganize into functions
// - seem to have fixed intermitent fails
// 21-10-29
// - extraction done
// - basic descriptor parsing done
// - create default palette
// - start on descriptor parsing
// 21-10-25
// - minor edits to help
// 21-08-12
// - draft general cmd interface

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "bmp.h"

void printHelp ();
void printHelpShort ();

int extract (FILE * fdsc, FILE * fimg, FILE * from, FILE * fpal);
int compile (FILE * fdsc, FILE * fimg, FILE * fasm);

struct Sprite {
	int addr;
	int w;
	int h;
	int x;
	int y;
	int bpp;
	int pal;

	char *data;

	struct Sprite *next;
};

int main (int argc, char ** argv) {
	// flags
	//int bExt = 0;
	//int bAsm = 0;
	//int bHelp = 0;

	// debug
	for (int i=0; i<argc; i++) {
		printf("%d: %s\n",i,argv[i]);
	}

	// default
	if (argc < 2) {
		printHelpShort();
		return 0;
	}
	
	// help
	if (strcmp(argv[1],"-h")==0) {
		printHelp();
		return 0;
	}

	if (argc < 4) {
		printf("not enough arguments\n");
		return 0;
	}

	// compile asm
	if (strcmp(argv[1],"-a")==0) {
		FILE * fdsc = 0;
		FILE * fimg = 0;
		FILE * fasm = 0;
		fdsc = fopen(argv[2],"rb");
		fimg = fopen(argv[3],"rb");
		fasm = fopen(argv[4],"wb");

		int code;
		code = compile(fdsc,fimg,fasm);
		return code;
	}
	
	// extract
	if (strcmp(argv[1],"-x")==0) {
		FILE * fdsc = 0;
		FILE * fimg = 0;
		FILE * from = 0;
		FILE * fpal = 0;
		fdsc = fopen(argv[2],"rb");
		fimg = fopen(argv[3],"wb");
		from = fopen(argv[4],"rb");
		if (argc > 5) fpal = fopen(argv[5],"rb");
		
		int code;
		code = extract(fdsc,fimg,from,fpal);

		//fclose(fdsc);
		//fclose(fimg);
		//fclose(from);
		//if (argc > 5) fclose(fpal);
		return code;
	}

	return 0;
}

int def_pal [] = {
	0x000010,0x000020,0x000030,0x000040,0x000050,0x000060,0x000070,0x000080,0x00008F,0x00009F,0x0000AF,0x0000BF,0x0000CF,0x0000DF,0x0000EF,0x0000FF,
	0x000610,0x000C20,0x001230,0x001840,0x001D50,0x002360,0x002970,0x002F80,0x00358F,0x003B9F,0x0041AF,0x0047BF,0x004CCF,0x0052DF,0x0058EF,0x005EFF,
	0x000C10,0x001820,0x002430,0x003040,0x003C50,0x004860,0x005470,0x006180,0x006D8F,0x00799F,0x0085AF,0x0091BF,0x009DCF,0x00A9DF,0x00B5EF,0x00C1FF,
	0x00100E,0x00201C,0x00302A,0x004038,0x005046,0x006054,0x007062,0x008071,0x008F7F,0x009F8D,0x00AF9B,0x00BFA9,0x00CFB7,0x00DFC5,0x00EFD3,0x00FFE1,
	0x001008,0x002010,0x003018,0x004020,0x005028,0x006030,0x007038,0x008040,0x008F48,0x009F50,0x00AF58,0x00BF60,0x00CF68,0x00DF70,0x00EF78,0x00FF80,
	0x001002,0x002004,0x003006,0x004008,0x005009,0x00600B,0x00700D,0x00800F,0x008F11,0x009F13,0x00AF15,0x00BF17,0x00CF18,0x00DF1A,0x00EF1C,0x00FF1E,
	0x041000,0x082000,0x0C3000,0x104000,0x135000,0x176000,0x1B7000,0x1F8000,0x238F00,0x279F00,0x2BAF00,0x2FBF00,0x32CF00,0x36DF00,0x3AEF00,0x3EFF00,
	0x0A1000,0x142000,0x1E3000,0x284000,0x325000,0x3C6000,0x467000,0x518000,0x5B8F00,0x659F00,0x6FAF00,0x79BF00,0x83CF00,0x8DDF00,0x97EF00,0xA1FF00,
	0x101000,0x202000,0x303000,0x404000,0x505000,0x606000,0x707000,0x808000,0x8F8F00,0x9F9F00,0xAFAF00,0xBFBF00,0xCFCF00,0xDFDF00,0xEFEF00,0xFFFF00,
	0x100A00,0x201400,0x301E00,0x402800,0x503200,0x603C00,0x704600,0x805100,0x8F5B00,0x9F6500,0xAF6F00,0xBF7900,0xCF8300,0xDF8D00,0xEF9700,0xFFA100,
	0x100400,0x200800,0x300C00,0x401000,0x501300,0x601700,0x701B00,0x801F00,0x8F2300,0x9F2700,0xAF2B00,0xBF2F00,0xCF3200,0xDF3600,0xEF3A00,0xFF3E00,
	0x100002,0x200004,0x300006,0x400008,0x500009,0x60000B,0x70000D,0x80000F,0x8F0011,0x9F0013,0xAF0015,0xBF0017,0xCF0018,0xDF001A,0xEF001C,0xFF001E,
	0x100008,0x200010,0x300018,0x400020,0x500028,0x600030,0x700038,0x800040,0x8F0047,0x9F004F,0xAF0057,0xBF005F,0xCF0067,0xDF006F,0xEF0077,0xFF007F,
	0x10000E,0x20001C,0x30002A,0x400038,0x500046,0x600054,0x700062,0x800071,0x8F007F,0x9F008D,0xAF009B,0xBF00A9,0xCF00B7,0xDF00C5,0xEF00D3,0xFF00E1,
	0x0C0010,0x180020,0x240030,0x300040,0x3C0050,0x480060,0x540070,0x610080,0x6D008F,0x79009F,0x8500AF,0x9100BF,0x9D00CF,0xA900DF,0xB500EF,0xC100FF,
	0x060010,0x0C0020,0x120030,0x180040,0x1D0050,0x230060,0x290070,0x2F0080,0x35008F,0x3B009F,0x4100AF,0x4700BF,0x4C00CF,0x5200DF,0x5800EF,0x5E00FF,
};

void printSprites (struct Sprite * head) {
	struct Sprite * curr;
	curr = head;

	while (curr) {
		printf("addr:%x x:%d y:%d w:%d h:%d\n",
			curr->addr,curr->x,curr->y,curr->w,curr->h
		);
		curr = curr->next;
	}
}

void printOBJImages (struct Sprite *head) {
	struct Sprite * curr;
	curr = head;
	while (curr) {
		int w_ = curr->w;
		int h_ = curr->h;
		char *a = curr->data;
		printf("%x:\n",curr->addr);
		for (int j=0; j<h_; j++) {
			for (int i=0; i<w_; i++) {
				printf("%x",a[0]);
				a++;
			}
			printf("\n");
		}
		curr = curr->next;
	}
}

void readPalette (FILE * fpal, int * pal) {
	if (fpal) {
		fseek(fpal,0x18,SEEK_SET);
		for (int i=0; i<256; i++) {
			int r,g,b;
			b = fgetc(fpal);
			g = fgetc(fpal);
			r = fgetc(fpal);
			fgetc(fpal); // junk
			pal[i] = r+(g<<8)+(b<<16);
		}
		fclose(fpal);
	} else {
		pal = def_pal;
		for(int i=0; i<256; i++){
			pal[i] = def_pal[i];
		}
	}
}

struct Sprite * parseDescriptor (FILE * fdsc, struct BMP * b) {
	// "registers" for parsing
	int addr = 0;
	int x = 0;
	int y = 0;
	int bpp = 4;
	int palIndex = 0;

	// image params
	int minx = INT32_MAX;
	int miny = INT32_MAX;
	int maxx = INT32_MIN;
	int maxy = INT32_MIN;
	int w = 0;
	int h = 0;

	// list of sprites
	int num = 0;
	struct Sprite * head = 0;
	struct Sprite * tail = 0;
	
	// string stuff
	char * buf;
	char * tok;

	buf = malloc(sizeof(char)*1024);
	while (fgets(buf,1024,fdsc)) {
		tok = strtok(buf," ");
		do {
			// whitespace
			if (tok[0] == '\n' || tok[0] == '\r') {
				// printf(".");
				tok = strtok(NULL," ");
				continue;
			}
			
			// line comment
			if (tok[0] == ';') break;
			
			// "a" command
			if (strcmp(tok,"a")==0) {
				tok = strtok(NULL," ");	sscanf(tok,"%x",&addr);
				tok = strtok(NULL," ");
				continue;
			}

			// "m" command
			if (strcmp(tok,"m")==0) {
				tok = strtok(NULL," ");	sscanf(tok,"%d",&x);
				tok = strtok(NULL," ");	sscanf(tok,"%d",&y);
				tok = strtok(NULL," ");
				continue;
			}

			// "p" command
			if (strcmp(tok,"p")==0) {
				tok = strtok(NULL," ");	sscanf(tok,"%d",&palIndex);
				tok = strtok(NULL," ");
				continue;
			}

			// "x" command
			if (strcmp(tok,"x")==0) {
				tok = strtok(NULL," ");	sscanf(tok,"%d",&x);
				tok = strtok(NULL," ");
				continue;
			}

			// "y" command
			if (strcmp(tok,"y")==0) {
				tok = strtok(NULL," ");	sscanf(tok,"%d",&y);
				tok = strtok(NULL," ");
				continue;
			}

			// "rx" command
			if (strcmp(tok,"rx")==0) {
				int dx;
				tok = strtok(NULL," ");	sscanf(tok,"%d",&dx);
				tok = strtok(NULL," ");

				x = x + dx;

				continue;
			}

			// "ry" command
			if (strcmp(tok,"ry")==0) {
				int dy;
				tok = strtok(NULL," ");	sscanf(tok,"%d",&dy);
				tok = strtok(NULL," ");

				y = y + dy;

				continue;
			}

			// "s" command
			if (strcmp(tok,"s")==0) {
				struct Sprite * s;
				s = malloc(sizeof(struct Sprite));
				s->addr = addr;
				s->bpp  = bpp;
				s->x    = x;
				s->y    = y;
				s->pal  = palIndex;
				s->data = 0;
				s->next = NULL;
				tok = strtok(NULL," ");	sscanf(tok,"%d",&(s->w));
				tok = strtok(NULL," ");	sscanf(tok,"%d",&(s->h));
				tok = strtok(NULL," ");

				// auto increment
				addr = addr + (s->w)*(s->h)*bpp/8;
				// todo: auto increment option for x or y

				// add to list
				if (num == 0) {
					head = s;
					tail = s;
				} else {
					tail->next = s;
					tail = s;
				}
				num++;

				// canvas size
				if (x < minx) minx = x;
				if (y < miny) miny = y;
				if (x+s->w > maxx) maxx = x+s->w;
				if (y+s->h > maxy) maxy = y+s->h;

				continue;
			}
			
			// other
			printf("%s ",tok);
			tok = strtok(NULL," ");

		} while (tok);
	}
	free(buf);
	
	// bmp parameters
	if (b) {
		w = maxx - minx;
		h = maxy - miny;
		printf("image dimensions: %d %d\n",w,h);
		b->width = w;
		b->height = h;
	}

	// fix x and y offsets
	struct Sprite * curr;
	if (minx) {
		curr = head;
		while (curr) {
			curr->x = curr->x - minx;
			curr = curr->next;
		}
	}
	if (miny) {
		curr = head;
		while (curr) {
			curr->y = curr->y - miny;
			curr = curr->next;
		}
	}

	fclose(fdsc);
	return head;
}

void readROM (FILE * from, struct Sprite * head) {
	struct Sprite * curr;
	curr = head;
	while (curr) {
		fseek(from,curr->addr,SEEK_SET);
		int w = curr->w;
		int h = curr->h;
		
		char *data = malloc(sizeof(char)*w*h);
		curr->data = data;
		
		int tw = w/8;
		int th = h/8;
		
		if (curr->bpp == 4) {
			for (int j=0; j<th; j++) {
				for (int i=0; i<tw; i++) {
					// get 32 bytes
					for (int k=0; k<32; k++) {
						int p = getc(from);
						// y: 0,top  -> h-1,bottom
						// x: 0,left -> w-1,right
						int x,y;
						x = i*8 + ((k*2)%8);
						y = j*8 + k/4;
						data[y*w + x]     = p&15;
						data[y*w + x + 1] = p>>4;
					}
				}
			}
		} else if (curr->bpp == 8) {
			for (int j=0; j<th; j++) {
				for (int i=0; i<tw; i++) {
					// get 64 bytes
					for (int k=0; k<64; k++) {
						int x,y;
						x = i*8 + (k%8);
						y = j*8 + k/8;
						data[y*w + x] = getc(from);
					}
				}
			}
		} else {
			printf("other BPP formats not implemented yet :(\n");
		}

		curr = curr->next;
	}
	fclose(from);
}

int writeOBJ (struct BMP *b, struct Sprite *s) {
	int w,h;
	w = b->width;
	h = b->height;
	
	char *des = b->image;
	char *src = s->data;

	int ws,hs;
	ws = s->w;
	hs = s->h;

	int po = s->pal * 16;

	int x,y;
	y = s->y;
	for(int i=0; i<hs; i++){
		x = s->x;
		for(int j=0; j<ws; j++) {
			des[y*w+x] = src[i*ws+j] + po;
			x++;
		}
		y++;
	}

	return 0;
}

int extract (FILE * fdsc, FILE * fimg, FILE * from, FILE * fpal) {
	struct BMP * b;
	b = malloc(sizeof(struct BMP));
	b->bitCount = 8;
	b->vflip = 0;
	
	// ~~~~~~~
	// palette
	// ~~~~~~~
	int * pal = 0;
	pal = malloc(sizeof(int)*256);
	readPalette(fpal,pal);
	b->palette = pal;
	
	// ~~~~~~~
	// parsing
	// ~~~~~~~
	struct Sprite * head;
	head = parseDescriptor(fdsc,b);
	
	// debug: print params
	// printSprites(head);

	// ~~~~~~~~
	// read rom
	// ~~~~~~~~
	readROM(from,head);

	// printf("reached\n");

	// debug: print images
	// printOBJImages(head);

	// ~~~~~~~~~~~~
	// write image
	// ~~~~~~~~~~~~
	b->image = malloc(sizeof(char)*(b->width)*(b->height));
	char * img;
	img = b->image;
	
	// clear image & set pattern

	// printf("reached\n");
	
	// paste sprites onto image canvas
	struct Sprite * curr;
	curr = head;
	while (curr) {
		writeOBJ(b,curr);
		curr = curr->next;
	}
	

	// printf("reached\n");
	// output
	bmpWrite(fimg,b);
	fclose(fimg);

	// ~~~~
	// free
	// ~~~~
	free(b->image);
	free(b->palette);
	free(b);
	curr = head;
	while (curr) {
		struct Sprite *t;
		free(curr->data);
		t = curr;
		curr = curr->next;
		free(t);
	}

	// printf("reached\n");
	return 0;
}

void readOBJ (struct BMP *b, struct Sprite *s) {
	// bmp vals
	int x, y, w;
	int xmin, ymin;
	int xmax, ymax;

	// sprite vals
	//int sx,sy,sw,sh;
	
	xmin = s->x;
	ymin = s->y;
	xmax = s->x+s->w;
	ymax = s->y+s->h;
	
	w = b->width;
	//sw = s->w;
	//sh = s->h;

	char *src;
	char *des;
	src = b->image;
	//des = s->data;
	des = malloc(sizeof(char)*s->w*s->h);

	int i,k;
	k=0;
	if (s->bpp == 4) {
		for (y=ymin; y<ymax; y++) {
			i = y*w+xmin;
			for (x=xmin; x<xmax; x++) {
				des[k] = src[i]&15;
				i++;
				k++;
			}
		}
	} else if(s->bpp == 8) {
		for (y=ymin; y<ymax; y++) {
			i = y*w+xmin;
			for (x=xmin; x<xmax; x++) {
				des[k] = src[i];
				i++;
				k++;
			}
		}
	} else {
		printf("bpp not supported :(\n");
	}
	s->data = des;
}

void renderOBJ (FILE *fasm, struct Sprite *s) {
	fprintf(fasm,".org 0x%x\n",0x8000000+s->addr);
	// left to right
	// top to bottom

	int x,y,w,h;
	int i,j;
	char *c = s->data;

	w = s->w;
	h = s->h;

	if (s->bpp == 4) {
		for	(y=0; y<h; y+=8) {
			for (x=0; x<w; x+=8) {
				// for every 8x8 tile
				i = y*w+x;
				fprintf(fasm,".word ");
				for (j=0; j<8; j++) {
					// each 8px line = 4 bytes
					int line;
					line = c[i]     +(c[i+1]<<4) +(c[i+2]<<8) +(c[i+3]<<12)+
						(c[i+4]<<16)+(c[i+5]<<20)+(c[i+6]<<24)+(c[i+7]<<28);
					if (j<7) {
						fprintf(fasm,"0x%08x,",line);
					} else {
						// armips can't handle an extra comma apparently
						fprintf(fasm,"0x%08x",line);
					}

					i+=w;
				}
				fprintf(fasm,"\n");
			}
		}
	} else if (s->bpp == 8) {
		for	(y=0; y<h; y+=8) {
			for (x=0; x<w; x+=8) {
				// for every 8x8 tile
				i = y*w+x;
				fprintf(fasm,".word ");
				for (j=0; j<8; j++) {
					// each 8px line = 8 bytes
					int line1,line2;
					line1 = c[i]  +(c[i+1]<<8)+(c[i+2]<<16)+(c[i+3]<<24);
					line2 = c[i+4]+(c[i+5]<<8)+(c[i+6]<<16)+(c[i+7]<<24);
					fprintf(fasm,"0x%08x,0x%08x,",line1,line2);
					i+=w;
				}
				fprintf(fasm,"\n");
			}
		}
	} else {
		printf("bpp not supported :(\n");
	}
}

void bmpVFlip (struct BMP *b) {
	char * a;
	int y,x,h,h2,w,i,j;
	
	a = b->image;
	h = b->height;
	w = b->width;
	h2 = h/2;
	for (y=0; y<h2; y++) {
		i = y*w;
		j = (h-y-1)*w;
		for (x=0; x<w; x++) {
			char t;
			t = a[i];
			a[i] = a[j];
			a[j] = t;
			i++;
			j++;
		}
	}
}

int compile (FILE * fdsc, FILE * fimg, FILE * fasm) {
	// ~~~~~~~
	// parsing
	// ~~~~~~~
	struct Sprite * head;
	struct BMP *d = malloc(sizeof(struct BMP));
	head = parseDescriptor(fdsc,d);
	
	//printf("0..\n");

	// ~~~~~~~~
	// read bmp
	// ~~~~~~~~
	struct BMP *b;
	b = bmpRead(fimg);

	if (b->vflip) {
		//printf("flipping bmp!");
		bmpVFlip(b);
		b->vflip = 0;
	}
	
	//printf("1!\n");

	// ~~~~~~~~~~~~~~~~~
	// copy data to objs
	// ~~~~~~~~~~~~~~~~~
	struct Sprite * s;
	s = head;
	while (s) {
		readOBJ(b,s);
		s = s->next;
	}

	//printf("2!\n");

	// ~~~~~~~~~~
	// render asm
	// ~~~~~~~~~~
	s = head;
	while (s) {
		renderOBJ(fasm,s);
		s = s->next;
	}

	return 0;
}

void printHelpShort (void) {
	printf(
		"-------------- ~~~~~~~\n"
		"Sprite Builder by teod\n"
		"-------------- ~~~~~~~\n"
		"ver. alpha\n"
		"extract bmp : spritebuilder -x dsc.txt img.bmp rom.bin (pal.pal)\n"
		"compile asm : spritebuilder -a dsc.txt img.bmp patch.asm\n"
		"extra help  : spritebuilder -h (please read)\n"
		"\n"
		"dsc.txt : descriptor file containing a list of commands\n"
		"img.bmp : generated (-x) | used as source (-a)\n"
		"rom.bin : source for extraction\n"
		"patch.asm : generate ARMIPS compatible patch (-a)\n"
		"pal.pal : must be RIFF format, replaces default palette (optional)\n"
		"\n"
	);
}

void printHelpDesc (void) {
	printf(
		"------------------\n"
		"Descriptor Opcodes\n"
		"------------------\n"
		"Memory Ops\n"
		"~~~~~~~~~~\n"
		"a [addr] : sets offset in rom (hex)\n"
		"\n"
		"Sprite Ops\n"
		"~~~~~~~~~~\n"
		"s [w] [h]: outputs sprite with w width and h height (pixels)\n"
		"p [index]: sets palette index for 4-bit sprites (0-15)\n"
		"h        : half, 4-bit color for every next sprite (default)\n"
		"f        : full, 8-bit sprite\n"
		"\n"
		"Movement Ops\n"
		"~~~~~~~~~~~~\n"
		"x [x]    : sets x offset in image (pixels)\n"
		"y [y]    : sets y offset in image (pixels)\n"
		"m [x] [y]: sets both x and y in image\n"
		"rx [dx]    : relative x offset\n"
		"ry [dy]    : relative y offset\n"
		"r [dx] [dy]: relative x and y\n"
		"l        : newline, y incremented to empty spot, x is preserved\n"
		"\n"
		"Misc. Ops\n"
		"~~~~~~~~~\n"
		";        : line comment\n"
	);
}

void printHelpProc (void) {
	printf(
		"--------------------\n"
		"Process For Patching\n"
		"--------------------\n"
		"1. create descriptor file\n"
		"2. extract bmp\n"
		"3. edit bmp (using usenti)\n"
		"4. use bmp to make asm file\n"
		"5. patch using ARMIPS\n"
		"\n"
	);
}

void printHelp (void) {
	printHelpShort();
	printHelpProc();
	printHelpDesc();
}
