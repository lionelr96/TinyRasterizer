#include <iostream>
#include <vector>
#include "tgaimage.h"

const TGAColor white = TGAColor (255, 255, 255, 255);
const TGAColor red = TGAColor (255, 0, 0, 255);

std::vector<float> interpolate (int i0, int d0, int i1, int d1) {
	std::vector<float> values{};
	if (i0 == i1) {
		values.push_back (d0);
		return values;
	}

	float a = (static_cast<float>(d1) - d0) / (i1 - i0);
	float d = d0;

	for (int i = i0; i <= i1; ++i) {
		values.push_back (d);
		d += a;
	}

	return values;
}

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

		std::vector<float> ys = interpolate (x0, y0, x1, y1);

		for (int x = x0; x <= x1; ++x) {
			image.set (x, ys[static_cast<std::vector<float, std::allocator<float>>::size_type>(x) - x0], color);
		}
	}
	else {
		// line is somewhat vertical
		// make sure y0 < y1
		if (y0 > y1) {
			std::swap (y0, y1);
			std::swap (x0, x1);
		}

		std::vector<float> xs = interpolate (y0, x0, y1, x1);

		for (int y = y0; y <= y1; ++y) {
			image.set (xs[static_cast<std::vector<float, std::allocator<float>>::size_type>(y) - y0], y, color);
		}
	}

}

int main (int argc, char** argv)
{
	TGAImage image (500, 500, TGAImage::RGB);

	draw_line (-50, -200, 60, 240, image, white);
	draw_line (-200, -100, 240, 120, image, white);
	image.flip_vertically ();

	image.write_tga_file ("output.tga");
	return 0;
}