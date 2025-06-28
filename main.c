
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_TOKEN 256
#define MAX_OPERATORS 50
#define MAX_KEYWORDS 50
#define MAX_COMMANDS 100

typedef struct {
    char name[MAX_TOKEN];
    char extension[MAX_TOKEN];
    char operators[MAX_OPERATORS][MAX_TOKEN];
    int op_count;
    char keywords[MAX_KEYWORDS][MAX_TOKEN];
    int keyword_count;
    char error_messages[10][MAX_TOKEN];
    char output_prefix[MAX_TOKEN];
    char commands[MAX_COMMANDS][MAX_TOKEN];
    int command_count;
} Language;

Language current_lang;

void init_language() {
    strcpy(current_lang.name, "");
    strcpy(current_lang.extension, ".txt");
    strcpy(current_lang.output_prefix, ">>> ");
    current_lang.op_count = 0;
    current_lang.keyword_count = 0;
    current_lang.command_count = 0;
    
    // Mensajes de error por defecto
    strcpy(current_lang.error_messages[0], "Error: comando no reconocido");
    strcpy(current_lang.error_messages[1], "Error: sintaxis incorrecta");
    strcpy(current_lang.error_messages[2], "Error: archivo no encontrado");
}

void trim_whitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

void parse_lilou_definition(char *line) {
    trim_whitespace(line);
    
    if (strstr(line, "Nombre del idioma:")) {
        char *name_start = strchr(line, ':') + 1;
        trim_whitespace(name_start);
        strcpy(current_lang.name, name_start);
        printf("Definiendo lenguaje: %s\n", current_lang.name);
    }
    else if (strstr(line, "extension:")) {
        char *ext_start = strchr(line, ':') + 1;
        trim_whitespace(ext_start);
        strcpy(current_lang.extension, ext_start);
        printf("Extension definida: %s\n", current_lang.extension);
    }
    else if (strstr(line, "operadores:")) {
        char *ops_start = strchr(line, ':') + 1;
        trim_whitespace(ops_start);
        char *token = strtok(ops_start, ",");
        while (token != NULL && current_lang.op_count < MAX_OPERATORS) {
            trim_whitespace(token);
            strcpy(current_lang.operators[current_lang.op_count], token);
            current_lang.op_count++;
            token = strtok(NULL, ",");
        }
        printf("Operadores definidos: %d\n", current_lang.op_count);
    }
    else if (strstr(line, "palabras clave:")) {
        char *keywords_start = strchr(line, ':') + 1;
        trim_whitespace(keywords_start);
        char *token = strtok(keywords_start, ",");
        while (token != NULL && current_lang.keyword_count < MAX_KEYWORDS) {
            trim_whitespace(token);
            strcpy(current_lang.keywords[current_lang.keyword_count], token);
            current_lang.keyword_count++;
            token = strtok(NULL, ",");
        }
        printf("Palabras clave definidas: %d\n", current_lang.keyword_count);
    }
    else if (strstr(line, "mostrar:")) {
        char *msg_start = strchr(line, ':') + 1;
        trim_whitespace(msg_start);
        printf("%s%s\n", current_lang.output_prefix, msg_start);
    }
    else if (strstr(line, "calcular:")) {
        char *calc_start = strchr(line, ':') + 1;
        trim_whitespace(calc_start);
        
        // Simple calculator for basic operations
        int result = 0;
        char op = '+';
        char *token = strtok(calc_start, " ");
        
        if (token) {
            result = atoi(token);
            token = strtok(NULL, " ");
            
            while (token) {
                if (strlen(token) == 1 && (token[0] == '+' || token[0] == '-' || 
                    token[0] == '*' || token[0] == '/')) {
                    op = token[0];
                } else {
                    int num = atoi(token);
                    switch(op) {
                        case '+': result += num; break;
                        case '-': result -= num; break;
                        case '*': result *= num; break;
                        case '/': if(num != 0) result /= num; break;
                    }
                }
                token = strtok(NULL, " ");
            }
        }
        printf("%s%d\n", current_lang.output_prefix, result);
    }
    else if (strstr(line, "mensaje de error:")) {
        char *error_start = strchr(line, ':') + 1;
        trim_whitespace(error_start);
        strcpy(current_lang.error_messages[0], error_start);
    }
    else if (strstr(line, "prefijo de salida:")) {
        char *prefix_start = strchr(line, ':') + 1;
        trim_whitespace(prefix_start);
        strcpy(current_lang.output_prefix, prefix_start);
    }
}

void execute_custom_language(char *lilou_file, char *code_file) {
    FILE *def_file = fopen(lilou_file, "r");
    if (!def_file) {
        printf("Error: No se puede abrir el archivo de definición %s\n", lilou_file);
        return;
    }
    
    // Cargar definición del lenguaje
    char line[MAX_LINE];
    int in_definition = 0;
    
    while (fgets(line, sizeof(line), def_file)) {
        if (strstr(line, "Lilou programa")) {
            in_definition = 1;
            continue;
        }
        if (in_definition && strlen(line) > 1) {
            parse_lilou_definition(line);
        }
    }
    fclose(def_file);
    
    printf("\n=== Ejecutando %s con el lenguaje %s ===\n", 
           code_file, current_lang.name);
    
    // Ejecutar el archivo de código
    FILE *code = fopen(code_file, "r");
    if (!code) {
        printf("%s\n", current_lang.error_messages[2]);
        return;
    }
    
    while (fgets(line, sizeof(line), code)) {
        trim_whitespace(line);
        if (strlen(line) > 0) {
            // Buscar palabras clave y operadores definidos
            int found = 0;
            for (int i = 0; i < current_lang.keyword_count; i++) {
                if (strstr(line, current_lang.keywords[i])) {
                    parse_lilou_definition(line);
                    found = 1;
                    break;
                }
            }
            if (!found && strlen(line) > 0) {
                printf("%s\n", current_lang.error_messages[0]);
            }
        }
    }
    fclose(code);
}

void test_lilou_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se puede abrir el archivo %s\n", filename);
        return;
    }
    
    printf("=== Probando definición de lenguaje en %s ===\n", filename);
    init_language();
    
    char line[MAX_LINE];
    int in_definition = 0;
    int line_number = 1;
    
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Lilou programa")) {
            in_definition = 1;
            printf("✓ Línea %d: Inicio de definición encontrado\n", line_number);
        }
        else if (in_definition && strlen(line) > 2) {
            trim_whitespace(line);
            if (strstr(line, "Nombre del idioma:") || 
                strstr(line, "extension:") ||
                strstr(line, "operadores:") ||
                strstr(line, "palabras clave:")) {
                printf("✓ Línea %d: Definición válida - %s\n", line_number, line);
                parse_lilou_definition(line);
            }
        }
        line_number++;
    }
    
    fclose(file);
    
    // Mostrar resumen
    printf("\n=== Resumen del lenguaje definido ===\n");
    printf("Nombre: %s\n", strlen(current_lang.name) > 0 ? current_lang.name : "Sin nombre");
    printf("Extensión: %s\n", current_lang.extension);
    printf("Operadores definidos: %d\n", current_lang.op_count);
    printf("Palabras clave definidas: %d\n", current_lang.keyword_count);
    
    if (strlen(current_lang.name) > 0) {
        printf("✓ Definición de lenguaje válida!\n");
    } else {
        printf("⚠ Advertencia: Falta el nombre del lenguaje\n");
    }
}

void show_help() {
    printf("=== LILOU - Meta-lenguaje de programación ===\n");
    printf("Uso:\n");
    printf("  ./main tester-lilou <archivo.lilou>     - Probar definición de lenguaje\n");
    printf("  ./main lilou <definicion.lilou> <codigo> - Ejecutar código con lenguaje definido\n");
    printf("  ./main ayuda                            - Mostrar esta ayuda\n");
    printf("\nEjemplos:\n");
    printf("  ./main tester-lilou mi_lenguaje.lilou\n");
    printf("  ./main lilou mi_lenguaje.lilou codigo.gg\n");
}

int main(int argc, char *argv[]) {
    init_language();
    
    if (argc < 2) {
        show_help();
        return 1;
    }
    
    if (strcmp(argv[1], "ayuda") == 0 || strcmp(argv[1], "help") == 0) {
        show_help();
    }
    else if (strcmp(argv[1], "tester-lilou") == 0) {
        if (argc < 3) {
            printf("Error: Especifica el archivo .lilou a probar\n");
            printf("Uso: ./main tester-lilou <archivo.lilou>\n");
            return 1;
        }
        test_lilou_file(argv[2]);
    }
    else if (strcmp(argv[1], "lilou") == 0) {
        if (argc < 4) {
            printf("Error: Especifica el archivo de definición y el archivo de código\n");
            printf("Uso: ./main lilou <definicion.lilou> <archivo_codigo>\n");
            return 1;
        }
        execute_custom_language(argv[2], argv[3]);
    }
    else {
        printf("Comando no reconocido: %s\n", argv[1]);
        show_help();
        return 1;
    }
    
    return 0;
}
