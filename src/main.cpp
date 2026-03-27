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

/*
 * Comparison functor structs for comparison function ingestions (Point)
 */
struct compare_by_y {
    inline bool operator() (const Point& a, const Point& b) {
        return (a.y < b.y);
    }
};
struct compare_by_x {
    inline bool operator() (const Point& a, const Point& b) {
        return (a.x < b.x);
    }
};

/* Pixel struct to represent rgb pixel value in ppm image
 */
struct Pixel {
    double r;
    double g;
    double b;
};

using Color = Pixel;

struct Triangle {
    Point a;
    Point b;
    Point c;
    Color color;
};


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
 * TODO: Optimize --> for loop with ceil + cast + double operations
 */
void draw_line_v2(std::vector<Pixel>& pixel_arr, const Point& a, const Point& b, const Color& c) {
    double t = 0;
    int a_x = a.x;
    int a_y = a.y;
    int b_x = b.x;
    int b_y = b.y;
    bool swapped = false; 

    if (std::abs(b.x - a.x) < std::abs(b.y - a.y)) {
        std::swap(a_x, a_y);
        std::swap(b_x, b_y);
        swapped = true;
    }

    if (a_x > b_x) {
        std::swap(a_x, b_x);
        std::swap(a_y, b_y);
    }

    int y = 0;
    for (auto x = a_x; x < b_x; x += 1) {
        t = (x - a_x) / static_cast<double>((b_x - a_x));
        y = std::round(a_y + t * (b_y - a_y));
        if (swapped) std::swap(x, y);
        Pixel& p = pixel_arr[(y * WIDTH) + x];
        if (swapped) std::swap(x, y);
        p.r = c.r;
        p.g = c.g;
        p.b = c.b;
    }
}

/*
 * Draw triangle outline method
 */
void draw_triangle(std::vector<Pixel>& pixel_arr, const Triangle& t) {
    draw_line_v2(pixel_arr, t.a, t.b, t.color);
    draw_line_v2(pixel_arr, t.b, t.c, t.color);
    draw_line_v2(pixel_arr, t.a, t.c, t.color);
}


/*
 * Scanline fill triangle method
 * TODO: Fix ('Slightly Misaligned Sprite')
 */
void fill_triangle(std::vector<Pixel>& pixel_arr, const Triangle& t, const Color& color) {
   // Sorts points by y value top to bottom (lowest to highest)
   std::vector<Point> t_points{t.a, t.b, t.c};
   std::sort(t_points.begin(), t_points.end(), compare_by_y());
   assert(t_points[0].y <= t_points[1].y);
   assert(t_points[1].y <= t_points[2].y);
    
   //TODO: Truncating error potentially
   double m_x = (static_cast<double>(t_points[2].x - t_points[0].x) / (t_points[2].y - t_points[0].y)) * (t_points[1].y - t_points[0].y) + t_points[0].x;
    
   if (t_points[1].y != t_points[0].y) {
       double inv_slope_1 = (static_cast<double>(t_points[1].x - t_points[0].x) / (t_points[1].y - t_points[0].y));
       double inv_slope_2 = (static_cast<double>(m_x - t_points[0].x) / (t_points[1].y - t_points[0].y));
       double x_1 = t_points[0].x;
       double x_2 = t_points[0].x;
       for (int y = t_points[0].y; y < t_points[1].y; y++) {
           draw_line_v2(pixel_arr, {static_cast<int>(x_1), y}, {static_cast<int>(x_2), y}, color);
           x_1 += inv_slope_1;
           x_2 += inv_slope_2;
       }
   }
    
   if (t_points[2].y != t_points[1].y) {
       double inv_slope_1 = (static_cast<double>(t_points[2].x - t_points[1].x) / (t_points[2].y - t_points[1].y));
       double inv_slope_2 = (static_cast<double>(t_points[2].x - m_x) / (t_points[2].y - t_points[1].y));
       double x_1 = t_points[1].x;
       double x_2 = m_x;
       for (int y = t_points[1].y; y < t_points[2].y; y++) {
           draw_line_v2(pixel_arr, {static_cast<int>(x_1), y}, {static_cast<int>(x_2), y}, color);
           x_1 += inv_slope_1;
           x_2 += inv_slope_2;
       }
   }
}

/*
 * Triangle Rasterize Method using 'Bounding Box'
 * - Creates 'boundary' around given vertices 
 * - Cuts out triangle within boundary
 */
void fill_triangle_bb(std::vector<Pixel>& pixel_arr, const Triangle& t, const Color& color) {
    int l_x = std::min(std::min(t.a.x, t.b.x), t.c.x);
    int h_x = std::max(std::max(t.a.x, t.b.x), t.c.x);
    int l_y = std::min(std::min(t.a.y, t.b.y), t.c.y);
    int h_y = std::max(std::max(t.a.y, t.b.y), t.c.y);

    for (int y = l_y; y <= h_y; y++) {
        draw_line_v2(pixel_arr, {l_x, y}, {h_x, y}, color);
    }



    
}

int main() {
    
    std::cout << "P3\n" << WIDTH << " " << HEIGHT << "\n" << "255\n";
    
    //Default black screen init
    Pixel default_p = {0.0, 0.0, 0.0};
    std::vector<Pixel> pixel_arr(HEIGHT * WIDTH, default_p);

    Triangle t = {
        {100, 100}, // a
        {200, 50}, // b
        {200, 150}, // c
        {255, 0, 0} // color
    };
    draw_triangle(pixel_arr, t);
    fill_triangle(pixel_arr, t, t.color);

    Triangle t_2 = {
        {150, 300},
        {180, 200},
        {210, 300},
        {0, 0, 255}
    };
    draw_triangle(pixel_arr, t_2);
    fill_triangle(pixel_arr, t_2, t_2.color);

    //TODO: Find way to output without using buffer
    for (int i = 0; i < HEIGHT * WIDTH; i++) { 
        std::cout << pixel_arr[i].r << ' ' << pixel_arr[i].g << ' ' << pixel_arr[i].b << '\n';
    }

}

