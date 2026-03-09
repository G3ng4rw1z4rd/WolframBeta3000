#pragma once
#define _USE_MATH_DEFINES

#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <math.h>
#include <locale>
#include "complexNumbers.h"
#include "Parser.h"
static void sanitizenum(std::ostringstream& oss, double v) {
    if (!std::isfinite(v)) {
        oss << "null";
    }
    else {
        oss << v;
    }
}
namespace cartesianR2 {


    struct vec2d {
        double x, y;
        vec2d(double x = 0, double y = 0) : x(x), y(y) {}
        std::vector<double> toVector(double step = 0.1f) {
            std::vector<double> ret;
            int count = static_cast<int>((y - x) / step);
            for (int i = 0; i <= count; i++) {
                ret.push_back(x + i * step);
            }
            return ret;
        }
        friend vec2d operator+(const vec2d& a, const vec2d& b) {
            return vec2d(a.x + b.x, a.y + b.y);
        }
        friend vec2d operator-(const vec2d& a, const vec2d& b) {
            return vec2d(a.x - b.x, a.y - b.y);
        }


        friend vec2d operator*(const vec2d& a, double scalar) {
            return vec2d(a.x * scalar, a.y * scalar);
        }

    };
    struct vec3 : vec2d {
        double x, y ,z;
        vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {};

       static vec3 crossProduct(vec3 u,vec3 v) {
/*
i j k i j 
i j k i j
i j k i j


 I = j(2 row) * k(3 row)  -  (k 2nd row * (j 3rd row))
 J = k(2 row) * i(3row)   -  (i 2nd row))*(k 3r row)
 K = i(2 row) * j(3row)   - (j 2nd row))*(i 3r row)
   

*/
 
            return vec3{ (u.y * v.z) - (u.z * v.y),
                         (u.z * v.x) - (u.x * v.z),
                         (u.x * v.y) - (u.y * v.x) };


        }

       static double dotProduct(vec3 u, vec3 v) {
            return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
        }

       static  double TripleScalar(vec3 v , vec3 u,vec3 w) {
            return dotProduct(v,crossProduct(u,w));
        }

        friend vec3 operator*(const vec3& a, double scalar) {
            return vec3(a.x * scalar, a.y * scalar, a.z * scalar);
        }

        friend vec3 operator+(const vec3& a, const vec3& b) {
            return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
        }
        friend vec3 operator-(const vec3& a, const vec3& b) {
            return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
        }




    };

    inline std::vector<vec2d> functToCart(std::function<double(double x)> F, vec2d domain = { -5.0,5.0 }) {
        std::vector<vec2d> ret;
        for (double i = domain.x; i < domain.y; i += 1.f) {
            ret.push_back({ i, F(i) });
        }
        return ret;
    }

    inline std::vector<std::string> genCart(int width, int height, int y_center) {
        std::vector<std::string> lines;

        for (int i = 0; i < height; ++i) {
            if (i == y_center) {
                lines.push_back(std::string(width / 2, '-') + '+' + std::string(width / 2, '-'));
            }
            else {
                lines.push_back(std::string(width / 2, ' ') + '|' + std::string(width / 2, ' '));
            }
        }

        return lines;
    }

    inline std::vector<std::string> plotter_cli(std::function<double(double)> F, vec2d domain = { -5.0, 5.0 }, vec2d y_range = { -10.0f, 10.0f }) {
        auto r2_array = cartesianR2::functToCart(F, domain);
        int width = static_cast<int>(r2_array.size());
        int height = 20;
        int x_center = width / 2;
        int y_center = -1;
        if (y_range.x < 0 && y_range.y > 0) {
            double zero_pos = (0.0f - y_range.x) / (y_range.y - y_range.x);
            y_center = static_cast<int>((1.0f - zero_pos) * height);
        }
        auto cartPlan = genCart(width, height, y_center);

        for (int px = 0; px < width; ++px) {
            double x = domain.x + (domain.y - domain.x) * px / width;
            double y = F(x);

            if (y < y_range.x || y > y_range.y || std::isnan(y) || std::isinf(y)) continue;

            double y_normalized = (y - y_range.x) / (y_range.y - y_range.x);
            int row = height - 1 - static_cast<int>(y_normalized * height);

            if (row >= 0 && row < height) {
                cartPlan[row][px] = '*';
            }
        }
        int resolution = 15;

        for (double i = domain.x; i < domain.y; i += 1.0f / resolution) {
            double y = F(i);

            if (y < y_range.x || y > y_range.y)
                continue;

            double y_normalized = (y - y_range.x) / (y_range.y - y_range.x);
            int row = height - 1 - static_cast<int>(y_normalized * height);
            int col = static_cast<int>((i - domain.x) * resolution);

            if (row >= 0 && row < height && col >= 0 && col < width)
                cartPlan[row][col] = '*';
        }
        return cartPlan;
    }

    inline std::string plotter_js(std::function<double(double)> F,vec2d domain = { -5.0, 5.0 },int resolution = 200){
        std::ostringstream oss;
        oss.imbue(std::locale::classic());

        oss << "[";

        bool first = true;

        for (int i = 0; i <= resolution; i++) {

            double x = domain.x + (domain.y - domain.x) * i / double(resolution);
            double y = F(x);

            if (!std::isfinite(y))
                continue;

            if (!first)
                oss << ",";

            first = false;

            oss << "[" << x << "," << y << "]";
        }

        oss << "]";
        return oss.str();
    }

    inline std::string plotter_js3d(std::function<double(double, double)> F, vec2d domain = { -5.0, 5.0 }, int resolution = 50) {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());

        std::vector<double> x_vals, y_vals;
        std::vector<std::vector<double>> z_matrix;

        for (int i = 0; i <= resolution; i++) {
            double x = domain.x + (domain.y - domain.x) * i / double(resolution);
            x_vals.push_back(x);
        }
        for (int j = 0; j <= resolution; j++) {
            double y = domain.x + (domain.y - domain.x) * j / double(resolution);
            y_vals.push_back(y);
        }

        for (int i = 0; i <= resolution; i++) {
            std::vector<double> row;
            double y = y_vals[i];
            for (int j = 0; j <= resolution; j++) {
                double x = x_vals[j];
                double z = F(x, y);
                row.push_back(std::isfinite(z) ? z : 0.0);
            }
            z_matrix.push_back(row);
        }

 

        oss << "{";

        oss << "\"x\":[";
        for (size_t i = 0; i < x_vals.size(); i++) {
            if (i > 0) oss << ",";
            sanitizenum(oss, x_vals[i]);
        }
        oss << "],";

        oss << "\"y\":[";
        for (size_t i = 0; i < y_vals.size(); i++) {
            if (i > 0) oss << ",";
            sanitizenum(oss, y_vals[i]);
        }
        oss << "],";

        oss << "\"z\":[";
        for (size_t i = 0; i < z_matrix.size(); i++) {
            if (i > 0) oss << ",";
            oss << "[";
            for (size_t j = 0; j < z_matrix[i].size(); j++) {
                if (j > 0) oss << ",";
                sanitizenum(oss, z_matrix[i][j]);
            }
            oss << "]";
        }
        oss << "]";

        oss << "}";
        return oss.str();
    }


    inline std::vector<std::string> revolutionX( std::function<double( double )> F, vec2d domain = { -5.0, 5.0 }, int height = 30, int width = 60 )   {
        std::vector<std::string> canvas( height, std::string( width, ' ' ) );

        double step = ( domain.y - domain.x ) / width;
        int center = height / 2;

        for ( int col = 0; col < width; ++col ) {
            double x = domain.x + col * step;
            double radius = std::fabs( F( x ) )/2;

            if ( !std::isfinite( radius ) || std::fabs( x ) > 1000 ) continue;

            int r = std::min( static_cast< int >( radius * 10 ), center - 1 );

            for ( int row = center - r; row <= center + r; ++row ) {
                if ( row >= 0 && row < height )
                    canvas[ row ][ col ] = '*';
            }
        }

        return canvas;
    }
    inline std::vector<std::string> revolutionY( std::function<double( double )> F, vec2d domain = { -5.0, 5.0 }, int width = 60, int height = 30 ) {
        std::vector<std::string> result( height, std::string( width, ' ' ) );

        double step = ( domain.y - domain.x ) / height;

        for ( int i = 0; i < height; ++i ) {
            double y = domain.y - i * step; 
            double x = F( y );
            if ( !std::isfinite( x ) || x < 0 ) continue;
            if ( !std::isfinite( x ) || std::fabs( x ) > 1000 ) continue;

            int stars = static_cast< int >( x * 10 );  
            int left = std::max( 0, width / 2 - stars );
            int right = std::min( width, width / 2 + stars );

            for ( int j = left; j < right; ++j ) {
                result[ i ][ j ] = '*';
            }
        }

        return result;
    }
    inline std::string revolution3D( std::function<double( double )> F, vec2d domain = { -5.0, 5.0 }, int resolution = 100, int segments = 40 ) {
        std::ostringstream oss;
        oss << "[";

        for ( int i = 0; i <= resolution; i++ ) {
            double x = domain.x + ( domain.y - domain.x ) * i / resolution;
            double r = std::fabs( F( x ) );
            if ( !std::isfinite( r ) ) continue;

            for ( int j = 0; j <= segments; j++ ) {
                double theta = 2.0 * M_PI * j / segments;
                double y = r * std::cos( theta );
                double z = r * std::sin( theta );

                if ( i > 0 || j > 0 ) oss << ",";
                oss << "[" << x << "," << y << "," << z << "]";
            }
        }

        oss << "]";
        return oss.str();
    }
    inline std::string revolution3D_Y( std::function<double( double )> F, vec2d domain = { -5.0, 5.0 }, int resolution = 100, int segments = 40, double tolerance = 1e-6,int maxIterations = 100) {
        std::ostringstream oss;
        oss << "[";
        bool first = true;

        auto inverse = [&](double y_target) -> double {
            double a = domain.x;
            double b = domain.y;

            for (int i = 0; i < maxIterations; ++i) {
                double mid = (a + b) / 2.0;
                double f_mid = F(mid);
                if (!std::isfinite(f_mid)) break;
                if (std::abs(f_mid - y_target) < tolerance) return mid;
                if (f_mid < y_target) a = mid;
                else b = mid;
            }
            return (a + b) / 2.0; 
            };

        for (int i = 0; i <= resolution; ++i) {
            double t = static_cast<double>(i) / resolution;
            double y = domain.x + t * (domain.y - domain.x);

            double x_radius = inverse(y);
            double r = std::fabs(x_radius);
            if (!std::isfinite(r)) continue;

            for (int j = 0; j <= segments; ++j) {
                double theta = 2.0 * M_PI * j / segments;
                double x = r * std::cos(theta);
                double z = r * std::sin(theta);

                if (!first) oss << ",";
                first = false;

                oss << "[" << x << "," << y << "," << z << "]";
            }
        }

        oss << "]";
        return oss.str();
    }

    inline std::vector<std::string> contourMap(std::function<double(double, double)> F,
        vec2d x_domain = { -5.0, 5.0 },
        vec2d y_domain = { -5.0, 5.0 },
        int width = 60, int height = 30,
        int levels = 8) {
        std::vector<std::string> map(height, std::string(width, ' '));

        double x_step = (x_domain.y - x_domain.x) / width;
        double y_step = (y_domain.y - y_domain.x) / height;

        double min_val = std::numeric_limits<double>::max();
        double max_val = std::numeric_limits<double>::lowest();

        for (int i = 0; i < height; ++i) {
            double y = y_domain.y - i * y_step;
            for (int j = 0; j < width; ++j) {
                double x = x_domain.x + j * x_step;
                double z = F(x, y);
                if (std::isfinite(z)) {
                    if (z < min_val) min_val = z;
                    if (z > max_val) max_val = z;
                }
            }
        }

        if (min_val == max_val) {
            min_val -= 1;
            max_val += 1;
        }

        std::string contour_chars = " .-:=+*#%@";

        for (int i = 0; i < height; ++i) {
            double y = y_domain.y - i * y_step;
            for (int j = 0; j < width; ++j) {
                double x = x_domain.x + j * x_step;
                double z = F(x, y);

                if (!std::isfinite(z)) {
                    map[i][j] = '?';
                    continue;
                }

                double normalized = (z - min_val) / (max_val - min_val);
                int char_index = static_cast<int>(normalized * (contour_chars.size() - 1));
                char_index = std::max(0, std::min(static_cast<int>(contour_chars.size() - 1), char_index));

                map[i][j] = contour_chars[char_index];
            }
        }

        return map;
    }

    inline std::string contourMap_js(std::function<double(double, double)> F,
        vec2d x_domain = { -5.0, 5.0 },
        vec2d y_domain = { -5.0, 5.0 },
        int resolution = 50) {
        std::ostringstream oss;

        std::vector<double> x_vals, y_vals;
        std::vector<std::vector<double>> z_matrix;

        for (int i = 0; i <= resolution; i++) {
            double x = x_domain.x + (x_domain.y - x_domain.x) * i / double(resolution);
            x_vals.push_back(x);
        }
        for (int j = 0; j <= resolution; j++) {
            double y = y_domain.x + (y_domain.y - y_domain.x) * j / double(resolution);
            y_vals.push_back(y);
        }

        for (int i = 0; i <= resolution; i++) {
            std::vector<double> row;
            double y = y_vals[i];
            for (int j = 0; j <= resolution; j++) {
                double x = x_vals[j];
                double z = F(x, y);
                row.push_back(std::isfinite(z) ? z : 0.0);
            }
            z_matrix.push_back(row);
        }

        oss << "{";
        oss << "\"x\":[";
        for (size_t i = 0; i < x_vals.size(); i++) {
            if (i > 0) oss << ",";
            oss << x_vals[i];
        }
        oss << "],\"y\":[";
        for (size_t i = 0; i < y_vals.size(); i++) {
            if (i > 0) oss << ",";
            oss << y_vals[i];
        }
        oss << "],\"z\":[";
        for (size_t i = 0; i < z_matrix.size(); i++) {
            if (i > 0) oss << ",";
            oss << "[";
            for (size_t j = 0; j < z_matrix[i].size(); j++) {
                if (j > 0) oss << ",";
                oss << z_matrix[i][j];
            }
            oss << "]";
        }
        oss << "]}";
        return oss.str();
    }

    inline bool usesTwoVariables(const std::string& expr) {
        return expr.find('x') != std::string::npos && expr.find('y') != std::string::npos;
    }
    inline std::string complex_plot_js(std::function<complex(complex)> F,vec2d domain = { -5.0, 5.0 },int resolution = 200)
    {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());

        std::vector<double> x_vals;
        std::vector<double> y_vals;
        std::vector<std::vector<double>> re_matrix;
        std::vector<std::vector<double>> im_matrix;

        x_vals.reserve(resolution + 1);
        y_vals.reserve(resolution + 1);
        re_matrix.reserve(resolution + 1);
        im_matrix.reserve(resolution + 1);

        for (int i = 0; i <= resolution; i++) {
            double x = domain.x + (domain.y - domain.x) * i / double(resolution);
            x_vals.push_back(x);
        }

        for (int j = 0; j <= resolution; j++) {
            double y = domain.x + (domain.y - domain.x) * j / double(resolution);
            y_vals.push_back(y);

            std::vector<double> re_row;
            std::vector<double> im_row;
            re_row.reserve(resolution + 1);
            im_row.reserve(resolution + 1);

            for (int i = 0; i <= resolution; i++) {
                double x = x_vals[i];

                try {
                    complex z(x, y);
                    complex value = F(z);

                    if (!std::isfinite(value.a) || !std::isfinite(value.b)) {
                        re_row.push_back(std::numeric_limits<double>::quiet_NaN());
                        im_row.push_back(std::numeric_limits<double>::quiet_NaN());
                    }
                    else {
                        re_row.push_back(value.a);
                        im_row.push_back(value.b);
                    }
                }
                catch (...) {
                    re_row.push_back(std::numeric_limits<double>::quiet_NaN());
                    im_row.push_back(std::numeric_limits<double>::quiet_NaN());
                }
            }

            re_matrix.push_back(re_row);
            im_matrix.push_back(im_row);
        }

        oss << "{";

        oss << "\"x\":[";
        for (size_t i = 0; i < x_vals.size(); i++) {
            if (i > 0) oss << ",";
            oss << x_vals[i];
        }
        oss << "],";

        oss << "\"y\":[";
        for (size_t i = 0; i < y_vals.size(); i++) {
            if (i > 0) oss << ",";
            oss << y_vals[i];
        }
        oss << "],";

        oss << "\"re\":[";
        for (size_t i = 0; i < re_matrix.size(); i++) {
            if (i > 0) oss << ",";
            oss << "[";
            for (size_t j = 0; j < re_matrix[i].size(); j++) {
                if (j > 0) oss << ",";
                if (!std::isfinite(re_matrix[i][j])) oss << "null";
                else oss << re_matrix[i][j];
            }
            oss << "]";
        }
        oss << "],";

        oss << "\"im\":[";
        for (size_t i = 0; i < im_matrix.size(); i++) {
            if (i > 0) oss << ",";
            oss << "[";
            for (size_t j = 0; j < im_matrix[i].size(); j++) {
                if (j > 0) oss << ",";
                if (!std::isfinite(im_matrix[i][j])) oss << "null";
                else oss << im_matrix[i][j];
            }
            oss << "]";
        }
        oss << "]";

        oss << "}";
        return oss.str();
    }

    inline std::string complex2GLSL(const std::string& expr) {
        parser::ComplexParser cp(expr, &complexVars);
        auto ast = cp.parse();
        return ast->toGLSL();
    }
    namespace utils {

        inline double vRevolution( std::function<double( double )> F, double a, double b, std::string eixo = "x", int n = 100000 ) {
            double delta = ( b - a ) / n;
            double sum = 0.0;

            for ( int i = 0; i < n; ++i ) {
                double x = a + ( i + 0.5 ) * delta;
                double fx = F( x );
                double dfx = ( F( x + 1e-5 ) - F( x - 1e-5 ) ) / ( 2e-5 );
                double arc = std::sqrt( 1 + dfx * dfx );

                if ( eixo == "x" ) {
                    sum += fx * arc * delta;
                }
                else if ( eixo == "y" ) {
                    sum += x * arc * delta;
                }
            }

            return std::fabs( 2.0 * 3.14159265358979323846 * sum);
        }

        inline bool B_injectora(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }) {
            auto x_vals = domain.toVector(0.1);
            std::vector<double> y_seen;

            for (double x : x_vals) {
                double y = F(x);
                for (double seen : y_seen) {
                    if (std::fabs(seen - y) < 0.001) {
                        return false;
                    }
                }
                y_seen.push_back(y);
            }

            return true;
        }
        inline bool B_sobrejectora(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, vec2d contra = { -1000.0f, 1000.0f }, double step = 0.1f, double epsilon = 0.01f) {
            auto domainVec = domain.toVector(step);
            auto contraVec = contra.toVector(step);
            std::vector<double> image;

            for (double x : domainVec) {
                image.push_back(F(x));
            }

            for (double y : contraVec) {
                bool found = false;
                for (double fy : image) {
                    if (std::fabs(fy - y) < epsilon) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
            return true;
        }
        inline bool B_Bijectora(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, vec2d contra = { -1000.0f, 1000.0f }, double step = 0.1f, double epsilon = 0.01f) {
            return B_sobrejectora(F, domain, contra, step, epsilon) && B_injectora(F, domain);
        }
        inline bool B_periodica(std::function<double(double)> F,vec2d domain = { -10.0f, 10.0f },double epsilon = 0.1f,double step = 0.01f) {

            std::vector<double> periods = { 3.14159f, 6.28318f, 1.5708f };

            auto values = domain.toVector(step);

            for (double p : periods) {
                bool periodic = true;

                for (double x : values) {
                    double x_p = x + p;
                    
                    if (x_p > domain.y || x_p < domain.x) continue;

                    double fx = F(x);
                    double fxp = F(x_p);

                    if (!std::isfinite(fx) || !std::isfinite(fxp)) {
                        periodic = false;
                        break;
                    }

                    if (std::fabs(fxp - fx) > epsilon) {
                        periodic = false;
                        break;
                    }
                }

                if (periodic) return true;
            }

            return false;
        }
        inline bool B_par(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, double epsilon = .01f) {
            for (double x : domain.toVector(0.1f)) {
                if (std::fabs(F(x) - F(-x)) > epsilon) {
                    return false;
                }
            }
            return true;
        }
        inline bool B_impar(std::function<double(double x)> F, vec2d domain = { -5.0, 5.0 }, double epsilon = .01f) {
            for (double x : domain.toVector(0.1f)) {
                if (std::fabs(F(-x) - (-F(x))) > epsilon) {
                    return false;
                }
            }
            return true;
        }
        inline double derivada(std::function<double(double x)> F,double a=0, double step = 0.01) {
            double h = 1e-5f;
            return (double)(F(a + h) - F(a - h)) / (2 * h);
        }
        inline double integralriemannstyle(std::function<double(double x)> F, double a = 0, double b=0,double n= 1000000) {
            double deltaecs = (b - a) / n;
            double sum = 0.0;

            for (int i = 0; i < n; i++) {
                double x = a + (i + 0.5) * deltaecs;
                sum += F(x) * deltaecs;
            }
            return sum;
        }
        inline double arcLength( std::function<double( double )> F, double a, double b, int n = 10000 ) {
            double delta = ( b - a ) / n;
            double sum = 0.0;
            for ( int i = 0; i < n; ++i ) {
                double x = a + ( i + 0.5 ) * delta;
                double dx = ( F( x + 1e-5 ) - F( x - 1e-5 ) ) / ( 2e-5 );  
                double arc = std::sqrt( 1 + dx * dx );
                sum += arc * delta;
            }
            return sum;
        }

    }
}