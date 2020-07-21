#define SUPPORT_8352B
#define OFFSET_9327 32             //costs about 103 bytes, 0.08s

#include "TFTScreen.h"
#include "TFTScreen_shield.h"

#define wait_ms(ms)  delay(ms)
#define MIPI_DCS_REV1   (1<<0)
#define AUTO_READINC    (1<<1)
#define READ_BGR        (1<<2)
#define READ_LOWHIGH    (1<<3)
#define READ_24BITS     (1<<4)
#define XSA_XEA_16BIT   (1<<5)
#define READ_NODUMMY    (1<<6)
#define INVERT_GS       (1<<8)
#define INVERT_SS       (1<<9)
#define MV_AXIS         (1<<10)
#define INVERT_RGB      (1<<11)
#define REV_SCREEN      (1<<12)
#define FLIP_VERT       (1<<13)
#define FLIP_HORIZ      (1<<14)

#define USING_16BIT_BUS 0

TFTScreen::TFTScreen(int CS, int RS, int WR, int RD, int RST) : Adafruit_GFX(240, 400)
{
	// we can not access GPIO pins until AHB has been enabled.
}

static uint8_t done_reset, is8347;

void TFTScreen::reset(void)
{
	done_reset = 1;
	setWriteDir();
	CTL_INIT();
	CS_IDLE;
	RD_IDLE;
	WR_IDLE;
	digitalWrite(5, LOW);
	delay(200);
	digitalWrite(5, HIGH);
}

void TFTScreen::WriteCmdData(uint16_t cmd, uint16_t dat)
{
	CS_ACTIVE;
	WriteCmd(cmd);
	WriteData(dat);
	CS_IDLE;
}

static void WriteCmdParamN(uint16_t cmd, int8_t N, uint8_t * block)
{
	CS_ACTIVE;
	WriteCmd(cmd);

	while (N-- > 0)
	{
		uint8_t u8 = *block++;
		CD_DATA;
		write8(u8);
	}

	CS_IDLE;
}

static inline void WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
	uint8_t d[4];
	d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;
	WriteCmdParamN(cmd, 4, d);
}

//#define WriteCmdParam4(cmd, d1, d2, d3, d4) {uint8_t d[4];d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;WriteCmdParamN(cmd, 4, d);}
void TFTScreen::pushCommand(uint16_t cmd, uint8_t * block, int8_t N)
{
	WriteCmdParamN(cmd, N, block);
}

static uint16_t read16bits(void)
{
	uint16_t ret;
	uint8_t lo;
	READ_8(ret);
	READ_8(lo);
	return (ret << 8) | lo;
}

uint32_t readReg40(uint16_t reg)
{
	uint16_t h, m, l;
	CS_ACTIVE;
	WriteCmd(reg);
	setReadDir();
	CD_DATA;
	h = read16bits();
	m = read16bits();
	l = read16bits();
	RD_IDLE;
	CS_IDLE;
	setWriteDir();
	return ((uint32_t)h << 24) | (m << 8) | (l >> 8);
}

uint16_t TFTScreen::readReg(uint16_t reg)
{
	uint16_t ret;
	uint8_t lo;

	if (!done_reset)
		reset();

	CS_ACTIVE;
	WriteCmd(reg);
	setReadDir();
	CD_DATA;
	ret = read16bits();
	RD_IDLE;
	CS_IDLE;
	setWriteDir();
	return ret;
}

uint32_t TFTScreen::readReg32(uint16_t reg)
{
	uint16_t h, l;
	CS_ACTIVE;
	WriteCmd(reg);
	setReadDir();
	CD_DATA;
	h = read16bits();
	l = read16bits();
	RD_IDLE;
	CS_IDLE;
	setWriteDir();
	return ((uint32_t)h << 16) | (l);
}

uint16_t TFTScreen::readID(void)
{
	return 0x9327;
}

// independent cursor and window registers.   S6D0154, ST7781 increments.  ILI92320/5 do not.  
int16_t TFTScreen::readGRAM(int16_t x, int16_t y, uint16_t * block, int16_t w, int16_t h)
{
	uint16_t ret, dummy, _MR = _MW;
	int16_t n = w * h, row = 0, col = 0;
	uint8_t r, g, b, tmp;

	if (_lcd_capable & MIPI_DCS_REV1)
		_MR = 0x2E;

	setAddrWindow(x, y, x + w - 1, y + h - 1);

	while (n > 0)
	{
		if (!(_lcd_capable & MIPI_DCS_REV1))
		{
			WriteCmdData(_MC, x + col);
			WriteCmdData(_MP, y + row);
		}

		CS_ACTIVE;
		WriteCmd(_MR);
		setReadDir();
		CD_DATA;

		if (_lcd_capable & READ_NODUMMY)
		{
			;
		}
		else if ((_lcd_capable & MIPI_DCS_REV1) || _lcd_ID == 0x1289)
		{
			READ_8(r);
		}
		else
		{
			READ_16(dummy);
		}
		if (_lcd_ID == 0x1511)
			READ_8(r);   //extra dummy for R61511

		while (n)
		{
			if (_lcd_capable & READ_24BITS)
			{
				READ_8(r);
				READ_8(g);
				READ_8(b);

				if (_lcd_capable & READ_BGR)
					ret = color565(b, g, r);
				else
					ret = color565(r, g, b);
			}
			else
			{
				READ_16(ret);
				if (_lcd_capable & READ_LOWHIGH)
					ret = (ret >> 8) | (ret << 8);

				if (_lcd_capable & READ_BGR)
					ret = (ret & 0x07E0) | (ret >> 11) | (ret << 11);
			}

			*block++ = ret;
			n--;

			if (!(_lcd_capable & AUTO_READINC))
				break;
		}

		if (++col >= w)
		{
			col = 0;
			if (++row >= h)
				row = 0;
		}

		RD_IDLE;
		CS_IDLE;
		setWriteDir();
	}

	if (!(_lcd_capable & MIPI_DCS_REV1))
		setAddrWindow(0, 0, width() - 1, height() - 1);

	return 0;
}

void TFTScreen::setRotation(uint8_t r)
{
	uint16_t GS, SS, ORG, REV = _lcd_rev;
	uint8_t val, d[3];
	rotation = r & 3;           // just perform the operation ourselves on the protected variables
	_width = (rotation & 1) ? HEIGHT : WIDTH;
	_height = (rotation & 1) ? WIDTH : HEIGHT;

	switch (rotation)
	{
	case 0:                    //PORTRAIT:
		val = 0x48;             //MY=0, MX=1, MV=0, ML=0, BGR=1
		break;
	case 1:                    //LANDSCAPE: 90 degrees
		val = 0x28;             //MY=0, MX=0, MV=1, ML=0, BGR=1
		break;
	case 2:                    //PORTRAIT_REV: 180 degrees
		val = 0x88;             //MY=1, MX=0, MV=0, ML=1, BGR=1
		break;
	case 3:                    //LANDSCAPE_REV: 270 degrees
		val = 0xF8;             //MY=1, MX=1, MV=1, ML=1, BGR=1
		break;
	}

	if (_lcd_capable & INVERT_GS)
		val ^= 0x80;

	if (_lcd_capable & INVERT_SS)
		val ^= 0x40;

	if (_lcd_capable & INVERT_RGB)
		val ^= 0x08;

	if (_lcd_capable & MIPI_DCS_REV1)
	{
	common_MC:
		_MC = 0x2A, _MP = 0x2B, _MW = 0x2C, _SC = 0x2A, _EC = 0x2A, _SP = 0x2B, _EP = 0x2B;
	common_BGR:
		WriteCmdParamN(is8347 ? 0x16 : 0x36, 1, &val);
		_lcd_madctl = val;
	}

	if ((rotation & 1) && ((_lcd_capable & MV_AXIS) == 0))
	{
		uint16_t x;
		x = _MC, _MC = _MP, _MP = x;
		x = _SC, _SC = _SP, _SP = x;    //.kbv check 0139
		x = _EC, _EC = _EP, _EP = x;    //.kbv check 0139
	}

	setAddrWindow(0, 0, width() - 1, height() - 1);
	vertScroll(0, HEIGHT, 0);   //reset scrolling after a rotation
}

void TFTScreen::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	// MCUFRIEND just plots at edge if you try to write outside of the box:
	if (x < 0 || y < 0 || x >= width() || y >= height())
		return;

	if (rotation == 2)
		y += 32;

	if (rotation == 3)
		x += 32;

	//if (_lcd_capable & MIPI_DCS_REV1) 
	//{
	WriteCmdParam4(_MC, x >> 8, x, x >> 8, x);
	WriteCmdParam4(_MP, y >> 8, y, y >> 8, y);
	//}
	//else 
	//{
 //       WriteCmdData(_MC, x);
 //       WriteCmdData(_MP, y);
 //   }
	WriteCmdData(_MW, ~color);
}

void TFTScreen::setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1)
{
	if (_lcd_ID == 0x9327)
	{
		if (rotation == 2) y += 32, y1 += 32;
		if (rotation == 3) x += 32, x1 += 32;
	}

	if (_lcd_capable & MIPI_DCS_REV1)
	{
		WriteCmdParam4(_MC, x >> 8, x, x1 >> 8, x1);
		WriteCmdParam4(_MP, y >> 8, y, y1 >> 8, y1);
	}
	else if (_lcd_ID == 0x65)//HX8352B
	{
		if (!flag_write_bmp) {
			int x0, y0, t;
			switch (rotation) {
			default:
				x0 = x;
				y0 = y;
				break;
			case 1:
				t = y;
				y = x;
				x = WIDTH - 1 - y1;
				y1 = x1;
				x1 = WIDTH - 1 - t;
				x0 = x1;
				y0 = y;
				break;
			case 2:
				t = x;
				x = WIDTH - 1 - x1;
				x1 = WIDTH - 1 - t;
				t = y;
				y = HEIGHT - 1 - y1;
				y1 = HEIGHT - 1 - t;
				x0 = x1;
				y0 = y1;
				break;
			case 3:
				t = x;
				x = y;
				y = HEIGHT - 1 - x1;
				x1 = y1;
				y1 = HEIGHT - 1 - t;
				x0 = x;
				y0 = y1;
				break;
			}
		}
		WriteCmdData(0x02, x >> 8);
		WriteCmdData(0x03, x);
		WriteCmdData(0x04, x1 >> 8);
		WriteCmdData(0x05, x1);
		WriteCmdData(0x06, y >> 8);
		WriteCmdData(0x07, y);
		WriteCmdData(0x08, y1 >> 8);
		WriteCmdData(0x09, y1);
		WriteCmdData(0x80, x >> 8);
		WriteCmdData(0x81, x);
		WriteCmdData(0x82, y >> 8);
		WriteCmdData(0x83, y);
		/* WriteCmdData(0x80,x0>>8);
		 WriteCmdData(0x81,x0);
		 WriteCmdData(0x82,y0>>8);
		 WriteCmdData(0x83,y0);*/

	}
	else {
		WriteCmdData(_MC, x);
		WriteCmdData(_MP, y);
		if (_lcd_capable & XSA_XEA_16BIT) {
			if (rotation & 1)
				y1 = y = (y1 << 8) | y;
			else
				x1 = x = (x1 << 8) | x;
		}
		WriteCmdData(_SC, x);
		WriteCmdData(_SP, y);
		WriteCmdData(_EC, x1);
		WriteCmdData(_EP, y1);
	}
}

void TFTScreen::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	int16_t end;

	if (w < 0)
	{
		w = -w;
		x -= w;
	}                           //+ve w

	end = x + w;

	if (x < 0)
		x = 0;

	if (end > width())
		end = width();

	w = end - x;

	if (h < 0)
	{
		h = -h;
		y -= h;
	}                           //+ve h

	end = y + h;

	if (y < 0)
		y = 0;

	if (end > height())
		end = height();

	h = end - y;

	setAddrWindow(x, y, x + w - 1, y + h - 1);

	CS_ACTIVE;
	WriteCmd(_MW);

	if (h > w)
	{
		end = h;
		h = w;
		w = end;
	}

	uint8_t hi = ~color >> 8;
	uint8_t lo = ~color & 0xFF;
	CD_DATA;

	while (h-- > 0)
	{
		end = w;

		do
		{
			write8(hi);
			write8(lo);
		} while (--end != 0);
	}

	CS_IDLE;

	if (!(_lcd_capable & MIPI_DCS_REV1))
		setAddrWindow(0, 0, width() - 1, height() - 1);
}

void TFTScreen::pushColors(uint16_t * block, int16_t n, bool first)
{
	uint16_t color;
	//delay();//
	CS_ACTIVE;
	if (first)
	{
		WriteCmd(_MW);
	}

	CD_DATA;

	while (n-- > 0)
	{
		color = *block++;
		write16(~color);
	}
	CS_IDLE;
}

void TFTScreen::pushColors(uint8_t * block, int16_t n, bool first)
{
	uint16_t color;
	uint8_t h, l;

	CS_ACTIVE;

	if (first)
		WriteCmd(_MW);

	CD_DATA;

	while (n-- > 0)
	{
		h = (*block++);
		l = (*block++);
		color = h << 8 | l;
		write16(~color);
	}
	CS_IDLE;
}

void TFTScreen::pushColors(const uint8_t * block, int16_t n, bool first)
{
	uint16_t color;
	uint8_t h, l;
	CS_ACTIVE;

	if (first)
		WriteCmd(_MW);

	CD_DATA;

	while (n-- > 0)
	{
		l = pgm_read_byte(block++);
		h = pgm_read_byte(block++);
		color = h << 8 | l;
		write16(~color);
	}

	CS_IDLE;
}

void TFTScreen::vertScroll(int16_t top, int16_t scrollines, int16_t offset)
{
#if defined(OFFSET_9327)
	if (_lcd_ID == 0x9327) {
		if (rotation == 2 || rotation == 3) top += OFFSET_9327;
	}
#endif
	int16_t bfa = HEIGHT - top - scrollines;  // bottom fixed area
	int16_t vsp;
	int16_t sea = top;
	if (_lcd_ID == 0x9327) bfa += 32;
	if (offset <= -scrollines || offset >= scrollines) offset = 0; //valid scroll
	vsp = top + offset; // vertical start position
	if (offset < 0)
		vsp += scrollines;          //keep in unsigned range
	sea = top + scrollines - 1;
	if (_lcd_capable & MIPI_DCS_REV1) {
		uint8_t d[6];           // for multi-byte parameters
/*
		if (_lcd_ID == 0x9327) {        //panel is wired for 240x432
			if (rotation == 2 || rotation == 3) { //180 or 270 degrees
				if (scrollines == HEIGHT) {
					scrollines = 432;   // we get a glitch but hey-ho
					vsp -= 432 - HEIGHT;
				}
				if (vsp < 0)
					vsp += 432;
			}
			bfa = 432 - top - scrollines;
		}
*/
		d[0] = top >> 8;        //TFA
		d[1] = top;
		d[2] = scrollines >> 8; //VSA
		d[3] = scrollines;
		d[4] = bfa >> 8;        //BFA
		d[5] = bfa;
		WriteCmdParamN(is8347 ? 0x0E : 0x33, 6, d);
		//        if (offset == 0 && rotation > 1) vsp = top + scrollines;   //make non-valid
		d[0] = vsp >> 8;        //VSP
		d[1] = vsp;
		WriteCmdParamN(is8347 ? 0x14 : 0x37, 2, d);
		if (is8347) {
			d[0] = (offset != 0) ? (_lcd_ID == 0x8347 ? 0x02 : 0x08) : 0;
			WriteCmdParamN(_lcd_ID == 0x8347 ? 0x18 : 0x01, 1, d);  //HX8347-D
		}
		else if (offset == 0 && (_lcd_capable & MIPI_DCS_REV1)) {
			WriteCmdParamN(0x13, 0, NULL);    //NORMAL i.e. disable scroll
		}
		return;
	}
	// cope with 9320 style variants:
	switch (_lcd_ID) {
	case 0x7783:
		WriteCmdData(0x61, _lcd_rev);   //!NDL, !VLE, REV
		WriteCmdData(0x6A, vsp);        //VL#
		break;
#ifdef SUPPORT_0139
	case 0x0139:
		WriteCmdData(0x41, sea);        //SEA
		WriteCmdData(0x42, top);        //SSA
		WriteCmdData(0x43, vsp - top);  //SST
		break;
#endif
	case 0x0154:
		WriteCmdData(0x31, sea);        //SEA
		WriteCmdData(0x32, top);        //SSA
		WriteCmdData(0x33, vsp - top);  //SST
		break;
#ifdef SUPPORT_1289
	case 0x1289:
		WriteCmdData(0x41, vsp);        //VL#
		break;
#endif
	case 0x7793:
	case 0x9326:
	case 0xB509:
		WriteCmdData(0x401, (1 << 1) | _lcd_rev);       //VLE, REV 
		WriteCmdData(0x404, vsp);       //VL# 
		break;
	default:
		// 0x6809, 0x9320, 0x9325, 0x9335, 0xB505 can only scroll whole screen
		WriteCmdData(0x61, (1 << 1) | _lcd_rev);        //!NDL, VLE, REV
		WriteCmdData(0x6A, vsp);        //VL#
		break;
	}
}

void TFTScreen::invertDisplay(boolean i)
{
	_lcd_rev = ((_lcd_capable & REV_SCREEN) != 0) ^ i;
	WriteCmdParamN(_lcd_rev ? 0x21 : 0x20, 0, NULL);
}

#define TFTLCD_DELAY 0xFFFF
#define TFTLCD_DELAY8 0xFF

static void init_table(const void *table, int16_t size)
{
	uint8_t *p = (uint8_t *)table, dat[16];
	while (size > 0)
	{
		uint8_t cmd = pgm_read_byte(p++);
		uint8_t len = pgm_read_byte(p++);

		if (cmd == TFTLCD_DELAY8)
		{
			delay(len);
			len = 0;
		}
		else
		{
			for (uint8_t i = 0; i < len; i++)
				dat[i] = pgm_read_byte(p++);

			WriteCmdParamN(cmd, len, dat);
		}
		size -= len + 2;
	}
	}

void TFTScreen::begin()
{
	int16_t *p16;               //so we can "write" to a const protected variable.
	reset();
	_lcd_xor = 0;
	int ID = 0x9327;

	switch (_lcd_ID = ID)
	{
	case 0x9327:
		_lcd_capable = AUTO_READINC | MIPI_DCS_REV1 | MV_AXIS;
		static const uint8_t ILI9327_regValues[] PROGMEM =
		{
			0x01, 0,            //Soft Reset
			0x28, 0,            //Display Off
			//            0xE0, 1, 0x20,      //NV Memory Write [00]
			0x11, 0,            //Sleep Out
			TFTLCD_DELAY8, 100,
			0xB0, 1, 0x00,      //Disable Protect for cmds B1-DF, E0-EF, F0-FF 
			//            0xD1, 3, 0x00, 0x71, 0x19,  //VCOM control [00 40 0F]
			//            0xD0, 3, 0x07, 0x01, 0x08,  //Power Setting [07 04 8C]
			0xC1, 4, 0x10, 0x10, 0x02, 0x02,    //Display Timing [10 10 02 02]
			0xC0, 6, 0x00, 0x35, 0x00, 0x00, 0x01, 0x02,        //Panel Drive [00 35 00 00 01 02 REV=0,GS=0,SS=0
			0xC5, 1, 0x04,      //Frame Rate [04]
			0xD2, 2, 0x01, 0x04,        //Power Setting [01 44]
			//            0xC8, 15, 0x04, 0x67, 0x35, 0x04, 0x08, 0x06, 0x24, 0x01, 0x37, 0x40, 0x03, 0x10, 0x08, 0x80, 0x00,
			//            0xC8, 15, 0x00, 0x77, 0x77, 0x04, 0x04, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
			0xCA, 1, 0x00,      //DGC LUT ???
			0xEA, 1, 0x80,      //3-Gamma Function Enable
			//                     0xB0, 1, 0x03,      //Enable Protect
			0x36, 1, 0x48,      // Memory Access
			0x3A, 1, 0x55,      //Pixel read=565, write=565
			0x2A, 4, 0x00, 0x00, 0x00, 0xEF,    // wid: 0, 239
			0x2B, 4, 0x00, 0x00, 0x01, 0x8F,    // ht: 0, 399
			0x30, 4, 0x00, 0x00, 0x01, 0x8F,    // Partial Area: 0, 399
			0x29, 0,            //Display On
		};

		init_table(ILI9327_regValues, sizeof(ILI9327_regValues));
		p16 = (int16_t *)& HEIGHT;
		*p16 = 400;
		p16 = (int16_t *)& WIDTH;
		*p16 = 240;
		break;
	}

	_lcd_rev = ((_lcd_capable & REV_SCREEN) != 0);
	setRotation(0);             //PORTRAIT
	//invertDisplay(true);
}
