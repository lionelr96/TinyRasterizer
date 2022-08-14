#include <iostream>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor (255, 255, 255, 255);
const TGAColor red = TGAColor (255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

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
	if (argc == 2) {
		model = new Model (argv[1]);
	}
	else {
		model = new Model ("obj/african_head.obj");
	}

	TGAImage image (width, height, TGAImage::RGB);

	for (int i = 0; i < model->nfaces (); ++i) {
		std::vector<int> face = model->face (i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert (face[j]);
			Vec3f v1 = model->vert (face[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			draw_line (x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically ();
	image.write_tga_file ("output_wireframe.tga");
	delete model;

	return 0;
}