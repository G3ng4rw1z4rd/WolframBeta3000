#pragma once


#include <string>
#include <ostream>
#include <format>
#include <numbers>
#include <vector>

struct complex {
	double a, b;

	complex(double a, double b) : a(a), b(b) {
	}

	static complex from_polar(double mod, double theta) {
		return complex(mod * std::cos(theta), mod * std::sin(theta));
	}

	complex() : a(0), b(0) {}

	friend complex operator+(const complex& z, const complex& w) {
		return complex(z.a + w.a, z.b + w.b);
	}
	friend complex operator-(const complex& z, const complex& w) {
		return complex(z.a - w.a, z.b - w.b);
	}
	friend complex operator-(const complex& z) {
		return complex(-z.a, -z.b);
	}


	friend complex operator*(const complex& z, const complex& w) {
		return complex(z.a * w.a - z.b * w.b, z.a * w.b + z.b * w.a);
	}

	friend complex operator/(const complex& z, const complex& w) {
		double denom = w.a * w.a + w.b * w.b;
		if (denom == 0)
			throw std::domain_error("Division by zero");

		return complex(
			(z.a * w.a + z.b * w.b) / denom,
			(z.b * w.a - z.a * w.b) / denom
		);
	}

	friend std::ostream& operator<<(std::ostream& os, const complex& z) {
		return os << z.repr() << " | " << z.repr_alg();
	}

	complex conjugate() const {
		return complex(a, -b);
	}

	double module() const {
		return std::sqrt(a * a + b * b);
	}

	double Arg() const {
		auto theta = std::atan2(b, a);
		return theta < 0 ? theta + 2 * std::numbers::pi_v<double> : theta;
	}
	double argument() const {
		return std::atan2(b, a);
	}


	std::string repr() const {
		return std::format("{:.6f} . cis({:.6f})", module(), argument());
	}
	std::string repr_alg() const {
		return std::format("{:.6f} {} {:.6f}i", a, (b < 0 ? "-" : "+"), std::abs(b));
	}

	complex pow(const int n) const {
		return  complex::from_polar(std::pow(module(), n), n * argument());
	}
	std::vector<complex> roots(const int n) const {
		std::vector<complex> root;
		double r = std::pow(module(), 1.0 / n);
		double theta = argument();
		for (int k = 0; k < n; k++) {
			root.emplace_back(from_polar(r, (theta + 2 * std::numbers::pi_v<double> *k) / n));
		}
		return root;
	}



};
