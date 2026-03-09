#pragma once
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <sstream>
#include "plotter.h"
#include "Parser.h"
#include "proposicional.h"

inline std::string join(const std::vector<std::string>& v, const std::string& delim) {
    std::ostringstream os;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i != 0) os << delim;
        os << v[i];
    }
    return os.str();
}


inline std::string replJs(std::string usrline) {
    static std::string mode = "math";
    static cartesianR2::vec2d domain = { -35.f, 35.f };
    static cartesianR2::vec2d domain2 = { -10.f, 10.f };

    std::ostringstream out;
    std::string line = usrline;

    if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
        line.pop_back();

    if (line.empty()) return "";
    if (line == "exit" || line == "sair") return "__EXIT__";

    if (line == "math" || line == "logic") {
        mode = line;
        return "Modo alterado para: " + mode;
    }

    if (line == "help")
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
        os << "sin(z) / Csin(z)\n";
        os << "cos(z) / Ccos(z)\n";
        os << "tan(z) / Ctan(z)\n";
        os << "sinh(z) / Csinh(z)\n";
        os << "cosh(z) / Ccosh(z)\n";
        os << "tanh(z) / Ctanh(z)\n";
        os << "exp(z) / Cexp(z)\n";
        os << "log(z) / Clog(z)\n";
        os << "sqrt(z) / Csqrt(z)\n";
        os << "invert(z) / Cinvert(z)\n";
        os << "conj(z) / Cconj(z)\n";
        os << "abs(z) / Cabs(z)\n";
        os << "arg(z) / Carg(z)\n";
        os << "re(z) / Cre(z)\n";
        os << "im(z) / Cim(z)\n";
        os << "floor(z) / Cfloor(z)\n";
        os << "ceil(z) / Cceil(z)\n";
        os << "sec(z) / Csec(z)\n";
        os << "csc(z) / Ccsc(z)\n";
        os << "cot(z) / Ccot(z)\n";
        os << "sech(z) / Csech(z)\n";
        os << "csch(z) / Ccsch(z)\n";
        os << "coth(z) / Ccoth(z)\n";
        os << "arcsin(z) / Carcsin(z)\n";
        os << "arccos(z) / Carccos(z)\n";
        os << "arctan(z) / Carctan(z)\n";
        os << "arcsinh(z) / Carcsinh(z)\n";
        os << "arccosh(z) / Carccosh(z)\n";
        os << "arctanh(z) / Carctanh(z)\n";
        os << "arcsec(z) / Carcsec(z)\n";
        os << "arccsc(z) / Carccsc(z)\n";
        os << "arccot(z) / Carccot(z)\n";
        os << "arcsech(z) / Carcsech(z)\n";
        os << "arccsch(z) / Carccsch(z)\n";
        os << "arccoth(z) / Carccoth(z)\n";
        os << "gamma(z) / Cgamma(z)\n";
        os << "zeta(z) / Czeta(z)\n";
        os << "phi(z) / Cphi(z)\n";
        os << "theta(z) / Ctheta(z)\n";
        os << "joukowsky(z) / Cjoukowsky(z)\n";
        os << "disk(z) / Cdisk(z)\n";
        os << "mandelbrot(z) / Cmandelbrot(z)\n";
        os << "map(z) / Cmap(z)\n";
        os << "transform(z) / Ctransform(z)\n";

        os << "\n===== COMPLEX FUNCTIONS (2 args) =====\n";
        os << "add(z,w) / Cadd(z,w)\n";
        os << "sub(z,w) / Csub(z,w)\n";
        os << "mul(z,w) / Cmul(z,w)\n";
        os << "div(z,w) / Cdiv(z,w)\n";
        os << "pow(z,w) / Cpow(z,w)\n";

        os << "\n===== USER REAL VARIABLES =====\n";
        for (auto& [k, v] : realVars)
            os << k << " = " << v << "\n";

        os << "\n===== USER COMPLEX VARIABLES =====\n";
        for (auto& [k, v] : complexVars)
            os << k << " = " << v << "\n";

        os << "\n===== EXAMPLES =====\n";
        os << "domaincolor (z-1)/(z+1)\n";
        os << "domaincolor z^3\n";
        os << "domaincolor sin(z)\n";
        os << "domaincolor exp(1/z)\n";
        os << "domaincolor gamma(z)\n";
        os << "domaincolor zeta(z)\n";
        os << "domaincolor joukowsky(z)\n";
        os << "domaincolor mandelbrot(z)\n";

        return os.str();
    }

    if (line.rfind("const ", 0) == 0) {
        std::istringstream iss(line.substr(6));
        std::string name, eq; double value;
        iss >> name >> eq >> value;
        if (eq == "=" && !name.empty() && iss) {
            parser::addConstant(name, value);
            return "Constante '" + name + "' = " + std::to_string(value);
        }
        return "Sintaxe incorreta. Use: const nome = valor";
    }

    if (line.rfind("domain", 0) == 0) {
        std::istringstream iss(line);
        std::string cmd, eq; float min, max;
        iss >> cmd >> eq >> min >> max;
        if (cmd == "domain" && eq == "=" && iss) {
            domain = { min, max };
            return "Domínio = [" + std::to_string(min) + "," + std::to_string(max) + "]";
        }
        return "Sintaxe incorreta. Use: domain = min max";
    }

    try {
        if (mode == "logic") {
            logica::Parser p(line);
            p.parse();
            std::ostringstream buf; p.eval(buf);
            return buf.str();
        }

        if (line.rfind("contour ", 0) == 0) {
            std::string exprStr = line.substr(8);
            exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());

            parser::Parser p(exprStr);
            p.Parse();

            if (cartesianR2::usesTwoVariables(exprStr)) {
                auto func2d = p.toFunction2D();

                std::ostringstream js;
                js << "{ \"contour\": " << cartesianR2::contourMap_js(func2d, domain, domain, 50) << "}";
                return js.str();
            }
            else {
                return "Expressão não parece usar duas variáveis (x e y)";
            }
        }

        if (line.rfind("surface ", 0) == 0) {
            std::string exprStr = line.substr(8);
            exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());

            parser::Parser p(exprStr);
            p.Parse();

            if (cartesianR2::usesTwoVariables(exprStr)) {
                auto func2d = p.toFunction2D();

                std::ostringstream js;
                js << "{ \"surface3D\": " << cartesianR2::plotter_js3d(func2d, domain, 50) << "}";
                return js.str();
            }
            else {
                return "Expressão não parece usar duas variáveis (x e y)";
            }
        }

        if (line.rfind("deriv ", 0) == 0) {
            std::string exprStr = line.substr(6);
            exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());
            double delta = 0.0; bool usedelta = false;
            auto atPos = exprStr.find("at");
            if (atPos != std::string::npos) {
                delta = std::stod(exprStr.substr(atPos + 2));
                exprStr = exprStr.substr(0, atPos);
                usedelta = true;
            }
            parser::Parser parser(exprStr); parser.Parse();
            if (usedelta) {
                auto f = parser.toFunction();
                return std::to_string(cartesianR2::utils::derivada(f, delta));
            }
            return parser.ast.back()->deriv();
        }

        if (line.rfind("integ", 0) == 0) {
            std::string exprStr = line.substr(6);
            exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());
            auto fromPos = exprStr.find("from");
            auto toPos = exprStr.find("to");
            if (fromPos == std::string::npos || toPos == std::string::npos || toPos < fromPos)
                return "Formato invalido. Use: integ <expr> from a to b";
            std::string expr = exprStr.substr(0, fromPos);
            double a = std::stod(exprStr.substr(fromPos + 4, toPos - (fromPos + 4)));
            double b = std::stod(exprStr.substr(toPos + 2));
            parser::Parser parser(expr); parser.Parse();
            auto f = parser.toFunction();
            return std::to_string(cartesianR2::utils::integralriemannstyle(f, a, b));
        }

        if (line.rfind("revolution", 0) == 0) {
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            std::string token;
            std::vector<std::string> parts;
            while (iss >> token) {
                if (token == "around") break;
                parts.push_back(token);
            }
            std::string expr = join(parts, " ");

            std::string axis;
            iss >> axis;

            std::string color = "#ffa500b3";
            std::string tail;
            std::getline(iss, tail);
            auto pos = tail.find("color=");
            if (pos != std::string::npos) {
                color = tail.substr(pos + 6);
                color.erase(std::remove_if(color.begin(), color.end(), ::isspace), color.end());
            }
            if (axis != "x" && axis != "y") {
                return "you mean: revolution <expr> around x|y [color=#rrggbbaa]";
            }

            parser::Parser parser(expr);
            parser.Parse();
            auto f = parser.toFunction();

            std::ostringstream js;
            if (axis == "x") {
                js << "{ \"solid3D\": " << cartesianR2::revolution3D(f, domain) << " }";
            }
            else {
                js << "{ \"solid3D\": " << cartesianR2::revolution3D_Y(f, domain) << " }";
            }
            return js.str();

            return js.str();
        }


        if (line.find('=') != std::string::npos) {
            auto eq_pos = line.find('=');
            line = line.substr(eq_pos + 1);
        }
        std::cerr << "DEBUG expr=[" << line << "] (size=" << line.size() << ")\n";
        parser::Parser p(line);
        p.Parse();

        if (cartesianR2::usesTwoVariables(line)) {
            auto func2d = p.toFunction2D();

            std::ostringstream js;
            js << "{ \"is2D\": true, "
                << "\"contour\": " << cartesianR2::contourMap_js(func2d, domain, domain, 50) << ", "
                << "\"surface3D\": " << cartesianR2::plotter_js3d(func2d, domain, 50) << ", "
                << "\"expr\": \"" << line << "\", "
                << "\"domain\": [" << domain.x << "," << domain.y << "]"
                << "}";
            return js.str();
        }
        else if (p.isFunctionExpr()) {
            auto function = p.toFunction();

            std::ostringstream js;
            js << "{ \"plot\": " << cartesianR2::plotter_js(function, domain, 400) << ",";

            std::string simp;
            try {
                parser::Parser derivParser(p.ast.back()->deriv());
                derivParser.Parse();
                simp = derivParser.ast.back()->simplify()->repr();
            }
            catch (...) {
                simp = p.ast.back()->deriv();
            }

            js << "\"analysis\": {"
                << "\"expr\": \"" << line << "\","
                << "\"domain\": [" << domain.x << "," << domain.y << "],"
                << "\"injective\": " << (cartesianR2::utils::B_injectora(function, domain) ? "true" : "false") << ","
                << "\"subjective\": " << (cartesianR2::utils::B_sobrejectora(function, domain) ? "true" : "false") << ","
                << "\"bijectora\": " << (cartesianR2::utils::B_Bijectora(function, domain) ? "true" : "false") << ","
                << "\"impar\": " << (cartesianR2::utils::B_impar(function, domain) ? "true" : "false") << ","
                << "\"par\": " << (cartesianR2::utils::B_par(function, domain) ? "true" : "false") << ","
                << "\"periodica\": " << (cartesianR2::utils::B_periodica(function, domain) ? "true" : "false") << ","
                << "\"derivative_at_0\": " << cartesianR2::utils::derivada(function) << ","
                << "\"derivative_symbolic\": \"" << simp << "\","
                << "\"integral\": " << cartesianR2::utils::integralriemannstyle(function, domain.x, domain.y)
                << "}}";
            return js.str();
        }
        else {
            return std::to_string(p.eval());
        }
    }
    catch (const std::exception& e) {
        return std::string("Erro: ") + e.what();
    }
    catch (...) {
        return "Expressao invalida.";
    }
}

namespace wasm_exports {
    std::string revolution3D_str(const std::string& expr, cartesianR2::vec2d domain) {
        parser::Parser parser(expr);
        parser.Parse();
        auto f = parser.toFunction();
        return cartesianR2::revolution3D(f, domain);
    }
    std::string complex_to_glsl(const std::string& expr) {
        return cartesianR2::complex2GLSL(expr);
    }
    std::string revolution3D_Y_str(const std::string& expr, cartesianR2::vec2d domain) {
        parser::Parser parser(expr);
        parser.Parse();
        auto f = parser.toFunction();
        return cartesianR2::revolution3D_Y(f, domain);
    }

    std::string contourMap_js_str(const std::string& expr, cartesianR2::vec2d x_domain, cartesianR2::vec2d y_domain, int resolution = 50) {
        parser::Parser parser(expr);
        parser.Parse();
        auto f = parser.toFunction2D();
        return cartesianR2::contourMap_js(f, x_domain, y_domain, resolution);
    }

    std::string plotter_js3d_str(const std::string& expr, cartesianR2::vec2d domain, int resolution = 50) {
        parser::Parser parser(expr);
        parser.Parse();
        auto f = parser.toFunction2D();
        return cartesianR2::plotter_js3d(f, domain, resolution);
    }

    std::string plotter_js_str(const std::string& expr, cartesianR2::vec2d domain, int resolution = 400) {
        parser::Parser parser(expr);
        parser.Parse();
        auto f = parser.toFunction();
        return cartesianR2::plotter_js(f, domain, resolution);
    }

    bool usesTwoVariables(const std::string& expr) {
        return cartesianR2::usesTwoVariables(expr);
    }

    double eval2D(const std::string& expr, double x, double y) {
        parser::Parser parser(expr);
        parser.Parse();
        auto f = parser.toFunction2D();
        return f(x, y);
    }

    double derivada(std::function<double(double)> f, double a = 0) {
        return cartesianR2::utils::derivada(f, a);
    }

    double integral(std::function<double(double)> f, double a, double b) {
        return cartesianR2::utils::integralriemannstyle(f, a, b);
    }

    bool injetora(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_injectora(f, d);
    }

    bool par(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_par(f, d);
    }

    bool impar(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_impar(f, d);
    }

    bool periodica(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_periodica(f, d);
    }

    std::string deriv_symbolic(const std::string& expr) {
        parser::Parser p(expr);
        p.Parse();
        return p.ast.back()->deriv();
    }

    double eval_expr(const std::string& expr) {
        parser::Parser p(expr);
        p.Parse();
        return p.eval();
    }

    std::string truth_table(const std::string& expr) {
        logica::Parser p(expr);
        p.parse();
        std::ostringstream buf;
        p.eval(buf);
        return buf.str();
    }
}

EMSCRIPTEN_BINDINGS(Descartes) {
    emscripten::function("repl", &replJs);

    emscripten::value_object<cartesianR2::vec2d>("vec2d")
        .field("x", &cartesianR2::vec2d::x)
        .field("y", &cartesianR2::vec2d::y);

    emscripten::function("derivada", &wasm_exports::derivada);
    emscripten::function("integral", &wasm_exports::integral);
    emscripten::function("injetora", &wasm_exports::injetora);
    emscripten::function("par", &wasm_exports::par);
    emscripten::function("impar", &wasm_exports::impar);
    emscripten::function("periodica", &wasm_exports::periodica);
    emscripten::function("revolution3D", &cartesianR2::revolution3D);
    emscripten::function("evalExpr", &wasm_exports::eval_expr);
    emscripten::function("derivSymbolic", &wasm_exports::deriv_symbolic);
    emscripten::function("truthTable", &wasm_exports::truth_table);
    emscripten::function("complexToGLSL", &wasm_exports::complex_to_glsl);
    emscripten::function("revolution3D_str", &wasm_exports::revolution3D_str);
    emscripten::function("revolution3D_Y_str", &wasm_exports::revolution3D_Y_str);
 
    emscripten::function("contourMap", &wasm_exports::contourMap_js_str);
    emscripten::function("plot3D", &wasm_exports::plotter_js3d_str);
    emscripten::function("plot2D", &wasm_exports::plotter_js_str);
    emscripten::function("usesTwoVariables", &wasm_exports::usesTwoVariables);
    emscripten::function("eval2D", &wasm_exports::eval2D);
}

/*
em++ wasmLib.cpp -O3 -std=c++20 --bind -fexceptions -sNO_DISABLE_EXCEPTION_CATCHING -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=Descartes -s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -o wasmLib.js
*/
