
# 🌟 LILOU - Meta-lenguaje de Programación

¡Bienvenido a **Lilou**! Un meta-lenguaje de programación revolucionario que te permite crear tus propios lenguajes de programación ésotéricos de manera simple y divertida, ¡todo en español!

## 🎯 ¿Qué es Lilou?

Lilou es un lenguaje de programación diseñado específicamente para crear otros lenguajes de programación. Con una sintaxe simple y amigable en español, cualquier persona puede diseñar y implementar su propio lenguaje de programación personalizado.

## 🚀 Características Principales

- **Sintaxis en Español**: Diseñado para ser natural e intuitivo
- **Meta-programación**: Crea lenguajes desde lenguajes
- **Personalización Completa**: Define operadores, palabras clave, mensajes de error
- **Extensiones Personalizadas**: Elige la extensión de archivo de tu lenguaje
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

• Nombre del idioma: HolaMundo
• extension: .hola
• operadores: +, -, *, /, =, ==, !=
• palabras clave: mostrar, calcular, si, entonces, fin
• mensaje de error: ¡Ay no! Algo salió mal
• prefijo de salida: [HolaMundo] 
```

### 2. Probar tu Definición

```bash
./main tester-lilou mi_lenguaje.lilou
```

### 3. Escribir Código en tu Lenguaje

Crea un archivo con la extensión que definiste:

```hola
mostrar: ¡Hola desde mi propio lenguaje!
calcular: 5 + 3 * 2
mostrar: ¡Funciona perfectamente!
```

### 4. Ejecutar tu Código

```bash
./main lilou mi_lenguaje.lilou codigo.hola
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
| **Operadores** | `• operadores: [op1, op2, ...]` | `• operadores: +, -, *, /` |
| **Palabras Clave** | `• palabras clave: [key1, key2, ...]` | `• palabras clave: mostrar, calcular` |
| **Mensajes de Error** | `• mensaje de error: [mensaje]` | `• mensaje de error: ¡Error!` |
| **Prefijo de Salida** | `• prefijo de salida: [prefijo]` | `• prefijo de salida: >>> ` |

## 🛠️ Palabras Clave Predefinidas

### `mostrar:`
Muestra texto en la consola
```
mostrar: ¡Hola Mundo!
```

### `calcular:`
Realiza operaciones matemáticas básicas
```
calcular: 5 + 3 * 2
calcular: 10 - 4 / 2
```

## 💡 Ejemplos Completos

### Ejemplo 1: Lenguaje Matemático Simple

**Definición (mate.lilou):**
```lilou
Lilou programa - lenguaje de programación : Matematicas

• Nombre del idioma: CalcuLoco
• extension: .calc
• operadores: +, -, *, /, ^, %
• palabras clave: calcular, resultado, mostrar
• mensaje de error: Error matemático detectado
• prefijo de salida: [CALC] 
```

**Código (operaciones.calc):**
```calc
mostrar: Calculadora Loca iniciada
calcular: 15 + 25
calcular: 100 - 30
mostrar: Cálculos completados
```

### Ejemplo 2: Lenguaje de Saludo Personalizado

**Definición (saludo.lilou):**
```lilou
Lilou programa - lenguaje de programación : Saludos

• Nombre del idioma: HolaLenguaje
• extension: .saludo
• palabras clave: mostrar, despedir, saludar
• mensaje de error: ¡Ups! Saludo incorrecto
• prefijo de salida: 🌟 
```

**Código (mi_saludo.saludo):**
```saludo
mostrar: ¡Bienvenido a mi lenguaje de saludos!
mostrar: Este es mi primer lenguaje creado con Lilou
mostrar: ¡Hasta la vista!
```

## 🎯 Comandos Disponibles

| Comando | Descripción | Ejemplo |
|---------|-------------|---------|
| `./main ayuda` | Muestra la ayuda | `./main ayuda` |
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

### Error: "Definición de lenguaje inválida"
- El archivo debe comenzar con "Lilou programa"
- Verifica que tengas al menos un "Nombre del idioma"

## 🌈 Características Avanzadas

### Personalización de Errores
```lilou
• mensaje de error: ¡Oops! Tu código tiene un problemita
```

### Prefijos Personalizados
```lilou
• prefijo de salida: 🚀 MiLenguaje: 
```

### Múltiples Operadores
```lilou
• operadores: +, -, *, /, %, ^, ==, !=, <, >, <=, >=
```

## 📚 Tutorial Paso a Paso

### Paso 1: Tu Primer Lenguaje
1. Crea `primer_lenguaje.lilou`
2. Define nombre y extensión
3. Prueba con `./main tester-lilou primer_lenguaje.lilou`

### Paso 2: Escribe Código
1. Crea un archivo con tu extensión personalizada
2. Usa las palabras clave que definiste
3. Ejecuta con `./main lilou`

### Paso 3: Experimenta
- Cambia operadores
- Personaliza mensajes
- Crea diferentes tipos de lenguajes

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
# Compila Lilou
make

# Prueba el ejemplo
./main tester-lilou ejemplo_lenguaje.lilou

# Ejecuta código de ejemplo
./main lilou ejemplo_lenguaje.lilou codigo_ejemplo.hola

# ¡Crea tu propio lenguaje!
```

**¡Diviértete creando tus propios lenguajes de programación con Lilou!** 🌟
