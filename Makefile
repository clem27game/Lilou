
# Makefile para Lilou 3.0 - Meta-lenguaje de programación avanzado

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
DEBUG_CFLAGS = -Wall -Wextra -std=c99 -g -DDEBUG -O0
LIBS = -lm
TARGET = main
DEBUG_TARGET = main-debug
SOURCE = main.c

# Regla principal
all: $(TARGET)

# Compilación optimizada
$(TARGET): $(SOURCE)
	@echo "Compilando Lilou 3.0..."
	$(CC) $(CFLAGS) $(SOURCE) $(LIBS) -o $(TARGET)
	@echo "✓ Lilou 3.0 compilado exitosamente!"

# Compilación para debug
$(DEBUG_TARGET): $(SOURCE)
	@echo "Compilando Lilou 3.0 en modo debug..."
	$(CC) $(DEBUG_CFLAGS) $(SOURCE) $(LIBS) -o $(DEBUG_TARGET)
	@echo "✓ Lilou 3.0 debug compilado exitosamente!"

# Compilación silenciosa
silent: $(SOURCE)
	@$(CC) $(CFLAGS) $(SOURCE) $(LIBS) -o $(TARGET)

# Limpiar archivos generados
clean:
	@echo "Limpiando archivos..."
	@rm -f $(TARGET) $(DEBUG_TARGET) *.o
	@rm -f *.txt *.log resultado.* saludo.* datos.* estudiantes.*
	@echo "✓ Limpieza completada"

# Instalar en el sistema (requiere permisos de administrador)
install: $(TARGET)
	@echo "Instalando Lilou 3.0..."
	@cp $(TARGET) /usr/local/bin/lilou
	@chmod +x /usr/local/bin/lilou
	@echo "✓ Lilou 3.0 instalado en /usr/local/bin/lilou"

# Desinstalar del sistema
uninstall:
	@echo "Desinstalando Lilou..."
	@rm -f /usr/local/bin/lilou
	@echo "✓ Lilou desinstalado"

# Ejecutar pruebas básicas
test: $(TARGET)
	@echo "Ejecutando pruebas básicas..."
	@./$(TARGET) tester-lilou ejemplo_lenguaje.lilou
	@echo "✓ Pruebas completadas"

# Mostrar ayuda
help: $(TARGET)
	@./$(TARGET) ayuda

# Mostrar ejemplos
examples: $(TARGET)
	@./$(TARGET) ejemplos

# Mostrar características
features: $(TARGET)
	@./$(TARGET) caracteristicas

# Compilación con información adicional
verbose: $(SOURCE)
	@echo "Compilando con información detallada..."
	$(CC) $(CFLAGS) $(SOURCE) $(LIBS) -o $(TARGET) -v
	@echo "✓ Compilación verbose completada"

# Verificar sintaxis sin compilar
check: $(SOURCE)
	@echo "Verificando sintaxis..."
	$(CC) $(CFLAGS) $(SOURCE) -fsyntax-only
	@echo "✓ Sintaxis verificada"

# Información del proyecto
info:
	@echo "=== LILOU 3.0 - Meta-lenguaje de Programación ==="
	@echo "Archivo fuente: $(SOURCE)"
	@echo "Ejecutable: $(TARGET)"
	@echo "Compilador: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Librerías: $(LIBS)"
	@echo ""
	@echo "Comandos disponibles:"
	@echo "  make          - Compilar versión optimizada"
	@echo "  make debug    - Compilar versión debug"
	@echo "  make clean    - Limpiar archivos"
	@echo "  make test     - Ejecutar pruebas"
	@echo "  make install  - Instalar en el sistema"
	@echo "  make help     - Mostrar ayuda de Lilou"

# Crear paquete de distribución
package: clean $(TARGET)
	@echo "Creando paquete de distribución..."
	@mkdir -p lilou-3.0
	@cp $(TARGET) $(SOURCE) Makefile README.md *.lilou *.hola *.super lilou-3.0/
	@tar -czf lilou-3.0.tar.gz lilou-3.0/
	@rm -rf lilou-3.0/
	@echo "✓ Paquete lilou-3.0.tar.gz creado"

# Reglas que no generan archivos
.PHONY: all clean install uninstall test help examples features verbose check info package silent

# Regla por defecto
.DEFAULT_GOAL := all
