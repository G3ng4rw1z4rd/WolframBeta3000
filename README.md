# 🔥 WolframBeta3000

**WolframBeta3000** é um **CAS (Computer Algebra System)** moderno escrito em **C++**, com renderização matemática avançada usando **WebAssembly + WebGL**.

Ele foi projetado para ser **rápido, visual e extremamente versátil**, permitindo explorar matemática simbólica, numérica e visual diretamente no navegador.

O núcleo matemático roda em **C++ compilado para WASM**, garantindo desempenho próximo ao nativo.

---

# ✨ Recursos

### 📊 Visualização matemática avançada

* Gráficos 2D interativos
* Superfícies 3D
* Mapas de contorno
* Domain Coloring para funções complexas
* Fractais complexos (ex: Mandelbrot)

---

### 🧠 CAS (Computer Algebra System)

Suporte para:

* Derivadas simbólicas
* Integrais numéricas
* Análise de função
* Constantes matemáticas
* Funções trigonométricas, exponenciais e logarítmicas

---

### 🧮 Funções complexas

Suporte a operações complexas incluindo:

```
sin(z)
cos(z)
tan(z)
exp(z)
log(z)
sqrt(z)

gamma(z)
zeta(z)

mandelbrot(z)
joukowsky(z)
```

Visualizadas com **domain coloring GPU-accelerated**.

---

# ⚙️ Arquitetura

O projeto é dividido em duas partes principais:

### 🧩 Núcleo matemático (C++)

Arquivos principais:

```
Parser.h
complexNumbers.h
numberTheory.h
plotter.h
descartes.cpp
main.cpp
```

Esse núcleo realiza:

* parsing de expressões
* cálculo simbólico
* geração de dados para gráficos
* conversão de funções complexas para GLSL

Ele é compilado para **WebAssembly** usando **Emscripten**.

---

### 🌐 Interface visual (HTML + WebGL)

Localizada na pasta:

```
site/
```

Ela contém:

```
index.html
```

Esse arquivo é responsável por:

* interface do usuário
* renderização 3D (Three.js)
* gráficos 2D (Plotly)
* domain coloring complexo (WebGL shader)

---

# ⚠️ Importante

Para que o sistema funcione corretamente:

```
index.html
deve estar no mesmo diretório
do executável ou servidor que carrega o WASM.
```

Isso ocorre porque o **WebAssembly precisa ser carregado pelo navegador usando caminhos relativos**.

Se o `index.html` estiver separado, o navegador não conseguirá localizar:

```
wasmLib.js
wasmLib.wasm
```

O resultado será erro de carregamento do módulo WASM.

Portanto mantenha a seguinte estrutura:

```
project/
│
├── index.html
├── wasmLib.js
├── wasmLib.wasm
```

Ou utilize um servidor local apontando para essa pasta.

---

# 🚀 Compilação

Compile o núcleo C++ usando **Emscripten**:

```bash
em++ wasmLib.cpp -O3 -std=c++20 --bind -fexceptions -sNO_DISABLE_EXCEPTION_CATCHING \
-s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=Descartes \
-s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH=1 \
-o wasmLib.js
```

Isso gera:

```
wasmLib.js
wasmLib.wasm
```

---

# ▶️ Executando

Abra o projeto usando um servidor local.

Exemplo:

```bash
python -m http.server
```

ou

```bash
npx serve
```

Depois abra no navegador:

```
http://localhost:8000
```

---

# 📌 Exemplos de uso

```
sin(x)
exp(-x^2)*sin(8*x)

surface sin(x)*cos(y)

contour sin(x^2+y^2)

revolution exp(-x^2) around x

domaincolor (z-1)/(z+1)

domaincolor mandelbrot(z)
```

---

# 🧑‍💻 Tecnologias

* **C++20**
* **WebAssembly**
* **Emscripten**
* **Three.js**
* **Plotly**
* **WebGL2**

---

 

# ⭐ WolframBeta3000

Um CAS moderno focado em **visualização matemática poderosa e rápida**.

Explore matemática de forma **interativa, visual e intuitiva**.



# 🔥 WolframBeta3000 — Guia de Uso

Este guia lista **constantes, funções e comandos disponíveis** no sistema.

---

# 🔢 Constantes Reais

```
k   = 5
g   = 9.80665
phi = 1.61803
tau = 6.28319
e   = 2.71828
h   = 6.62607e-34
c   = 2.99792e+08
pi  = 3.14159
```

---

# 📈 Funções Reais (1 argumento)

```
sin(x)
cos(x)
tan(x)

asin(x)
acos(x)
atan(x)

sinh(x)
cosh(x)
tanh(x)

exp(x)
ln(x)
log(x)

sqrt(x)
cbrt(x)

abs(x)
sign(x)

ceil(x)
floor(x)
round(x)
trunc(x)

deg(x)

min(x)
max(x)

gcd(x)
lcm(x)
```

---

# 📈 Funções Reais (2 argumentos)

```
max(x,y)
min(x,y)

gcd(x,y)
lcm(x,y)
```

---

# 🧮 Funções Complexas (1 argumento)

As funções podem ser usadas com **alias simples** ou com prefixo `C`.

```
sin(z)       / Csin(z)
cos(z)       / Ccos(z)
tan(z)       / Ctan(z)

sinh(z)      / Csinh(z)
cosh(z)      / Ccosh(z)
tanh(z)      / Ctanh(z)

exp(z)       / Cexp(z)
log(z)       / Clog(z)
sqrt(z)      / Csqrt(z)

invert(z)    / Cinvert(z)
conj(z)      / Cconj(z)

abs(z)       / Cabs(z)
arg(z)       / Carg(z)
re(z)        / Cre(z)
im(z)        / Cim(z)

floor(z)     / Cfloor(z)
ceil(z)      / Cceil(z)

sec(z)       / Csec(z)
csc(z)       / Ccsc(z)
cot(z)       / Ccot(z)

sech(z)      / Csech(z)
csch(z)      / Ccsch(z)
coth(z)      / Ccoth(z)

arcsin(z)    / Carcsin(z)
arccos(z)    / Carccos(z)
arctan(z)    / Carctan(z)

arcsinh(z)   / Carcsinh(z)
arccosh(z)   / Carccosh(z)
arctanh(z)   / Carctanh(z)

arcsec(z)    / Carcsec(z)
arccsc(z)    / Carccsc(z)
arccot(z)    / Carccot(z)

arcsech(z)   / Carcsech(z)
arccsch(z)   / Carccsch(z)
arccoth(z)   / Carccoth(z)

gamma(z)     / Cgamma(z)
zeta(z)      / Czeta(z)

phi(z)       / Cphi(z)
theta(z)     / Ctheta(z)

joukowsky(z) / Cjoukowsky(z)

disk(z)      / Cdisk(z)

mandelbrot(z) / Cmandelbrot(z)

map(z)       / Cmap(z)
transform(z) / Ctransform(z)
```

---

# 🧮 Funções Complexas (2 argumentos)

```
add(z,w) / Cadd(z,w)
sub(z,w) / Csub(z,w)

mul(z,w) / Cmul(z,w)
div(z,w) / Cdiv(z,w)

pow(z,w) / Cpow(z,w)
```

---

# 👤 Variáveis Definidas pelo Usuário

### Variáveis reais

```
(nenhuma definida)
```

### Variáveis complexas

```
z = 1.414214 cis(-0.785398)
  = 1.000000 - 1.000000i
```

---

# 🎨 Exemplos

### Domain Coloring

```
domaincolor (z-1)/(z+1)

domaincolor z^3

domaincolor sin(z)

domaincolor exp(1/z)

domaincolor gamma(z)

domaincolor zeta(z)

domaincolor joukowsky(z)

domaincolor mandelbrot(z)
```

---

### Superfícies

```
surface x + y
surface sin(x)*cos(y)
surface exp(-(x^2+y^2))
```

---

### Contornos

```
x + y
sin(x)*cos(y)
sin(x^2 + y^2)
```

*(uma função de duas variáveis desenha automaticamente o contorno)*

---

### Sólidos de revolução

```
revolution x around y

revolution y around x
```
![image](https://github.com/user-attachments/assets/60405ec1-460e-473b-9a1d-b4a0dd22f563)
![image](https://github.com/user-attachments/assets/b2f694d7-8e6a-4d5e-91b1-e15c1a58e5b0)
![image](https://github.com/user-attachments/assets/1be0f0c0-4be6-4398-a3c3-912dacc471f3)
![image](https://github.com/user-attachments/assets/1182aa58-afbc-4181-a642-bba065fadd9e)
<img width="1390" height="887" alt="Screenshot_10" src="https://github.com/user-attachments/assets/c4e3b6d1-a72b-4ed8-a8f2-c21ffd2752a7" />

---

# 📄 Licença

MIT License
