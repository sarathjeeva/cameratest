#ifndef CONVERTOR_H
#define CONVERTOR_H

#include "convertor_registry.h"
/**
 * Base class for a color convertor. 
 */
class convertor {
public:
	convertor(int width) {
		width_ = width;
	}
	virtual ~convertor() {}

	virtual void convert(const unsigned char* inbuf, 
						 unsigned char* outbuf, int rows) = 0;
	int get_width() const { return width_; }

protected:
	int		width_;
};

#endif
