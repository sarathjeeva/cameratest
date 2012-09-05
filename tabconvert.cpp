#include "tabconvert.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#define CENTERSAMPLE 128
#define	MAXSAMPLE 255

#define RIGHT_SHIFT(x, shft)	((x) >> (shft))
#define SCALEBITS	16
#define ONE_HALF	((INT32) 1 << (SCALEBITS-1))
#define FIX(x)		((INT32) ((x) * (1L<<SCALEBITS) + 0.5))


TabConvert::TabConvert(int width, int height)
	: mWidth(width), mHeight(height)
{
	prepare_range_limit_table();
	build_ycc_rgb_table();
}

TabConvert::~TabConvert()
{
	unsigned char *table = sample_range_limit - (MAXSAMPLE+1);
	delete table;
	delete Cr_r_tab;
	delete Cb_b_tab;
	delete Cr_g_tab;
	delete Cb_g_tab;

}

bool TabConvert::setRGBFormat(unsigned int pixfmt)
{
	if (!checkRGBFormat(pixfmt)) {
		std::cout << "Pixel format doesn't support!" << std::endl;
		std::cout << "NOTE: fall back to RGB" << std::endl;
		return false;
	}
	switch (pixfmt) {
	case V4L2_PIX_FMT_BGR24:  
		RGB_b = 0;
		RGB_g = 1;
		RGB_r = 2;
		RGB_size = 3;
		break;
	case V4L2_PIX_FMT_RGB24:
		RGB_r = 0;
		RGB_g = 1;
		RGB_b = 2;
		RGB_size = 3;
		break;
	}
	return true;
}

bool TabConvert::setYUVFormat(unsigned int pixfmt)
{
	if (!checkYUVFormat(pixfmt)) {
		std::cout << "Pixel format doesn't support!" << std::endl;
		std::cout << "NOTE: fall back to UYVY" << std::endl;
		pixfmt = V4L2_PIX_FMT_UYVY;
		return false;
	}
	switch (pixfmt) {
	case V4L2_PIX_FMT_YUYV:
		YCrCb_cr = 3;
		YCrCb_cb = 1;
		YCrCb_y0 = 0;
		YCrCb_y1 = 2;
		YCrCb_size = 4;
		break;
	case V4L2_PIX_FMT_YYUV:
		YCrCb_cr = 3;
		YCrCb_cb = 2;
		YCrCb_y0 = 0;
		YCrCb_y1 = 1;
		YCrCb_size = 4;
		break;
	case V4L2_PIX_FMT_YVYU:
		YCrCb_cr = 1;
		YCrCb_cb = 3;
		YCrCb_y0 = 0;
		YCrCb_y1 = 2;
		YCrCb_size = 4;
		break;
	case V4L2_PIX_FMT_UYVY:
		YCrCb_cr = 2;
		YCrCb_cb = 0;
		YCrCb_y0 = 1;
		YCrCb_y1 = 3;
		YCrCb_size = 4;
		break;
	case V4L2_PIX_FMT_VYUY:
		YCrCb_cr = 0;
		YCrCb_cb = 2;
		YCrCb_y0 = 1;
		YCrCb_y1 = 3;
		YCrCb_size = 4;
		break;
	}
	return true;
}
bool TabConvert::checkRGBFormat(unsigned int pixfmt)
{
	if (pixfmt == V4L2_PIX_FMT_BGR24 ||
		pixfmt == V4L2_PIX_FMT_RGB24) {
		return true;
	}
	return false;
}
bool TabConvert::checkYUVFormat(unsigned int pixfmt)
{
	if (pixfmt == V4L2_PIX_FMT_UYVY ||
		pixfmt == V4L2_PIX_FMT_YYUV ||
		pixfmt == V4L2_PIX_FMT_VYUY ||
		pixfmt == V4L2_PIX_FMT_YVYU ||
		pixfmt == V4L2_PIX_FMT_YUYV)
		return true;
	return false;
}

void TabConvert::build_ycc_rgb_table(void)
{
	int x, i;

	Cr_r_tab = new int[MAXSAMPLE+1];
	Cb_b_tab = new int[MAXSAMPLE+1];
	Cr_g_tab = new INT32[MAXSAMPLE+1];
	Cb_g_tab = new INT32[MAXSAMPLE+1];

	for (i = 0, x = -CENTERSAMPLE; i <= MAXSAMPLE; i++, x++) {
		Cr_r_tab[i] = (int)
			RIGHT_SHIFT(FIX(1.40200) * x + ONE_HALF, SCALEBITS);
		Cb_b_tab[i] = (int)
			RIGHT_SHIFT(FIX(1.77200) * x + ONE_HALF, SCALEBITS);
		Cr_g_tab[i] = (- FIX(0.71414)) * x;
		Cb_g_tab[i] = (- FIX(0.34414)) * x + ONE_HALF;
	}
}
void TabConvert::prepare_range_limit_table()
{
	unsigned char *table;
	int i;

	table = new uchar[5 * (MAXSAMPLE+1) + CENTERSAMPLE];
	table += (MAXSAMPLE+1);
	this->sample_range_limit = table;
	memset(table - (MAXSAMPLE+1), 0, (MAXSAMPLE+1));
	for (i = 0; i <= MAXSAMPLE; i++)
		table[i] = (unsigned char)i;
	table += CENTERSAMPLE;
	/* End of samplte table, rest of first half of post-IDCT table */
	for (i = CENTERSAMPLE; i < 2*(MAXSAMPLE+1); i++)
		table[i] = MAXSAMPLE;
	memset(table + (2*(MAXSAMPLE+1)), 0, (2*(MAXSAMPLE+1)-CENTERSAMPLE));
	memcpy(table + (4*(MAXSAMPLE+1) - CENTERSAMPLE), 
			this->sample_range_limit, CENTERSAMPLE);
}

void TabConvert::convertYCC2RGB(unsigned char *input_buf,
		unsigned char *output_buf, int num_rows)
{
	register int y, cb, cr;
	register unsigned char *outptr = output_buf;

	register unsigned char *range_limit = this->sample_range_limit;
	register int *Crrtab = this->Cr_r_tab;
	register int *Cbbtab = this->Cb_b_tab;
	register INT32 *Crgtab = this->Cr_g_tab;
	register INT32 *Cbgtab = this->Cb_g_tab;

	int row, col;

	for (row = 0; row < num_rows; row++) {
		for (col = 0; col < (this->mWidth/2); col++) {
			y  = input_buf[YCrCb_y0];
			cb = input_buf[YCrCb_cb];
			cr = input_buf[YCrCb_cr];
			outptr[RGB_r] = range_limit[y + Crrtab[cr]];
			outptr[RGB_g] = range_limit[y + 
				((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], 
					SCALEBITS))];
			outptr[RGB_b] = range_limit[y + Cbbtab[cb]];
			outptr += RGB_size;

			y  = input_buf[YCrCb_y1];
			outptr[RGB_r] = range_limit[y + Crrtab[cr]];
			outptr[RGB_g] = range_limit[y + 
				((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], 
					SCALEBITS))];
			outptr[RGB_b] = range_limit[y + Cbbtab[cb]];

			input_buf += YCrCb_size;
			outptr += RGB_size;
		}
	}
}

void TabConvert::printTables(void)
{
	int i;

	printf("int Cr_r_tab = { \n\t");
	for (i = 0; i <= MAXSAMPLE; i++) {
		printf("%d, ", Cr_r_tab[i]);
		if (((i+1) % 16) == 0)
			printf("\n\t");
	}
	printf("%d};\n", Cr_r_tab[i]);

	printf("int Cb_b_tab = { \n\t");
	for (i = 0; i <= MAXSAMPLE; i++) {
		printf("%d, ", Cb_b_tab[i]);
		if (((i+1) % 16) == 0)
			printf("\n\t");
	}
	printf("%d};\n", Cb_b_tab[i]);

	printf("INT32 Cr_g_tab = { \n\t");
	for (i = 0; i <= MAXSAMPLE; i++) {
		printf("%ld, ", Cr_g_tab[i]);
		if (((i+1) % 16) == 0)
			printf("\n\t");
	}
	printf("%ld};\n", Cr_g_tab[i]);

	printf("INT32 Cb_g_tab = { \n\t");
	for (i = 0; i <= MAXSAMPLE; i++) {
		printf("%ld, ", Cb_g_tab[i]);
		if (((i+1) % 16) == 0)
			printf("\n\t");
	}
	printf("%ld};\n", Cb_g_tab[i]);

	unsigned char *table = sample_range_limit - (MAXSAMPLE+1);
	int count = 5 * (MAXSAMPLE+1) + CENTERSAMPLE;
	printf("unsigned char sample_range_limit[] = { \n\t");
	for (i = 0; i < count; i++) {
		printf("0x%x, ", table[i]);
		if (((i+1) % 16) == 0)
			printf("\n\t");
	}
	printf("0x%x};\n", table[i]);
}

