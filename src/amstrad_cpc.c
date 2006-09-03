#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include "config.h"
#include "xcpc.h"
#include "z80.h"
#include "crtc_6845.h"
#include "ppi_8255.h"
#include "ay_3_8910.h"
#include "fdc_765.h"
#include "amstrad_cpc.h"

unsigned int amstrad_cpc_width = 800;
unsigned int amstrad_cpc_height = 624;

static char *tbl_version[6] = {
  "[unknown]",
  "CPC 464",
  "CPC 664",
  "CPC 6128",
  "CPC 464+",
  "CPC 6128+"
};

static char *tbl_clock[4] = {
  "2 MHz",
  "4 MHz",
  "8 MHz",
  "16 MHz"
};

static char *tbl_refresh[2] = {
  "60 Hz",
  "50 Hz"
};

static char *tbl_manufacturer[8] = {
  "Isp",
  "Triumph",
  "Saisho",
  "Solavox",
  "Awa",
  "Schneider",
  "Orion",
  "Amstrad"
};

static char *tbl_monitor[2] = {
  "CTM65",
  "CTM644"
};

static char *tbl_expansion[2] = {
  "Absent",
  "Present"
};

typedef struct {
  int version;
  int clock;
  int refresh;
  int manufacturer;
  int monitor;
  int expansion;
  int frequency;
  int interrupt;
  int ramsize;
  char *rom[8];
} AMSTRAD_CPC_CONFIGURATION;

static AMSTRAD_CPC_CONFIGURATION cfg = {
  AMSTRAD_CPC_6128,    /* version           */
  AMSTRAD_CPC_4MHZ,    /* clock             */
  AMSTRAD_CPC_50HZ,    /* refresh           */
  AMSTRAD_CPC_AMSTRAD, /* manufacturer      */
  AMSTRAD_CPC_CTM644,  /* monitor           */
  AMSTRAD_CPC_PRESENT, /* expansion         */
  4000000,             /* frequency (in Hz) */
  64,                  /* interrupt (in us) */
  128,                 /* ramsize           */
  "rom/cpc6128.rom",   /* system rom        */
  NULL,                /* expansion rom #1  */
  NULL,                /* expansion rom #2  */
  NULL,                /* expansion rom #3  */
  NULL,                /* expansion rom #4  */
  NULL,                /* expansion rom #5  */
  NULL,                /* expansion rom #6  */
  "rom/amsdos.rom",    /* expansion rom #7  */
};

AMSTRAD_CPC amstrad_cpc;

static int _ticks = 0;

static void amstrad_cpc_synchronize(int signum)
{
  if(!paused) {
    _ticks++;
  }
}

static byte *_bank[4];
static byte *_read[4];
static byte *_write[4];

void amstrad_cpc_set_ram_configuration(void)
{
  switch(amstrad_cpc.gate_array.ram_configuration) {
    case 0x00:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x04000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x01:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x04000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x1C000;
      break;
    case 0x02:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x10000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x14000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x18000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x1C000;
      break;
    case 0x03:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x04000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x1C000;
      break;
    case 0x04:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x10000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x05:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x14000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x06:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x18000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
    case 0x07:
      _write[0] = _bank[0] = amstrad_cpc.memory.ram + 0x00000;
      _write[1] = _bank[1] = amstrad_cpc.memory.ram + 0x1C000;
      _write[2] = _bank[2] = amstrad_cpc.memory.ram + 0x08000;
      _write[3] = _bank[3] = amstrad_cpc.memory.ram + 0x0C000;
      break;
  }
  _read[0] = (!amstrad_cpc.gate_array.lower_ram_enable ? amstrad_cpc.memory.lower_rom : _bank[0]);
  _read[1] = _bank[1];
  _read[2] = _bank[2];
  if(!amstrad_cpc.gate_array.upper_ram_enable) {
    _read[3] = (amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion] == NULL ? amstrad_cpc.memory.upper_rom[0] : amstrad_cpc.memory.upper_rom[amstrad_cpc.memory.expansion]);
  }
  else {
    _read[3] = _bank[3];
  }
}

static byte _mode0[256] = {
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x02, 0x02, 0x0A, 0x0A, 0x02, 0x02, 0x0A, 0x0A,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x04, 0x04, 0x0C, 0x0C, 0x04, 0x04, 0x0C, 0x0C, 0x06, 0x06, 0x0E, 0x0E, 0x06, 0x06, 0x0E, 0x0E,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09, 0x03, 0x03, 0x0B, 0x0B, 0x03, 0x03, 0x0B, 0x0B,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F,
  0x05, 0x05, 0x0D, 0x0D, 0x05, 0x05, 0x0D, 0x0D, 0x07, 0x07, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F
};

static byte _mode1[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
};

static byte _mode2[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

static unsigned long _ink[17];

void amstrad_cpc_redraw_0(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (amstrad_cpc_width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((amstrad_cpc_height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned long color;
unsigned long border = _ink[0x10];
int col, row = 0;

  for(cy = 0; cy < vp; cy++) {
    col = 0;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      XPutPixel(x11.ximage, col, row++, border);
      XPutPixel(x11.ximage, col++, row--, border);
    }
    row += 2;
  }
  switch(amstrad_cpc.gate_array.mode) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          col = 0;
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(x11.ximage, col, row++, border);
            XPutPixel(x11.ximage, col++, row--, border);
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
              XPutPixel(x11.ximage, col, row++, color);
              XPutPixel(x11.ximage, col++, row, color);
              XPutPixel(x11.ximage, col, row--, color);
              XPutPixel(x11.ximage, col++, row, color);
              XPutPixel(x11.ximage, col, row++, color);
              XPutPixel(x11.ximage, col++, row, color);
              XPutPixel(x11.ximage, col, row--, color);
              XPutPixel(x11.ximage, col++, row, color);
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(x11.ximage, col, row++, border);
            XPutPixel(x11.ximage, col++, row--, border);
          }
          row += 2;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          col = 0;
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(x11.ximage, col, row++, border);
            XPutPixel(x11.ximage, col++, row--, border);
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              XPutPixel(x11.ximage, col, row++, color);
              XPutPixel(x11.ximage, col++, row, color);
              XPutPixel(x11.ximage, col, row--, color);
              XPutPixel(x11.ximage, col++, row, color);
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(x11.ximage, col, row++, border);
            XPutPixel(x11.ximage, col++, row--, border);
          }
          row += 2;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          col = 0;
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(x11.ximage, col, row++, border);
            XPutPixel(x11.ximage, col++, row--, border);
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              XPutPixel(x11.ximage, col, row++, color);
              XPutPixel(x11.ximage, col++, row--, color);
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            XPutPixel(x11.ximage, col, row++, border);
            XPutPixel(x11.ximage, col++, row--, border);
          }
          row += 2;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    col = 0;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      XPutPixel(x11.ximage, col, row++, border);
      XPutPixel(x11.ximage, col++, row--, border);
    }
    row += 2;
  }
  XPutImage(x11.display, x11.window, x11.gc, x11.ximage, 0, 0, 0, 0, amstrad_cpc_width, amstrad_cpc_height);
}

void amstrad_cpc_redraw_8(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (amstrad_cpc_width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((amstrad_cpc_height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned char color;
unsigned char border = _ink[0x10];
unsigned char *dst = (unsigned char *) x11.ximage->data;
unsigned char *nxt = (unsigned char *) x11.ximage->data;

  for(cy = 0; cy < vp; cy++) {
    nxt += amstrad_cpc_width;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  switch(amstrad_cpc.gate_array.mode) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += amstrad_cpc_width;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  XPutImage(x11.display, x11.window, x11.gc, x11.ximage, 0, 0, 0, 0, amstrad_cpc_width, amstrad_cpc_height);
}

void amstrad_cpc_redraw_16(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (amstrad_cpc_width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((amstrad_cpc_height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned short color;
unsigned short border = _ink[0x10];
unsigned short *dst = (unsigned short *) x11.ximage->data;
unsigned short *nxt = (unsigned short *) x11.ximage->data;

  for(cy = 0; cy < vp; cy++) {
    nxt += amstrad_cpc_width;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  switch(amstrad_cpc.gate_array.mode) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += amstrad_cpc_width;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  XPutImage(x11.display, x11.window, x11.gc, x11.ximage, 0, 0, 0, 0, amstrad_cpc_width, amstrad_cpc_height);
}

void amstrad_cpc_redraw_32(void)
{
byte *src = _write[crtc_6845.registers[12] >> 4];
word base = 0x0000;
word offset = ((crtc_6845.registers[12] << 8) | crtc_6845.registers[13]) << 1;
unsigned int hd = crtc_6845.registers[1] << 1;
unsigned int hp = (amstrad_cpc_width - (hd << 3)) >> 1;
unsigned int mr = crtc_6845.registers[9] + 1;
unsigned int mask = (mr << 11) - 1;
unsigned int vd = crtc_6845.registers[6];
unsigned int vp = ((amstrad_cpc_height >> 1) - (vd * mr)) >> 1;
unsigned int cx, cy, cxx, cyy;
byte value;
unsigned int color;
unsigned int border = _ink[0x10];
unsigned int *dst = (unsigned int *) x11.ximage->data;
unsigned int *nxt = (unsigned int *) x11.ximage->data;

  for(cy = 0; cy < vp; cy++) {
    nxt += amstrad_cpc_width;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  switch(amstrad_cpc.gate_array.mode) {
    case 0x00:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 2; cxx++) {
              color = _ink[_mode0[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x01:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 4; cxx++) {
              color = _ink[_mode1[value]];
              *dst++ = *nxt++ = color;
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
    case 0x02:
      for(cy = 0; cy < vd; cy++) {
        for(cyy = 0; cyy < mr; cyy++) {
          nxt += amstrad_cpc_width;
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          for(cx = 0; cx < hd; cx++) {
            value = src[base + ((offset + hd * cy + cx) & 0x07FF)];
            for(cxx = 0; cxx < 8; cxx++) {
              color = _ink[_mode2[value]];
              *dst++ = *nxt++ = color;
              value <<= 1;
            }
          }
          for(cx = 0; cx < hp; cx++) {
            *dst++ = *nxt++ = border;
          }
          dst = nxt;
          base = (base + 2048) & mask;
        }
      }
      break;
  }
  for(cy = 0; cy < vp; cy++) {
    nxt += amstrad_cpc_width;
    for(cx = 0; cx < amstrad_cpc_width; cx++) {
      *dst++ = *nxt++ = border;
    }
    dst = nxt;
  }
  XPutImage(x11.display, x11.window, x11.gc, x11.ximage, 0, 0, 0, 0, amstrad_cpc_width, amstrad_cpc_height);
}

void (*amstrad_cpc_redraw)(void) = amstrad_cpc_redraw_0;

static int _cycle = 0;

static XColor _palette[32] = {
  { 0L, 0x7F7F, 0x7F7F, 0x7F7F, 0 },
  { 0L, 0x7F7F, 0x7F7F, 0x7F7F, 0 },
  { 0L, 0x0000, 0xFFFF, 0x7F7F, 0 },
  { 0L, 0xFFFF, 0xFFFF, 0x7F7F, 0 },
  { 0L, 0x0000, 0x0000, 0x7F7F, 0 },
  { 0L, 0xFFFF, 0x0000, 0x7F7F, 0 },
  { 0L, 0x0000, 0x7F7F, 0x7F7F, 0 },
  { 0L, 0xFFFF, 0x7F7F, 0x7F7F, 0 },
  { 0L, 0xFFFF, 0x0000, 0x7F7F, 0 },
  { 0L, 0xFFFF, 0xFFFF, 0x7F7F, 0 },
  { 0L, 0xFFFF, 0xFFFF, 0x0000, 0 },
  { 0L, 0xFFFF, 0xFFFF, 0xFFFF, 0 },
  { 0L, 0xFFFF, 0x0000, 0x0000, 0 },
  { 0L, 0xFFFF, 0x0000, 0xFFFF, 0 },
  { 0L, 0xFFFF, 0x7F7F, 0x0000, 0 },
  { 0L, 0xFFFF, 0x7F7F, 0xFFFF, 0 },
  { 0L, 0x0000, 0x0000, 0x7F7F, 0 },
  { 0L, 0x0000, 0xFFFF, 0x7F7F, 0 },
  { 0L, 0x0000, 0xFFFF, 0x0000, 0 },
  { 0L, 0x0000, 0xFFFF, 0xFFFF, 0 },
  { 0L, 0x0000, 0x0000, 0x0000, 0 },
  { 0L, 0x0000, 0x0000, 0xFFFF, 0 },
  { 0L, 0x0000, 0x7F7F, 0x0000, 0 },
  { 0L, 0x0000, 0x7F7F, 0xFFFF, 0 },
  { 0L, 0x7F7F, 0x0000, 0x7F7F, 0 },
  { 0L, 0x7F7F, 0xFFFF, 0x7F7F, 0 },
  { 0L, 0x7F7F, 0xFFFF, 0x0000, 0 },
  { 0L, 0x7F7F, 0xFFFF, 0xFFFF, 0 },
  { 0L, 0x7F7F, 0x0000, 0x0000, 0 },
  { 0L, 0x7F7F, 0x0000, 0xFFFF, 0 },
  { 0L, 0x7F7F, 0x7F7F, 0x0000, 0 },
  { 0L, 0x7F7F, 0x7F7F, 0xFFFF, 0 },
};

void amstrad_cpc_parse(int argc, char **argv)
{
char *progname = *argv;

  while(--argc) {
    argv++;
    if(!strcmp("-cpc464", *argv)) {
      cfg.version = AMSTRAD_CPC_464;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.expansion = AMSTRAD_CPC_ABSENT;
      cfg.frequency = 4000000;
      cfg.interrupt = 64;
      cfg.ramsize = 64;
      cfg.rom[0x00] = "rom/cpc464.rom";
      cfg.rom[0x07] = NULL;
    }
    else if(!strcmp("-cpc664", *argv)) {
      cfg.version = AMSTRAD_CPC_664;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.frequency = 4000000;
      cfg.interrupt = 64;
      cfg.ramsize = 64;
      cfg.rom[0x00] = "rom/cpc664.rom";
      cfg.rom[0x07] = "rom/amsdos.rom";
    }
    else if(!strcmp("-cpc6128", *argv)) {
      cfg.version = AMSTRAD_CPC_6128;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.frequency = 4000000;
      cfg.interrupt = 64;
      cfg.ramsize = 128;
      cfg.rom[0x00] = "rom/cpc6128.rom";
      cfg.rom[0x07] = "rom/amsdos.rom";
    }
    else if(!strcmp("-cpc464+", *argv)) {
      cfg.version = AMSTRAD_CPC_464_PLUS;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.expansion = AMSTRAD_CPC_ABSENT;
      cfg.frequency = 4000000;
      cfg.interrupt = 64;
      cfg.ramsize = 64;
      cfg.rom[0x00] = "rom/cpc464+.rom";
      cfg.rom[0x07] = NULL;
    }
    else if(!strcmp("-cpc6128+", *argv)) {
      cfg.version = AMSTRAD_CPC_6128_PLUS;
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
      cfg.monitor = AMSTRAD_CPC_CTM644;
      cfg.expansion = AMSTRAD_CPC_PRESENT;
      cfg.frequency = 4000000;
      cfg.interrupt = 64;
      cfg.ramsize = 128;
      cfg.rom[0x00] = "rom/cpc6128+.rom";
      cfg.rom[0x07] = "rom/amsdos.rom";
    }
    else if(!strcmp("-2MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_2MHZ;
      cfg.frequency = 2000000;
    }
    else if(!strcmp("-4MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_4MHZ;
      cfg.frequency = 4000000;
    }
    else if(!strcmp("-8MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_8MHZ;
      cfg.frequency = 8000000;
    }
    else if(!strcmp("-16MHz", *argv)) {
      cfg.clock = AMSTRAD_CPC_16MHZ;
      cfg.frequency = 16000000;
    }
    else if(!strcmp("-50Hz", *argv)) {
      cfg.refresh = AMSTRAD_CPC_50HZ;
      cfg.interrupt = 64;
    }
    else if(!strcmp("-60Hz", *argv)) {
      cfg.refresh = AMSTRAD_CPC_60HZ;
      cfg.interrupt = 53;
    }
    else if(!strcmp("-isp", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_ISP;
    }
    else if(!strcmp("-triumph", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_TRIUMPH;
    }
    else if(!strcmp("-saisho", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_SAISHO;
    }
    else if(!strcmp("-solavox", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_SOLAVOX;
    }
    else if(!strcmp("-awa", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_AWA;
    }
    else if(!strcmp("-schneider", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_SCHNEIDER;
    }
    else if(!strcmp("-orion", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_ORION;
    }
    else if(!strcmp("-amstrad", *argv)) {
      cfg.manufacturer = AMSTRAD_CPC_AMSTRAD;
    }
    else if(!strcmp("-CTM65", *argv)) {
      cfg.monitor = AMSTRAD_CPC_CTM65;
    }
    else if(!strcmp("-CTM644", *argv)) {
      cfg.monitor = AMSTRAD_CPC_CTM644;
    }
    else if(!strcmp("-tiny", *argv)) {
      amstrad_cpc_width = 656;
      amstrad_cpc_height = 416;
    }
    else if(!strcmp("-small", *argv)) {
      amstrad_cpc_width = 680;
      amstrad_cpc_height = 456;
    }
    else if(!strcmp("-medium", *argv)) {
      amstrad_cpc_width = 720;
      amstrad_cpc_height = 512;
    }
    else if(!strcmp("-big", *argv)) {
      amstrad_cpc_width = 760;
      amstrad_cpc_height = 568;
    }
    else if(!strcmp("-huge", *argv)) {
      amstrad_cpc_width = 800;
      amstrad_cpc_height = 624;
    }
    else {
      fprintf(stderr, "usage: %s [-h]\n", progname);
      fprintf(stderr, "             [-cpc464] [-cpc664] [-cpc6128]\n");
      fprintf(stderr, "             [-2MHz] [-4MHz] [-8MHz] [-16MHz]\n");
      fprintf(stderr, "             [-50Hz] [-60Hz]\n");
      fprintf(stderr, "             [-isp] [-triumph] [-saisho] [-solavox]\n");
      fprintf(stderr, "             [-awa] [-schneider] [-orion] [-amstrad]\n");
      fprintf(stderr, "             [-CTM65] [-CTM644]\n");
      fprintf(stderr, "             [-tiny] [-small] [-medium] [-big] [-huge]\n");
      exit(0);
    }
  }
}

void amstrad_cpc_init(void)
{
int ix;
XColor xcolor;
FILE *file;

  fprintf(stderr, "amstrad_cpc: version ................. %s\n", tbl_version[cfg.version]);
  fprintf(stderr, "amstrad_cpc: clock ................... %s\n", tbl_clock[cfg.clock]);
  fprintf(stderr, "amstrad_cpc: refresh ................. %s\n", tbl_refresh[cfg.refresh]);
  fprintf(stderr, "amstrad_cpc: manufacturer ............ %s\n", tbl_manufacturer[cfg.manufacturer]);
  fprintf(stderr, "amstrad_cpc: monitor ................. %s\n", tbl_monitor[cfg.monitor]);
  fprintf(stderr, "amstrad_cpc: expansion ............... %s\n", tbl_expansion[cfg.expansion]);
  switch(x11.ximage->depth) {
    case 8:
      fprintf(stderr, "amstrad_cpc: screen driver ........... optimized 8 bpp\n");
      amstrad_cpc_redraw = amstrad_cpc_redraw_8;
      break;
    case 15:
    case 16:
      fprintf(stderr, "amstrad_cpc: screen driver ........... optimized 16 bpp\n");
      amstrad_cpc_redraw = amstrad_cpc_redraw_16;
      break;
    case 24:
    case 32:
      fprintf(stderr, "amstrad_cpc: screen driver ........... optimized 32 bpp\n");
      amstrad_cpc_redraw = amstrad_cpc_redraw_32;
      break;
    default:
      fprintf(stderr, "amstrad_cpc: screen driver ........... generic (%d bpp)\n", x11.ximage->depth);
      amstrad_cpc_redraw = amstrad_cpc_redraw_0;
      break;
  }
  for(ix = 0; ix < 32; ix++) {
    if(cfg.monitor == AMSTRAD_CPC_CTM65) {
      xcolor.red = 0x0000;
      xcolor.green = (_palette[ix].red + _palette[ix].green + _palette[ix].blue) / 3;
      xcolor.blue = 0x0000;
      xcolor.flags = DoRed | DoGreen | DoBlue;
    }
    else {
      xcolor.red = _palette[ix].red;
      xcolor.green = _palette[ix].green;
      xcolor.blue = _palette[ix].blue;
      xcolor.flags = DoRed | DoGreen | DoBlue;
    }
    if(XAllocColor(x11.display, DefaultColormapOfScreen(x11.screen), &xcolor) == False) {
      fprintf(stderr, "amstrad_cpc: cannot allocate color ... %04x/%04x/%04x\n", xcolor.red, xcolor.green, xcolor.blue);
    }
    _palette[ix].pixel = xcolor.pixel;
  }
  fprintf(stderr, "amstrad_cpc: on-board ram ............ %d Kb\n", cfg.ramsize);
  if((amstrad_cpc.memory.ram = (byte *) malloc(cfg.ramsize * 1024)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  if((file = fopen(cfg.rom[0], "r")) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fprintf(stderr, "amstrad_cpc: system lower rom ........ %s\n", cfg.rom[0]);
  if((amstrad_cpc.memory.lower_rom = (byte *) malloc(16384)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fread(amstrad_cpc.memory.lower_rom, 1, 16384, file);
  fprintf(stderr, "amstrad_cpc: system upper rom ........ %s\n", cfg.rom[0]);
  if((amstrad_cpc.memory.upper_rom[0] = (byte *) malloc(16384)) == NULL) {
    perror("amstrad_cpc"); exit(-1);
  }
  fread(amstrad_cpc.memory.upper_rom[0], 1, 16384, file);
  fclose(file);
  for(ix = 1; ix < 8; ix++) {
    amstrad_cpc.memory.upper_rom[ix] = NULL;
    fprintf(stderr, "amstrad_cpc: expansion rom #%d ........ %s\n", ix, (cfg.rom[ix] == NULL ? "(not present)" : cfg.rom[ix]));
    if(cfg.rom[ix] != NULL) {
      if((file = fopen(cfg.rom[ix], "r")) == NULL) {
        perror("amstrad_cpc"); continue;
      }
      if((amstrad_cpc.memory.upper_rom[ix] = (byte *) malloc(16384)) == NULL) {
        perror("amstrad_cpc"); exit(-1);
      }
      fread(amstrad_cpc.memory.upper_rom[ix], 1, 16384, file);
      fclose(file);
    }
  }
  z80.IPeriod = (cfg.frequency * cfg.interrupt) / 1000000;
  z80_init();
  crtc_6845_init();
  ppi_8255_init();
  ay_3_8910_init();
  fdc_765_init();
  amstrad_cpc_reset();
}

void amstrad_cpc_reset(void)
{
int ix;

  _cycle = 0x00;
  amstrad_cpc.memory.expansion = 0x00;
  amstrad_cpc.keyboard.row = 0x00;
  for(ix = 0; ix < 16; ix++) {
    amstrad_cpc.keyboard.line[ix] = 0xFF;
  }
  amstrad_cpc.gate_array.pen = 0x00;
  for(ix = 0; ix < 17; ix++) {
    amstrad_cpc.gate_array.ink[ix] = 0x00;
    _ink[ix] = _palette[amstrad_cpc.gate_array.ink[ix]].pixel;
  }
  amstrad_cpc.gate_array.generate_interrupts = 0x00;
  amstrad_cpc.gate_array.upper_ram_enable = 0x00;
  amstrad_cpc.gate_array.lower_ram_enable = 0x00;
  amstrad_cpc.gate_array.mode = 0x00;
  amstrad_cpc.gate_array.ram_configuration = 0x00;
  amstrad_cpc.gate_array.counter = 0x00;
  amstrad_cpc_set_ram_configuration();
  z80_reset();
  crtc_6845_reset();
  ppi_8255_reset();
  ppi_8255.port_b = (1 << 7) | (cfg.expansion << 5) | (cfg.refresh << 4) | (cfg.manufacturer << 1);
  ay_3_8910_reset();
  fdc_765_reset();
}

void amstrad_cpc_exit(void)
{
int ix;

  amstrad_cpc_redraw = amstrad_cpc_redraw_0;
  free(amstrad_cpc.memory.lower_rom);
  free(amstrad_cpc.memory.ram);
  for(ix = 0; ix < 8; ix++) {
    if(amstrad_cpc.memory.upper_rom[ix] != NULL) {
      free(amstrad_cpc.memory.upper_rom[ix]);
    }
  }
  z80_exit();
  crtc_6845_exit();
  ppi_8255_exit();
  ay_3_8910_exit();
  fdc_765_exit();
}

void amstrad_cpc_run(void)
{
struct itimerval new;
struct itimerval old;

  new.it_interval.tv_sec = new.it_value.tv_sec = 0;
  new.it_interval.tv_usec = new.it_value.tv_usec = 1000000 / (cfg.refresh == AMSTRAD_CPC_60HZ ? 60 : 50);
  signal(SIGALRM, amstrad_cpc_synchronize);
  setitimer(ITIMER_REAL, &new, &old);
  z80_run();
  setitimer(ITIMER_REAL, &old, &new);
}

void amstrad_cpc_load_snapshot(char *filename)
{
FILE *file;
byte buffer[256], *bufptr = buffer;
int ix;
int ramsize;

  if((file = fopen(filename, "r")) == NULL) {
    perror("amstrad_cpc");
    return;
  }
  fread(buffer, 1, 256, file);
  if(strncmp(bufptr, "MV - SNA", 8)) {
    fprintf(stderr, "amstrad_cpc: not a valid snapshot file (bad signature)\n");
    fclose(file);
    return;
  }
  bufptr += 8;
  bufptr += 8;
  bufptr++;
  z80.AF.B.l = *bufptr++;
  z80.AF.B.h = *bufptr++;
  z80.BC.B.l = *bufptr++;
  z80.BC.B.h = *bufptr++;
  z80.DE.B.l = *bufptr++;
  z80.DE.B.h = *bufptr++;
  z80.HL.B.l = *bufptr++;
  z80.HL.B.h = *bufptr++;
  z80.Refresh = *bufptr++;
  z80.I = *bufptr++;
  z80.IFF = (!(*bufptr++) ? z80.IFF & 0x7F : z80.IFF | 0x80); /* IFF1 */
  z80.IFF = (!(*bufptr++) ? z80.IFF & 0xFE : z80.IFF | 0x01); /* IFF2 */
  z80.IX.B.l = *bufptr++;
  z80.IX.B.h = *bufptr++;
  z80.IY.B.l = *bufptr++;
  z80.IY.B.h = *bufptr++;
  z80.SP.B.l = *bufptr++;
  z80.SP.B.h = *bufptr++;
  z80.PC.B.l = *bufptr++;
  z80.PC.B.h = *bufptr++;
  switch(*bufptr++) { /* IM */
    case 0:
      z80.IFF &= 0xF9;
      break;
    case 1:
      z80.IFF = (z80.IFF & 0xF9) | 2;
      break;
    case 2:
      z80.IFF = (z80.IFF & 0xF9) | 4;
      break;
  }
  z80.AF1.B.l = *bufptr++;
  z80.AF1.B.h = *bufptr++;
  z80.BC1.B.l = *bufptr++;
  z80.BC1.B.h = *bufptr++;
  z80.DE1.B.l = *bufptr++;
  z80.DE1.B.h = *bufptr++;
  z80.HL1.B.l = *bufptr++;
  z80.HL1.B.h = *bufptr++;
  amstrad_cpc.gate_array.pen = *bufptr++;
  for(ix = 0; ix < 17; ix++) {
    amstrad_cpc.gate_array.ink[ix] = *bufptr++;
    _ink[ix] = _palette[amstrad_cpc.gate_array.ink[ix]].pixel;
  }
  amstrad_cpc.gate_array.generate_interrupts = *bufptr & 0x10;
  amstrad_cpc.gate_array.upper_ram_enable = *bufptr & 0x08;
  amstrad_cpc.gate_array.lower_ram_enable = *bufptr & 0x04;
  amstrad_cpc.gate_array.mode = *bufptr & 0x03;
  bufptr++;
  amstrad_cpc.gate_array.ram_configuration = *bufptr++;
  amstrad_cpc_set_ram_configuration();
  crtc_6845.current = *bufptr++;
  for(ix = 0; ix < 18; ix++) {
    crtc_6845.registers[ix] = *bufptr++;
  }
  amstrad_cpc.memory.expansion = *bufptr++;
  ppi_8255.port_a = *bufptr++;
  ppi_8255.port_b = *bufptr++;
  ppi_8255.port_c = *bufptr++;
  ppi_8255.control = *bufptr++;
  ay_3_8910.current = *bufptr++;
  for(ix = 0; ix < 16; ix++) {
    ay_3_8910.registers[ix] = *bufptr++;
  }
  ramsize = *bufptr++;
  ramsize |= *bufptr++ << 8;
  if(ramsize > cfg.ramsize) {
    fprintf(stderr, "amstrad_cpc: snapshot file too large (%d Kb)\n", ramsize);
    amstrad_cpc_reset();
    fclose(file);
    return;
  }
  fread(amstrad_cpc.memory.ram, 1, ramsize * 1024, file);
  fclose(file);
}

void amstrad_cpc_save_snapshot(char *filename)
{
FILE *file;
byte buffer[256], *bufptr = buffer;
int ix;
int ramsize;

  if((file = fopen(filename, "w")) == NULL) {
    perror("amstrad_cpc");
    return;
  }

  strncpy(bufptr, "MV - SNA", 8);
  bufptr += 8;
  memset(bufptr, 0, 8);
  bufptr += 8;

  *bufptr++ = 1; /* snapshot version */

  *bufptr++ = z80.AF.B.l;
  *bufptr++ = z80.AF.B.h;
  *bufptr++ = z80.BC.B.l;
  *bufptr++ = z80.BC.B.h;
  *bufptr++ = z80.DE.B.l;
  *bufptr++ = z80.DE.B.h;
  *bufptr++ = z80.HL.B.l;
  *bufptr++ = z80.HL.B.h;
  *bufptr++ = z80.Refresh;
  *bufptr++ = z80.I;
  *bufptr++ = (z80.IFF & 0x80 ? 0x01 : 0x00);
  *bufptr++ = (z80.IFF & 0x01 ? 0x01 : 0x00);
  *bufptr++ = z80.IX.B.l;
  *bufptr++ = z80.IX.B.h;
  *bufptr++ = z80.IY.B.l;
  *bufptr++ = z80.IY.B.h;
  *bufptr++ = z80.SP.B.l;
  *bufptr++ = z80.SP.B.h;
  *bufptr++ = z80.PC.B.l;
  *bufptr++ = z80.PC.B.h;
  switch(z80.IFF & 0x06) {
    case 0:
      *bufptr++ = 0x00;
      break;
    case 2:
      *bufptr++ = 0x01;
      break;
    case 4:
      *bufptr++ = 0x02;
      break;
    default:
      *bufptr++ = 0x00;
      break;
  }
  *bufptr++ = z80.AF1.B.l;
  *bufptr++ = z80.AF1.B.h;
  *bufptr++ = z80.BC1.B.l;
  *bufptr++ = z80.BC1.B.h;
  *bufptr++ = z80.DE1.B.l;
  *bufptr++ = z80.DE1.B.h;
  *bufptr++ = z80.HL1.B.l;
  *bufptr++ = z80.HL1.B.h;
  *bufptr++ = amstrad_cpc.gate_array.pen;
  for(ix = 0; ix < 17; ix++) {
    *bufptr++ = amstrad_cpc.gate_array.ink[ix];
  }
  *bufptr |= amstrad_cpc.gate_array.generate_interrupts;
  *bufptr |= amstrad_cpc.gate_array.upper_ram_enable;
  *bufptr |= amstrad_cpc.gate_array.lower_ram_enable;
  *bufptr |= amstrad_cpc.gate_array.mode;
  bufptr++;
  *bufptr++ = amstrad_cpc.gate_array.ram_configuration;
  *bufptr++ = crtc_6845.current;
  for(ix = 0; ix < 18; ix++) {
    *bufptr++ = crtc_6845.registers[ix];
  }
  *bufptr++ = amstrad_cpc.memory.expansion;
  *bufptr++ = ppi_8255.port_a;
  *bufptr++ = ppi_8255.port_b;
  *bufptr++ = ppi_8255.port_c;
  *bufptr++ = ppi_8255.control;
  *bufptr++ = ay_3_8910.current;
  for(ix = 0; ix < 16; ix++) {
    *bufptr++ = ay_3_8910.registers[ix];
  }
  *bufptr++ = cfg.ramsize & 0xFF;
  *bufptr++ = (cfg.ramsize >> 8) & 0xFF;
  memset(bufptr, 0, 147);
  bufptr += 147;
  fwrite(buffer, 1, 256, file);
  fwrite(amstrad_cpc.memory.ram, 1, cfg.ramsize * 1024, file);
  fclose(file);
}

static byte _key[256][4] = {
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x09, 0x80, 0x00 }, /* bs  */
  { 0x01, 0x08, 0x10, 0x00 }, /* ht  */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x02, 0x04, 0x00 }, /* cr  */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x08, 0x04, 0x00 }, /* esc */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x05, 0x80, 0x00 }, /* sp  */
  { 0x01, 0x08, 0x01, 0x01 }, /* !   */
  { 0x01, 0x08, 0x02, 0x01 }, /* "   */
  { 0x01, 0x07, 0x02, 0x01 }, /* #   */
  { 0x01, 0x07, 0x01, 0x01 }, /* $   */
  { 0x01, 0x06, 0x02, 0x01 }, /* %   */
  { 0x01, 0x06, 0x01, 0x01 }, /* &   */
  { 0x01, 0x05, 0x02, 0x01 }, /* '   */
  { 0x01, 0x05, 0x01, 0x01 }, /* (   */
  { 0x01, 0x04, 0x02, 0x01 }, /* )   */
  { 0x01, 0x03, 0x20, 0x01 }, /* *   */
  { 0x01, 0x03, 0x10, 0x01 }, /* +   */
  { 0x01, 0x04, 0x80, 0x00 }, /* ,   */
  { 0x01, 0x03, 0x02, 0x00 }, /* -   */
  { 0x01, 0x03, 0x80, 0x00 }, /* .   */
  { 0x01, 0x03, 0x40, 0x00 }, /* /   */
  { 0x01, 0x04, 0x01, 0x00 }, /* 0   */
  { 0x01, 0x08, 0x01, 0x00 }, /* 1   */
  { 0x01, 0x08, 0x02, 0x00 }, /* 2   */
  { 0x01, 0x07, 0x02, 0x00 }, /* 3   */
  { 0x01, 0x07, 0x01, 0x00 }, /* 4   */
  { 0x01, 0x06, 0x02, 0x00 }, /* 5   */
  { 0x01, 0x06, 0x01, 0x00 }, /* 6   */
  { 0x01, 0x05, 0x02, 0x00 }, /* 7   */
  { 0x01, 0x05, 0x01, 0x00 }, /* 8   */
  { 0x01, 0x04, 0x02, 0x00 }, /* 9   */
  { 0x01, 0x03, 0x20, 0x00 }, /* :   */
  { 0x01, 0x03, 0x10, 0x00 }, /* ;   */
  { 0x01, 0x04, 0x80, 0x00 }, /* <   */
  { 0x01, 0x03, 0x02, 0x01 }, /* =   */
  { 0x01, 0x03, 0x80, 0x00 }, /* >   */
  { 0x01, 0x03, 0x40, 0x01 }, /* ?   */
  { 0x01, 0x03, 0x04, 0x00 }, /* @   */
  { 0x01, 0x08, 0x20, 0x01 }, /* A   */
  { 0x01, 0x06, 0x40, 0x01 }, /* B   */
  { 0x01, 0x07, 0x40, 0x01 }, /* C   */
  { 0x01, 0x07, 0x20, 0x01 }, /* D   */
  { 0x01, 0x07, 0x04, 0x01 }, /* E   */
  { 0x01, 0x06, 0x20, 0x01 }, /* F   */
  { 0x01, 0x06, 0x10, 0x01 }, /* G   */
  { 0x01, 0x05, 0x10, 0x01 }, /* H   */
  { 0x01, 0x04, 0x08, 0x01 }, /* I   */
  { 0x01, 0x05, 0x20, 0x01 }, /* J   */
  { 0x01, 0x04, 0x20, 0x01 }, /* K   */
  { 0x01, 0x04, 0x10, 0x01 }, /* L   */
  { 0x01, 0x04, 0x40, 0x01 }, /* M   */
  { 0x01, 0x05, 0x40, 0x01 }, /* N   */
  { 0x01, 0x04, 0x04, 0x01 }, /* O   */
  { 0x01, 0x03, 0x08, 0x01 }, /* P   */
  { 0x01, 0x08, 0x08, 0x01 }, /* Q   */
  { 0x01, 0x06, 0x04, 0x01 }, /* R   */
  { 0x01, 0x07, 0x10, 0x01 }, /* S   */
  { 0x01, 0x06, 0x08, 0x01 }, /* T   */
  { 0x01, 0x05, 0x04, 0x01 }, /* U   */
  { 0x01, 0x06, 0x80, 0x01 }, /* V   */
  { 0x01, 0x07, 0x08, 0x01 }, /* W   */
  { 0x01, 0x07, 0x80, 0x01 }, /* X   */
  { 0x01, 0x05, 0x08, 0x01 }, /* Y   */
  { 0x01, 0x08, 0x80, 0x01 }, /* Z   */
  { 0x01, 0x02, 0x02, 0x00 }, /* [   */
  { 0x01, 0x02, 0x40, 0x00 }, /* \   */
  { 0x01, 0x02, 0x08, 0x00 }, /* ]   */
  { 0x01, 0x03, 0x01, 0x00 }, /* ^   */
  { 0x01, 0x04, 0x01, 0x01 }, /* _   */
  { 0x01, 0x02, 0x40, 0x01 }, /* `   */
  { 0x01, 0x08, 0x20, 0x00 }, /* a   */
  { 0x01, 0x06, 0x40, 0x00 }, /* b   */
  { 0x01, 0x07, 0x40, 0x00 }, /* c   */
  { 0x01, 0x07, 0x20, 0x00 }, /* d   */
  { 0x01, 0x07, 0x04, 0x00 }, /* e   */
  { 0x01, 0x06, 0x20, 0x00 }, /* f   */
  { 0x01, 0x06, 0x10, 0x00 }, /* g   */
  { 0x01, 0x05, 0x10, 0x00 }, /* h   */
  { 0x01, 0x04, 0x08, 0x00 }, /* i   */
  { 0x01, 0x05, 0x20, 0x00 }, /* j   */
  { 0x01, 0x04, 0x20, 0x00 }, /* k   */
  { 0x01, 0x04, 0x10, 0x00 }, /* l   */
  { 0x01, 0x04, 0x40, 0x00 }, /* m   */
  { 0x01, 0x05, 0x40, 0x00 }, /* n   */
  { 0x01, 0x04, 0x04, 0x00 }, /* o   */
  { 0x01, 0x03, 0x08, 0x00 }, /* p   */
  { 0x01, 0x08, 0x08, 0x00 }, /* q   */
  { 0x01, 0x06, 0x04, 0x00 }, /* r   */
  { 0x01, 0x07, 0x10, 0x00 }, /* s   */
  { 0x01, 0x06, 0x08, 0x00 }, /* t   */
  { 0x01, 0x05, 0x04, 0x00 }, /* u   */
  { 0x01, 0x06, 0x80, 0x00 }, /* v   */
  { 0x01, 0x07, 0x08, 0x00 }, /* w   */
  { 0x01, 0x07, 0x80, 0x00 }, /* x   */
  { 0x01, 0x05, 0x08, 0x00 }, /* y   */
  { 0x01, 0x08, 0x80, 0x00 }, /* z   */
  { 0x01, 0x02, 0x02, 0x01 }, /* {   */
  { 0x01, 0x03, 0x04, 0x01 }, /* |   */
  { 0x01, 0x02, 0x08, 0x01 }, /* }   */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x01, 0x02, 0x01, 0x00 }, /* del */
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00 },
};

void amstrad_cpc_key_press(Widget widget, XtPointer data, XEvent *event)
{
char buffer[8], ascii;
KeySym keysym;
byte row, value, shift;

  XLookupString((XKeyEvent *) event, buffer, 8, &keysym, NULL);
  ascii = buffer[0];
  if(IsCursorKey(keysym)) {
    switch(keysym) {
      case XK_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsKeypadKey(keysym)) {
    switch(keysym) {
      case XK_KP_Enter:
        row = 0x00; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Decimal:
        row = 0x00; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_0:
        row = 0x01; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_1:
        row = 0x01; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_2:
        row = 0x01; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_3:
        row = 0x00; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_4:
        row = 0x02; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_5:
        row = 0x01; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_6:
        row = 0x00; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_7:
        row = 0x01; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_8:
        row = 0x01; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_9:
        row = 0x00; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsModifierKey(keysym)) {
    switch(keysym) {
      case XK_Shift_L:
        row = 0x02; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Control_L:
        row = 0x02; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Caps_Lock:
        row = 0x08; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Alt_L:
        row = 0x01; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else {
    if(!_key[ascii][0]) {
      return;
    }
    row = _key[ascii][1];
    value = _key[ascii][2];
    shift = _key[ascii][3];
  }
  amstrad_cpc.keyboard.line[row] &= ~value;
  if(shift) {
    amstrad_cpc.keyboard.line[0x02] &= ~0x20;
  }
}

void amstrad_cpc_key_release(Widget widget, XtPointer data, XEvent *event)
{
char buffer[8], ascii;
KeySym keysym;
byte row, value, shift;

  XLookupString((XKeyEvent *) event, buffer, 8, &keysym, NULL);
  ascii = buffer[0];
  if(IsCursorKey(keysym)) {
    switch(keysym) {
      case XK_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsKeypadKey(keysym)) {
    switch(keysym) {
      case XK_KP_Enter:
        row = 0x00; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Decimal:
        row = 0x00; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_0:
        row = 0x01; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_1:
        row = 0x01; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_2:
        row = 0x01; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_3:
        row = 0x00; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_4:
        row = 0x02; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_5:
        row = 0x01; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_6:
        row = 0x00; value = 0x10; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_7:
        row = 0x01; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_8:
        row = 0x01; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_9:
        row = 0x00; value = 0x08; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Up:
        row = 0x00; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Down:
        row = 0x00; value = 0x04; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Left:
        row = 0x01; value = 0x01; shift = event->xkey.state & ShiftMask;
        break;
      case XK_KP_Right:
        row = 0x00; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else if(IsModifierKey(keysym)) {
    switch(keysym) {
      case XK_Shift_L:
        row = 0x02; value = 0x20; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Control_L:
        row = 0x02; value = 0x80; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Caps_Lock:
        row = 0x08; value = 0x40; shift = event->xkey.state & ShiftMask;
        break;
      case XK_Alt_L:
        row = 0x01; value = 0x02; shift = event->xkey.state & ShiftMask;
        break;
      default:
        return;
        break;
    }
  }
  else {
    if(!_key[ascii][0]) {
      return;
    }
    row = _key[ascii][1];
    value = _key[ascii][2];
    shift = _key[ascii][3];
  }
  amstrad_cpc.keyboard.line[row] |= value;
  amstrad_cpc.keyboard.line[0x02] |= 0x20;
}

void amstrad_cpc_expose(Widget widget, XtPointer data, XEvent *event)
{
  XPutImage(event->xexpose.display, event->xexpose.window, x11.gc, x11.ximage, event->xexpose.x, event->xexpose.y, event->xexpose.x, event->xexpose.y, event->xexpose.width, event->xexpose.height);
}

byte z80_read(register word address)
{
  return(_read[address >> 14][address & 0x3FFF]);
}

void z80_write(register word address, register byte value)
{
  _write[address >> 14][address & 0x3FFF] = value;
}

byte z80_in(register word port)
{
  if(!(port & 0x8000)) { /* Gate-Array (bit 15 = 0; Port 7Fxx) */
    fprintf(stderr, "amstrad_cpc: (Gate-Array) ............. IN (%04X) (Illegal; write only)\n", port);
  }
  if(!(port & 0x4000)) { /* CRTC 6845 (bit 14 = 0; Port BCxx-BFxx) */
    switch(port & 0x0300) {
      case 0x0000: /* Register select (port BCxx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845 select) ....... IN (%04X) (Illegal; write only)\n", port);
        break;
      case 0x0100: /* Register write (port BDxx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845 write) ........ IN (%04X) (Illegal; write only)\n", port);
        break;
      case 0x0200: /* Dependent of version (port BExx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845) .............. IN (%04X) (What to do ?)\n", port);
        break;
      case 0x0300: /* Register read (port BFxx) */
        return(crtc_6845.registers[crtc_6845.current]);
        break;
    }
  }
  if(!(port & 0x2000)) { /* ROM select (bit 13 = 0; Port DFxx) */
    fprintf(stderr, "amstrad_cpc: (ROM select) ............. IN (%04X)\n", port);
  }
  if(!(port & 0x1000)) { /* Printer port (bit 12 = 0; Port EFxx) */
    fprintf(stderr, "amstrad_cpc: (Printer port) ........... IN (%04X)\n", port);
  }
  if(!(port & 0x0800)) { /* PPI 8255 (bit 11 = 0; Port F4xx-F7xx) */
    switch(port & 0x0300) {
      case 0x0000: /* Port A (port F4xx) */
        return(amstrad_cpc.keyboard.line[amstrad_cpc.keyboard.row]);
        /*return(ppi_8255.port_a);*/
        break;
      case 0x0100: /* Port B (port F5xx) */
        return(ppi_8255.port_b);
        break;
      case 0x0200: /* Port C (port F6xx) */
        return(ppi_8255.port_c);
        break;
      case 0x0300: /* Control register (port F7xx) */
        fprintf(stderr, "amstrad_cpc: (PPI 8255 CTRL) .......... IN (%04X) (Illegal; write only)\n", port);
        break;
    }
  }
  if(!(port & 0x0400)) { /* Expansion (bit 10 = 0; Port F8xx-FBxx) */
    switch(port & 0x300) {
      case 0x0000: /* Serial port (port F8xx) */
        fprintf(stderr, "amstrad_cpc: (Serial port) ............ IN (%04X)\n", port);
        break;
      case 0x0100: /* (port F9xx) */
        fprintf(stderr, "amstrad_cpc: (F9xx) ................... IN (%04X)\n", port);
        break;
      case 0x0200: /* FDC motor (port FAxx) */
        return(fdc_765_get_motor());
        break;
      case 0x0300: /* FDC register (port FBxx) */
        if(!(port & 0x0001)) { /* status (port FB7E) */
          return(fdc_765_get_status());
        }
        else { /* data  (port FB7F) */
          fprintf(stderr, "amstrad_cpc: (FDC 765 - FB7F) ......... IN (%04X)\n", port);
        }
        break;
    }
  }
  return(0x00);
}

void z80_out(register word port, register byte value)
{
  if(!(port & 0x8000)) { /* Gate-Array (bit 15 = 0; Port 7Fxx) */
    switch(value & 0xC0) { /* Select function */
      case 0x00: /* Select pen */
        amstrad_cpc.gate_array.pen = (value & 0x10 ? 0x10 : value & 0x0F);
        break;
      case 0x40: /* Select color */
        amstrad_cpc.gate_array.ink[amstrad_cpc.gate_array.pen] = value & 0x1F;
        _ink[amstrad_cpc.gate_array.pen] = _palette[amstrad_cpc.gate_array.ink[amstrad_cpc.gate_array.pen]].pixel;
        break;
      case 0x80: /* Interrupt control, ROM configuration and screen mode */
        amstrad_cpc.gate_array.generate_interrupts = value & 0x10;
        amstrad_cpc.gate_array.upper_ram_enable = value & 0x08;
        amstrad_cpc.gate_array.lower_ram_enable = value & 0x04;
        amstrad_cpc.gate_array.mode = value & 0x03;
        amstrad_cpc_set_ram_configuration();
        break;
      case 0xC0: /* RAM memory management */
        amstrad_cpc.gate_array.ram_configuration = value & 0x07;
        amstrad_cpc_set_ram_configuration();
        break;
    }
  }
  if(!(port & 0x4000)) { /* CRTC 6845 (bit 14 = 0; Port BCxx-BFxx) */
    switch(port & 0x0300) {
      case 0x0000: /* Register select (port BCxx) */
        crtc_6845.current = value;
        break;
      case 0x0100: /* Register write (port BDxx) */
        crtc_6845.registers[crtc_6845.current] = value;
        break;
      case 0x0200: /* Dependent of version (port BExx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845) ............. OUT (%04X),%02X (What to do ?)\n", port, value);
        break;
      case 0x0300: /* Register read (port BFxx) */
        fprintf(stderr, "amstrad_cpc: (CRTC 6845 read) ........ OUT (%04X),%02X\n (Illegal; read only)\n", port, value);
        break;
    }
  }
  if(!(port & 0x2000)) { /* ROM select (bit 13 = 0; Port DFxx) */
    amstrad_cpc.memory.expansion = value & 0x07;
    amstrad_cpc_set_ram_configuration();
  }
  if(!(port & 0x1000)) { /* Printer port (bit 12 = 0; Port EFxx) */
    /*printf("cpc (Printer port) ...... OUT (%04X),%02X\n", port, value);*/
  }
  if(!(port & 0x0800)) { /* PPI 8255 (bit 11 = 0; Port F4xx-F7xx) */
    switch(port & 0x0300) {
      case 0x0000: /* Port A (port F4xx) */
        ppi_8255.port_a = value;
        break;
      case 0x0100: /* Port B (port F5xx) */
        /*ppi_8255.port_b = value;*/
        break;
      case 0x0200: /* Port C (port F6xx) */
        ppi_8255.port_c = value;
        amstrad_cpc.keyboard.row = value & 0x0F;
        break;
      case 0x0300: /* Control register (port F7xx) */
        ppi_8255.control = value;
        break;
    }
  }
  if(!(port & 0x0400)) { /* Expansion (bit 10 = 0; Port F8xx-FBxx) */
    switch(port & 0x0300) {
      case 0x0000: /* Serial port (port F8xx) */
        fprintf(stderr, "amstrad_cpc: (Serial port) ........... OUT (%04X),%02X\n", port, value);
        break;
      case 0x0100: /* (port F9xx) */
        fprintf(stderr, "amstrad_cpc: (F9xx) .................. OUT (%04X),%02X\n", port, value);
        break;
      case 0x0200: /* FDC motor (port FAxx) */
        fdc_765_set_motor(value);
        break;
      case 0x0300: /* FDC register (port FBxx) */
        if(!(port & 0x0001)) { /* status (port FB7E) */
          fprintf(stderr, "amstrad_cpc: (FDC status) ............ OUT (%04X),%02X\n", port, value);
        }
        else { /* data (port FB7F) */
          fprintf(stderr, "amstrad_cpc: (FDC data) .............. OUT (%04X),%02X\n", port, value);
        }
        break;
    }
  }
}

word z80_periodic(void)
{
XtInputMask mask;

  if(amstrad_cpc.gate_array.counter++ >= 51) { /* 300Hz irq */
    amstrad_cpc.gate_array.counter = 0;
    if(_cycle++ >= 5) { /* 50Hz irq */
      _cycle = 0;
      while(mask = (!paused ? XtAppPending(appcontext) : XtIMAll)) {
        XtAppProcessEvent(appcontext, mask);
      }
      ppi_8255.port_b |= 0x01; /* set VSYNC */
      if(_ticks-- <= 0) {
        amstrad_cpc_redraw();
        pause(); /* 50Hz real-time synchronization */
      }
    }
    else {
      ppi_8255.port_b &= 0xFE; /* reset VSYNC */
    }
    return(0x0038); /* DO IRQ */
  }
  return(0xFFFF); /* DO NOTHING */
}

