#include <iostream>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor (255, 255, 255, 255);
const TGAColor red = TGAColor (255, 0, 0, 255);
const TGAColor green = TGAColor (0, 255, 0, 255);
Model* model = NULL;
const int width = 1000;
const int height = 1000;

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

void draw_wireframe_triangle (Pointi p0, Pointi p1, Pointi p2, TGAImage& image, TGAColor color) {

	// draws triangles based on points
	draw_line (p0.x, p0.y, p1.x, p1.y, image, color);
	draw_line (p1.x, p1.y, p2.x, p2.y, image, color);
	draw_line (p2.x, p2.y, p0.x, p0.y, image, color);
}

void draw_filled_triangle (Pointi p0, Pointi p1, Pointi p2, TGAImage& image, TGAColor color) {

	// sorting the points
	if (p1.y < p0.y)
		std::swap (p1, p0);
	if (p2.y < p0.y)
		std::swap (p2, p0);
	if (p2.y < p1.y)
		std::swap (p2, p1);

	// compute the x coordinates of the triangle edges
	std::vector <float> x01 = interpolate (p0.y, p0.x, p1.y, p1.x);
	std::vector <float> x12 = interpolate (p1.y, p1.x, p2.y, p2.x);
	std::vector <float> x02 = interpolate (p0.y, p0.x, p2.y, p2.x);

	// remove final value to stop overcounting
	x01.pop_back ();

	// concatenate the short sides
	std::vector<float> x012;
	x01.insert (x01.end (), x12.begin (), x12.end ());
	x012 = x01;

	// determine which side is left or right
	int m = std::floor (x02.size () / 2);
	std::vector<float> x_left;
	std::vector<float> x_right;
	if (x02[m] < x012[m]) {
		x_left = x02;
		x_right = x012;
	}
	else {
		x_left = x012;
		x_right = x02;
	}

	// draw the horizontal segments
	for (int y = p0.y; y <= p2.y; ++y) {
		for (int x = x_left[static_cast<std::vector<float, std::allocator<float>>::size_type>(y) - p0.y]; x <= x_right[static_cast<std::vector<float, std::allocator<float>>::size_type>(y) - p0.y]; ++x) {
			image.set (x, y, color);
		}
	}


}

int main (int argc, char** argv)
{
	TGAImage image (width, height, TGAImage::RGB);

	Pointi p0 (-200 + 300, -250 + 300);
	Pointi p1 (200 + 300, 50 + 300);
	Pointi p2 (20 + 300, 250 + 300);

	draw_filled_triangle (p0, p1, p2, image, green); // drawing triangle
	draw_wireframe_triangle (p0, p1, p2, image, white); // coloring in triangle

	image.flip_vertically ();
	image.write_tga_file ("output_filled_triangle.tga");
	delete model;

	return 0;
}