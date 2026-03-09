#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <cctype>
#include <math.h>
#include <numeric>
#include <stdexcept>
#include <algorithm>
#include <source_location>
#include <ostream>
#include <regex>
#include "numberTheory.h"
#include "complexNumbers.h"



[[noreturn]]
inline void throwLoc(
	const std::string& msg,
	const std::source_location& loc =
	std::source_location::current())
{
	std::ostringstream os;
	os << msg
		<< "\nArquivo: " << loc.file_name()
		<< "\nFunçao: " << loc.function_name()
		<< "\nLinha: " << loc.line();

	throw std::runtime_error(os.str());
}





namespace parser {

#pragma region Common
	inline std::unordered_map<std::string, double> constants = {
		{"pi", M_PI},
		{"e", M_E},
		{"tau", 2 * M_PI},
		{"phi", 1.61803398875},
		{"c", 299792458},
		{"g", 9.80665},
		{"h", 6.62607015e-34},
		{"k", 5}
	};

	inline const std::unordered_map<std::string, std::function<double(double)>> funcs = {
		{"sin",   std::function<double(double)>(static_cast<double(*)(double)>(std::sin))},
		{"cos",   std::function<double(double)>(static_cast<double(*)(double)>(std::cos))},
		{"tan",   std::function<double(double)>(static_cast<double(*)(double)>(std::tan))},
		{"asin",  std::function<double(double)>(static_cast<double(*)(double)>(std::asin))},
		{"acos",  std::function<double(double)>(static_cast<double(*)(double)>(std::acos))},
		{"atan",  std::function<double(double)>(static_cast<double(*)(double)>(std::atan))},
		{"sinh",  std::function<double(double)>(static_cast<double(*)(double)>(std::sinh))},
		{"cosh",  std::function<double(double)>(static_cast<double(*)(double)>(std::cosh))},
		{"tanh",  std::function<double(double)>(static_cast<double(*)(double)>(std::tanh))},
		{"log",   std::function<double(double)>(static_cast<double(*)(double)>(std::log10))},
		{"ln",    std::function<double(double)>(static_cast<double(*)(double)>(std::log))},
		{"exp",   std::function<double(double)>(static_cast<double(*)(double)>(std::exp))},
		{"abs",   std::function<double(double)>(static_cast<double(*)(double)>(std::fabs))},
		{"sqrt",  std::function<double(double)>(static_cast<double(*)(double)>(std::sqrt))},
		{"cbrt",  std::function<double(double)>(static_cast<double(*)(double)>(std::cbrt))},
		{"ceil",  std::function<double(double)>(static_cast<double(*)(double)>(std::ceil))},
		{"floor", std::function<double(double)>(static_cast<double(*)(double)>(std::floor))},
		{"round", std::function<double(double)>(static_cast<double(*)(double)>(std::round))},
		{"trunc", std::function<double(double)>(static_cast<double(*)(double)>(std::trunc))},
		{"sign",  std::function<double(double)>([](double x) {
			return x < 0 ? -1.0 : (x > 0 ? 1.0 : 0.0);
		})},
		{"min",   std::function<double(double)>([](double x) { return x; })},
		{"max",   std::function<double(double)>([](double x) { return x; })},
		{"gcd",   std::function<double(double)>([](double x) { return x; })},
		{"lcm",   std::function<double(double)>([](double x) { return x; })},
		{"deg", [](double x) { return x * M_PI / 180.0; }}
	};

	inline const std::unordered_map<std::string, std::function<double(double, double)>> funcs2 = {
		{"gcd", [](double a, double b) {
			std::string result = numberTheory::gcd(true, a, b);
			#ifndef __EMSCRIPTEN__
			//std::cout << result;
			#endif
			return std::gcd(static_cast<long long>(a), static_cast<long long>(b));
		}},
		{"lcm", [](double a, double b) {
			return static_cast<double>(std::lcm(
				static_cast<long long>(a), static_cast<long long>(b)));
		}},
		{"min", [](double a, double b) { return std::min(a, b); }},
		{"max", [](double a, double b) { return std::max(a, b); }}
	};

	inline std::unordered_map<std::string, std::function<complex(complex, complex)>> funcsZ2 = {
		{"Cpow", [](complex base, complex expo) {
			if (base.a == 0.0 && base.b == 0.0) return complex(0,0);
			complex logBase(std::log(base.module()), base.argument());
			complex w = expo * logBase;
			return complex::from_polar(std::exp(w.a), w.b);
		}},

		{"Cadd", [](complex a, complex b) { return a + b; }},
		{"Csub", [](complex a, complex b) { return a - b; }},
		{"Cmul", [](complex a, complex b) { return a * b; }},
		{"Cdiv", [](complex a, complex b) {
			if (std::abs(b.a) < 1e-15 && std::abs(b.b) < 1e-15)
				throwLoc("Division by zero in Cdiv");
			return a / b;
		}}
	};


	inline std::unordered_map<std::string, std::function<complex(complex)>> funcsZ = {
	{"Csqrt", [](complex z) {
		return complex::from_polar(std::sqrt(z.module()), z.argument() / 2.0);
	}},

	{"Cinvert", [](complex z) {
		double d = z.a * z.a + z.b * z.b;
		if (std::abs(d) < 1e-15) throwLoc("Division by zero in Cinvert");
		return complex(z.a / d, -z.b / d);
	}},

	{"Cexp", [](complex z) {
		return complex::from_polar(std::exp(z.a), z.b);
	}},

	{"Clog", [](complex z) {
		if (z.a == 0.0 && z.b == 0.0) throwLoc("log(0) undefined");
		return complex(std::log(z.module()), z.argument());
	}},

	{"Csin", [](complex z) {
		return complex(std::sin(z.a) * std::cosh(z.b),
					   std::cos(z.a) * std::sinh(z.b));
	}},

	{"Ccos", [](complex z) {
		return complex(std::cos(z.a) * std::cosh(z.b),
					  -std::sin(z.a) * std::sinh(z.b));
	}},

	{"Ctan", [](complex z) {
		double d = std::cos(2.0 * z.a) + std::cosh(2.0 * z.b);
		if (std::abs(d) < 1e-15) throwLoc("Division by zero in Ctan");
		return complex(std::sin(2.0 * z.a) / d, std::sinh(2.0 * z.b) / d);
	}},

	{"Csinh", [](complex z) {
		return complex(std::sinh(z.a) * std::cos(z.b),
					   std::cosh(z.a) * std::sin(z.b));
	}},

	{"Ccosh", [](complex z) {
		return complex(std::cosh(z.a) * std::cos(z.b),
					   std::sinh(z.a) * std::sin(z.b));
	}},

	{"Ctanh", [](complex z) {
		double d = std::cosh(2.0 * z.a) + std::cos(2.0 * z.b);
		if (std::abs(d) < 1e-15) throwLoc("Division by zero in Ctanh");
		return complex(std::sinh(2.0 * z.a) / d, std::sin(2.0 * z.b) / d);
	}},

	{"Cabs", [](complex z) {
		return complex(z.module(), 0.0);
	}},

	{"Carg", [](complex z) {
		return complex(z.argument(), 0.0);
	}},

	{"Cre", [](complex z) {
		return complex(z.a, 0.0);
	}},

	{"Cim", [](complex z) {
		return complex(z.b, 0.0);
	}},

	{"Cconj", [](complex z) {
		return z.conjugate();
	}},

	{"Cfloor", [](complex z) {
		return complex(std::floor(z.a), std::floor(z.b));
	}},

	{"Cceil", [](complex z) {
		return complex(std::ceil(z.a), std::ceil(z.b));
	}},

	{"Csec", [](complex z) {
		return funcsZ["Cinvert"](funcsZ["Ccos"](z));
	}},

	{"Ccsc", [](complex z) {
		return funcsZ["Cinvert"](funcsZ["Csin"](z));
	}},

	{"Ccot", [](complex z) {
		return funcsZ["Cinvert"](funcsZ["Ctan"](z));
	}},

	{"Csech", [](complex z) {
		return funcsZ["Cinvert"](funcsZ["Ccosh"](z));
	}},

	{"Ccsch", [](complex z) {
		return funcsZ["Cinvert"](funcsZ["Csinh"](z));
	}},

	{"Ccoth", [](complex z) {
		return funcsZ["Cinvert"](funcsZ["Ctanh"](z));
	}},

	{"Carcsin", [](complex z) {
		complex i(0,1), one(1,0);
		complex w = i * z + funcsZ["Csqrt"](one - z * z);
		complex L = funcsZ["Clog"](w);
		return complex(L.b, -L.a);
	}},

	{"Carccos", [](complex z) {
		return complex(M_PI / 2.0, 0.0) - funcsZ["Carcsin"](z);
	}},

	{"Carctan", [](complex z) {
		complex i(0,1), one(1,0);
		complex a = funcsZ["Clog"](one - i * z);
		complex b = funcsZ["Clog"](one + i * z);
		return complex(0, 0.5) * (a - b);
	}},

	{"Carcsinh", [](complex z) {
		complex one(1,0);
		return funcsZ["Clog"](z + funcsZ["Csqrt"](z* z + one));
	}},

	{"Carccosh", [](complex z) {
		complex one(1,0);
		return funcsZ["Clog"](z + funcsZ["Csqrt"](z - one) * funcsZ["Csqrt"](z + one));
	}},

	{"Carctanh", [](complex z) {
		complex one(1,0);
		return complex(0.5,0.0) * funcsZ["Clog"]((one + z) / (one - z));
	}},

	{"Carcsec", [](complex z) {
		return funcsZ["Carccos"](funcsZ["Cinvert"](z));
	}},

	{"Carccsc", [](complex z) {
		return funcsZ["Carcsin"](funcsZ["Cinvert"](z));
	}},

	{"Carccot", [](complex z) {
		return funcsZ["Carctan"](funcsZ["Cinvert"](z));
	}},

	{"Carcsech", [](complex z) {
		return funcsZ["Carccosh"](funcsZ["Cinvert"](z));
	}},

	{"Carccsch", [](complex z) {
		return funcsZ["Carcsinh"](funcsZ["Cinvert"](z));
	}},

	{"Carccoth", [](complex z) {
		return funcsZ["Carctanh"](funcsZ["Cinvert"](z));
	}},

	{ "Cgamma", [](complex z) {
		static const double g = 7.0;
		static const double p[] = {
			0.99999999999980993,
			676.5203681218851,
		   -1259.1392167224028,
			771.32342877765313,
		   -176.61502916214059,
			 12.507343278686905,
			 -0.13857109526572012,
			  9.9843695780195716e-6,
			  1.5056327351493116e-7
		};

		complex one(1.0, 0.0);
		complex pi(M_PI, 0.0);

		if (z.a < 0.5) {
			complex piz = z * pi;
			complex sin_piz(
				std::sin(piz.a) * std::cosh(piz.b),
				std::cos(piz.a) * std::sinh(piz.b)
			);

			if (std::abs(sin_piz.a) < 1e-15 && std::abs(sin_piz.b) < 1e-15)
				throwLoc("Gamma pole");

			return pi / (sin_piz * funcsZ["Cgamma"](one - z));
		}

		z = z - one;

		complex x(p[0], 0.0);
		for (int i = 1; i < 9; ++i) {
			x = x + complex(p[i], 0.0) / (z + complex((double)i, 0.0));
		}

		complex t = z + complex(g + 0.5, 0.0);

		complex logt(std::log(t.module()), t.argument());
		complex zph = z + complex(0.5, 0.0);

		complex powTerm = funcsZ2["Cpow"](t, zph);
		complex expTerm = funcsZ["Cexp"](-t);

		return complex(std::sqrt(2.0 * M_PI), 0.0) * powTerm * expTerm * x;
	}},

	{"Czeta", [](complex z) {
		complex sum(0,0);
		for (int n = 1; n <= 40; ++n) {
			complex logn(std::log((double)n), 0);
			complex term = funcsZ2["Cpow"](complex((double)n,0), -z);
			sum = sum + term;
		}
		return sum;
	}},

	{"Cphi", [](complex z) {
		return complex((1.0 + std::sqrt(5.0)) * 0.5, 0.0);
	}},

	{"Ctheta", [](complex z) {
		return complex(z.argument(), 0.0);
	}},

	{"Cjoukowsky", [](complex z) {
		return z + funcsZ["Cinvert"](z);
	}},

	{"Cdisk", [](complex z) {
		return complex(z.module() <= 1.0 ? 1.0 : 0.0, 0.0);
	}},

	{"Cmandelbrot", [](complex c) {
		complex z(0,0);
		int maxIt = 80;
		int it = 0;
		while (it < maxIt && z.module() <= 2.0) {
			z = z * z + c;
			++it;
		}
		return complex((double)it / (double)maxIt, 0.0);
	}},

	{"Cmap", [](complex z) {
		return z;
	}},

	{"Ctransform", [](complex z) {
		return z;
	}}
	};



#pragma endregion

#pragma region Real

	inline void addConstant(std::string name, double val) {
		constants[name] = val;
	}

	inline bool isVar(const std::string& x) {
		return x == "x" || x == "y";
	}

	inline bool isNumber(char x) {
		return std::isdigit(x);
	}

	inline bool isSym(char x) {
		return x == '+' || x == '-' || x == '*' || x == '/' || x == '^';
	}

	inline bool isConstant(const std::string& x) {
		return constants.find(x) != constants.end();
	}

	inline bool isFunction(const std::string& x) {
		return funcs.find(x) != funcs.end();
	}

	inline std::function<double(double)> getFunction(const std::string& x) {
		return funcs.at(x);
	}

	class Expr {
	public:
		virtual double eval(double x = 0.0, double y = 0.0) const = 0;
		virtual std::string repr() const = 0;
		virtual std::string deriv() const = 0;
		virtual std::shared_ptr<Expr> simplify() const = 0;

		virtual ~Expr() = default;
	};

	class Lit : public Expr {
		double value;
	public:
		Lit(double val) : value(val) {}
		double eval(double x = 0.0, double y = 0.0) const override { return value; }
		std::string repr() const override { return std::to_string(value); }
		std::string deriv() const override { return "0"; }
		std::shared_ptr<Expr> simplify() const override {
			return std::make_shared<Lit>(value);
		}
	};

	class Var : public Expr {
		std::string name;
	public:
		Var(std::string varname = "x") : name(std::move(varname)) {}

		double eval(double x = 0.0, double y = 0.0) const override {
			if (name == "x") return x;
			if (name == "y") return y;
			throwLoc("Unknown variable: " + name);
		}

		std::string repr() const override {
			return name;
		}

		std::string deriv() const override {
			return name == "x" ? "1" : "0";
		}

		std::shared_ptr<Expr> simplify() const override {
			return std::make_shared<Var>(name);
		}
	};

	class FnExpr : public Expr {
		std::string name;
		std::function<double(double)> func;
	public:
		std::shared_ptr<Expr> argument;

		FnExpr(const std::string& fname, std::function<double(double)> f, std::shared_ptr<Expr> arg)
			: name(fname), func(f), argument(arg) {
		}

		double eval(double x = 0.0, double y = 0.0) const override {
			return func(argument->eval(x, y));
		}

		std::string repr() const override {
			return name + "(" + argument->repr() + ")";
		}

		std::string deriv() const override {
			std::string arg = argument->repr();
			std::string darg = argument->deriv();

			if (name == "sin")    return "cos(" + arg + ") * " + darg;
			if (name == "cos")    return "-sin(" + arg + ") * " + darg;
			if (name == "tan")    return "(1 + tan(" + arg + ")^2) * " + darg;
			if (name == "asin")   return "(1 / sqrt(1 - " + arg + "^2)) * " + darg;
			if (name == "acos")   return "(-1 / sqrt(1 - " + arg + "^2)) * " + darg;
			if (name == "atan")   return "(1 / (1 + " + arg + "^2)) * " + darg;
			if (name == "sinh")   return "cosh(" + arg + ") * " + darg;
			if (name == "cosh")   return "sinh(" + arg + ") * " + darg;
			if (name == "tanh")   return "(1 - tanh(" + arg + ")^2) * " + darg;
			if (name == "log")    return "(1 / (" + arg + " * ln(10))) * " + darg;
			if (name == "ln")     return "(1 / " + arg + ") * " + darg;
			if (name == "exp")    return "exp(" + arg + ") * " + darg;
			if (name == "abs")    return "(" + arg + " / abs(" + arg + ")) * " + darg;
			if (name == "sqrt")   return "(1 / (2 * sqrt(" + arg + "))) * " + darg;
			if (name == "cbrt")   return "(1 / (3 * cbrt(" + arg + ")^2)) * " + darg;
			if (name == "ceil")   return "0";
			if (name == "floor")  return "0";
			if (name == "round")  return "0";
			if (name == "trunc")  return "0";
			if (name == "sign")   return "0";
			if (name == "min")    return "0";
			if (name == "max")    return "0";
			if (name == "gcd")    return "0";
			if (name == "lcm")    return "0";

			return name + "'(" + arg + ") * " + darg;
		}

		std::shared_ptr<Expr> simplify() const override {
			auto simplifiedArg = argument->simplify();
			return std::make_shared<FnExpr>(name, func, simplifiedArg);
		}
	};

	class FnExpr2 : public Expr {
		std::string name;
		std::function<double(double, double)> func;
	public:
		std::shared_ptr<Expr> arg1, arg2;

		FnExpr2(const std::string& fname, std::function<double(double, double)> f,
			std::shared_ptr<Expr> a1, std::shared_ptr<Expr> a2)
			: name(fname), func(f), arg1(a1), arg2(a2) {
		}

		double eval(double x = 0.0, double y = 0.0) const override {
			return func(arg1->eval(x, y), arg2->eval(x, y));
		}

		std::string repr() const override {
			return name + "(" + arg1->repr() + "," + arg2->repr() + ")";
		}

		std::string deriv() const override {
			return "0";
		}

		std::shared_ptr<Expr> simplify() const override {
			auto s1 = arg1->simplify();
			auto s2 = arg2->simplify();
			return std::make_shared<FnExpr2>(name, func, s1, s2);
		}
	};

	class Operation : public Expr {
	public:
		std::shared_ptr<Expr> left;
		std::shared_ptr<Expr> right;
		Operation(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : left(l), right(r) {}
	};

	class Add : public Operation {
	public:
		Add(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
		double eval(double x = 0.0, double y = 0.0) const override {
			return left->eval(x, y) + right->eval(x, y);
		}
		std::string repr() const override {
			return "(" + left->repr() + " + " + right->repr() + ")";
		}
		std::string deriv() const override {
			return "(" + left->deriv() + " + " + right->deriv() + ")";
		}
		std::shared_ptr<Expr> simplify() const override {
			auto l = left->simplify();
			auto r = right->simplify();

			auto litL = dynamic_cast<Lit*>(l.get());
			auto litR = dynamic_cast<Lit*>(r.get());

			if (litL && litL->eval() == 0) return r;
			if (litR && litR->eval() == 0) return l;

			if (litL && litR) return std::make_shared<Lit>(litL->eval() + litR->eval());

			return std::make_shared<Add>(l, r);
		}
	};

	class Sub : public Operation {
	public:
		Sub(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
		double eval(double x = 0.0, double y = 0.0) const override {
			return left->eval(x, y) - right->eval(x, y);
		}
		std::string repr() const override {
			return "(" + left->repr() + " - " + right->repr() + ")";
		}
		std::string deriv() const override {
			return "(" + left->deriv() + " - " + right->deriv() + ")";
		}
		std::shared_ptr<Expr> simplify() const override {
			auto l = left->simplify();
			auto r = right->simplify();
			auto litL = dynamic_cast<Lit*>(l.get());
			auto litR = dynamic_cast<Lit*>(r.get());

			if (litR && litR->eval() == 0) return l;

			if (litL && litR)
				return std::make_shared<Lit>(litL->eval() - litR->eval());

			return std::make_shared<Sub>(l, r);
		}
	};

	class Mul : public Operation {
	public:
		Mul(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
		double eval(double x = 0.0, double y = 0.0) const override {
			return left->eval(x, y) * right->eval(x, y);
		}
		std::string repr() const override {
			return "(" + left->repr() + " * " + right->repr() + ")";
		}
		std::string deriv() const override {
			return "(" + left->deriv() + " * " + right->repr() + " + " +
				left->repr() + " * " + right->deriv() + ")";
		}
		std::shared_ptr<Expr> simplify() const override {
			auto l = left->simplify();
			auto r = right->simplify();

			if (auto litL = dynamic_cast<Lit*>(l.get()); litL) {
				if (litL->eval() == 0) return std::make_shared<Lit>(0);
				if (litL->eval() == 1) return r;
			}

			if (auto litR = dynamic_cast<Lit*>(r.get()); litR) {
				if (litR->eval() == 0) return std::make_shared<Lit>(0);
				if (litR->eval() == 1) return l;
			}

			if (auto litL = dynamic_cast<Lit*>(l.get()), litR = dynamic_cast<Lit*>(r.get()); litL && litR)
				return std::make_shared<Lit>(litL->eval() * litR->eval());

			return std::make_shared<Mul>(l, r);
		}
	};

	class Div : public Operation {
	public:
		Div(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
		double eval(double x = 0.0, double y = 0.0) const override {
			return left->eval(x, y) / right->eval(x, y);
		}
		std::string repr() const override {
			return "(" + left->repr() + " / " + right->repr() + ")";
		}
		std::string deriv() const override {
			return "((" + left->deriv() + " * " + right->repr() + " - " +
				left->repr() + " * " + right->deriv() + ") / (" +
				right->repr() + " ^ 2))";
		}
		std::shared_ptr<Expr> simplify() const override {
			auto l = left->simplify();
			auto r = right->simplify();

			if (auto litL = dynamic_cast<Lit*>(l.get()); litL && litL->eval() == 0)
				return std::make_shared<Lit>(0);

			if (auto litR = dynamic_cast<Lit*>(r.get()); litR && litR->eval() == 1)
				return l;

			if (auto litL = dynamic_cast<Lit*>(l.get()), litR = dynamic_cast<Lit*>(r.get()); litL && litR)
				return std::make_shared<Lit>(litL->eval() / litR->eval());

			return std::make_shared<Div>(l, r);
		}
	};

	class Pow : public Operation {
	public:
		Pow(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
		double eval(double x = 0.0, double y = 0.0) const override {
			return std::pow(left->eval(x, y), right->eval(x, y));
		}
		std::string repr() const override {
			return "(" + left->repr() + " ^ " + right->repr() + ")";
		}
		std::string deriv() const override {
			auto leftLit = dynamic_cast<Lit*>(left.get());
			auto rightLit = dynamic_cast<Lit*>(right.get());
			auto leftVar = dynamic_cast<Var*>(left.get());

			if (leftVar && rightLit) {
				double n = rightLit->eval();
				return std::to_string(n) + " * x ^ " + std::to_string(n - 1);
			}

			if (leftLit && !rightLit) {
				std::string base = left->repr();
				std::string expo = right->repr();
				std::string dexpo = right->deriv();
				return "ln(" + base + ") * " + base + "^(" + expo + ") * " + dexpo;
			}

			if (rightLit && !leftLit) {
				double c = rightLit->eval();
				std::string fx = left->repr();
				std::string dfx = left->deriv();
				return std::to_string(c) + " * " + fx + " ^ " + std::to_string(c - 1) + " * " + dfx;
			}

			std::string fx = left->repr();
			std::string gx = right->repr();
			std::string dfx = left->deriv();
			std::string dgx = right->deriv();

			return "(" + repr() + ") * (" + dgx + " * ln(" + fx + ") + " + gx + " * " + dfx + " / " + fx + ")";
		}
		std::shared_ptr<Expr> simplify() const override {
			auto l = left->simplify();
			auto r = right->simplify();

			if (auto litR = dynamic_cast<Lit*>(r.get()); litR) {
				double exp = litR->eval();
				if (exp == 0) return std::make_shared<Lit>(1);
				if (exp == 1) return l;
			}

			if (auto litL = dynamic_cast<Lit*>(l.get()), litR = dynamic_cast<Lit*>(r.get()); litL && litR)
				return std::make_shared<Lit>(std::pow(litL->eval(), litR->eval()));

			return std::make_shared<Pow>(l, r);
		}
	};

	class Mod : public Expr {
	public:
		std::shared_ptr<Expr> expr;
		Mod(std::shared_ptr<Expr> e) : expr(e) {}
		double eval(double x = 0.0, double y = 0.0) const override {
			return std::fabs(expr->eval(x, y));
		}
		std::string repr() const override {
			return "|" + expr->repr() + "|";
		}
		std::string deriv() const override {
			std::string fx = expr->repr();
			std::string dfx = expr->deriv();
			return "((" + fx + " / |" + fx + "|) * " + dfx + ")";
		}
		std::shared_ptr<Expr> simplify() const override {
			auto simplifiedExpr = expr->simplify();

			if (auto lit = dynamic_cast<Lit*>(simplifiedExpr.get()))
				return std::make_shared<Lit>(std::fabs(lit->eval()));

			return std::make_shared<Mod>(simplifiedExpr);
		}
	};

	class Parser {
		std::string str;
		int pos;

		int getPrecedence(char op) {
			switch (op) {
			case '+': case '-': return 1;
			case '*': case '/': return 2;
			case '^': return 3;
			default: return -1;
			}
		}

		void skipWhitespace() {
			while (pos < str.size() && std::isspace(str[pos])) pos++;
		}

		std::shared_ptr<Expr> parseExpression(int precedence = 0) {
			auto left = parsePrimary();
			skipWhitespace();

			while (pos < str.size()) {
				char op = str[pos];
				int currentPrecedence = getPrecedence(op);

				if (currentPrecedence < precedence) break;

				pos++;
				auto right = parseExpression(currentPrecedence + 1);

				if (op == '+') left = std::make_shared<Add>(left, right);
				else if (op == '-') left = std::make_shared<Sub>(left, right);
				else if (op == '*') left = std::make_shared<Mul>(left, right);
				else if (op == '/') left = std::make_shared<Div>(left, right);
				else if (op == '^') left = std::make_shared<Pow>(left, right);
			}

			return left;
		}

		std::shared_ptr<Expr> parsePrimary() {
			skipWhitespace();
			if (pos >= str.size()) {
				throwLoc("Unexpected end of input: '" + str + "'");
			}

			if (str[pos] == '-') {
				pos++;
				return std::make_shared<Mul>(std::make_shared<Lit>(-1), parsePrimary());
			}

			if (str[pos] == '|') {
				pos++;
				std::string innerStr;
				int depth = 1;
				while (pos < str.size() && depth > 0) {
					if (str[pos] == '|') {
						depth--;
						if (depth == 0) break;
					}
					innerStr += str[pos];
					pos++;
				}
				if (depth != 0) throwLoc("Unmatched '|'");

				Parser innerParser(innerStr);
				innerParser.Parse();
				return std::make_shared<Mod>(innerParser.ast.back());
			}

			if (str[pos] == '(') {
				pos++;
				auto expr = parseExpression();
				if (pos >= str.size() || str[pos] != ')')
					throwLoc("Expected ')'");
				pos++;
				return expr;
			}

			if (std::isdigit(str[pos]) || str[pos] == '.') {
				int start = pos;
				while (pos < str.size() && (std::isdigit(str[pos]) || str[pos] == '.'))
					pos++;
				return std::make_shared<Lit>(std::stod(str.substr(start, pos - start)));
			}

			if (str.compare(pos, 5, "d/dx(") == 0) {
				pos += 5;
				std::string inner;
				int depth = 1;
				while (pos < str.size() && depth > 0) {
					if (str[pos] == '(') depth++;
					else if (str[pos] == ')') depth--;
					if (depth > 0) inner += str[pos];
					pos++;
				}
				if (depth != 0) throwLoc("Unmatched parentheses in d/dx");

				Parser innerParser(inner);
				innerParser.Parse();
				std::string derived = innerParser.ast.back()->deriv();

				Parser dParser(derived);
				dParser.Parse();
				return dParser.ast.back();
			}

			if (std::isalpha(str[pos])) {
				std::string id;
				while (pos < str.size() &&
					(std::isalnum(str[pos]) || str[pos] == '_'))
					id += str[pos++];

				if (externalVars &&
					externalVars->find(id) != externalVars->end())
				{
					return std::make_shared<Lit>(
						(*externalVars)[id]
					);
				}


				if (id == "x" || id == "y")
					return std::make_shared<Var>(id);

				if (constants.find(id) != constants.end())
					return std::make_shared<Lit>(constants[id]);

				if (funcs.find(id) != funcs.end()) {
					if (pos >= str.size() || str[pos] != '(')
						throwLoc("Expected '('");

					pos++;
					auto arg = parseExpression();

					if (str[pos] != ')')
						throwLoc("Expected ')'");

					pos++;

					return std::make_shared<FnExpr>(
						id, funcs.at(id), arg
					);
				}

				throwLoc("Unknown identifier: " + id);
			}


			throwLoc("Unexpected character: '" + std::string(1, str[pos]) + "'");
		}

	public:

		std::unordered_map<std::string, double>* externalVars;

		std::vector<std::shared_ptr<Expr>> ast;

		Parser(std::string input,
			std::unordered_map<std::string, double>* vars = nullptr)
			: str(std::move(input)), pos(0), externalVars(vars)
		{
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		}


		void Parse() {
			ast.clear();
			pos = 0;
			auto expr = parseExpression();
			ast.push_back(expr);
		}
		bool usesVar(const std::string& var) const {
			if (ast.empty()) return false;

			std::function<bool(std::shared_ptr<Expr>)> check;

			check = [&](std::shared_ptr<Expr> node) -> bool {

				if (!node) return false;

				if (auto v = dynamic_cast<Var*>(node.get()))
					return v->repr() == var;

				if (auto fn = dynamic_cast<FnExpr*>(node.get()))
					return check(fn->argument);

				if (auto fn2 = dynamic_cast<FnExpr2*>(node.get()))
					return check(fn2->arg1) || check(fn2->arg2);

				if (auto op = dynamic_cast<Operation*>(node.get()))
					return check(op->left) || check(op->right);

				return false;
				};

			return check(ast.back());
		}

		double eval(double x = 0.0, double y = 0.0) const {
			if (ast.empty()) throwLoc("Empty AST");
			return ast.back()->eval(x, y);
		}

		std::function<double(double)> toFunction() const {
			if (ast.empty()) throwLoc("Empty AST");
			auto expr = ast.back();
			return [expr](double x) { return expr->eval(x, 0.0); };
		}

		std::function<double(double, double)> toFunction2D() const {
			if (ast.empty()) throwLoc("Empty AST");
			auto expr = ast.back();
			return [expr](double x, double y) { return expr->eval(x, y); };
		}

		bool isFunctionExpr() const {
			if (ast.empty()) return false;

			std::function<bool(std::shared_ptr<Expr>)> checkForVariables;
			checkForVariables = [&](std::shared_ptr<Expr> node) -> bool {
				if (!node) return false;

				if (dynamic_cast<Var*>(node.get()))
					return true;

				if (auto fn = dynamic_cast<FnExpr*>(node.get()))
					return checkForVariables(fn->argument);

				if (auto fn2 = dynamic_cast<FnExpr2*>(node.get()))
					return checkForVariables(fn2->arg1) || checkForVariables(fn2->arg2);

				if (auto op = dynamic_cast<Operation*>(node.get()))
					return checkForVariables(op->left) || checkForVariables(op->right);

				if (auto mod = dynamic_cast<Mod*>(node.get()))
					return checkForVariables(mod->expr);

				return false;
				};

			return checkForVariables(ast.back());
		}
	};
#pragma endregion


#pragma region Complex

	class CExpr {
	public:
		virtual complex eval(const std::unordered_map<std::string, complex>& vars) const = 0;
		virtual std::string toGLSL() const = 0;
		virtual ~CExpr() = default;
	};


	class CLit : public CExpr {
		complex value;
	public:
		CLit(complex v) : value(v) {}

		complex eval(const std::unordered_map<std::string, complex>&) const override {
			return value;
		}

		std::string toGLSL() const override {
			std::ostringstream os;
			os << "vec2(" << value.a << "," << value.b << ")";
			return os.str();
		}
	};


	class CVar : public CExpr {
		std::string name;
	public:
		CVar(std::string n) : name(std::move(n)) {}

		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			auto it = vars.find(name);
			if (it == vars.end())
				throwLoc("Undefined complex variable: " + name);
			return it->second;
		}

		std::string toGLSL() const override {
			if (name == "z") return "z";
			if (name == "i") return "vec2(0.0,1.0)";
			return name;
		}
	};



	class CFunc : public CExpr {
	public:
		std::string name;
		std::function<complex(complex)> func;
		std::shared_ptr<CExpr> arg;

		CFunc(const std::string& n,
			std::function<complex(complex)> f,
			std::shared_ptr<CExpr> a)
			: name(n), func(f), arg(a) {
		}

		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return func(arg->eval(vars));
		}

		std::string toGLSL() const override {
			static const std::unordered_map<std::string, std::string> map1 = {
				{"Csin","c_sin"},
				{"Ccos","c_cos"},
				{"Ctan","c_tan"},
				{"Csinh","c_sinh"},
				{"Ccosh","c_cosh"},
				{"Ctanh","c_tanh"},
				{"Cexp","c_exp"},
				{"Clog","c_log"},
				{"Csqrt","c_sqrt"},
				{"Cinvert","c_inv"},
				{"Cconj","c_conj"},
				{"Cabs","c_abs_vec"},
				{"Carg","c_arg_vec"},
				{"Cre","c_re_vec"},
				{"Cim","c_im_vec"},
				{"Cfloor","c_floor"},
				{"Cceil","c_ceil"},
				{"Csec","c_sec"},
				{"Ccsc","c_csc"},
				{"Ccot","c_cot"},
				{"Csech","c_sech"},
				{"Ccsch","c_csch"},
				{"Ccoth","c_coth"},
				{"Carcsin","c_arcsin"},
				{"Carccos","c_arccos"},
				{"Carctan","c_arctan"},
				{"Carcsinh","c_arcsinh"},
				{"Carccosh","c_arccosh"},
				{"Carctanh","c_arctanh"},
				{"Carcsec","c_arcsec"},
				{"Carccsc","c_arccsc"},
				{"Carccot","c_arccot"},
				{"Carcsech","c_arcsech"},
				{"Carccsch","c_arccsch"},
				{"Carccoth","c_arccoth"},
				{"Cgamma","c_gamma"},
				{"Czeta","c_zeta"},
				{"Cphi","c_phi"},
				{"Ctheta","c_theta"},
				{"Cjoukowsky","c_joukowsky"},
				{"Cdisk","c_disk"},
				{"Cmandelbrot","c_mandelbrot"},
				{"Cmap","c_map"},
				{"Ctransform","c_transform"}
			};

			auto it = map1.find(name);
			if (it == map1.end())
				throwLoc("No GLSL mapping for complex function: " + name);

			return it->second + "(" + arg->toGLSL() + ")";
		}
	};

	class CBinary : public CExpr {
	public:
		std::shared_ptr<CExpr> left, right;

		CBinary(std::shared_ptr<CExpr> l,
			std::shared_ptr<CExpr> r)
			: left(l), right(r) {
		}
	};

	class CAdd : public CBinary {
	public:
		using CBinary::CBinary;
		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return left->eval(vars) + right->eval(vars);
		}
		std::string toGLSL() const override {
			return "c_add(" + left->toGLSL() + "," + right->toGLSL() + ")";
		}
	};

	class CSub : public CBinary {
	public:
		using CBinary::CBinary;
		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return left->eval(vars) - right->eval(vars);
		}
		std::string toGLSL() const override {
			return "c_sub(" + left->toGLSL() + "," + right->toGLSL() + ")";
		}
	};

	class CMul : public CBinary {
	public:
		using CBinary::CBinary;
		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return left->eval(vars) * right->eval(vars);
		}
		std::string toGLSL() const override {
			return "c_mul(" + left->toGLSL() + "," + right->toGLSL() + ")";
		}
	};

	class CDiv : public CBinary {
	public:
		using CBinary::CBinary;
		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return left->eval(vars) / right->eval(vars);
		}
		std::string toGLSL() const override {
			return "c_div(" + left->toGLSL() + "," + right->toGLSL() + ")";
		}
	};

	class CPow : public CBinary {
	public:
		using CBinary::CBinary;
		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return funcsZ2["Cpow"](left->eval(vars), right->eval(vars));
		}
		std::string toGLSL() const override {
			return "c_pow(" + left->toGLSL() + "," + right->toGLSL() + ")";
		}
	};

	class CFunc2 : public CExpr {
	public:
		std::string name;
		std::function<complex(complex, complex)> func;
		std::shared_ptr<CExpr> arg1, arg2;

		CFunc2(const std::string& n,
			std::function<complex(complex, complex)> f,
			std::shared_ptr<CExpr> a,
			std::shared_ptr<CExpr> b)
			: name(n), func(f), arg1(a), arg2(b) {
		}

		complex eval(const std::unordered_map<std::string, complex>& vars) const override {
			return func(arg1->eval(vars), arg2->eval(vars));
		}

		std::string toGLSL() const override {
			static const std::unordered_map<std::string, std::string> map2 = {
				{"Cadd", "c_add"},
				{"Csub", "c_sub"},
				{"Cmul", "c_mul"},
				{"Cdiv", "c_div"},
				{"Cpow", "c_pow"}
			};

			auto it = map2.find(name);
			if (it == map2.end())
				throwLoc("No GLSL mapping for binary complex function: " + name);

			return it->second + "(" + arg1->toGLSL() + "," + arg2->toGLSL() + ")";
		}
	};

	class ComplexParser {
		std::string str;
		int pos = 0;
		std::unordered_map<std::string, complex>* externalVars;

		void skipWhitespace() {
			while (pos < str.size() && std::isspace(str[pos]))
				pos++;
		}

		int getPrec(char op) {
			if (op == '+' || op == '-') return 1;
			if (op == '*' || op == '/') return 2;
			if (op == '^') return 3;
			return -1;
		}

		std::shared_ptr<CExpr> parseExpression(int prec = 0) {
			auto left = parsePrimary();
			skipWhitespace();

			while (true) {
				skipWhitespace();
				if (pos >= str.size()) break;

				if (pos >= str.size())
					break;

				char op = str[pos];
				int p = getPrec(op);

				if (p < prec)
					break;

				pos++;
				auto right = parseExpression(p + 1);

				if (op == '+') left = std::make_shared<CAdd>(left, right);
				else if (op == '-') left = std::make_shared<CSub>(left, right);
				else if (op == '*') left = std::make_shared<CMul>(left, right);
				else if (op == '/') left = std::make_shared<CDiv>(left, right);
				else if (op == '^') left = std::make_shared<CPow>(left, right);
			}

			return left;
		}


		std::shared_ptr<CExpr> parsePrimary() {
			skipWhitespace();

			if (pos >= str.size())
				throwLoc("Unexpected end");

			if (str[pos] == '-') {
				pos++;
				return std::make_shared<CMul>(
					std::make_shared<CLit>(complex(-1, 0)),
					parsePrimary()
				);
			}

			if (str[pos] == '(') {
				pos++;
				auto expr = parseExpression();
				skipWhitespace();

				if (pos >= str.size() || str[pos] != ')')
					throwLoc("Expected )");

				pos++;
				return expr;
			}

			if (std::isdigit(str[pos]) || str[pos] == '.') {
				int start = pos;
				while (pos < str.size() &&
					(std::isdigit(str[pos]) || str[pos] == '.'))
					pos++;

				double val = std::stod(str.substr(start, pos - start));
				return std::make_shared<CLit>(complex(val, 0));
			}

			if (std::isalpha(str[pos])) {
				std::string name;
				while (pos < str.size() &&
					(std::isalnum(str[pos]) || str[pos] == '_'))
					name += str[pos++];

				if (name == "i")
					return std::make_shared<CLit>(complex(0, 1));

				if (pos < str.size() && str[pos] == '(') {
					pos++;
					auto arg1 = parseExpression();
					skipWhitespace();

					if (pos < str.size() && str[pos] == ',') {
						pos++;
						auto arg2 = parseExpression();
						skipWhitespace();

						if (pos >= str.size() || str[pos] != ')')
							throwLoc("Expected )");

						pos++;

						if (funcsZ2.find(name) == funcsZ2.end())
							throwLoc("Unknown complex binary function: " + name);

						return std::make_shared<CFunc2>(name, funcsZ2[name], arg1, arg2);
					}

					if (pos >= str.size() || str[pos] != ')')
						throwLoc("Expected )");

					pos++;

					if (funcsZ.find(name) == funcsZ.end())
						throwLoc("Unknown complex function: " + name);

					return std::make_shared<CFunc>(name, funcsZ[name], arg1);
				}

				return std::make_shared<CVar>(name);
			}

			throwLoc("Invalid complex expression");
		}



	public:
		ComplexParser(std::string s,
			std::unordered_map<std::string, complex>* vars = nullptr)
			: str(std::move(s)), externalVars(vars) {
		}
		std::shared_ptr<CExpr> root;



		std::shared_ptr<CExpr> parse() {
			pos = 0;
			root = parseExpression();

			skipWhitespace();
			if (pos != str.size())
				throwLoc("Unexpected token at end");

			return root;
		}

		bool usesComplexVar() const {

			if (!root) return false;

			std::function<bool(std::shared_ptr<CExpr>)> check;

			check = [&](std::shared_ptr<CExpr> node) -> bool {

				if (!node) return false;

				if (dynamic_cast<CVar*>(node.get()))
					return true;

				if (auto b = dynamic_cast<CBinary*>(node.get()))
					return check(b->left) || check(b->right);

				if (auto f = dynamic_cast<CFunc*>(node.get()))
					return check(f->arg);

				return false;
				};

			return check(root);
		}


	};

#pragma endregion


}



static std::unordered_map<std::string, double> realVars;
static std::unordered_map<std::string, complex> complexVars = { {"z",complex(1,-1)} };

inline std::string repl(const std::string& input)
{

	try
	{
		std::string expr = input;
		expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());

		if (expr == "help")
		{
			std::ostringstream os;

			os << "===== REAL CONSTANTS =====\n";
			for (auto& [k, v] : parser::constants)
				os << k << " = " << v << "\n";

			os << "\n===== REAL FUNCTIONS (1 arg) =====\n";
			for (auto& [k, _] : parser::funcs)
				os << k << "(x)\n";

			os << "\n===== REAL FUNCTIONS (2 args) =====\n";
			for (auto& [k, _] : parser::funcs2)
				os << k << "(x,y)\n";

			os << "\n===== COMPLEX FUNCTIONS (1 arg) =====\n";
			for (auto& [k, _] : parser::funcsZ)
				os << k << "(z)\n";

			os << "\n===== COMPLEX FUNCTIONS (2 args) =====\n";
			for (auto& [k, _] : parser::funcsZ2)
				os << k << "(z,w)\n";

			os << "\n===== USER REAL VARIABLES =====\n";
			for (auto& [k, v] : realVars)
				os << k << " = " << v << "\n";

			os << "\n===== USER COMPLEX VARIABLES =====\n";
			for (auto& [k, v] : complexVars)
				os << k << " = " << v << "\n";

			return os.str();
		}


		auto eq = expr.find('=');
		if (eq != std::string::npos)
		{
			std::string name = expr.substr(0, eq);
			std::string rhs = expr.substr(eq + 1);

			try {
				parser::ComplexParser cp(rhs, &complexVars);
				auto ast = cp.parse();
				complex val = ast->eval(complexVars);

				if (std::abs(val.b) < 1e-12)
				{
					realVars[name] = val.a;
					return std::to_string(val.a);
				}

				complexVars[name] = val;

				std::ostringstream os;
				os << val;
				return os.str();
			}
			catch (...) {}

			parser::Parser p(rhs, &realVars);
			p.Parse();
			double val = p.eval();

			realVars[name] = val;
			return std::to_string(val);
		}


		auto contains = [&](const std::string& s)
			{
				return expr.find(s) != std::string::npos;
			};
		std::regex imagRegex(R"(\bi\b)");
		std::regex zRegex(R"(\bz\b)");

		bool hasI = std::regex_search(expr, imagRegex);
		bool hasZ = std::regex_search(expr, zRegex);

		bool hasComplexFunc = false;

		for (const auto& [name, _] : parser::funcsZ)
		{
			if (contains(name))
			{
				hasComplexFunc = true;
				break;
			}
		}

		if (!hasComplexFunc)
		{
			for (const auto& [name, _] : parser::funcsZ2)
			{
				if (contains(name))
				{
					hasComplexFunc = true;
					break;
				}
			}
		}

		bool mustBeComplex =
			contains("C") 
			|| hasComplexFunc
			|| hasI
			|| hasZ;


		if (mustBeComplex)
		{
			parser::ComplexParser cp(expr, &complexVars);
			auto ast = cp.parse();

			complex result = ast->eval(complexVars);

			if (std::abs(result.b) < 1e-12)
				return std::to_string(result.a);


			std::ostringstream os;

			if (cp.usesComplexVar())
			{
				if (true)
					os << "[Funcao complexa f(z)]\n";
			}

			os << result;
			return os.str();
		}




		parser::Parser p(expr, &realVars);
		p.Parse();

		if (p.isFunctionExpr())
		{
			bool usesX = expr.find('x') != std::string::npos;
			bool usesY = expr.find('y') != std::string::npos;

			if (usesX && usesY)
				return "[Funcao real f(x,y)]";

			if (usesX)
				return "[Funcao real f(x)]";
		}

		double result = p.eval();
		return std::to_string(result);
	}
	catch (std::exception& e)
	{
		return std::string("Erro: ") + e.what();
	}
}


