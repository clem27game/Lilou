
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LINE 1024
#define MAX_TOKEN 256
#define MAX_OPERATORS 50
#define MAX_KEYWORDS 50
#define MAX_COMMANDS 100
#define MAX_VARIABLES 100
#define MAX_FUNCTIONS 50

typedef struct {
    char name[MAX_TOKEN];
    int value;
} Variable;

typedef struct {
    char name[MAX_TOKEN];
    char code[MAX_LINE * 10];
} Function;

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
    
    // Nuevas características
    Variable variables[MAX_VARIABLES];
    int var_count;
    Function functions[MAX_FUNCTIONS];
    int func_count;
    int if_condition_result;
    int loop_active;
    int loop_count;
    int loop_max;
} Language;

Language current_lang;
int debug_mode = 0;

void init_language() {
    strcpy(current_lang.name, "");
    strcpy(current_lang.extension, ".txt");
    strcpy(current_lang.output_prefix, ">>> ");
    current_lang.op_count = 0;
    current_lang.keyword_count = 0;
    current_lang.command_count = 0;
    current_lang.var_count = 0;
    current_lang.func_count = 0;
    current_lang.if_condition_result = 0;
    current_lang.loop_active = 0;
    current_lang.loop_count = 0;
    current_lang.loop_max = 0;
    
    // Mensajes de error por defecto
    strcpy(current_lang.error_messages[0], "Error: comando no reconocido");
    strcpy(current_lang.error_messages[1], "Error: sintaxis incorrecta");
    strcpy(current_lang.error_messages[2], "Error: archivo no encontrado");
    strcpy(current_lang.error_messages[3], "Error: variable no definida");
    strcpy(current_lang.error_messages[4], "Error: función no definida");
    strcpy(current_lang.error_messages[5], "Error: operación matemática inválida");
    
    srand(time(NULL)); // Inicializar generador de números aleatorios
}

void trim_whitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

int find_variable(char *name) {
    for (int i = 0; i < current_lang.var_count; i++) {
        if (strcmp(current_lang.variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_variable(char *name, int value) {
    int index = find_variable(name);
    if (index >= 0) {
        current_lang.variables[index].value = value;
    } else if (current_lang.var_count < MAX_VARIABLES) {
        strcpy(current_lang.variables[current_lang.var_count].name, name);
        current_lang.variables[current_lang.var_count].value = value;
        current_lang.var_count++;
        if (debug_mode) {
            printf("[DEBUG] Variable '%s' creada con valor %d\n", name, value);
        }
    }
}

int get_variable_value(char *name) {
    int index = find_variable(name);
    if (index >= 0) {
        return current_lang.variables[index].value;
    }
    return 0;
}

int evaluate_expression(char *expr) {
    trim_whitespace(expr);
    
    // Si es solo un número
    if (isdigit(expr[0]) || (expr[0] == '-' && isdigit(expr[1]))) {
        return atoi(expr);
    }
    
    // Si es una variable
    if (find_variable(expr) >= 0) {
        return get_variable_value(expr);
    }
    
    // Evaluación de expresiones simples
    int result = 0;
    char op = '+';
    char *token = strtok(expr, " ");
    
    if (token) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            result = atoi(token);
        } else {
            result = get_variable_value(token);
        }
        
        token = strtok(NULL, " ");
        
        while (token) {
            if (strlen(token) == 1 && (token[0] == '+' || token[0] == '-' || 
                token[0] == '*' || token[0] == '/' || token[0] == '%')) {
                op = token[0];
            } else {
                int num;
                if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
                    num = atoi(token);
                } else {
                    num = get_variable_value(token);
                }
                
                switch(op) {
                    case '+': result += num; break;
                    case '-': result -= num; break;
                    case '*': result *= num; break;
                    case '/': 
                        if(num != 0) result /= num; 
                        else printf("%s\n", current_lang.error_messages[5]);
                        break;
                    case '%': 
                        if(num != 0) result %= num; 
                        else printf("%s\n", current_lang.error_messages[5]);
                        break;
                }
            }
            token = strtok(NULL, " ");
        }
    }
    return result;
}

int evaluate_condition(char *condition) {
    trim_whitespace(condition);
    
    // Buscar operadores de comparación
    char *operators[] = {"==", "!=", "<=", ">=", "<", ">"};
    int op_count = 6;
    
    for (int i = 0; i < op_count; i++) {
        char *op_pos = strstr(condition, operators[i]);
        if (op_pos) {
            char left[MAX_TOKEN], right[MAX_TOKEN];
            
            // Extraer lado izquierdo
            int left_len = op_pos - condition;
            strncpy(left, condition, left_len);
            left[left_len] = '\0';
            trim_whitespace(left);
            
            // Extraer lado derecho
            strcpy(right, op_pos + strlen(operators[i]));
            trim_whitespace(right);
            
            int left_val = evaluate_expression(left);
            int right_val = evaluate_expression(right);
            
            switch(i) {
                case 0: return left_val == right_val; // ==
                case 1: return left_val != right_val; // !=
                case 2: return left_val <= right_val; // <=
                case 3: return left_val >= right_val; // >=
                case 4: return left_val < right_val;  // <
                case 5: return left_val > right_val;  // >
            }
        }
    }
    
    // Si no hay operador de comparación, evaluar como expresión
    return evaluate_expression(condition) != 0;
}

void execute_function(char *func_name) {
    for (int i = 0; i < current_lang.func_count; i++) {
        if (strcmp(current_lang.functions[i].name, func_name) == 0) {
            if (debug_mode) {
                printf("[DEBUG] Ejecutando función '%s'\n", func_name);
            }
            
            // Ejecutar código de la función línea por línea
            char *line = strtok(current_lang.functions[i].code, "\n");
            while (line != NULL) {
                parse_lilou_definition(line);
                line = strtok(NULL, "\n");
            }
            return;
        }
    }
    printf("%s: función '%s'\n", current_lang.error_messages[4], func_name);
}

void parse_lilou_definition(char *line) {
    trim_whitespace(line);
    
    // Ignorar comentarios
    if (strstr(line, "//") == line || strstr(line, "#") == line || strlen(line) == 0) {
        return;
    }
    
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
        
        // Reemplazar variables en el mensaje
        char output[MAX_LINE];
        strcpy(output, msg_start);
        
        // Buscar y reemplazar variables del formato {variable}
        char *start = strchr(output, '{');
        while (start != NULL) {
            char *end = strchr(start, '}');
            if (end != NULL) {
                char var_name[MAX_TOKEN];
                int var_len = end - start - 1;
                strncpy(var_name, start + 1, var_len);
                var_name[var_len] = '\0';
                
                int var_value = get_variable_value(var_name);
                char replacement[MAX_TOKEN];
                sprintf(replacement, "%d", var_value);
                
                // Reemplazar en el string
                memmove(start + strlen(replacement), end + 1, strlen(end + 1) + 1);
                memcpy(start, replacement, strlen(replacement));
                
                start = strchr(start + strlen(replacement), '{');
            } else {
                break;
            }
        }
        
        printf("%s%s\n", current_lang.output_prefix, output);
    }
    else if (strstr(line, "calcular:")) {
        char *calc_start = strchr(line, ':') + 1;
        trim_whitespace(calc_start);
        int result = evaluate_expression(calc_start);
        printf("%s%d\n", current_lang.output_prefix, result);
    }
    else if (strstr(line, "variable:")) {
        char *var_start = strchr(line, ':') + 1;
        trim_whitespace(var_start);
        
        char *equal_pos = strchr(var_start, '=');
        if (equal_pos) {
            char var_name[MAX_TOKEN];
            int name_len = equal_pos - var_start;
            strncpy(var_name, var_start, name_len);
            var_name[name_len] = '\0';
            trim_whitespace(var_name);
            
            char *value_str = equal_pos + 1;
            trim_whitespace(value_str);
            
            int value = evaluate_expression(value_str);
            set_variable(var_name, value);
            
            printf("%sVariable '%s' = %d\n", current_lang.output_prefix, var_name, value);
        }
    }
    else if (strstr(line, "si:")) {
        char *condition_start = strchr(line, ':') + 1;
        trim_whitespace(condition_start);
        current_lang.if_condition_result = evaluate_condition(condition_start);
        
        if (debug_mode) {
            printf("[DEBUG] Condición evaluada: %s = %s\n", 
                   condition_start, current_lang.if_condition_result ? "verdadero" : "falso");
        }
    }
    else if (strstr(line, "entonces:") && current_lang.if_condition_result) {
        char *then_start = strchr(line, ':') + 1;
        trim_whitespace(then_start);
        parse_lilou_definition(then_start);
    }
    else if (strstr(line, "sino:") && !current_lang.if_condition_result) {
        char *else_start = strchr(line, ':') + 1;
        trim_whitespace(else_start);
        parse_lilou_definition(else_start);
    }
    else if (strstr(line, "repetir:")) {
        char *repeat_start = strchr(line, ':') + 1;
        trim_whitespace(repeat_start);
        current_lang.loop_max = evaluate_expression(repeat_start);
        current_lang.loop_count = 0;
        current_lang.loop_active = 1;
        
        if (debug_mode) {
            printf("[DEBUG] Iniciando bucle de %d repeticiones\n", current_lang.loop_max);
        }
    }
    else if (strstr(line, "hacer:") && current_lang.loop_active) {
        char *do_start = strchr(line, ':') + 1;
        trim_whitespace(do_start);
        
        while (current_lang.loop_count < current_lang.loop_max) {
            set_variable("i", current_lang.loop_count);
            parse_lilou_definition(do_start);
            current_lang.loop_count++;
        }
        current_lang.loop_active = 0;
    }
    else if (strstr(line, "aleatorio:")) {
        char *range_start = strchr(line, ':') + 1;
        trim_whitespace(range_start);
        
        char *dash_pos = strchr(range_start, '-');
        if (dash_pos) {
            int min = evaluate_expression(range_start);
            int max = evaluate_expression(dash_pos + 1);
            int random_num = min + rand() % (max - min + 1);
            printf("%s%d\n", current_lang.output_prefix, random_num);
        } else {
            int max = evaluate_expression(range_start);
            int random_num = rand() % max;
            printf("%s%d\n", current_lang.output_prefix, random_num);
        }
    }
    else if (strstr(line, "escribir_archivo:")) {
        char *file_start = strchr(line, ':') + 1;
        trim_whitespace(file_start);
        
        char *comma_pos = strchr(file_start, ',');
        if (comma_pos) {
            char filename[MAX_TOKEN];
            int name_len = comma_pos - file_start;
            strncpy(filename, file_start, name_len);
            filename[name_len] = '\0';
            trim_whitespace(filename);
            
            char *content = comma_pos + 1;
            trim_whitespace(content);
            
            FILE *file = fopen(filename, "w");
            if (file) {
                fprintf(file, "%s\n", content);
                fclose(file);
                printf("%sArchivo '%s' creado\n", current_lang.output_prefix, filename);
            } else {
                printf("%s\n", current_lang.error_messages[2]);
            }
        }
    }
    else if (strstr(line, "leer_archivo:")) {
        char *filename = strchr(line, ':') + 1;
        trim_whitespace(filename);
        
        FILE *file = fopen(filename, "r");
        if (file) {
            char file_line[MAX_LINE];
            while (fgets(file_line, sizeof(file_line), file)) {
                trim_whitespace(file_line);
                printf("%s%s\n", current_lang.output_prefix, file_line);
            }
            fclose(file);
        } else {
            printf("%s\n", current_lang.error_messages[2]);
        }
    }
    else if (strstr(line, "funcion:")) {
        char *func_start = strchr(line, ':') + 1;
        trim_whitespace(func_start);
        
        char *brace_pos = strchr(func_start, '{');
        if (brace_pos) {
            char func_name[MAX_TOKEN];
            int name_len = brace_pos - func_start;
            strncpy(func_name, func_start, name_len);
            func_name[name_len] = '\0';
            trim_whitespace(func_name);
            
            char *code_start = brace_pos + 1;
            char *end_brace = strrchr(code_start, '}');
            if (end_brace) {
                *end_brace = '\0';
                
                if (current_lang.func_count < MAX_FUNCTIONS) {
                    strcpy(current_lang.functions[current_lang.func_count].name, func_name);
                    strcpy(current_lang.functions[current_lang.func_count].code, code_start);
                    current_lang.func_count++;
                    
                    printf("%sFunción '%s' definida\n", current_lang.output_prefix, func_name);
                }
            }
        }
    }
    else if (strstr(line, "llamar:")) {
        char *func_name = strchr(line, ':') + 1;
        trim_whitespace(func_name);
        execute_function(func_name);
    }
    else if (strstr(line, "debug:")) {
        char *debug_cmd = strchr(line, ':') + 1;
        trim_whitespace(debug_cmd);
        
        if (strcmp(debug_cmd, "on") == 0) {
            debug_mode = 1;
            printf("%sModo debug activado\n", current_lang.output_prefix);
        } else if (strcmp(debug_cmd, "off") == 0) {
            debug_mode = 0;
            printf("%sModo debug desactivado\n", current_lang.output_prefix);
        } else if (strcmp(debug_cmd, "variables") == 0) {
            printf("%s=== Variables actuales ===\n", current_lang.output_prefix);
            for (int i = 0; i < current_lang.var_count; i++) {
                printf("%s%s = %d\n", current_lang.output_prefix, 
                       current_lang.variables[i].name, current_lang.variables[i].value);
            }
        }
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
           code_file, strlen(current_lang.name) > 0 ? current_lang.name : "Sin nombre");
    
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
            
            // Lista de comandos predefinidos
            char *predefined_commands[] = {
                "mostrar:", "calcular:", "variable:", "si:", "entonces:", "sino:",
                "repetir:", "hacer:", "aleatorio:", "escribir_archivo:", "leer_archivo:",
                "funcion:", "llamar:", "debug:"
            };
            int predefined_count = 14;
            
            for (int i = 0; i < predefined_count; i++) {
                if (strstr(line, predefined_commands[i])) {
                    parse_lilou_definition(line);
                    found = 1;
                    break;
                }
            }
            
            if (!found) {
                for (int i = 0; i < current_lang.keyword_count; i++) {
                    if (strstr(line, current_lang.keywords[i])) {
                        parse_lilou_definition(line);
                        found = 1;
                        break;
                    }
                }
            }
            
            if (!found && strlen(line) > 0 && line[0] != '/' && line[0] != '#') {
                printf("%s: '%s'\n", current_lang.error_messages[0], line);
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
    
    printf("\n=== Comandos disponibles ===\n");
    printf("- mostrar: Mostrar texto en consola\n");
    printf("- calcular: Realizar operaciones matemáticas\n");
    printf("- variable: Crear y asignar variables\n");
    printf("- si/entonces/sino: Estructuras condicionales\n");
    printf("- repetir/hacer: Bucles\n");
    printf("- aleatorio: Generar números aleatorios\n");
    printf("- escribir_archivo/leer_archivo: Manejo de archivos\n");
    printf("- funcion/llamar: Definir y ejecutar funciones\n");
    printf("- debug: Herramientas de depuración\n");
    
    if (strlen(current_lang.name) > 0) {
        printf("✓ Definición de lenguaje válida!\n");
    } else {
        printf("⚠ Advertencia: Falta el nombre del lenguaje\n");
    }
}

void show_help() {
    printf("=== LILOU - Meta-lenguaje de programación ===\n");
    printf("Versión 2.0 - Con funcionalidades avanzadas\n\n");
    printf("Uso:\n");
    printf("  ./main tester-lilou <archivo.lilou>     - Probar definición de lenguaje\n");
    printf("  ./main lilou <definicion.lilou> <codigo> - Ejecutar código con lenguaje definido\n");
    printf("  ./main ayuda                            - Mostrar esta ayuda\n");
    printf("  ./main ejemplos                         - Mostrar ejemplos de uso\n");
    printf("\nEjemplos:\n");
    printf("  ./main tester-lilou mi_lenguaje.lilou\n");
    printf("  ./main lilou mi_lenguaje.lilou codigo.gg\n");
    printf("\nNuevas funcionalidades v2.0:\n");
    printf("  ✓ Variables y almacenamiento de datos\n");
    printf("  ✓ Estructuras de control (si/entonces/sino)\n");
    printf("  ✓ Bucles (repetir/hacer)\n");
    printf("  ✓ Funciones personalizadas\n");
    printf("  ✓ Manejo de archivos\n");
    printf("  ✓ Generación de números aleatorios\n");
    printf("  ✓ Modo debug\n");
    printf("  ✓ Comentarios en código\n");
}

void show_examples() {
    printf("=== EJEMPLOS DE LILOU 2.0 ===\n\n");
    
    printf("1. VARIABLES:\n");
    printf("variable: contador = 0\n");
    printf("variable: suma = 5 + 3\n");
    printf("mostrar: El contador es {contador}\n\n");
    
    printf("2. CONDICIONALES:\n");
    printf("si: contador > 5\n");
    printf("entonces: mostrar: El contador es mayor que 5\n");
    printf("sino: mostrar: El contador es menor o igual que 5\n\n");
    
    printf("3. BUCLES:\n");
    printf("repetir: 5\n");
    printf("hacer: mostrar: Repetición número {i}\n\n");
    
    printf("4. FUNCIONES:\n");
    printf("funcion: saludar { mostrar: ¡Hola desde la función! }\n");
    printf("llamar: saludar\n\n");
    
    printf("5. ARCHIVOS:\n");
    printf("escribir_archivo: mi_archivo.txt, Contenido del archivo\n");
    printf("leer_archivo: mi_archivo.txt\n\n");
    
    printf("6. NÚMEROS ALEATORIOS:\n");
    printf("aleatorio: 1-10\n");
    printf("aleatorio: 100\n\n");
    
    printf("7. DEBUG:\n");
    printf("debug: on\n");
    printf("debug: variables\n");
    printf("debug: off\n\n");
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
    else if (strcmp(argv[1], "ejemplos") == 0) {
        show_examples();
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
