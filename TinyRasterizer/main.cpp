#include <iostream>
#include "tgaimage.h"

const TGAColor white = TGAColor (255, 255, 255, 255);
const TGAColor red = TGAColor (255, 0, 0, 255);

void draw_line (int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	float dx = x1 - x0;
	float dy = y1 - y0;

	if (std::abs (dx) > std::abs (dy)) {
		// line is somewhat horizontal
		// make sure x0 < x1
		if (x0 > x1) {
			std::swap (x0, x1);
			std::swap (y0, y1);
		}

		float a = dy / dx;
		float y = y0;

		for (int x = x0; x <= x1; ++x) {
			image.set (x, y, color);
			y += a;
		}
	}
	else {
		// line is somewhat vertical
		// make sure y0 < y1
		if (y0 > y1) {
			std::swap (y0, y1);
			std::swap (x0, x1);
		}

		float a = dx / dy;
		float x = x0;

		for (int y = y0; y <= y1; ++y) {
			image.set (x, y, color);
			x += a;
		}
	}

}

int main (int argc, char** argv)
{
	TGAImage image (500, 500, TGAImage::RGB);

	draw_line (-50, -200, 60, 240, image, white);
	image.flip_vertically ();

	image.write_tga_file ("output.tga");
	return 0;
}