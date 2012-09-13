#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "convertor.h"
#include "convertor_registry.h"

/**
 * Prints a failure message and exists
 *
 * @param fmt C format string followed by additional arguments
 */
void failure(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	printf("\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	convertor* con;
	con = convertor_registry::get_convertor("yuyv_rgb24", 720);
	int width = con->get_width();
	std::cout << "Width: " << width << std::endl;
	return 0;
}
