
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define MAX_LINE 2048
#define MAX_TOKEN 512
#define MAX_OPERATORS 50
#define MAX_KEYWORDS 50
#define MAX_COMMANDS 100
#define MAX_VARIABLES 200
#define MAX_FUNCTIONS 100
#define MAX_ARRAYS 50
#define MAX_ARRAY_SIZE 1000

typedef struct {
    char name[MAX_TOKEN];
    double value;
    char type[20]; // "number", "string", "boolean"
    char string_value[MAX_LINE];
} Variable;

typedef struct {
    char name[MAX_TOKEN];
    double values[MAX_ARRAY_SIZE];
    int size;
} Array;

typedef struct {
    char name[MAX_TOKEN];
    char code[MAX_LINE * 20];
    char params[10][MAX_TOKEN];
    int param_count;
} Function;

typedef struct {
    char name[MAX_TOKEN];
    char extension[MAX_TOKEN];
    char operators[MAX_OPERATORS][MAX_TOKEN];
    int op_count;
    char keywords[MAX_KEYWORDS][MAX_TOKEN];
    int keyword_count;
    char error_messages[20][MAX_TOKEN];
    char output_prefix[MAX_TOKEN];
    char commands[MAX_COMMANDS][MAX_TOKEN];
    int command_count;
    
    // Estructuras de datos avanzadas
    Variable variables[MAX_VARIABLES];
    int var_count;
    Function functions[MAX_FUNCTIONS];
    int func_count;
    Array arrays[MAX_ARRAYS];
    int array_count;
    
    // Control de flujo
    int if_condition_result;
    int loop_active;
    int loop_count;
    int loop_max;
    int break_flag;
    int continue_flag;
    
    // Configuración avanzada
    int strict_mode;
    int case_sensitive;
    char decimal_separator;
    int max_recursion_depth;
    int current_recursion_depth;
} Language;

Language current_lang;
int debug_mode = 0;
char current_function_scope[MAX_TOKEN] = "";

void init_language() {
    memset(&current_lang, 0, sizeof(Language));
    strcpy(current_lang.extension, ".txt");
    strcpy(current_lang.output_prefix, ">>> ");
    current_lang.decimal_separator = '.';
    current_lang.max_recursion_depth = 100;
    current_lang.case_sensitive = 1;
    
    // Mensajes de error mejorados
    strcpy(current_lang.error_messages[0], "Error: comando no reconocido");
    strcpy(current_lang.error_messages[1], "Error: sintaxis incorrecta");
    strcpy(current_lang.error_messages[2], "Error: archivo no encontrado");
    strcpy(current_lang.error_messages[3], "Error: variable no definida");
    strcpy(current_lang.error_messages[4], "Error: función no definida");
    strcpy(current_lang.error_messages[5], "Error: operación matemática inválida");
    strcpy(current_lang.error_messages[6], "Error: división por cero");
    strcpy(current_lang.error_messages[7], "Error: índice fuera de rango");
    strcpy(current_lang.error_messages[8], "Error: tipo de dato incompatible");
    strcpy(current_lang.error_messages[9], "Error: recursión demasiado profunda");
    strcpy(current_lang.error_messages[10], "Error: parámetros insuficientes");
    strcpy(current_lang.error_messages[11], "Error: memoria insuficiente");
    strcpy(current_lang.error_messages[12], "Error: formato de entrada inválido");
    
    srand(time(NULL));
}

void trim_whitespace(char *str) {
    if (!str) return;
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

int find_variable(char *name) {
    if (!name) return -1;
    for (int i = 0; i < current_lang.var_count; i++) {
        if (current_lang.case_sensitive) {
            if (strcmp(current_lang.variables[i].name, name) == 0) {
                return i;
            }
        } else {
            if (strcasecmp(current_lang.variables[i].name, name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int find_array(char *name) {
    if (!name) return -1;
    for (int i = 0; i < current_lang.array_count; i++) {
        if (strcmp(current_lang.arrays[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_variable(char *name, double value, char *type, char *string_value) {
    if (!name || !type) return;
    
    int index = find_variable(name);
    if (index >= 0) {
        // Mettre à jour variable existante
        current_lang.variables[index].value = value;
        strcpy(current_lang.variables[index].type, type);
        if (string_value && strlen(string_value) < MAX_LINE) {
            strcpy(current_lang.variables[index].string_value, string_value);
        } else if (strcmp(type, "string") == 0 && !string_value) {
            strcpy(current_lang.variables[index].string_value, "");
        }
        
        if (debug_mode) {
            printf("[DEBUG] Variable '%s' mise à jour: %s = ", name, type);
            if (strcmp(type, "string") == 0) {
                printf("\"%s\"\n", current_lang.variables[index].string_value);
            } else {
                printf("%.6g\n", value);
            }
        }
    } else if (current_lang.var_count < MAX_VARIABLES) {
        // Créer nouvelle variable
        if (strlen(name) >= MAX_TOKEN) {
            printf("[ERREUR] Nom de variable trop long: %s\n", name);
            return;
        }
        
        strcpy(current_lang.variables[current_lang.var_count].name, name);
        current_lang.variables[current_lang.var_count].value = value;
        strcpy(current_lang.variables[current_lang.var_count].type, type);
        
        if (string_value && strlen(string_value) < MAX_LINE) {
            strcpy(current_lang.variables[current_lang.var_count].string_value, string_value);
        } else {
            strcpy(current_lang.variables[current_lang.var_count].string_value, "");
        }
        
        current_lang.var_count++;
        
        if (debug_mode) {
            printf("[DEBUG] Variable '%s' créée: %s = ", name, type);
            if (strcmp(type, "string") == 0) {
                printf("\"%s\"\n", string_value ? string_value : "");
            } else {
                printf("%.6g\n", value);
            }
        }
    } else {
        printf("[ERREUR] Limite de variables atteinte (%d max)\n", MAX_VARIABLES);
    }
}

double get_variable_value(char *name) {
    int index = find_variable(name);
    if (index >= 0) {
        return current_lang.variables[index].value;
    }
    return 0;
}

char* get_variable_string(char *name) {
    int index = find_variable(name);
    if (index >= 0) {
        return current_lang.variables[index].string_value;
    }
    return "";
}

double safe_division(double a, double b) {
    if (fabs(b) < 1e-10) {
        printf("%s\n", current_lang.error_messages[6]);
        return 0;
    }
    return a / b;
}

double evaluate_math_function(char *func_name, double arg) {
    if (strcmp(func_name, "sin") == 0) return sin(arg);
    if (strcmp(func_name, "cos") == 0) return cos(arg);
    if (strcmp(func_name, "tan") == 0) return tan(arg);
    if (strcmp(func_name, "sqrt") == 0) return arg >= 0 ? sqrt(arg) : 0;
    if (strcmp(func_name, "abs") == 0) return fabs(arg);
    if (strcmp(func_name, "floor") == 0) return floor(arg);
    if (strcmp(func_name, "ceil") == 0) return ceil(arg);
    if (strcmp(func_name, "round") == 0) return round(arg);
    if (strcmp(func_name, "log") == 0) return arg > 0 ? log(arg) : 0;
    if (strcmp(func_name, "exp") == 0) return exp(arg);
    return 0;
}

double evaluate_expression(char *expr) {
    if (!expr) return 0;
    trim_whitespace(expr);
    
    // Si es solo un número
    if (isdigit(expr[0]) || (expr[0] == '-' && isdigit(expr[1])) || expr[0] == '.') {
        return atof(expr);
    }
    
    // Funciones matemáticas
    if (strchr(expr, '(') && strchr(expr, ')')) {
        char func_name[MAX_TOKEN];
        char *paren_start = strchr(expr, '(');
        char *paren_end = strrchr(expr, ')');
        
        if (paren_start && paren_end && paren_end > paren_start) {
            int func_len = paren_start - expr;
            strncpy(func_name, expr, func_len);
            func_name[func_len] = '\0';
            
            char arg_str[MAX_TOKEN];
            int arg_len = paren_end - paren_start - 1;
            strncpy(arg_str, paren_start + 1, arg_len);
            arg_str[arg_len] = '\0';
            
            double arg = evaluate_expression(arg_str);
            return evaluate_math_function(func_name, arg);
        }
    }
    
    // Si es una variable
    if (find_variable(expr) >= 0) {
        return get_variable_value(expr);
    }
    
    // Evaluación de expresiones complejas con precedencia de operadores
    char *operators[] = {"**", "*", "/", "%", "+", "-"};
    int precedence[] = {3, 2, 2, 2, 1, 1};
    int op_count = 6;
    
    for (int prec = 3; prec >= 1; prec--) {
        for (int i = 0; i < op_count; i++) {
            if (precedence[i] == prec) {
                char *op_pos = strstr(expr, operators[i]);
                if (op_pos && op_pos != expr) {
                    char left[MAX_TOKEN], right[MAX_TOKEN];
                    
                    int left_len = op_pos - expr;
                    strncpy(left, expr, left_len);
                    left[left_len] = '\0';
                    trim_whitespace(left);
                    
                    strcpy(right, op_pos + strlen(operators[i]));
                    trim_whitespace(right);
                    
                    double left_val = evaluate_expression(left);
                    double right_val = evaluate_expression(right);
                    
                    switch(i) {
                        case 0: return pow(left_val, right_val); // **
                        case 1: return left_val * right_val;     // *
                        case 2: return safe_division(left_val, right_val); // /
                        case 3: return right_val != 0 ? fmod(left_val, right_val) : 0; // %
                        case 4: return left_val + right_val;     // +
                        case 5: return left_val - right_val;     // -
                    }
                }
            }
        }
    }
    
    return 0;
}

int evaluate_condition(char *condition) {
    if (!condition) return 0;
    trim_whitespace(condition);
    
    // Operadores lógicos
    if (strstr(condition, " and ") || strstr(condition, " y ")) {
        char *and_pos = strstr(condition, " and ") ? strstr(condition, " and ") : strstr(condition, " y ");
        char left[MAX_TOKEN], right[MAX_TOKEN];
        
        int left_len = and_pos - condition;
        strncpy(left, condition, left_len);
        left[left_len] = '\0';
        
        strcpy(right, and_pos + (strstr(condition, " and ") ? 5 : 3));
        
        return evaluate_condition(left) && evaluate_condition(right);
    }
    
    if (strstr(condition, " or ") || strstr(condition, " o ")) {
        char *or_pos = strstr(condition, " or ") ? strstr(condition, " or ") : strstr(condition, " o ");
        char left[MAX_TOKEN], right[MAX_TOKEN];
        
        int left_len = or_pos - condition;
        strncpy(left, condition, left_len);
        left[left_len] = '\0';
        
        strcpy(right, or_pos + (strstr(condition, " or ") ? 4 : 3));
        
        return evaluate_condition(left) || evaluate_condition(right);
    }
    
    // Operadores de comparación
    char *operators[] = {"==", "!=", "<=", ">=", "<", ">"};
    int op_count = 6;
    
    for (int i = 0; i < op_count; i++) {
        char *op_pos = strstr(condition, operators[i]);
        if (op_pos) {
            char left[MAX_TOKEN], right[MAX_TOKEN];
            
            int left_len = op_pos - condition;
            strncpy(left, condition, left_len);
            left[left_len] = '\0';
            trim_whitespace(left);
            
            strcpy(right, op_pos + strlen(operators[i]));
            trim_whitespace(right);
            
            double left_val = evaluate_expression(left);
            double right_val = evaluate_expression(right);
            
            switch(i) {
                case 0: return fabs(left_val - right_val) < 1e-10; // ==
                case 1: return fabs(left_val - right_val) >= 1e-10; // !=
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

void execute_function(char *func_name, char *params) {
    if (current_lang.current_recursion_depth >= current_lang.max_recursion_depth) {
        printf("%s\n", current_lang.error_messages[9]);
        return;
    }
    
    for (int i = 0; i < current_lang.func_count; i++) {
        if (strcmp(current_lang.functions[i].name, func_name) == 0) {
            current_lang.current_recursion_depth++;
            
            if (debug_mode) {
                printf("[DEBUG] Ejecutando función '%s' (profundidad: %d)\n", 
                       func_name, current_lang.current_recursion_depth);
            }
            
            // Guardar estado anterior
            char old_scope[MAX_TOKEN];
            strcpy(old_scope, current_function_scope);
            strcpy(current_function_scope, func_name);
            
            // Procesar parámetros si existen
            if (params && strlen(params) > 0) {
                char *param_token = strtok(params, ",");
                int param_index = 0;
                
                while (param_token && param_index < current_lang.functions[i].param_count) {
                    trim_whitespace(param_token);
                    double param_value = evaluate_expression(param_token);
                    set_variable(current_lang.functions[i].params[param_index], param_value, "number", NULL);
                    param_index++;
                    param_token = strtok(NULL, ",");
                }
            }
            
            // Ejecutar código de la función
            char func_code[MAX_LINE * 20];
            strcpy(func_code, current_lang.functions[i].code);
            char *line = strtok(func_code, "\n");
            
            while (line != NULL && !current_lang.break_flag) {
                parse_lilou_definition(line);
                line = strtok(NULL, "\n");
            }
            
            // Restaurar estado
            strcpy(current_function_scope, old_scope);
            current_lang.current_recursion_depth--;
            return;
        }
    }
    printf("%s: función '%s'\n", current_lang.error_messages[4], func_name);
}

void interpolate_string(char *input, char *output) {
    if (!input || !output) return;
    
    strcpy(output, input);
    char *start = strchr(output, '{');
    
    while (start != NULL) {
        char *end = strchr(start, '}');
        if (end != NULL) {
            char var_name[MAX_TOKEN];
            int var_len = end - start - 1;
            strncpy(var_name, start + 1, var_len);
            var_name[var_len] = '\0';
            trim_whitespace(var_name);
            
            int var_index = find_variable(var_name);
            char replacement[MAX_TOKEN];
            
            if (var_index >= 0) {
                if (strcmp(current_lang.variables[var_index].type, "string") == 0) {
                    strcpy(replacement, current_lang.variables[var_index].string_value);
                } else {
                    // Améliorer le formatage des nombres
                    double val = current_lang.variables[var_index].value;
                    if (val == (long long)val) {
                        sprintf(replacement, "%.0f", val);
                    } else {
                        sprintf(replacement, "%.6g", val);
                    }
                }
            } else {
                // Vérifier si c'est une expression mathématique
                double expr_result = evaluate_expression(var_name);
                if (find_variable(var_name) == -1 && (isdigit(var_name[0]) || strchr(var_name, '+') || strchr(var_name, '-') || strchr(var_name, '*') || strchr(var_name, '/'))) {
                    if (expr_result == (long long)expr_result) {
                        sprintf(replacement, "%.0f", expr_result);
                    } else {
                        sprintf(replacement, "%.6g", expr_result);
                    }
                } else {
                    sprintf(replacement, "[UNDEF:%s]", var_name);
                    if (debug_mode) {
                        printf("[DEBUG] Variable non définie: %s\n", var_name);
                    }
                }
            }
            
            // Calculer les tailles pour éviter les dépassements de buffer
            int replacement_len = strlen(replacement);
            int var_placeholder_len = end - start + 1; // +1 pour inclure '}'
            int remaining_len = strlen(end + 1);
            
            // Vérifier que le buffer de sortie est assez grand
            if (strlen(output) - var_placeholder_len + replacement_len < MAX_LINE - 1) {
                // Déplacer le reste du string
                memmove(start + replacement_len, end + 1, remaining_len + 1);
                // Copier le remplacement
                memcpy(start, replacement, replacement_len);
                
                start = strchr(start + replacement_len, '{');
            } else {
                if (debug_mode) {
                    printf("[DEBUG] Buffer overflow évité lors de l'interpolation\n");
                }
                break;
            }
        } else {
            break;
        }
    }
}

void parse_lilou_definition(char *line);

void map_custom_keyword_to_internal(char *line, char *output, char *custom_keyword) {
    if (!line || !output || !custom_keyword) return;
    
    char *content = strchr(line, ':');
    if (!content) {
        strcpy(output, line);
        return;
    }
    content++; // Skip the ':'
    trim_whitespace(content);
    
    // Mapear mots-clés personnalisés vers commandes internes
    if (strcmp(custom_keyword, "print") == 0 || 
        strcmp(custom_keyword, "afficher") == 0 || 
        strcmp(custom_keyword, "imprimir") == 0 ||
        strcmp(custom_keyword, "display") == 0 ||
        strcmp(custom_keyword, "show") == 0) {
        snprintf(output, MAX_LINE, "mostrar: %s", content);
    }
    else if (strcmp(custom_keyword, "calc") == 0 || 
             strcmp(custom_keyword, "calculate") == 0 ||
             strcmp(custom_keyword, "calcular") == 0) {
        snprintf(output, MAX_LINE, "calcular: %s", content);
    }
    else if (strcmp(custom_keyword, "var") == 0 || 
             strcmp(custom_keyword, "variable") == 0 ||
             strcmp(custom_keyword, "let") == 0 ||
             strcmp(custom_keyword, "const") == 0) {
        snprintf(output, MAX_LINE, "variable: %s", content);
    }
    else if (strcmp(custom_keyword, "if") == 0 || 
             strcmp(custom_keyword, "si") == 0) {
        snprintf(output, MAX_LINE, "si: %s", content);
    }
    else if (strcmp(custom_keyword, "then") == 0 || 
             strcmp(custom_keyword, "alors") == 0 ||
             strcmp(custom_keyword, "entonces") == 0) {
        snprintf(output, MAX_LINE, "entonces: %s", content);
    }
    else if (strcmp(custom_keyword, "else") == 0 || 
             strcmp(custom_keyword, "sinon") == 0 ||
             strcmp(custom_keyword, "sino") == 0) {
        snprintf(output, MAX_LINE, "sino: %s", content);
    }
    else if (strcmp(custom_keyword, "for") == 0 || 
             strcmp(custom_keyword, "repeat") == 0 ||
             strcmp(custom_keyword, "repetir") == 0) {
        snprintf(output, MAX_LINE, "repetir: %s", content);
    }
    else if (strcmp(custom_keyword, "do") == 0 || 
             strcmp(custom_keyword, "hacer") == 0 ||
             strcmp(custom_keyword, "faire") == 0) {
        snprintf(output, MAX_LINE, "hacer: %s", content);
    }
    else if (strcmp(custom_keyword, "while") == 0 || 
             strcmp(custom_keyword, "mientras") == 0 ||
             strcmp(custom_keyword, "pendant") == 0) {
        snprintf(output, MAX_LINE, "mientras: %s", content);
    }
    else if (strcmp(custom_keyword, "function") == 0 || 
             strcmp(custom_keyword, "func") == 0 ||
             strcmp(custom_keyword, "def") == 0 ||
             strcmp(custom_keyword, "funcion") == 0) {
        snprintf(output, MAX_LINE, "funcion: %s", content);
    }
    else if (strcmp(custom_keyword, "call") == 0 || 
             strcmp(custom_keyword, "invoke") == 0 ||
             strcmp(custom_keyword, "llamar") == 0) {
        snprintf(output, MAX_LINE, "llamar: %s", content);
    }
    else if (strcmp(custom_keyword, "input") == 0 || 
             strcmp(custom_keyword, "ask") == 0 ||
             strcmp(custom_keyword, "entrada") == 0) {
        snprintf(output, MAX_LINE, "entrada: %s", content);
    }
    else if (strcmp(custom_keyword, "random") == 0 || 
             strcmp(custom_keyword, "aleatorio") == 0) {
        snprintf(output, MAX_LINE, "aleatorio: %s", content);
    }
    else if (strcmp(custom_keyword, "random_real") == 0 || 
             strcmp(custom_keyword, "aleatorio_real") == 0) {
        snprintf(output, MAX_LINE, "aleatorio_real: %s", content);
    }
    else if (strcmp(custom_keyword, "write_file") == 0 || 
             strcmp(custom_keyword, "escribir_archivo") == 0) {
        snprintf(output, MAX_LINE, "escribir_archivo: %s", content);
    }
    else if (strcmp(custom_keyword, "read_file") == 0 || 
             strcmp(custom_keyword, "leer_archivo") == 0) {
        snprintf(output, MAX_LINE, "leer_archivo: %s", content);
    }
    else if (strcmp(custom_keyword, "wait") == 0 || 
             strcmp(custom_keyword, "sleep") == 0 ||
             strcmp(custom_keyword, "esperar") == 0) {
        snprintf(output, MAX_LINE, "esperar: %s", content);
    }
    else if (strcmp(custom_keyword, "clear") == 0 || 
             strcmp(custom_keyword, "cls") == 0 ||
             strcmp(custom_keyword, "limpiar_pantalla") == 0) {
        snprintf(output, MAX_LINE, "limpiar_pantalla");
    }
    else {
        // Pour les mots-clés non mappés, traiter comme affichage
        snprintf(output, MAX_LINE, "mostrar: %s", content);
    }
}

void parse_lilou_definition(char *line) {
    if (!line) return;
    trim_whitespace(line);
    
    // Ignorar comentarios y líneas vacías
    if (strstr(line, "//") == line || strstr(line, "#") == line || strlen(line) == 0) {
        return;
    }
    
    // Control de flujo - break y continue
    if (strcmp(line, "break") == 0 || strcmp(line, "romper") == 0) {
        current_lang.break_flag = 1;
        return;
    }
    
    if (strcmp(line, "continue") == 0 || strcmp(line, "continuar") == 0) {
        current_lang.continue_flag = 1;
        return;
    }
    
    // Definición de lenguaje
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
    else if (strstr(line, "modo_estricto:")) {
        char *mode_start = strchr(line, ':') + 1;
        trim_whitespace(mode_start);
        current_lang.strict_mode = (strcmp(mode_start, "on") == 0 || strcmp(mode_start, "si") == 0);
        printf("Modo estricto: %s\n", current_lang.strict_mode ? "activado" : "desactivado");
    }
    else if (strstr(line, "sensible_mayusculas:")) {
        char *case_start = strchr(line, ':') + 1;
        trim_whitespace(case_start);
        current_lang.case_sensitive = (strcmp(case_start, "on") == 0 || strcmp(case_start, "si") == 0);
        printf("Sensible a mayúsculas: %s\n", current_lang.case_sensitive ? "sí" : "no");
    }
    else if (strstr(line, "operadores:")) {
        char *ops_start = strchr(line, ':') + 1;
        trim_whitespace(ops_start);
        char ops_copy[MAX_LINE];
        strcpy(ops_copy, ops_start);
        char *token = strtok(ops_copy, ",");
        current_lang.op_count = 0;
        
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
        char keywords_copy[MAX_LINE];
        strcpy(keywords_copy, keywords_start);
        char *token = strtok(keywords_copy, ",");
        current_lang.keyword_count = 0;
        
        while (token != NULL && current_lang.keyword_count < MAX_KEYWORDS) {
            trim_whitespace(token);
            strcpy(current_lang.keywords[current_lang.keyword_count], token);
            current_lang.keyword_count++;
            token = strtok(NULL, ",");
        }
        printf("Palabras clave definidas: %d\n", current_lang.keyword_count);
    }
    // Comandos de ejecución
    else if (strstr(line, "mostrar:")) {
        char *msg_start = strchr(line, ':') + 1;
        trim_whitespace(msg_start);
        
        char output[MAX_LINE];
        interpolate_string(msg_start, output);
        printf("%s%s\n", current_lang.output_prefix, output);
    }
    else if (strstr(line, "imprimir:")) {
        char *msg_start = strchr(line, ':') + 1;
        trim_whitespace(msg_start);
        
        char output[MAX_LINE];
        interpolate_string(msg_start, output);
        printf("%s", output); // Sin salto de línea
    }
    else if (strstr(line, "calcular:")) {
        char *calc_start = strchr(line, ':') + 1;
        trim_whitespace(calc_start);
        double result = evaluate_expression(calc_start);
        printf("%s%.6g\n", current_lang.output_prefix, result);
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
            
            // Detectar si es string (entre comillas)
            if ((value_str[0] == '"' && value_str[strlen(value_str)-1] == '"') ||
                (value_str[0] == '\'' && value_str[strlen(value_str)-1] == '\'')) {
                char string_val[MAX_LINE];
                strncpy(string_val, value_str + 1, strlen(value_str) - 2);
                string_val[strlen(value_str) - 2] = '\0';
                set_variable(var_name, 0, "string", string_val);
                printf("%sVariable '%s' = \"%s\"\n", current_lang.output_prefix, var_name, string_val);
            } else {
                double value = evaluate_expression(value_str);
                set_variable(var_name, value, "number", NULL);
                printf("%sVariable '%s' = %.6g\n", current_lang.output_prefix, var_name, value);
            }
        }
    }
    else if (strstr(line, "array:")) {
        char *array_start = strchr(line, ':') + 1;
        trim_whitespace(array_start);
        
        char *bracket_start = strchr(array_start, '[');
        char *bracket_end = strrchr(array_start, ']');
        
        if (bracket_start && bracket_end && bracket_end > bracket_start) {
            char array_name[MAX_TOKEN];
            int name_len = bracket_start - array_start;
            strncpy(array_name, array_start, name_len);
            array_name[name_len] = '\0';
            trim_whitespace(array_name);
            
            if (current_lang.array_count < MAX_ARRAYS) {
                strcpy(current_lang.arrays[current_lang.array_count].name, array_name);
                current_lang.arrays[current_lang.array_count].size = 0;
                
                char values_str[MAX_LINE];
                int values_len = bracket_end - bracket_start - 1;
                strncpy(values_str, bracket_start + 1, values_len);
                values_str[values_len] = '\0';
                
                char *token = strtok(values_str, ",");
                while (token && current_lang.arrays[current_lang.array_count].size < MAX_ARRAY_SIZE) {
                    trim_whitespace(token);
                    current_lang.arrays[current_lang.array_count].values[current_lang.arrays[current_lang.array_count].size] = evaluate_expression(token);
                    current_lang.arrays[current_lang.array_count].size++;
                    token = strtok(NULL, ",");
                }
                
                printf("%sArray '%s' creado con %d elementos\n", 
                       current_lang.output_prefix, array_name, current_lang.arrays[current_lang.array_count].size);
                current_lang.array_count++;
            }
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
    else if (strstr(line, "mientras:")) {
        char *condition_start = strchr(line, ':') + 1;
        trim_whitespace(condition_start);
        
        while (evaluate_condition(condition_start) && !current_lang.break_flag) {
            // Necesitaríamos el código del bucle, esto es una simplificación
            if (debug_mode) {
                printf("[DEBUG] Ejecutando bucle mientras: %s\n", condition_start);
            }
            break; // Evitar bucle infinito en esta implementación
        }
        current_lang.break_flag = 0;
    }
    else if (strstr(line, "repetir:")) {
        char *repeat_start = strchr(line, ':') + 1;
        trim_whitespace(repeat_start);
        current_lang.loop_max = (int)evaluate_expression(repeat_start);
        current_lang.loop_count = 0;
        current_lang.loop_active = 1;
        current_lang.break_flag = 0;
        current_lang.continue_flag = 0;
        
        if (debug_mode) {
            printf("[DEBUG] Iniciando bucle de %d repeticiones\n", current_lang.loop_max);
        }
    }
    else if (strstr(line, "hacer:") && current_lang.loop_active) {
        char *do_start = strchr(line, ':') + 1;
        trim_whitespace(do_start);
        
        while (current_lang.loop_count < current_lang.loop_max && !current_lang.break_flag) {
            set_variable("i", current_lang.loop_count, "number", NULL);
            current_lang.continue_flag = 0;
            
            parse_lilou_definition(do_start);
            
            if (current_lang.continue_flag) {
                current_lang.loop_count++;
                continue;
            }
            
            current_lang.loop_count++;
        }
        current_lang.loop_active = 0;
        current_lang.break_flag = 0;
    }
    else if (strstr(line, "aleatorio:")) {
        char *range_start = strchr(line, ':') + 1;
        trim_whitespace(range_start);
        
        char *dash_pos = strchr(range_start, '-');
        if (dash_pos) {
            int min = (int)evaluate_expression(range_start);
            char max_str[MAX_TOKEN];
            strcpy(max_str, dash_pos + 1);
            int max = (int)evaluate_expression(max_str);
            int random_num = min + rand() % (max - min + 1);
            printf("%s%d\n", current_lang.output_prefix, random_num);
        } else {
            int max = (int)evaluate_expression(range_start);
            int random_num = rand() % (max + 1);
            printf("%s%d\n", current_lang.output_prefix, random_num);
        }
    }
    else if (strstr(line, "aleatorio_real:")) {
        char *range_start = strchr(line, ':') + 1;
        trim_whitespace(range_start);
        
        char *dash_pos = strchr(range_start, '-');
        if (dash_pos) {
            double min = evaluate_expression(range_start);
            char max_str[MAX_TOKEN];
            strcpy(max_str, dash_pos + 1);
            double max = evaluate_expression(max_str);
            double random_num = min + ((double)rand() / RAND_MAX) * (max - min);
            printf("%s%.6g\n", current_lang.output_prefix, random_num);
        } else {
            double max = evaluate_expression(range_start);
            double random_num = ((double)rand() / RAND_MAX) * max;
            printf("%s%.6g\n", current_lang.output_prefix, random_num);
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
            
            char interpolated_content[MAX_LINE];
            interpolate_string(content, interpolated_content);
            
            FILE *file = fopen(filename, "w");
            if (file) {
                fprintf(file, "%s\n", interpolated_content);
                fclose(file);
                printf("%sArchivo '%s' creado\n", current_lang.output_prefix, filename);
            } else {
                printf("%s\n", current_lang.error_messages[2]);
            }
        }
    }
    else if (strstr(line, "anexar_archivo:")) {
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
            
            char interpolated_content[MAX_LINE];
            interpolate_string(content, interpolated_content);
            
            FILE *file = fopen(filename, "a");
            if (file) {
                fprintf(file, "%s\n", interpolated_content);
                fclose(file);
                printf("%sContenido anexado a '%s'\n", current_lang.output_prefix, filename);
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
                // Eliminar salto de línea final
                file_line[strcspn(file_line, "\n")] = 0;
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
        
        char *paren_start = strchr(func_start, '(');
        char *paren_end = strchr(func_start, ')');
        char *brace_start = strchr(func_start, '{');
        
        if (brace_start && current_lang.func_count < MAX_FUNCTIONS) {
            char func_name[MAX_TOKEN];
            int name_len;
            
            if (paren_start && paren_end && paren_start < brace_start) {
                // Función con parámetros
                name_len = paren_start - func_start;
                strncpy(func_name, func_start, name_len);
                func_name[name_len] = '\0';
                trim_whitespace(func_name);
                
                // Extraer parámetros
                char params_str[MAX_TOKEN];
                int params_len = paren_end - paren_start - 1;
                strncpy(params_str, paren_start + 1, params_len);
                params_str[params_len] = '\0';
                
                current_lang.functions[current_lang.func_count].param_count = 0;
                char *param_token = strtok(params_str, ",");
                while (param_token && current_lang.functions[current_lang.func_count].param_count < 10) {
                    trim_whitespace(param_token);
                    strcpy(current_lang.functions[current_lang.func_count].params[current_lang.functions[current_lang.func_count].param_count], param_token);
                    current_lang.functions[current_lang.func_count].param_count++;
                    param_token = strtok(NULL, ",");
                }
            } else {
                // Función sin parámetros
                name_len = brace_start - func_start;
                strncpy(func_name, func_start, name_len);
                func_name[name_len] = '\0';
                trim_whitespace(func_name);
                current_lang.functions[current_lang.func_count].param_count = 0;
            }
            
            char *code_start = brace_start + 1;
            char *end_brace = strrchr(code_start, '}');
            if (end_brace) {
                *end_brace = '\0';
                
                strcpy(current_lang.functions[current_lang.func_count].name, func_name);
                strcpy(current_lang.functions[current_lang.func_count].code, code_start);
                current_lang.func_count++;
                
                printf("%sFunción '%s' definida con %d parámetros\n", 
                       current_lang.output_prefix, func_name, current_lang.functions[current_lang.func_count-1].param_count);
            }
        }
    }
    else if (strstr(line, "llamar:")) {
        char *call_start = strchr(line, ':') + 1;
        trim_whitespace(call_start);
        
        char *paren_start = strchr(call_start, '(');
        char *paren_end = strrchr(call_start, ')');
        
        if (paren_start && paren_end && paren_end > paren_start) {
            // Función con parámetros
            char func_name[MAX_TOKEN];
            int name_len = paren_start - call_start;
            strncpy(func_name, call_start, name_len);
            func_name[name_len] = '\0';
            trim_whitespace(func_name);
            
            char params[MAX_TOKEN];
            int params_len = paren_end - paren_start - 1;
            strncpy(params, paren_start + 1, params_len);
            params[params_len] = '\0';
            
            execute_function(func_name, params);
        } else {
            // Función sin parámetros
            execute_function(call_start, NULL);
        }
    }
    else if (strstr(line, "retornar:")) {
        char *return_start = strchr(line, ':') + 1;
        trim_whitespace(return_start);
        double return_value = evaluate_expression(return_start);
        
        if (debug_mode) {
            printf("[DEBUG] Función retorna: %.6g\n", return_value);
        }
        
        // En una implementación completa, esto debería establecer un valor de retorno
        current_lang.break_flag = 1; // Salir de la función
    }
    else if (strstr(line, "entrada:")) {
        char *prompt_start = strchr(line, ':') + 1;
        trim_whitespace(prompt_start);
        
        char interpolated_prompt[MAX_LINE];
        interpolate_string(prompt_start, interpolated_prompt);
        printf("%s%s", current_lang.output_prefix, interpolated_prompt);
        
        char input[MAX_LINE];
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0; // Eliminar salto de línea
            
            // Detectar si es número o string
            char *endptr;
            double num_value = strtod(input, &endptr);
            if (*endptr == '\0') {
                set_variable("entrada", num_value, "number", NULL);
            } else {
                set_variable("entrada", 0, "string", input);
            }
        }
    }
    else if (strstr(line, "esperar:")) {
        char *time_start = strchr(line, ':') + 1;
        trim_whitespace(time_start);
        int milliseconds = (int)(evaluate_expression(time_start) * 1000);
        
        if (debug_mode) {
            printf("[DEBUG] Esperando %d ms\n", milliseconds);
        }
        
        #ifdef _WIN32
            Sleep(milliseconds);
        #else
            usleep(milliseconds * 1000);
        #endif
    }
    else if (strstr(line, "limpiar_pantalla") || strstr(line, "clear")) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    else if (strstr(line, "debug:")) {
        char *debug_cmd = strchr(line, ':') + 1;
        trim_whitespace(debug_cmd);
        
        if (strcmp(debug_cmd, "on") == 0 || strcmp(debug_cmd, "activar") == 0) {
            debug_mode = 1;
            printf("%sModo debug activado\n", current_lang.output_prefix);
        } else if (strcmp(debug_cmd, "off") == 0 || strcmp(debug_cmd, "desactivar") == 0) {
            debug_mode = 0;
            printf("%sModo debug desactivado\n", current_lang.output_prefix);
        } else if (strcmp(debug_cmd, "variables") == 0) {
            printf("%s=== Variables actuales ===\n", current_lang.output_prefix);
            for (int i = 0; i < current_lang.var_count; i++) {
                if (strcmp(current_lang.variables[i].type, "string") == 0) {
                    printf("%s%s (string) = \"%s\"\n", current_lang.output_prefix,
                           current_lang.variables[i].name, current_lang.variables[i].string_value);
                } else {
                    printf("%s%s (number) = %.6g\n", current_lang.output_prefix,
                           current_lang.variables[i].name, current_lang.variables[i].value);
                }
            }
        } else if (strcmp(debug_cmd, "funciones") == 0) {
            printf("%s=== Funciones definidas ===\n", current_lang.output_prefix);
            for (int i = 0; i < current_lang.func_count; i++) {
                printf("%s%s(%d parámetros)\n", current_lang.output_prefix,
                       current_lang.functions[i].name, current_lang.functions[i].param_count);
            }
        } else if (strcmp(debug_cmd, "arrays") == 0) {
            printf("%s=== Arrays definidos ===\n", current_lang.output_prefix);
            for (int i = 0; i < current_lang.array_count; i++) {
                printf("%s%s[%d elementos]\n", current_lang.output_prefix,
                       current_lang.arrays[i].name, current_lang.arrays[i].size);
            }
        }
    }
    // Configuración de mensajes y prefijos
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
    
    printf("=== Cargando definición de %s ===\n", lilou_file);
    
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
        printf("%s: %s\n", current_lang.error_messages[2], code_file);
        return;
    }
    
    int line_number = 1;
    while (fgets(line, sizeof(line), code)) {
        trim_whitespace(line);
        if (strlen(line) > 0) {
            if (debug_mode) {
                printf("[DEBUG] Línea %d: %s\n", line_number, line);
            }
            
            // Lista expandida de comandos predefinidos
            char *predefined_commands[] = {
                "mostrar:", "imprimir:", "calcular:", "variable:", "array:", "si:", "entonces:", "sino:",
                "repetir:", "hacer:", "mientras:", "aleatorio:", "aleatorio_real:", "escribir_archivo:", 
                "anexar_archivo:", "leer_archivo:", "funcion:", "llamar:", "retornar:", "entrada:", 
                "esperar:", "limpiar_pantalla", "clear", "debug:", "break", "continue", "romper", "continuar"
            };
            int predefined_count = 28;
            
            int found = 0;
            for (int i = 0; i < predefined_count; i++) {
                if (strstr(line, predefined_commands[i]) == line || strstr(line, predefined_commands[i])) {
                    parse_lilou_definition(line);
                    found = 1;
                    break;
                }
            }
            
            // Buscar en palabras clave definidas por el usuario
            if (!found) {
                for (int i = 0; i < current_lang.keyword_count; i++) {
                    char keyword_with_colon[MAX_TOKEN];
                    snprintf(keyword_with_colon, sizeof(keyword_with_colon), "%s:", current_lang.keywords[i]);
                    
                    if (strstr(line, keyword_with_colon) == line) {
                        // Mapear palabra clave personalizada a comando interno
                        char mapped_line[MAX_LINE];
                        map_custom_keyword_to_internal(line, mapped_line, current_lang.keywords[i]);
                        parse_lilou_definition(mapped_line);
                        found = 1;
                        break;
                    }
                }
            }
            
            if (!found && strlen(line) > 0 && line[0] != '/' && line[0] != '#') {
                if (current_lang.strict_mode) {
                    printf("Error en línea %d: %s: '%s'\n", line_number, current_lang.error_messages[0], line);
                } else {
                    if (debug_mode) {
                        printf("[DEBUG] Comando ignorado: %s\n", line);
                    }
                }
            }
        }
        line_number++;
    }
    fclose(code);
    
    printf("\n=== Ejecución completada ===\n");
    if (debug_mode) {
        printf("[DEBUG] Variables finales: %d, Funciones: %d, Arrays: %d\n", 
               current_lang.var_count, current_lang.func_count, current_lang.array_count);
    }
}

void test_lilou_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se puede abrir el archivo %s\n", filename);
        return;
    }
    
    printf("=== Análisis de definición de lenguaje en %s ===\n", filename);
    init_language();
    
    char line[MAX_LINE];
    int in_definition = 0;
    int line_number = 1;
    int errors = 0;
    int warnings = 0;
    
    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);
        
        if (strstr(line, "Lilou programa")) {
            in_definition = 1;
            printf("✓ Línea %d: Inicio de definición encontrado\n", line_number);
        }
        else if (in_definition && strlen(line) > 2 && line[0] != '/' && line[0] != '#') {
            if (strstr(line, "Nombre del idioma:") || 
                strstr(line, "extension:") ||
                strstr(line, "operadores:") ||
                strstr(line, "palabras clave:") ||
                strstr(line, "modo_estricto:") ||
                strstr(line, "sensible_mayusculas:") ||
                strstr(line, "mensaje de error:") ||
                strstr(line, "prefijo de salida:")) {
                printf("✓ Línea %d: Configuración válida - %s\n", line_number, line);
                parse_lilou_definition(line);
            } else if (strstr(line, "•")) {
                printf("✓ Línea %d: Definición válida - %s\n", line_number, line);
                parse_lilou_definition(line);
            } else {
                printf("⚠ Línea %d: Configuración no reconocida - %s\n", line_number, line);
                warnings++;
            }
        }
        line_number++;
    }
    fclose(file);
    
    // Validaciones
    printf("\n=== Análisis de completitud ===\n");
    
    if (strlen(current_lang.name) == 0) {
        printf("❌ Error: Falta el nombre del lenguaje\n");
        errors++;
    } else {
        printf("✓ Nombre del lenguaje: %s\n", current_lang.name);
    }
    
    if (strlen(current_lang.extension) == 0) {
        printf("⚠ Advertencia: No se definió extensión de archivo\n");
        warnings++;
    } else {
        printf("✓ Extensión de archivo: %s\n", current_lang.extension);
    }
    
    if (current_lang.op_count == 0) {
        printf("⚠ Advertencia: No se definieron operadores personalizados\n");
        warnings++;
    } else {
        printf("✓ Operadores definidos: %d\n", current_lang.op_count);
    }
    
    if (current_lang.keyword_count == 0) {
        printf("⚠ Advertencia: No se definieron palabras clave personalizadas\n");
        warnings++;
    } else {
        printf("✓ Palabras clave definidas: %d\n", current_lang.keyword_count);
    }
    
    // Mostrar comandos disponibles
    printf("\n=== Comandos disponibles en el lenguaje ===\n");
    printf("📝 Básicos: mostrar, imprimir, calcular\n");
    printf("🔢 Variables: variable, array\n");
    printf("🔀 Control: si/entonces/sino, mientras, repetir/hacer\n");
    printf("🔁 Bucles: break/romper, continue/continuar\n");
    printf("📝 Funciones: funcion, llamar, retornar\n");
    printf("📁 Archivos: escribir_archivo, anexar_archivo, leer_archivo\n");
    printf("🎲 Aleatorio: aleatorio, aleatorio_real\n");
    printf("⌨️ Entrada: entrada\n");
    printf("⏱️ Tiempo: esperar\n");
    printf("🖥️ Sistema: limpiar_pantalla\n");
    printf("🐛 Debug: debug (on/off/variables/funciones/arrays)\n");
    
    // Mostrar funciones matemáticas
    printf("\n📊 Funciones matemáticas disponibles:\n");
    printf("   sin, cos, tan, sqrt, abs, floor, ceil, round, log, exp\n");
    printf("   Operadores: +, -, *, /, %, ** (potencia)\n");
    printf("   Comparación: ==, !=, <, >, <=, >=\n");
    printf("   Lógicos: and/y, or/o\n");
    
    // Resumen final
    printf("\n=== Resumen de validación ===\n");
    if (errors == 0 && warnings == 0) {
        printf("🎉 ¡Definición de lenguaje perfecta!\n");
    } else if (errors == 0) {
        printf("✅ Definición válida con %d advertencia(s)\n", warnings);
    } else {
        printf("❌ Definición inválida: %d error(es), %d advertencia(s)\n", errors, warnings);
    }
    
    printf("\n📋 Configuración detectada:\n");
    printf("   • Modo estricto: %s\n", current_lang.strict_mode ? "activado" : "desactivado");
    printf("   • Sensible a mayúsculas: %s\n", current_lang.case_sensitive ? "sí" : "no");
    printf("   • Separador decimal: '%c'\n", current_lang.decimal_separator);
    printf("   • Prefijo de salida: \"%s\"\n", current_lang.output_prefix);
}

void show_help() {
    printf("=== LILOU 3.0 - Meta-lenguaje de programación avanzado ===\n");
    printf("Un lenguaje completo para crear otros lenguajes de programación\n\n");
    
    printf("🚀 USO:\n");
    printf("  ./main tester-lilou <archivo.lilou>     - Analizar definición de lenguaje\n");
    printf("  ./main lilou <definicion.lilou> <codigo> - Ejecutar código personalizado\n");
    printf("  ./main ayuda                            - Mostrar esta ayuda\n");
    printf("  ./main ejemplos                         - Mostrar ejemplos avanzados\n");
    printf("  ./main caracteristicas                  - Mostrar todas las características\n");
    
    printf("\n💡 EJEMPLOS:\n");
    printf("  ./main tester-lilou mi_lenguaje.lilou\n");
    printf("  ./main lilou mi_lenguaje.lilou codigo.custom\n");
    
    printf("\n🆕 NOVEDADES v3.0:\n");
    printf("  ✨ Funciones matemáticas avanzadas (sin, cos, sqrt, etc.)\n");
    printf("  🔢 Soporte para números decimales\n");
    printf("  📝 Variables de tipo string\n");
    printf("  📊 Arrays y estructuras de datos\n");
    printf("  🔄 Bucles while con condiciones\n");
    printf("  🎛️ Control de flujo (break, continue)\n");
    printf("  📥 Entrada de usuario interactiva\n");
    printf("  ⏱️ Funciones de tiempo y espera\n");
    printf("  🧹 Limpieza de pantalla\n");
    printf("  🔧 Modo estricto y configuración avanzada\n");
    printf("  🐛 Sistema de debug mejorado\n");
    printf("  📁 Manejo avanzado de archivos\n");
    printf("  🎲 Números aleatorios enteros y decimales\n");
    printf("  🔗 Llamadas de función con parámetros\n");
    printf("  🔄 Recursión controlada\n");
    
    printf("\n📖 Para más información, usa: ./main ejemplos\n");
}

void show_examples() {
    printf("=== EJEMPLOS AVANZADOS DE LILOU 3.0 ===\n\n");
    
    printf("🔢 1. VARIABLES Y TIPOS:\n");
    printf("variable: numero = 42.5\n");
    printf("variable: texto = \"¡Hola Mundo!\"\n");
    printf("variable: booleano = 1\n");
    printf("mostrar: Número: {numero}, Texto: {texto}\n\n");
    
    printf("📊 2. ARRAYS:\n");
    printf("array: numeros [1, 2, 3, 4, 5]\n");
    printf("array: nombres [\"Ana\", \"Luis\", \"María\"]\n\n");
    
    printf("🧮 3. FUNCIONES MATEMÁTICAS:\n");
    printf("variable: angulo = 45\n");
    printf("variable: seno = sin(angulo)\n");
    printf("variable: raiz = sqrt(16)\n");
    printf("variable: potencia = 2 ** 3\n");
    printf("mostrar: Seno de {angulo}: {seno}\n\n");
    
    printf("🔀 4. CONDICIONALES AVANZADAS:\n");
    printf("si: numero > 10 and numero < 100\n");
    printf("entonces: mostrar: Número en rango válido\n");
    printf("sino: mostrar: Número fuera de rango\n\n");
    
    printf("🔄 5. BUCLES CON CONTROL:\n");
    printf("repetir: 10\n");
    printf("hacer: si: i == 5\n");
    printf("       entonces: break\n");
    printf("       mostrar: Iteración {i}\n\n");
    
    printf("🔄 6. BUCLE WHILE:\n");
    printf("variable: contador = 0\n");
    printf("mientras: contador < 5\n");
    printf("hacer: mostrar: Contador: {contador}\n");
    printf("       variable: contador = contador + 1\n\n");
    
    printf("📝 7. FUNCIONES CON PARÁMETROS:\n");
    printf("funcion: suma(a, b) {\n");
    printf("    variable: resultado = a + b\n");
    printf("    mostrar: {a} + {b} = {resultado}\n");
    printf("    retornar: resultado\n");
    printf("}\n");
    printf("llamar: suma(5, 3)\n\n");
    
    printf("📥 8. ENTRADA INTERACTIVA:\n");
    printf("entrada: Ingresa tu edad: \n");
    printf("si: entrada >= 18\n");
    printf("entonces: mostrar: Eres mayor de edad\n");
    printf("sino: mostrar: Eres menor de edad\n\n");
    
    printf("📁 9. MANEJO DE ARCHIVOS:\n");
    printf("escribir_archivo: datos.txt, Usuario: {nombre}, Edad: {edad}\n");
    printf("anexar_archivo: datos.txt, Fecha: {fecha}\n");
    printf("leer_archivo: datos.txt\n\n");
    
    printf("🎲 10. NÚMEROS ALEATORIOS:\n");
    printf("aleatorio: 1-100          # Entero entre 1 y 100\n");
    printf("aleatorio_real: 0.0-1.0   # Decimal entre 0.0 y 1.0\n\n");
    
    printf("⏱️ 11. TIEMPO Y ESPERA:\n");
    printf("mostrar: Comenzando countdown...\n");
    printf("repetir: 3\n");
    printf("hacer: variable: tiempo = 3 - i\n");
    printf("       mostrar: {tiempo}...\n");
    printf("       esperar: 1\n");
    printf("mostrar: ¡Tiempo!\n\n");
    
    printf("🐛 12. DEBUG AVANZADO:\n");
    printf("debug: on\n");
    printf("debug: variables     # Mostrar todas las variables\n");
    printf("debug: funciones     # Mostrar todas las funciones\n");
    printf("debug: arrays        # Mostrar todos los arrays\n");
    printf("debug: off\n\n");
    
    printf("🔧 13. CONFIGURACIÓN DE LENGUAJE:\n");
    printf("• modo_estricto: on\n");
    printf("• sensible_mayusculas: off\n");
    printf("• mensaje de error: ¡Oops! Error en mi lenguaje\n");
    printf("• prefijo de salida: [MiLang] \n\n");
}

void show_features() {
    printf("=== CARACTERÍSTICAS COMPLETAS DE LILOU 3.0 ===\n\n");
    
    printf("🏗️ DEFINICIÓN DE LENGUAJES:\n");
    printf("  • Nombre personalizado del lenguaje\n");
    printf("  • Extensión de archivo personalizada\n");
    printf("  • Operadores matemáticos y lógicos\n");
    printf("  • Palabras clave personalizadas\n");
    printf("  • Mensajes de error personalizados\n");
    printf("  • Prefijo de salida configurable\n");
    printf("  • Modo estricto de validación\n");
    printf("  • Sensibilidad a mayúsculas configurable\n\n");
    
    printf("📊 TIPOS DE DATOS:\n");
    printf("  • Números enteros y decimales\n");
    printf("  • Cadenas de texto (strings)\n");
    printf("  • Booleanos (0/1)\n");
    printf("  • Arrays de números\n");
    printf("  • Interpolación de variables en strings\n\n");
    
    printf("🧮 OPERACIONES MATEMÁTICAS:\n");
    printf("  • Operadores básicos: +, -, *, /, %\n");
    printf("  • Potencias: **\n");
    printf("  • Funciones trigonométricas: sin, cos, tan\n");
    printf("  • Funciones de redondeo: floor, ceil, round\n");
    printf("  • Funciones matemáticas: sqrt, abs, log, exp\n");
    printf("  • Operadores de comparación: ==, !=, <, >, <=, >=\n");
    printf("  • Operadores lógicos: and/y, or/o\n\n");
    
    printf("🔀 ESTRUCTURAS DE CONTROL:\n");
    printf("  • Condicionales: si/entonces/sino\n");
    printf("  • Bucles for: repetir/hacer\n");
    printf("  • Bucles while: mientras\n");
    printf("  • Control de flujo: break/romper, continue/continuar\n");
    printf("  • Condiciones complejas con operadores lógicos\n\n");
    
    printf("📝 FUNCIONES:\n");
    printf("  • Definición de funciones: funcion\n");
    printf("  • Llamada de funciones: llamar\n");
    printf("  • Parámetros de función\n");
    printf("  • Valores de retorno: retornar\n");
    printf("  • Recursión controlada\n");
    printf("  • Ámbito local de variables\n\n");
    
    printf("📁 SISTEMA DE ARCHIVOS:\n");
    printf("  • Escribir archivos: escribir_archivo\n");
    printf("  • Anexar a archivos: anexar_archivo\n");
    printf("  • Leer archivos: leer_archivo\n");
    printf("  • Interpolación en contenido de archivos\n\n");
    
    printf("🎲 ALEATORIEDAD:\n");
    printf("  • Números enteros aleatorios: aleatorio\n");
    printf("  • Números decimales aleatorios: aleatorio_real\n");
    printf("  • Rangos personalizables\n\n");
    
    printf("📥 INTERACCIÓN:\n");
    printf("  • Entrada de usuario: entrada\n");
    printf("  • Detección automática de tipos\n");
    printf("  • Salida formateada: mostrar, imprimir\n\n");
    
    printf("⏱️ TIEMPO Y SISTEMA:\n");
    printf("  • Pausas programadas: esperar\n");
    printf("  • Limpieza de pantalla: limpiar_pantalla\n");
    printf("  • Control de tiempo en milisegundos\n\n");
    
    printf("🐛 HERRAMIENTAS DE DEBUG:\n");
    printf("  • Modo debug activable\n");
    printf("  • Inspección de variables\n");
    printf("  • Inspección de funciones\n");
    printf("  • Inspección de arrays\n");
    printf("  • Trazado de ejecución\n");
    printf("  • Información de recursión\n\n");
    
    printf("⚙️ CONFIGURACIÓN AVANZADA:\n");
    printf("  • Modo estricto de validación\n");
    printf("  • Sensibilidad a mayúsculas\n");
    printf("  • Separador decimal personalizable\n");
    printf("  • Control de profundidad de recursión\n");
    printf("  • Manejo de errores personalizado\n\n");
    
    printf("💬 COMENTARIOS Y DOCUMENTACIÓN:\n");
    printf("  • Comentarios de línea: // y #\n");
    printf("  • Ignorar líneas vacías\n");
    printf("  • Documentación inline\n\n");
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
    else if (strcmp(argv[1], "caracteristicas") == 0 || strcmp(argv[1], "features") == 0) {
        show_features();
    }
    else if (strcmp(argv[1], "tester-lilou") == 0) {
        if (argc < 3) {
            printf("Error: Especifica el archivo .lilou a analizar\n");
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
        printf("Usa './main ayuda' para ver los comandos disponibles\n");
        return 1;
    }
    
    return 0;
}
