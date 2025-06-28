

# 🌟 LILOU 2.0 - Meta-lenguaje de Programación Avanzado

¡Bienvenido a **Lilou 2.0**! Un meta-lenguaje de programación revolucionario que te permite crear tus propios lenguajes de programación completos con funcionalidades avanzadas, ¡todo en español!

## 🆕 ¡Nuevas Funcionalidades en la Versión 2.0!

- **🔢 Variables y Almacenamiento**: Crea y manipula variables
- **🔀 Estructuras de Control**: Condicionales (si/entonces/sino)
- **🔁 Bucles**: Repetir acciones múltiples veces
- **📝 Funciones Personalizadas**: Define y ejecuta tus propias funciones
- **📁 Manejo de Archivos**: Lee y escribe archivos
- **🎲 Números Aleatorios**: Genera números al azar
- **🐛 Modo Debug**: Herramientas de depuración
- **💬 Comentarios**: Documenta tu código

## 🎯 ¿Qué es Lilou?

Lilou es un meta-lenguaje de programación diseñado específicamente para crear otros lenguajes de programación. Con una sintaxis simple y amigable en español, cualquier persona puede diseñar e implementar su propio lenguaje de programación personalizado con características avanzadas.

## 🚀 Características Principales

- **Sintaxis en Español**: Diseñado para ser natural e intuitivo
- **Meta-programación**: Crea lenguajes desde lenguajes
- **Personalización Completa**: Define operadores, palabras clave, mensajes de error
- **Extensiones Personalizadas**: Elige la extensión de archivo de tu lenguaje
- **Programación Completa**: Variables, condicionales, bucles, funciones
- **Fácil de Usar**: Perfecto para principiantes en diseño de lenguajes

## 📋 Instalación y Compilación

```bash
# Compilar Lilou
make

# O compilar manualmente
gcc -o main main.c
```

## 🎮 Uso Básico

### 1. Crear tu Lenguaje

Crea un archivo con extensión `.lilou` para definir tu lenguaje:

```lilou
Lilou programa - lenguaje de programación : MiLenguaje

• Nombre del idioma: SuperLenguaje
• extension: .super
• operadores: +, -, *, /, %, =, ==, !=, <, >, <=, >=
• palabras clave: mostrar, calcular, variable, si, entonces, sino, repetir, hacer
• mensaje de error: ¡Error en SuperLenguaje!
• prefijo de salida: [SuperLenguaje] 
```

### 2. Probar tu Definición

```bash
./main tester-lilou mi_lenguaje.lilou
```

### 3. Escribir Código en tu Lenguaje

Crea un archivo con la extensión que definiste:

```super
// Variables
variable: contador = 0
variable: limite = 10

// Mostrar valores
mostrar: Contador: {contador}, Límite: {limite}

// Condicional
si: contador < limite
entonces: mostrar: Contador dentro del límite
sino: mostrar: Contador fuera del límite

// Bucle
repetir: 5
hacer: mostrar: Iteración número {i}

// Función
funcion: despedida { mostrar: ¡Adiós! }
llamar: despedida
```

### 4. Ejecutar tu Código

```bash
./main lilou mi_lenguaje.lilou codigo.super
```

## 📖 Sintaxis de Definición

### Estructura Básica

Toda definición de lenguaje debe comenzar con:
```
Lilou programa - lenguaje de programación : [NombreDelLenguaje]
```

### Elementos Configurables

| Elemento | Sintaxis | Ejemplo |
|----------|----------|---------|
| **Nombre** | `• Nombre del idioma: [nombre]` | `• Nombre del idioma: MiLenguaje` |
| **Extensión** | `• extension: [.ext]` | `• extension: .ml` |
| **Operadores** | `• operadores: [op1, op2, ...]` | `• operadores: +, -, *, /, ==, !=` |
| **Palabras Clave** | `• palabras clave: [key1, key2, ...]` | `• palabras clave: mostrar, calcular` |
| **Mensajes de Error** | `• mensaje de error: [mensaje]` | `• mensaje de error: ¡Error!` |
| **Prefijo de Salida** | `• prefijo de salida: [prefijo]` | `• prefijo de salida: >>> ` |

## 🛠️ Comandos Disponibles

### 📝 Comandos Básicos

#### `mostrar:`
Muestra texto en la consola. Soporta interpolación de variables con `{variable}`.
```
mostrar: ¡Hola Mundo!
mostrar: El valor es {mi_variable}
```

#### `calcular:`
Realiza operaciones matemáticas básicas
```
calcular: 5 + 3 * 2
calcular: mi_variable + 10
```

### 🔢 Variables

#### `variable:`
Crea y asigna valores a variables
```
variable: contador = 0
variable: suma = 5 + 3
variable: resultado = contador * 2
```

### 🔀 Estructuras de Control

#### `si:` / `entonces:` / `sino:`
Implementa lógica condicional
```
si: contador > 5
entonces: mostrar: Mayor que 5
sino: mostrar: Menor o igual que 5
```

**Operadores de comparación disponibles:**
- `==` (igual)
- `!=` (diferente)
- `<` (menor que)
- `>` (mayor que)
- `<=` (menor o igual)
- `>=` (mayor o igual)

### 🔁 Bucles

#### `repetir:` / `hacer:`
Ejecuta código múltiples veces
```
repetir: 5
hacer: mostrar: Repetición número {i}
```

La variable `i` se crea automáticamente y contiene el índice actual (0-based).

### 📝 Funciones

#### `funcion:` / `llamar:`
Define y ejecuta funciones personalizadas
```
funcion: mi_funcion { 
    mostrar: ¡Hola desde la función!
    variable: x = 10
    mostrar: Valor de x: {x}
}
llamar: mi_funcion
```

### 🎲 Números Aleatorios

#### `aleatorio:`
Genera números aleatorios
```
aleatorio: 100          // Número entre 0 y 99
aleatorio: 1-10         // Número entre 1 y 10
```

### 📁 Manejo de Archivos

#### `escribir_archivo:`
Crea un archivo con contenido
```
escribir_archivo: mi_archivo.txt, Contenido del archivo
```

#### `leer_archivo:`
Lee y muestra el contenido de un archivo
```
leer_archivo: mi_archivo.txt
```

### 🐛 Herramientas de Debug

#### `debug:`
Controla el modo de depuración
```
debug: on               // Activa debug
debug: variables        // Muestra todas las variables
debug: off              // Desactiva debug
```

### 💬 Comentarios

Puedes usar comentarios en tu código:
```
// Este es un comentario de línea
# Este también es un comentario
```

## 💡 Ejemplos Completos

### Ejemplo 1: Calculadora Avanzada

**Definición (calc.lilou):**
```lilou
Lilou programa - lenguaje de programación : Calculadora

• Nombre del idioma: CalcAvanzada
• extension: .calc
• operadores: +, -, *, /, %, ^, ==, !=, <, >
• palabras clave: calcular, resultado, mostrar, variable, si, entonces
• mensaje de error: Error en cálculo
• prefijo de salida: [CALC] 
```

**Código (operaciones.calc):**
```calc
// Calculadora avanzada
variable: a = 15
variable: b = 4

mostrar: === Calculadora Avanzada ===
mostrar: a = {a}, b = {b}

variable: suma = a + b
mostrar: Suma: {a} + {b} = {suma}

variable: division = a / b
mostrar: División: {a} / {b} = {division}

variable: resto = a % b
mostrar: Resto: {a} % {b} = {resto}

si: suma > 15
entonces: mostrar: La suma es mayor que 15
sino: mostrar: La suma es menor o igual que 15
```

### Ejemplo 2: Juego de Adivinanzas

**Definición (juego.lilou):**
```lilou
Lilou programa - lenguaje de programación : Juego

• Nombre del idioma: AdivinaNum
• extension: .juego
• palabras clave: mostrar, variable, aleatorio, si, entonces, sino, repetir, hacer
• mensaje de error: ¡Oops! Error en el juego
• prefijo de salida: 🎮 
```

**Código (adivinanza.juego):**
```juego
// Juego de adivinanzas
mostrar: === Juego de Adivinanzas ===

aleatorio: 1-10
variable: numero_secreto = 7  // En una implementación real, esto sería aleatorio
variable: intentos = 3

mostrar: He pensado un número entre 1 y 10
mostrar: Tienes {intentos} intentos

repetir: 3
hacer: mostrar: Intento {i}: ¿Cuál es el número?

// Simulación de respuesta correcta
si: numero_secreto == 7
entonces: mostrar: ¡Correcto! Has ganado
sino: mostrar: Incorrecto, sigue intentando
```

### Ejemplo 3: Sistema de Archivos

**Definición (archivos.lilou):**
```lilou
Lilou programa - lenguaje de programación : SistemaArchivos

• Nombre del idioma: FileSystem
• extension: .fs
• palabras clave: mostrar, escribir_archivo, leer_archivo, variable, si, funcion, llamar
• mensaje de error: Error del sistema de archivos
• prefijo de salida: [FS] 
```

**Código (sistema.fs):**
```fs
// Sistema de gestión de archivos
mostrar: === Sistema de Archivos ===

// Crear archivo de configuración
escribir_archivo: config.txt, version=1.0;debug=true;lang=es

// Crear archivo de datos
variable: datos = Usuario: Juan, Edad: 25
escribir_archivo: datos.txt, {datos}

// Leer archivos
mostrar: Contenido de config.txt:
leer_archivo: config.txt

mostrar: Contenido de datos.txt:
leer_archivo: datos.txt

// Función para backup
funcion: backup {
    mostrar: Realizando backup...
    escribir_archivo: backup.txt, Backup creado
    mostrar: Backup completado
}

llamar: backup
```

## 🎯 Comandos de Terminal

| Comando | Descripción | Ejemplo |
|---------|-------------|---------|
| `./main ayuda` | Muestra la ayuda | `./main ayuda` |
| `./main ejemplos` | Muestra ejemplos de comandos | `./main ejemplos` |
| `./main tester-lilou <archivo>` | Prueba una definición | `./main tester-lilou lenguaje.lilou` |
| `./main lilou <def> <código>` | Ejecuta código | `./main lilou def.lilou code.ext` |

## 🐛 Solución de Problemas

### Error: "No se puede abrir el archivo"
- Verifica que el archivo existe
- Comprueba la ruta del archivo
- Asegúrate de tener permisos de lectura

### Error: "Comando no reconocido"
- Verifica que las palabras clave estén bien definidas
- Comprueba la sintaxis de tu definición de lenguaje
- Usa `debug: on` para ver más información

### Error: "Variable no definida"
- Asegúrate de crear la variable con `variable: nombre = valor`
- Verifica que el nombre de la variable sea correcto
- Usa `debug: variables` para ver todas las variables

### Error: "Función no definida"
- Define la función con `funcion: nombre { código }`
- Verifica que el nombre de la función sea correcto
- Asegúrate de que la función esté definida antes de llamarla

## 🌈 Características Avanzadas

### Interpolación de Variables
```
variable: nombre = Juan
mostrar: Hola {nombre}, bienvenido
```

### Expresiones Matemáticas Complejas
```
variable: resultado = 5 + 3 * 2 - 1
calcular: resultado / 2 + 10
```

### Condicionales Anidadas
```
si: a > b
entonces: si: a > 10
          entonces: mostrar: a es mayor que 10 y que b
          sino: mostrar: a es mayor que b pero menor que 10
sino: mostrar: a es menor o igual que b
```

### Bucles con Variables
```
variable: suma = 0
repetir: 5
hacer: variable: suma = suma + i
mostrar: La suma total es {suma}
```

### Funciones con Lógica Compleja
```
funcion: fibonacci {
    variable: a = 0
    variable: b = 1
    repetir: 10
    hacer: variable: temp = a + b
           variable: a = b
           variable: b = temp
           mostrar: Fibonacci: {b}
}
llamar: fibonacci
```

## 📚 Tutorial Paso a Paso

### Paso 1: Tu Primer Lenguaje Avanzado
1. Crea `primer_lenguaje_avanzado.lilou`
2. Define nombre, extensión y comandos avanzados
3. Prueba con `./main tester-lilou primer_lenguaje_avanzado.lilou`

### Paso 2: Programa con Variables
1. Crea variables con `variable:`
2. Usa interpolación con `{variable}`
3. Realiza operaciones matemáticas

### Paso 3: Añade Lógica
1. Usa condicionales con `si:/entonces:/sino:`
2. Implementa bucles con `repetir:/hacer:`
3. Crea funciones con `funcion:/llamar:`

### Paso 4: Funcionalidades Avanzadas
1. Maneja archivos con `escribir_archivo:/leer_archivo:`
2. Genera números aleatorios con `aleatorio:`
3. Usa herramientas de debug

## 🎲 Ejercicios Prácticos

### Ejercicio 1: Contador
Crea un lenguaje que cuente del 1 al 10 y muestre si cada número es par o impar.

### Ejercicio 2: Tabla de Multiplicar
Implementa un lenguaje que genere la tabla de multiplicar de un número.

### Ejercicio 3: Gestor de Tareas
Crea un lenguaje que permita crear, leer y gestionar una lista de tareas en archivos.

## 🤝 Contribuciones

¿Tienes ideas para mejorar Lilou? ¡Las contribuciones son bienvenidas!

1. Haz fork del proyecto
2. Crea una rama para tu feature
3. Commit tus cambios
4. Push a la rama
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la Licencia MIT. Ver el archivo `LICENSE` para más detalles.

## 👥 Créditos

Creado con ❤️ para hacer la programación más accesible y divertida.

---

## 🎉 ¡Comienza a Crear!

```bash
# Compila Lilou 2.0
make

# Prueba el ejemplo básico
./main tester-lilou ejemplo_lenguaje.lilou

# Ejecuta código básico
./main lilou ejemplo_lenguaje.lilou codigo_ejemplo.hola

# Ejecuta código avanzado
./main lilou ejemplo_lenguaje.lilou codigo_avanzado.super

# Ve todos los ejemplos
./main ejemplos

# ¡Crea tu propio lenguaje avanzado!
```

**¡Diviértete creando lenguajes de programación completos con Lilou 2.0!** 🌟

### 🔥 Novedades de la Versión 2.0

- **Variables dinámicas** con interpolación de texto
- **Estructuras de control** completas (if/else)
- **Bucles** con contador automático
- **Funciones personalizadas** con scope local
- **Sistema de archivos** integrado
- **Generador de números aleatorios**
- **Modo debug** completo
- **Comentarios** para documentar código
- **Mejor manejo de errores** con mensajes específicos

¡Lilou 2.0 te permite crear lenguajes de programación realmente funcionales! 🚀

