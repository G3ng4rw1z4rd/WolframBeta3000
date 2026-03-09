#include <iostream>
#include <sstream>
#include <map>
#include "plotter.h"
#include "Parser.h"
#include "proposicional.h"
int main() {
    std::string mode = "math";
    std::string line;

    cartesianR2::vec2d domain = { -35.f, 35.f };
    cartesianR2::vec2d domain2 = { -10.f, 10.f };

    std::cout << "Modo inicial: math\n";
    std::cout << "Digite 'help' para ver os comandos.\n";

    while (true) {
        std::cout << "~[" << mode << "]> ";
        std::getline(std::cin, line);

        if (line.empty()) continue;
        if (line == "exit" || line == "sair") break;

        if (line == "math" || line == "logic") {
            mode = line;
            std::cout << "Modo alterado para: " << mode << "\n";
            continue;
        }

        if (line == "help") {
            std::cout << "\nComandos disponivel:\n";
            std::cout << "  math                            - Muda para modo matemático\n";
            std::cout << "  logic                           - Muda para modo lógico\n";
            std::cout << "  const <k> = <valor>             - Define constante matemática (ex: const k = 2.5)\n";
            std::cout << "  domain = <min> <max>            - Define domínio para análise (ex: domain = -10 10)\n";
            std::cout << "  deriv <expr>                    - Deriva uma expressão simbolicamente (ex: deriv x^2)\n";
            std::cout << "  deriv <expr> at <v>             - Calcula valor da derivada em x = v (ex: deriv sin(x) at 0)\n";
            std::cout << "  integ <expr>                    - Integra uma expressão simbolicamente (ex: integ x^2)\n";
            std::cout << "  integ <expr> from <v> to <v2>   - Calcula valor da integral em a,b = v v1 (ex: integ sin(x) at 0)\n";
            std::cout << "  contour <expr>           - Gera mapa de contorno para função 2D (ex: contour x^2+y^2)\n";
            std::cout << "  surface <expr>           - Gera superfície 3D para função 2D (ex: surface x^2+y^2)\n";
            std::cout << "  sair / exit / x       - Sai do programa\n\n";

            std::cout << "Constantes matemáticas pré-definidas:\n";
            std::cout << "  pi     ≈ 3.14159\n";
            std::cout << "  e      ≈ 2.71828\n";
            std::cout << "  tau    = 2*pi\n";
            std::cout << "  phi    ≈ 1.61803\n";
            std::cout << "  c      = 299792458 (velocidade da luz)\n";
            std::cout << "  g      = 9.80665 (gravidade)\n";
            std::cout << "  h      = 6.62607015e-34 (Planck)\n";
            std::cout << "  k      = 5 (padrão, pode ser sobrescrito)\n\n";

            std::cout << "Funções matemáticas disponivel:\n";
            std::cout << "  Trigonométricas:      sin, cos, tan, asin, acos, atan\n";
            std::cout << "  Hiperbólicas:         sinh, cosh, tanh\n";
            std::cout << "  Logarítmicas:         log (base 10), ln (natural)\n";
            std::cout << "  Exponenciais:         exp\n";
            std::cout << "  Outros utilitários:   abs, sqrt, cbrt, ceil, floor, round, trunc\n";
            std::cout << "  Sinais e misc:        sign, min, max, gcd, lcm\n";
            std::cout << "  Valor absoluto:       |expr| (ex: |x+1|)\n";

            std::cout << "Funções lógicas disponivel:\n";
            std::cout << "  negação,conjunção,disjunção,implicação,equivalencia, e parenteses:      !, &&, ||,|,->, <->, ( p -> q) \n";



            std::cout << "\nVariável padrão: x\n";
            std::cout << "Exemplo de uso: sin(x)^2 + cos(x)^2\n";

            continue;
        }

        if ( line.rfind( "revolution ", 0 ) == 0 ) {
            std::string input = line.substr( 11 ); 
            std::istringstream iss( input );
            std::string expr, temp, eixo;

            while ( iss >> temp && temp != "around" ) {
                expr += temp;
            }

            if ( !( iss >> eixo ) || ( eixo != "x" && eixo != "y" ) ) {
                std::cerr << "Uso: revolution <expr> around x|y\n";
                continue;
            }

            try {
                parser::Parser p( expr );
                p.Parse();

                if ( !p.isFunctionExpr() ) {
                    std::cerr << "Expressão não é função de x.\n";
                    continue;
                }

                auto func = p.toFunction();

                std::cout << "\n--- Revolution!  " << eixo << "axis ---\n";

                if ( eixo == "x" ) {
                    auto lines = cartesianR2::revolutionX( func, domain, 30, 60 );
                    for ( auto& row : lines ) std::cout << row << '\n';

                    double vol = cartesianR2::utils::vRevolution( func, domain.x, domain.y, "x" );
                    std::cout << "vol: " << vol << "\n";
                }
                else if ( eixo == "y" ) {
                    auto ascii = cartesianR2::revolutionY( func, domain2 );
                    for ( const auto& line : ascii ) std::cout << line << "\n";
                    double vol = cartesianR2::utils::vRevolution( func, domain2.x, domain2.y, "y" );
                    std::cout << "vol: " << vol << "\n";
                }

            }
            catch ( const std::exception& e ) {
                std::cerr << "Erro: " << e.what() << "\n";
            }

            continue;
        }


        if (line.rfind("const ", 0) == 0) {
            std::istringstream iss(line.substr(6));
            std::string name, eq;
            double value;
            iss >> name >> eq >> value;

            if (eq != "=" || name.empty() || !iss) {
                std::cerr << "Sintaxe incorreta. Use: const nome = valor\n";
            }
            else {
                parser::addConstant(name, value);
                std::cout << "Constante '" << name << "' definida como " << value << "\n";
            }
            continue;
        }

        if (line.rfind("domain", 0) == 0) {
            std::istringstream iss(line);
            std::string cmd, eq;
            float min, max;
            iss >> cmd >> eq >> min >> max;
            if (cmd != "domain" || eq != "=" || !iss) {
                std::cerr << "Sintaxe incorreta. Use: domain = min max\n";
            }
            else {
                domain = { min, max };
                std::cout << "Domínio definido como [" << min << ", " << max << "]\n";
            }
            continue;
        }
        if (line.rfind("prime_factors", 0) == 0) {
            std::istringstream iss(line);
            std::string cmd;
            unsigned long long number;

            iss >> cmd >> number;

            if (cmd != "prime_factors" || !iss) {
                std::cerr << "Sintaxe incorreta. Use: prime_factors <numero>\n";
            }
            else {
                std::map<unsigned long long, unsigned long long> factor_counts;

                for (const auto& factor : numberTheory::prime_factors(number)) {
                    factor_counts[factor]++;
                }

                for (const auto& [factor, exponent] : factor_counts) {
                    std::cout << factor << "^" << exponent << " ";
                }

                std::cout << "\n";
            }

            continue;
        }





        try {
            if (mode == "logic") {
                logica::Parser p(line);
                p.parse();
                p.eval();
                continue;
            }


 
            if (line.rfind("contour ", 0) == 0) {
                std::string exprStr = line.substr(8);
                exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());

                parser::Parser p(exprStr);
                p.Parse();

                if (cartesianR2::usesTwoVariables(exprStr)) {
                    auto func2d = p.toFunction2D();

                    std::cout << "\n--- Mapa de Contorno ---\n";
                    auto contour = cartesianR2::contourMap(func2d, domain, domain, 60, 30, 12);

                     for (int i = 0; i < contour.size(); ++i) {
                        if (i == contour.size() / 2) {
                            std::cout << std::string(contour[i].size() / 2, '-') << "+"
                                << std::string(contour[i].size() / 2, '-') << "\n";
                        }
                        else {
                            std::cout << contour[i] << "\n";
                        }
                    }

                    std::cout << "Domínio: x,y e [" << domain.x << ", " << domain.y << "]\n";
                }
                else {
                    std::cout << "Expressão não parece usar duas variáveis (x e y)\n";
                }
                continue;
            }

            if (line.rfind("surface ", 0) == 0) {
                std::string exprStr = line.substr(8);
                exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());

                parser::Parser p(exprStr);
                p.Parse();

                if (cartesianR2::usesTwoVariables(exprStr)) {
                    auto func2d = p.toFunction2D();

                    std::cout << "\n--- Superfície 3D (dados para plot) ---\n";
                    std::string surfaceData = cartesianR2::plotter_js3d(func2d, domain);
                    std::cout << "Dados gerados para superfície 3D.\n";
                     std::cout << "Número de pontos: " << std::count(surfaceData.begin(), surfaceData.end(), '[') << "\n";
                }
                else {
                    std::cout << "Expressão não parece usar duas variáveis (x e y)\n";
                }
                continue;
            }

            if (line.rfind("deriv ", 0) == 0) {
                std::string exprStr = line.substr(6);
                exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());

                double delta = 0.0;
                bool usedelta = false;

                auto atPos = exprStr.find("at");
                if (atPos != std::string::npos) {
                    std::string valStr = exprStr.substr(atPos + 2);
                    exprStr = exprStr.substr(0, atPos);
                    delta = std::stod(valStr);
                    usedelta = true;
                }

                parser::Parser parser(exprStr);
                parser.Parse();

                if (usedelta) {
                    double h = 1e-5;
                    auto f = parser.toFunction();
                    double derivative = cartesianR2::utils::derivada(f, delta);
                    std::cout << derivative << "\n";
                }
                else {
                    std::cout << parser.ast.back()->deriv() << "\n";
                }

                continue;
            }
            if (line.rfind("integ", 0) == 0) {
                std::string exprStr = line.substr(6);
                exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());

                auto fromPos = exprStr.find("from");
                auto toPos = exprStr.find("to");

                if (fromPos == std::string::npos || toPos == std::string::npos || toPos < fromPos) {
                    std::cerr << "Formato inválido. Use: integr <funcao> from <a> to <b>\n";
                    continue;
                }

                std::string expr = exprStr.substr(0, fromPos);
                double a = std::stod(exprStr.substr(fromPos + 4, toPos - (fromPos + 4)));
                double b = std::stod(exprStr.substr(toPos + 2));

                parser::Parser parser(expr);
                parser.Parse();
                auto f = parser.toFunction();

                int n = 100000;
                double delta = (b - a) / n;
                double sum = 0.0;

                for (int i = 0; i < n; ++i) {
                    double x = a + i * delta;
                    sum += f(x) * delta;
                }

                std::cout << sum << "\n";
                continue;
            }

            if (line.find('=') != std::string::npos) {
                std::cout << repl(line) << "\n";
                continue;
            }

            if (cartesianR2::usesTwoVariables(line)) {
                parser::Parser p(line);
                p.Parse();

                auto func2d = p.toFunction2D();

                std::cout << "\n--- Função 2V Detectada ---\n";
                std::cout << "Use 'contour " << line << "' para mapa de contorno\n";
                std::cout << "Use 'surface " << line << "' para superfície 3D\n";

                std::cout << "\nValores de exemplo:\n";
                std::cout << "f(0,0) = " << func2d(0, 0) << "\n";
                std::cout << "f(1,1) = " << func2d(1, 1) << "\n";
                std::cout << "f(2,2) = " << func2d(2, 2) << "\n";
            }
            else {
                std::string out = repl(line);

                if (out == "[Funcao real f(x)]") {
                    parser::Parser p(line, &realVars);
                    p.Parse();

                    auto function = p.toFunction();

                    std::cout << "\n--- Plot ---\n";
                    for (const auto& row : cartesianR2::plotter_cli(function, domain, domain2)) {
                        std::cout << row << "\n";
                    }

                    parser::Parser derivParser(p.ast.back()->deriv(), &realVars);
                    derivParser.Parse();
                    auto simp = derivParser.ast.back()->simplify()->repr();

                    std::cout << "--------------- análise ---------------\n";
                    std::cout << line << "    |    Domain:[" << domain.x << "," << domain.y << "]\n";
                    std::cout
                        << "Injetora: " << (cartesianR2::utils::B_injectora(function, domain) ? "yes" : "X") << "    |    Derivada no ponto 0: " << cartesianR2::utils::derivada(function) << "\n"
                        << "Sobrejetora: " << (cartesianR2::utils::B_sobrejectora(function, domain) ? "yes" : "X") << "    |    Derivada simbolica: " << simp << "\n"
                        << "Bijetora: " << (cartesianR2::utils::B_Bijectora(function, domain) ? "yes" : "X") << "    |    integral no dominio: " << cartesianR2::utils::integralriemannstyle(function, domain.x, domain.y) << "\n"
                        << "Periódica: " << (cartesianR2::utils::B_periodica(function, domain) ? "yes" : "X") << "\n"
                        << "Par: " << (cartesianR2::utils::B_par(function, domain) ? "yes" : "X") << "\n"
                        << "Ímpar: " << (cartesianR2::utils::B_impar(function, domain) ? "yes" : "X") << "\n";
                }
                else if (out == "[Funcao real f(x,y)]") {
                    std::cout << "\n--- Função 2V Detectada ---\n";
                    std::cout << "Use 'contour " << line << "' para mapa de contorno\n";
                    std::cout << "Use 'surface " << line << "' para superfície 3D\n";
                }
                else {
                    std::cout << out << "\n";
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Erro: " << e.what() << "\n";
        }
        catch (...) {
            std::cerr << "Expressão inválida.\n";
        }

        std::cout << "\n";
    }

    return 0;
}