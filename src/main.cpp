// \ --> for cpy paste bc my computer backslash key is broken thanks Lenovo
// |

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

int constexpr HEIGHT = 500;
int constexpr WIDTH = 500;

/* Point struct to represent <x, y> point on pixel screen 
 */
struct Point {
    int x;
    int y;
};

/* Pixel struct to represent rgb pixel value in ppm image
 */
struct Pixel {
    double r;
    double g;
    double b;
};

using Color = Pixel;

void place_point(std::vector<Pixel>& pixel_arr, const int x, const int y, const Color& c) {
    assert(x < WIDTH && y < HEIGHT);
    Pixel& p = pixel_arr[(y * WIDTH) + x];
    p.r = c.r;
    p.g = c.g;
    p.b = c.b;
}

void draw_line(std::vector<Pixel>& pixel_arr, const Point& a, const Point& b, const Color& c) {
    int x = 0;
    int y = 0;
    for (auto t = 0.0; t < 1.0; t += 0.01) {
        x = a.x + t * (b.x - a.x);
        y = a.y + t * (b.y - a.y);
        Pixel& p = pixel_arr[(y * WIDTH) + x];
        p.r = c.r;
        p.g = c.g;
        p.b = c.b;
    }
}


/*
 * Barycentric coordinate line drawing function which uses increments along axes, interpolating 
 * the respective 'other'axis value along the way with a calculated barycentric value
 *
 */
void draw_line_v2(std::vector<Pixel>& pixel_arr, const Point& a, const Point& b, const Color& c) {
    double t = 0;
    int a_x = a.x;
    int a_y = a.y;
    int b_x = b.x;
    int b_y = b.y;

    if (a.x > b.x) {
        int temp_x = a_x;
        int temp_y = a_y;
        a_x = b_x;
        a_y = b_y;
        b_x = temp_x;
        b_y = temp_y;
    }

    if (std::abs(b_x - a_x) >= std::abs(b_y - a_y)) {
        int y = 0;
        for (auto x = a_x; x < b_x; x += 1) {
            t = (x - a_x) / static_cast<double>((b_x - a_x));
            y = std::ceil(a_y + t * (b_y - a_y));
            Pixel& p = pixel_arr[(y * WIDTH) + x];
            p.r = c.r;
            p.g = c.g;
            p.b = c.b;
        }
    }else {
        int x = 0;
        for (auto y = a_y; y > b_y; y -= 1) {
            t = (y - a_y) / static_cast<double>((b_y - a_y));
            x = std::ceil(a_x + t * (b_x - a_x));
            Pixel& p = pixel_arr[(y * WIDTH) + x];
            p.r = c.r;
            p.g = c.g;
            p.b = c.b;
        }
    }
}

int main() {
    
    std::cout << "P3\n" << WIDTH << " " << HEIGHT << "\n" << "255\n";
    
    //Default black screen init
    Pixel default_p = {0.0, 0.0, 0.0};
    std::vector<Pixel> pixel_arr(HEIGHT * WIDTH, default_p);

//    place_point(pixel_arr, 100, 100, {255, 255, 255});

    draw_line_v2(pixel_arr, {100, 100}, {200, 200}, {255, 0, 0});
    draw_line_v2(pixel_arr, {100, 100}, {200, 200}, {0, 0, 255});
    draw_line_v2(pixel_arr, {100, 100}, {150, 5}, {0, 255, 0});
    draw_line_v2(pixel_arr, {100, 100}, {150, 5}, {0, 255, 255});
    
    //TODO: Find way to output without using buffer
    for (int i = 0; i < HEIGHT * WIDTH; i++) { 
        std::cout << pixel_arr[i].r << ' ' << pixel_arr[i].g << ' ' << pixel_arr[i].b << '\n';
    }

}

