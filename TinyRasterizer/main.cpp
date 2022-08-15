#include <iostream>
#include <vector>
#include <array>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor (255, 255, 255, 255);
const TGAColor red = TGAColor (255, 0, 0, 255);
const TGAColor green = TGAColor (0, 255, 0, 255);
const TGAColor yellow = TGAColor (255, 255, 0, 255);
const TGAColor blue = TGAColor (0, 0, 255, 255);
const TGAColor purple = TGAColor (159, 43, 104, 255);
const TGAColor cyan = TGAColor (0, 255, 255, 255);

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

std::pair<int, int> convert_2D_points_to_coordinates (Pointi p) {
	return std::make_pair (p.x, p.y);
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

std::pair<float, float> viewport_to_canvas (float x, float y) {
	return std::make_pair (x * 1000 / width, y * 1000 / height);
}

Pointi project_vertex (Vec3f& v, int d) {
	float x = (v[0] * d) / v[2];
	float y = (v[1] * d) / v[2];
	std::pair<float, float> point = viewport_to_canvas (x, y);
	Pointi p (point.first, point.second);
	return p;
}

void render_triangle (Triangle& triangle, std::vector<Pointi>& projected, TGAImage& image) {
	Vec3f vertex0 = triangle.vertices[0];
	Vec3f vertex1 = triangle.vertices[1];
	Vec3f vertex2 = triangle.vertices[2];
	Pointi point0 = project_vertex (vertex0, 200);
	Pointi point1 = project_vertex (vertex1, 200);
	Pointi point2 = project_vertex (vertex2, 200);

	draw_wireframe_triangle (point0, point1, point2, image, triangle.color);
}

void render_object (std::vector<Vec3f>& vertices, std::vector<Triangle>& triangles, TGAImage& image, int d) {
	std::vector<Pointi> projected{};
	for (int i = 0; i < vertices.size (); ++i) {
		projected.push_back (project_vertex (vertices[i], d));
	}

	for (int i = 0; i < triangles.size (); ++i) {
		render_triangle (triangles[i], projected, image);
	}

}

int main (int argc, char** argv)
{
	TGAImage image (width, height, TGAImage::RGB);
	int distance = 200;

	/*Pointi p0 (-200 + 300, -250 + 300);
	Pointi p1 (200 + 300, 50 + 300);
	Pointi p2 (20 + 300, 250 + 300);*/

	//draw_filled_triangle (p0, p1, p2, image, green); // drawing triangle
	//draw_wireframe_triangle (p0, p1, p2, image, white); // coloring in triangle

	// the four "front" vertices
	//Vec3f vAf (-1 + 2, 1 + 2, 1);
	//Vec3f vBf (1 + 2, 1 + 2, 1);
	//Vec3f vCf (1 + 2, -1 + 2, 1);
	//Vec3f vDf (-1 + 2, -1 + 2, 1);

	//// the four "back" vertices
	//Vec3f vAb (-1 + 2, 1 + 2, 1.2);
	//Vec3f vBb (1 + 2, 1 + 2, 1.2);
	//Vec3f vCb (1 + 2, -1 + 2, 1.2);
	//Vec3f vDb (-1 + 2, -1 + 2, 1.2);

	//// the front face
	//Pointi point0 = project_vertex (vAf, distance);
	//Pointi point1 = project_vertex (vBf, distance);
	//Pointi point2 = project_vertex (vCf, distance);
	//Pointi point3 = project_vertex (vDf, distance);
	//std::pair<float, float> coordinate0 = convert_2D_points_to_coordinates (point0);
	//std::pair<float, float> coordinate1 = convert_2D_points_to_coordinates (point1);
	//std::pair<float, float> coordinate2 = convert_2D_points_to_coordinates (point2);
	//std::pair<float, float> coordinate3 = convert_2D_points_to_coordinates (point3);
	//draw_line (coordinate0.first, coordinate0.second, coordinate1.first, coordinate1.second, image, blue);
	//draw_line (coordinate1.first, coordinate1.second, coordinate2.first, coordinate2.second, image, blue);
	//draw_line (coordinate2.first, coordinate2.second, coordinate3.first, coordinate3.second, image, blue);
	//draw_line (coordinate3.first, coordinate3.second, coordinate0.first, coordinate0.second, image, blue);

	//// the back face
	//point0 = project_vertex (vAb, distance);
	//point1 = project_vertex (vBb, distance);
	//point2 = project_vertex (vCb, distance);
	//point3 = project_vertex (vDb, distance);
	//std::pair<float, float> coordinate4 = convert_2D_points_to_coordinates (point0);
	//std::pair<float, float> coordinate5 = convert_2D_points_to_coordinates (point1);
	//std::pair<float, float> coordinate6 = convert_2D_points_to_coordinates (point2);
	//std::pair<float, float> coordinate7 = convert_2D_points_to_coordinates (point3);
	//draw_line (coordinate4.first, coordinate4.second, coordinate5.first, coordinate5.second, image, red);
	//draw_line (coordinate5.first, coordinate5.second, coordinate6.first, coordinate6.second, image, red);
	//draw_line (coordinate6.first, coordinate6.second, coordinate7.first, coordinate7.second, image, red);
	//draw_line (coordinate7.first, coordinate7.second, coordinate4.first, coordinate4.second, image, red);

	//// the front-to-back edges
	//draw_line (coordinate0.first, coordinate0.second, coordinate4.first, coordinate4.second, image, green);
	//draw_line (coordinate1.first, coordinate1.second, coordinate5.first, coordinate5.second, image, green);
	//draw_line (coordinate2.first, coordinate2.second, coordinate6.first, coordinate6.second, image, green);
	//draw_line (coordinate3.first, coordinate3.second, coordinate7.first, coordinate7.second, image, green);

	// vertices
	Vec3f v0 (1 + 2, 1 + 2, 1 + 2);
	Vec3f v1 (-1 + 2, 1 + 2, 1 + 2);
	Vec3f v2 (-1 + 2, -1 + 2, 1 + 2);
	Vec3f v3 (1 + 2, -1 + 2, 1 + 2);
	Vec3f v4 (1 + 2, 1 + 2, -1 + 2);
	Vec3f v5 (-1 + 2, 1 + 2, -1 + 2);
	Vec3f v6 (-1 + 2, -1 + 2, -1 + 2);
	Vec3f v7 (1 + 2, -1 + 2, -1 + 2);

	std::vector<Vec3f> vertices;
	vertices.push_back (v0);
	vertices.push_back (v1);
	vertices.push_back (v2);
	vertices.push_back (v3);
	vertices.push_back (v4);
	vertices.push_back (v5);
	vertices.push_back (v6);
	vertices.push_back (v7);

	std::vector<Triangle> triangles;
	triangles.push_back (Triangle (vertices[0], vertices[1], vertices[2], red));
	triangles.push_back (Triangle (vertices[0], vertices[2], vertices[3], red));
	triangles.push_back (Triangle (vertices[4], vertices[0], vertices[3], green));
	triangles.push_back (Triangle (vertices[4], vertices[3], vertices[7], green));
	triangles.push_back (Triangle (vertices[5], vertices[4], vertices[7], blue));
	triangles.push_back (Triangle (vertices[5], vertices[7], vertices[6], blue));
	triangles.push_back (Triangle (vertices[1], vertices[5], vertices[6], yellow));
	triangles.push_back (Triangle (vertices[1], vertices[6], vertices[2], yellow));
	triangles.push_back (Triangle (vertices[4], vertices[5], vertices[1], purple));
	triangles.push_back (Triangle (vertices[4], vertices[1], vertices[0], purple));
	triangles.push_back (Triangle (vertices[2], vertices[6], vertices[7], cyan));
	triangles.push_back (Triangle (vertices[2], vertices[7], vertices[3], cyan));

	render_object (vertices, triangles, image, distance);


	image.flip_vertically ();
	image.write_tga_file ("output_perspective_projection.tga");
	delete model;

	return 0;
}