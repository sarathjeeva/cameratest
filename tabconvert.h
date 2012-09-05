#ifndef __TABCONVERT_H
#define __TABCONVERT_H

#include <linux/videodev2.h>
typedef long INT32;
typedef unsigned char uchar;

#if 0
#define RGB_RED		0
#define RGB_GREEN	1
#define RGB_BLUE	2
#define RGB_PIXELSIZE 3

#define CrYCbY_Cr 2
#define CrYCbY_Y0 1
#define CrYCbY_Cb 0
#define CrYCbY_Y1 3
#define	CrYCbY_SIZE 4 
#endif

class TabConvert {
public:
	explicit TabConvert(int width, int height);
	~TabConvert();

	void convertYCC2RGB(unsigned char *input_buf, 
			unsigned char *output_buf, int output_rows);

	bool setYUVFormat(unsigned int pixfmt);
	bool setRGBFormat(unsigned int pixfmt);
	void printTables(void);
private:

	void prepare_range_limit_table(void);
	void build_ycc_rgb_table(void);
	bool checkYUVFormat(unsigned int pixfmt);
	bool checkRGBFormat(unsigned int pixfmt);

	unsigned char *sample_range_limit;
	int *Cr_r_tab;
	int *Cb_b_tab;
	INT32 *Cr_g_tab;
	INT32 *Cb_g_tab;

	int mWidth, mHeight;
	int YCrCb_y0, YCrCb_y1, YCrCb_cr, YCrCb_cb, YCrCb_size;
	int RGB_r, RGB_g, RGB_b, RGB_size;
};
#endif
