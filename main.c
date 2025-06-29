
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <strings.h>

// Définir les constantes mathématiques si elles ne sont pas disponibles
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.7182818284590452354
#endif

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
    int is_constant;
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
    
    // Variables de entrada
    char last_input[MAX_LINE];
    double last_input_number;
} Language;

Language current_lang;
int debug_mode = 0;
char current_function_scope[MAX_TOKEN] = "";

// Déclaration de fonction
void parse_lilou_definition(char *line);

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

    // Inicializar variables entrada
    strcpy(current_lang.last_input, "");
    current_lang.last_input_number = 0.0;

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

    // Nettoyer le nom de la variable
    char clean_name[MAX_TOKEN];
    strncpy(clean_name, name, MAX_TOKEN - 1);
    clean_name[MAX_TOKEN - 1] = '\0';
    trim_whitespace(clean_name);

    if (strlen(clean_name) == 0) {
        if (debug_mode) printf("[DEBUG] Nom de variable vide ignoré\n");
        return;
    }

    int index = find_variable(clean_name);
    if (index >= 0) {
        // Vérifier si c'est une constante
        if (current_lang.variables[index].is_constant) {
            if (debug_mode) printf("[DEBUG] Tentative de modification d'une constante '%s' ignorée\n", clean_name);
            return;
        }
        
        // Mettre à jour variable existante
        current_lang.variables[index].value = value;
        strncpy(current_lang.variables[index].type, type, 19);
        current_lang.variables[index].type[19] = '\0';
        
        if (strcmp(type, "string") == 0) {
            if (string_value && strlen(string_value) < MAX_LINE) {
                strncpy(current_lang.variables[index].string_value, string_value, MAX_LINE - 1);
                current_lang.variables[index].string_value[MAX_LINE - 1] = '\0';
            } else {
                strcpy(current_lang.variables[index].string_value, "");
            }
        }

        if (debug_mode) {
            printf("[DEBUG] Variable '%s' mise à jour: %s = ", clean_name, type);
            if (strcmp(type, "string") == 0) {
                printf("\"%s\"\n", current_lang.variables[index].string_value);
            } else {
                printf("%.6g\n", value);
            }
        }
    } else if (current_lang.var_count < MAX_VARIABLES) {
        // Créer nouvelle variable
        strncpy(current_lang.variables[current_lang.var_count].name, clean_name, MAX_TOKEN - 1);
        current_lang.variables[current_lang.var_count].name[MAX_TOKEN - 1] = '\0';
        current_lang.variables[current_lang.var_count].value = value;
        strncpy(current_lang.variables[current_lang.var_count].type, type, 19);
        current_lang.variables[current_lang.var_count].type[19] = '\0';
        current_lang.variables[current_lang.var_count].is_constant = 0;

        if (strcmp(type, "string") == 0 && string_value && strlen(string_value) < MAX_LINE) {
            strncpy(current_lang.variables[current_lang.var_count].string_value, string_value, MAX_LINE - 1);
            current_lang.variables[current_lang.var_count].string_value[MAX_LINE - 1] = '\0';
        } else {
            strcpy(current_lang.variables[current_lang.var_count].string_value, "");
        }

        current_lang.var_count++;

        if (debug_mode) {
            printf("[DEBUG] Variable '%s' créée: %s = ", clean_name, type);
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

void set_constant(char *name, double value) {
    set_variable(name, value, "number", NULL);
    int index = find_variable(name);
    if (index >= 0) {
        current_lang.variables[index].is_constant = 1;
    }
}

double get_variable_value(char *name) {
    int index = find_variable(name);
    if (index >= 0) {
        return current_lang.variables[index].value;
    }
    
    // Variables spéciales automatiques
    if (strcmp(name, "entrada") == 0) {
        return current_lang.last_input_number;
    }
    if (strcmp(name, "input") == 0) {
        return current_lang.last_input_number;
    }
    if (strcmp(name, "i") == 0) {
        return current_lang.loop_count;
    }
    if (strcmp(name, "iteration") == 0) {
        return current_lang.loop_count + 1;
    }
    
    return 0;
}

char* get_variable_string(char *name) {
    int index = find_variable(name);
    if (index >= 0) {
        if (strcmp(current_lang.variables[index].type, "string") == 0) {
            return current_lang.variables[index].string_value;
        }
    }
    
    // Variables spéciales automatiques
    if (strcmp(name, "entrada") == 0 || strcmp(name, "input") == 0) {
        return current_lang.last_input;
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
    // Funciones trigonométricas (en grados)
    if (strcmp(func_name, "sin") == 0) return sin(arg * M_PI / 180.0);
    if (strcmp(func_name, "cos") == 0) return cos(arg * M_PI / 180.0);
    if (strcmp(func_name, "tan") == 0) return tan(arg * M_PI / 180.0);
    if (strcmp(func_name, "asin") == 0) return asin(arg) * 180.0 / M_PI;
    if (strcmp(func_name, "acos") == 0) return acos(arg) * 180.0 / M_PI;
    if (strcmp(func_name, "atan") == 0) return atan(arg) * 180.0 / M_PI;
    
    // Funciones trigonométricas en radianes
    if (strcmp(func_name, "sinr") == 0) return sin(arg);
    if (strcmp(func_name, "cosr") == 0) return cos(arg);
    if (strcmp(func_name, "tanr") == 0) return tan(arg);
    
    // Funciones logarítmicas y exponenciales
    if (strcmp(func_name, "sqrt") == 0) return arg >= 0 ? sqrt(arg) : 0;
    if (strcmp(func_name, "log") == 0) return arg > 0 ? log(arg) : 0;
    if (strcmp(func_name, "log10") == 0) return arg > 0 ? log10(arg) : 0;
    if (strcmp(func_name, "log2") == 0) return arg > 0 ? log(arg) / log(2) : 0;
    if (strcmp(func_name, "exp") == 0) return exp(arg);
    if (strcmp(func_name, "pow2") == 0) return pow(2, arg);
    if (strcmp(func_name, "pow10") == 0) return pow(10, arg);
    
    // Funciones de redondeo y valor absoluto
    if (strcmp(func_name, "abs") == 0) return fabs(arg);
    if (strcmp(func_name, "floor") == 0) return floor(arg);
    if (strcmp(func_name, "ceil") == 0) return ceil(arg);
    if (strcmp(func_name, "round") == 0) return round(arg);
    if (strcmp(func_name, "trunc") == 0) return trunc(arg);
    if (strcmp(func_name, "frac") == 0) return arg - floor(arg);
    
    // Funciones hiperbólicas
    if (strcmp(func_name, "sinh") == 0) return sinh(arg);
    if (strcmp(func_name, "cosh") == 0) return cosh(arg);
    if (strcmp(func_name, "tanh") == 0) return tanh(arg);
    
    // Funciones de utilidad avanzadas
    if (strcmp(func_name, "factorial") == 0) {
        if (arg < 0 || arg != (int)arg || arg > 170) return 0;
        double result = 1;
        for (int i = 1; i <= (int)arg; i++) result *= i;
        return result;
    }
    if (strcmp(func_name, "random") == 0) return ((double)rand() / RAND_MAX) * arg;
    if (strcmp(func_name, "sign") == 0) return (arg > 0) ? 1 : ((arg < 0) ? -1 : 0);
    if (strcmp(func_name, "deg") == 0) return arg * 180.0 / M_PI;
    if (strcmp(func_name, "rad") == 0) return arg * M_PI / 180.0;
    
    return 0;
}

double evaluate_math_function_two_args(char *func_name, double arg1, double arg2) {
    // Funciones a dos argumentos
    if (strcmp(func_name, "pow") == 0) return pow(arg1, arg2);
    if (strcmp(func_name, "atan2") == 0) return atan2(arg1, arg2) * 180.0 / M_PI;
    if (strcmp(func_name, "mod") == 0) return fmod(arg1, arg2);
    if (strcmp(func_name, "max") == 0) return fmax(arg1, arg2);
    if (strcmp(func_name, "min") == 0) return fmin(arg1, arg2);
    if (strcmp(func_name, "hypot") == 0) return hypot(arg1, arg2);
    if (strcmp(func_name, "ldexp") == 0) return ldexp(arg1, (int)arg2);
    if (strcmp(func_name, "remainder") == 0) return remainder(arg1, arg2);
    
    return 0;
}

double evaluate_expression(char *expr) {
    if (!expr) return 0;
    trim_whitespace(expr);

    if (strlen(expr) == 0) return 0;

    // Vérifier d'abord si c'est un simple nombre
    char *endptr;
    double num_val = strtod(expr, &endptr);
    if (*endptr == '\0') {
        return num_val;
    }

    // Vérifier si c'est une simple variable
    int var_index = find_variable(expr);
    if (var_index >= 0) {
        if (strcmp(current_lang.variables[var_index].type, "number") == 0) {
            return current_lang.variables[var_index].value;
        } else {
            // Tenter de convertir string en nombre
            double val = strtod(current_lang.variables[var_index].string_value, &endptr);
            if (*endptr == '\0') {
                return val;
            }
            return 0; // String non numérique
        }
    }

    // Variables spéciales
    if (strcmp(expr, "entrada") == 0 || strcmp(expr, "input") == 0) {
        return current_lang.last_input_number;
    }
    if (strcmp(expr, "i") == 0) {
        return current_lang.loop_count;
    }
    if (strcmp(expr, "iteration") == 0) {
        return current_lang.loop_count + 1;
    }

    // Constantes mathématiques prédéfinies
    if (strcmp(expr, "pi") == 0) return M_PI;
    if (strcmp(expr, "e") == 0) return M_E;

    // Résoudre les variables dans l'expression complexe
    char resolved_expr[MAX_LINE];
    strcpy(resolved_expr, expr);
    
    // Trier les variables par longueur décroissante
    int var_indices[MAX_VARIABLES];
    for (int i = 0; i < current_lang.var_count; i++) {
        var_indices[i] = i;
    }
    
    // Tri par longueur de nom (plus long en premier)
    for (int i = 0; i < current_lang.var_count - 1; i++) {
        for (int j = i + 1; j < current_lang.var_count; j++) {
            if (strlen(current_lang.variables[var_indices[i]].name) < 
                strlen(current_lang.variables[var_indices[j]].name)) {
                int temp = var_indices[i];
                var_indices[i] = var_indices[j];
                var_indices[j] = temp;
            }
        }
    }
    
    // Remplacer chaque variable par sa valeur
    for (int idx = 0; idx < current_lang.var_count; idx++) {
        int i = var_indices[idx];
        char var_pattern[MAX_TOKEN + 2];
        snprintf(var_pattern, sizeof(var_pattern), "%s", current_lang.variables[i].name);
        
        char *pos = strstr(resolved_expr, var_pattern);
        while (pos != NULL) {
            // Vérifier que c'est bien une variable isolée
            int is_isolated = 1;
            if (pos > resolved_expr && (isalnum(*(pos-1)) || *(pos-1) == '_')) {
                is_isolated = 0;
            }
            if (*(pos + strlen(var_pattern)) && (isalnum(*(pos + strlen(var_pattern))) || *(pos + strlen(var_pattern)) == '_')) {
                is_isolated = 0;
            }
            
            if (is_isolated) {
                char temp_expr[MAX_LINE];
                int pos_index = pos - resolved_expr;
                
                // Copier la partie avant
                strncpy(temp_expr, resolved_expr, pos_index);
                temp_expr[pos_index] = '\0';
                
                // Ajouter la valeur de la variable
                char value_str[MAX_TOKEN];
                if (strcmp(current_lang.variables[i].type, "number") == 0) {
                    snprintf(value_str, sizeof(value_str), "%.15g", current_lang.variables[i].value);
                } else {
                    // Essayer de convertir string en nombre
                    char *str_endptr;
                    double val = strtod(current_lang.variables[i].string_value, &str_endptr);
                    if (*str_endptr == '\0') {
                        snprintf(value_str, sizeof(value_str), "%.15g", val);
                    } else {
                        strcpy(value_str, "0"); // String non numérique = 0
                    }
                }
                strcat(temp_expr, value_str);
                
                // Copier la partie après
                strcat(temp_expr, pos + strlen(var_pattern));
                
                strcpy(resolved_expr, temp_expr);
                pos = strstr(resolved_expr, var_pattern);
            } else {
                pos = strstr(pos + 1, var_pattern);
            }
        }
    }
    
    // Remplacer les variables spéciales
    char *special_vars[] = {"entrada", "input", "i", "iteration", "pi", "e"};
    double special_values[] = {
        current_lang.last_input_number, current_lang.last_input_number,
        current_lang.loop_count, current_lang.loop_count + 1,
        M_PI, M_E
    };
    
    for (int sv = 0; sv < 6; sv++) {
        char *pos = strstr(resolved_expr, special_vars[sv]);
        while (pos != NULL) {
            int is_isolated = 1;
            if (pos > resolved_expr && (isalnum(*(pos-1)) || *(pos-1) == '_')) {
                is_isolated = 0;
            }
            if (*(pos + strlen(special_vars[sv])) && 
                (isalnum(*(pos + strlen(special_vars[sv]))) || *(pos + strlen(special_vars[sv])) == '_')) {
                is_isolated = 0;
            }
            
            if (is_isolated) {
                char temp_expr[MAX_LINE];
                int pos_index = pos - resolved_expr;
                
                strncpy(temp_expr, resolved_expr, pos_index);
                temp_expr[pos_index] = '\0';
                
                char value_str[MAX_TOKEN];
                snprintf(value_str, sizeof(value_str), "%.15g", special_values[sv]);
                strcat(temp_expr, value_str);
                strcat(temp_expr, pos + strlen(special_vars[sv]));
                
                strcpy(resolved_expr, temp_expr);
                pos = strstr(resolved_expr, special_vars[sv]);
            } else {
                pos = strstr(pos + 1, special_vars[sv]);
            }
        }
    }
    
    if (debug_mode) {
        printf("[DEBUG] Expression originale: '%s' -> résolue: '%s'\n", expr, resolved_expr);
    }

    // Réessayer d'évaluer comme nombre simple
    num_val = strtod(resolved_expr, &endptr);
    if (*endptr == '\0') {
        return num_val;
    }

    // Fonctions mathématiques avec un ou deux arguments
    if (strchr(resolved_expr, '(') && strchr(resolved_expr, ')')) {
        char func_name[MAX_TOKEN];
        char *paren_start = strchr(resolved_expr, '(');
        char *paren_end = strrchr(resolved_expr, ')');

        if (paren_start && paren_end && paren_end > paren_start) {
            int func_len = paren_start - resolved_expr;
            if (func_len > 0 && func_len < MAX_TOKEN) {
                strncpy(func_name, resolved_expr, func_len);
                func_name[func_len] = '\0';
                trim_whitespace(func_name);

                char arg_str[MAX_TOKEN];
                int arg_len = paren_end - paren_start - 1;
                if (arg_len > 0 && arg_len < MAX_TOKEN) {
                    strncpy(arg_str, paren_start + 1, arg_len);
                    arg_str[arg_len] = '\0';

                    // Vérifier s'il y a deux arguments séparés par une virgule
                    char *comma_pos = strchr(arg_str, ',');
                    if (comma_pos) {
                        char arg1_str[MAX_TOKEN], arg2_str[MAX_TOKEN];
                        int arg1_len = comma_pos - arg_str;
                        strncpy(arg1_str, arg_str, arg1_len);
                        arg1_str[arg1_len] = '\0';
                        trim_whitespace(arg1_str);
                        
                        strcpy(arg2_str, comma_pos + 1);
                        trim_whitespace(arg2_str);
                        
                        double arg1 = evaluate_expression(arg1_str);
                        double arg2 = evaluate_expression(arg2_str);
                        
                        double result = evaluate_math_function_two_args(func_name, arg1, arg2);
                        if (result != 0 || strcmp(func_name, "pow") == 0 || strcmp(func_name, "max") == 0 || 
                            strcmp(func_name, "min") == 0 || strcmp(func_name, "mod") == 0) {
                            return result;
                        }
                        
                        // Si ce n'est pas une fonction à deux arguments, utiliser le premier
                        return evaluate_math_function(func_name, arg1);
                    } else {
                        // Fonction à un seul argument
                        double arg = evaluate_expression(arg_str);
                        return evaluate_math_function(func_name, arg);
                    }
                }
            }
        }
    }

    // Évaluation des expressions complexes avec opérateurs
    char *operators[] = {"**", "*", "/", "%", "+", "-"};
    int precedence[] = {3, 2, 2, 2, 1, 1};
    int op_count = 6;

    for (int prec = 3; prec >= 1; prec--) {
        for (int i = 0; i < op_count; i++) {
            if (precedence[i] == prec) {
                // Chercher l'opérateur de droite à gauche pour respecter l'associativité
                char *op_pos = NULL;
                char *search_pos = resolved_expr;
                
                while ((search_pos = strstr(search_pos, operators[i])) != NULL) {
                    // Vérifier qu'on n'est pas dans une fonction
                    char *func_paren = strchr(resolved_expr, '(');
                    if (func_paren && search_pos > func_paren) {
                        char *close_paren = strchr(func_paren, ')');
                        if (close_paren && search_pos < close_paren) {
                            search_pos += strlen(operators[i]);
                            continue;
                        }
                    }
                    
                    op_pos = search_pos;
                    search_pos += strlen(operators[i]);
                }
                
                if (op_pos && op_pos != resolved_expr) {
                    char left[MAX_TOKEN], right[MAX_TOKEN];
                    int left_len = op_pos - resolved_expr;
                    if (left_len > 0 && left_len < MAX_TOKEN) {
                        strncpy(left, resolved_expr, left_len);
                        left[left_len] = '\0';
                        trim_whitespace(left);

                        strncpy(right, op_pos + strlen(operators[i]), MAX_TOKEN - 1);
                        right[MAX_TOKEN - 1] = '\0';
                        trim_whitespace(right);

                        if (strlen(left) > 0 && strlen(right) > 0) {
                            double left_val = evaluate_expression(left);
                            double right_val = evaluate_expression(right);

                            switch(i) {
                                case 0: return pow(left_val, right_val);
                                case 1: return left_val * right_val;
                                case 2: return safe_division(left_val, right_val);
                                case 3: return fabs(right_val) > 1e-10 ? fmod(left_val, right_val) : 0;
                                case 4: return left_val + right_val;
                                case 5: return left_val - right_val;
                            }
                        }
                    }
                }
            }
        }
    }

    if (debug_mode) {
        printf("[DEBUG] Expression non évaluable: '%s'\n", resolved_expr);
    }
    return 0;
}

int evaluate_condition(char *condition) {
    if (!condition) return 0;
    trim_whitespace(condition);
    
    if (debug_mode) {
        printf("[DEBUG] Évaluation condition: '%s'\n", condition);
    }

    // Operadores lógicos (procesarlos primero para evitar problemas de precedencia)
    if (strstr(condition, " and ") || strstr(condition, " y ")) {
        char *and_pos = strstr(condition, " and ") ? strstr(condition, " and ") : strstr(condition, " y ");
        char left[MAX_TOKEN], right[MAX_TOKEN];

        int left_len = and_pos - condition;
        strncpy(left, condition, left_len);
        left[left_len] = '\0';

        strcpy(right, and_pos + (strstr(condition, " and ") ? 5 : 3));

        int left_result = evaluate_condition(left);
        int right_result = evaluate_condition(right);
        int final_result = left_result && right_result;
        
        if (debug_mode) {
            printf("[DEBUG] '%s' = %d AND '%s' = %d => %d\n", left, left_result, right, right_result, final_result);
        }
        
        return final_result;
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

            if (debug_mode) {
                printf("[DEBUG] Comparaison: '%s' (%.6g) %s '%s' (%.6g)\n", 
                       left, left_val, operators[i], right, right_val);
            }

            int result;
            switch(i) {
                case 0: result = fabs(left_val - right_val) < 1e-10; break; // ==
                case 1: result = fabs(left_val - right_val) >= 1e-10; break; // !=
                case 2: result = left_val <= right_val; break; // <=
                case 3: result = left_val >= right_val; break; // >=
                case 4: result = left_val < right_val; break;  // <
                case 5: result = left_val > right_val; break;  // >
                default: result = 0;
            }
            
            if (debug_mode) {
                printf("[DEBUG] Résultat de la comparaison: %d\n", result);
            }
            
            return result;
        }
    }

    // Si no hay operador de comparación, evaluar como expresión
    double expr_result = evaluate_expression(condition);
    return expr_result != 0;
}

void execute_function(char *func_name, char *params) {
    if (current_lang.current_recursion_depth >= current_lang.max_recursion_depth) {
        printf("%s\n", current_lang.error_messages[9]);
        return;
    }

    trim_whitespace(func_name);

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
                char params_copy[MAX_LINE];
                strcpy(params_copy, params);
                char *param_token = strtok(params_copy, ",");
                int param_index = 0;

                while (param_token && param_index < current_lang.functions[i].param_count) {
                    trim_whitespace(param_token);
                    double param_value = evaluate_expression(param_token);
                    set_variable(current_lang.functions[i].params[param_index], param_value, "number", NULL);
                    
                    if (debug_mode) {
                        printf("[DEBUG] Paramètre '%s' = %.6g\n", current_lang.functions[i].params[param_index], param_value);
                    }
                    
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
    printf("Error: función no definida: función '%s'\n", func_name);
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
            if (var_len > 0 && var_len < MAX_TOKEN) {
                strncpy(var_name, start + 1, var_len);
                var_name[var_len] = '\0';
                trim_whitespace(var_name);

                int var_index = find_variable(var_name);
                char replacement[MAX_TOKEN];
                replacement[0] = '\0';

                if (var_index >= 0) {
                    if (strcmp(current_lang.variables[var_index].type, "string") == 0) {
                        strncpy(replacement, current_lang.variables[var_index].string_value, MAX_TOKEN - 1);
                        replacement[MAX_TOKEN - 1] = '\0';
                    } else {
                        double val = current_lang.variables[var_index].value;
                        if (val == (long long)val && val > -1000000 && val < 1000000) {
                            snprintf(replacement, MAX_TOKEN, "%.0f", val);
                        } else {
                            snprintf(replacement, MAX_TOKEN, "%.6g", val);
                        }
                    }
                } else {
                    // Variables spéciales
                    if (strcmp(var_name, "entrada") == 0 || strcmp(var_name, "input") == 0) {
                        snprintf(replacement, MAX_TOKEN, "%.6g", current_lang.last_input_number);
                    } else if (strcmp(var_name, "i") == 0) {
                        snprintf(replacement, MAX_TOKEN, "%d", current_lang.loop_count);
                    } else if (strcmp(var_name, "iteration") == 0) {
                        snprintf(replacement, MAX_TOKEN, "%d", current_lang.loop_count + 1);
                    } else if (strcmp(var_name, "pi") == 0) {
                        snprintf(replacement, MAX_TOKEN, "%.15g", M_PI);
                    } else if (strcmp(var_name, "e") == 0) {
                        snprintf(replacement, MAX_TOKEN, "%.15g", M_E);
                    } else {
                        // Evaluar como expresión
                        double expr_result = evaluate_expression(var_name);
                        if (debug_mode) {
                            printf("[DEBUG] Expression '{%s}' évaluée à %.6g\n", var_name, expr_result);
                        }
                        if (expr_result == (long long)expr_result && expr_result > -1000000 && expr_result < 1000000) {
                            snprintf(replacement, MAX_TOKEN, "%.0f", expr_result);
                        } else {
                            snprintf(replacement, MAX_TOKEN, "%.6g", expr_result);
                        }
                    }
                }

                // Remplacement sécurisé
                int replacement_len = strlen(replacement);
                int var_placeholder_len = end - start + 1;
                int remaining_len = strlen(end + 1);
                int total_len = strlen(output) - var_placeholder_len + replacement_len;

                if (total_len < MAX_LINE - 1) {
                    memmove(start + replacement_len, end + 1, remaining_len + 1);
                    memcpy(start, replacement, replacement_len);
                    start = strchr(start + replacement_len, '{');
                } else {
                    if (debug_mode) {
                        printf("[DEBUG] Dépassement de buffer évité\n");
                    }
                    break;
                }
            } else {
                start = strchr(start + 1, '{');
            }
        } else {
            break;
        }
    }
}

void map_custom_keyword_to_internal(char *line, char *output, char *custom_keyword) {
    if (!line || !output || !custom_keyword) return;

    char *content = strchr(line, ':');
    if (!content) {
        strcpy(output, line);
        return;
    }
    content++; // Skip the ':'
    trim_whitespace(content);

    // Vérifier d'abord si c'est une définition de variable (contient '=')
    if (strchr(content, '=')) {
        snprintf(output, MAX_LINE, "variable: %s", content);
        return;
    }
    
    // Mapear TOUS les mots-clés personnalisés vers des commandes internes appropriées
    // Mots-clés d'affichage
    if (strcmp(custom_keyword, "print") == 0 || 
        strcmp(custom_keyword, "afficher") == 0 || 
        strcmp(custom_keyword, "imprimir") == 0 ||
        strcmp(custom_keyword, "display") == 0 ||
        strcmp(custom_keyword, "show") == 0 ||
        strcmp(custom_keyword, "echo") == 0 ||
        strcmp(custom_keyword, "write") == 0 ||
        strcmp(custom_keyword, "output") == 0) {
        snprintf(output, MAX_LINE, "mostrar: %s", content);
    }
    // Mots-clés de calcul
    else if (strcmp(custom_keyword, "calc") == 0 || 
             strcmp(custom_keyword, "calculate") == 0 ||
             strcmp(custom_keyword, "calculer") == 0 ||
             strcmp(custom_keyword, "compute") == 0 ||
             strcmp(custom_keyword, "eval") == 0) {
        snprintf(output, MAX_LINE, "calcular: %s", content);
    }
    // Mots-clés de variables
    else if (strcmp(custom_keyword, "var") == 0 || 
             strcmp(custom_keyword, "variable") == 0 ||
             strcmp(custom_keyword, "let") == 0 ||
             strcmp(custom_keyword, "const") == 0 ||
             strcmp(custom_keyword, "set") == 0 ||
             strcmp(custom_keyword, "define") == 0) {
        snprintf(output, MAX_LINE, "variable: %s", content);
    }
    // Mots-clés conditionnels
    else if (strcmp(custom_keyword, "if") == 0 || 
             strcmp(custom_keyword, "si") == 0 ||
             strcmp(custom_keyword, "si_marius") == 0) {
        snprintf(output, MAX_LINE, "si: %s", content);
    }
    else if (strcmp(custom_keyword, "then") == 0 || 
             strcmp(custom_keyword, "alors") == 0 ||
             strcmp(custom_keyword, "entonces") == 0 ||
             strcmp(custom_keyword, "alors_marius") == 0 ||
             strcmp(custom_keyword, "do") == 0) {
        snprintf(output, MAX_LINE, "entonces: %s", content);
    }
    else if (strcmp(custom_keyword, "else") == 0 || 
             strcmp(custom_keyword, "sinon") == 0 ||
             strcmp(custom_keyword, "sino") == 0 ||
             strcmp(custom_keyword, "sinon_marius") == 0) {
        snprintf(output, MAX_LINE, "sino: %s", content);
    }
    // Mots-clés de boucles
    else if (strcmp(custom_keyword, "for") == 0 || 
             strcmp(custom_keyword, "repeat") == 0 ||
             strcmp(custom_keyword, "repetir") == 0 ||
             strcmp(custom_keyword, "pour") == 0 ||
             strcmp(custom_keyword, "pour_marius") == 0 ||
             strcmp(custom_keyword, "loop") == 0) {
        snprintf(output, MAX_LINE, "repetir: %s", content);
    }
    else if (strcmp(custom_keyword, "hacer") == 0 ||
             strcmp(custom_keyword, "faire") == 0 ||
             strcmp(custom_keyword, "execute") == 0 ||
             strcmp(custom_keyword, "run") == 0) {
        snprintf(output, MAX_LINE, "hacer: %s", content);
    }
    else if (strcmp(custom_keyword, "while") == 0 || 
             strcmp(custom_keyword, "mientras") == 0 ||
             strcmp(custom_keyword, "tant_que") == 0 ||
             strcmp(custom_keyword, "pendant") == 0) {
        snprintf(output, MAX_LINE, "mientras: %s", content);
    }
    // Mots-clés de fonctions
    else if (strcmp(custom_keyword, "function") == 0 || 
             strcmp(custom_keyword, "func") == 0 ||
             strcmp(custom_keyword, "def") == 0 ||
             strcmp(custom_keyword, "funcion") == 0 ||
             strcmp(custom_keyword, "fonction") == 0) {
        snprintf(output, MAX_LINE, "funcion: %s", content);
    }
    else if (strcmp(custom_keyword, "call") == 0 || 
             strcmp(custom_keyword, "invoke") == 0 ||
             strcmp(custom_keyword, "llamar") == 0 ||
             strcmp(custom_keyword, "appeler") == 0 ||
             strcmp(custom_keyword, "execute") == 0) {
        snprintf(output, MAX_LINE, "llamar: %s", content);
    }
    // Mots-clés d'entrée
    else if (strcmp(custom_keyword, "input") == 0 || 
             strcmp(custom_keyword, "ask") == 0 ||
             strcmp(custom_keyword, "entrada") == 0 ||
             strcmp(custom_keyword, "read") == 0 ||
             strcmp(custom_keyword, "prompt") == 0) {
        snprintf(output, MAX_LINE, "entrada: %s", content);
    }
    // Mots-clés aléatoires
    else if (strcmp(custom_keyword, "random") == 0 || 
             strcmp(custom_keyword, "aleatorio") == 0 ||
             strcmp(custom_keyword, "rand") == 0) {
        snprintf(output, MAX_LINE, "aleatorio: %s", content);
    }
    else if (strcmp(custom_keyword, "random_real") == 0 || 
             strcmp(custom_keyword, "aleatorio_real") == 0) {
        snprintf(output, MAX_LINE, "aleatorio_real: %s", content);
    }
    // Mots-clés de fichiers
    else if (strcmp(custom_keyword, "write_file") == 0 || 
             strcmp(custom_keyword, "escribir_archivo") == 0 ||
             strcmp(custom_keyword, "save") == 0) {
        snprintf(output, MAX_LINE, "escribir_archivo: %s", content);
    }
    else if (strcmp(custom_keyword, "read_file") == 0 || 
             strcmp(custom_keyword, "leer_archivo") == 0 ||
             strcmp(custom_keyword, "load") == 0) {
        snprintf(output, MAX_LINE, "leer_archivo: %s", content);
    }
    // Mots-clés de temps
    else if (strcmp(custom_keyword, "wait") == 0 || 
             strcmp(custom_keyword, "sleep") == 0 ||
             strcmp(custom_keyword, "esperar") == 0 ||
             strcmp(custom_keyword, "pause") == 0) {
        snprintf(output, MAX_LINE, "esperar: %s", content);
    }
    // Mots-clés de nettoyage
    else if (strcmp(custom_keyword, "clear") == 0 || 
             strcmp(custom_keyword, "cls") == 0 ||
             strcmp(custom_keyword, "limpiar_pantalla") == 0) {
        snprintf(output, MAX_LINE, "limpiar_pantalla");
    }
    else {
        // Pour TOUS les autres mots-clés personnalisés, les traiter comme des commandes d'affichage
        snprintf(output, MAX_LINE, "mostrar: %s", content);
        
        if (debug_mode) {
            printf("[DEBUG] Mot-clé personnalisé '%s' mappé vers 'mostrar'\n", custom_keyword);
        }
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
    // Nouvelles définitions personnalisables en Lilou
    else if (strstr(line, "definir_variable:")) {
        char *var_def = strchr(line, ':') + 1;
        trim_whitespace(var_def);
        char var_copy[MAX_LINE];
        strcpy(var_copy, var_def);
        
        char *equal_pos = strchr(var_copy, '=');
        if (equal_pos) {
            char var_name[MAX_TOKEN];
            int name_len = equal_pos - var_copy;
            strncpy(var_name, var_copy, name_len);
            var_name[name_len] = '\0';
            trim_whitespace(var_name);
            
            char *value_str = equal_pos + 1;
            trim_whitespace(value_str);
            
            if ((value_str[0] == '"' && value_str[strlen(value_str)-1] == '"') ||
                (value_str[0] == '\'' && value_str[strlen(value_str)-1] == '\'')) {
                char string_val[MAX_LINE];
                int str_len = strlen(value_str) - 2;
                strncpy(string_val, value_str + 1, str_len);
                string_val[str_len] = '\0';
                set_variable(var_name, 0, "string", string_val);
                printf("Variable global '%s' definida como string: \"%s\"\n", var_name, string_val);
            } else {
                double value = evaluate_expression(value_str);
                set_variable(var_name, value, "number", NULL);
                printf("Variable global '%s' definida como número: %.6g\n", var_name, value);
            }
        }
    }
    else if (strstr(line, "definir_constante:")) {
        char *const_def = strchr(line, ':') + 1;
        trim_whitespace(const_def);
        
        if (strstr(const_def, "pi")) {
            set_constant("pi", M_PI);
            printf("Constante matemática 'pi' definida globalmente\n");
        } else if (strstr(const_def, "euler") || strstr(const_def, " e")) {
            set_constant("e", M_E);
            printf("Constante matemática 'e' (Euler) definida globalmente\n");
        } else {
            char const_copy[MAX_LINE];
            strcpy(const_copy, const_def);
            char *equal_pos = strchr(const_copy, '=');
            if (equal_pos) {
                char const_name[MAX_TOKEN];
                int name_len = equal_pos - const_copy;
                strncpy(const_name, const_copy, name_len);
                const_name[name_len] = '\0';
                trim_whitespace(const_name);
                
                char *value_str = equal_pos + 1;
                trim_whitespace(value_str);
                double value = evaluate_expression(value_str);
                set_constant(const_name, value);
                printf("Constante '%s' definida globalmente: %.6g\n", const_name, value);
            }
        }
    }
    else if (strstr(line, "definir_estructura_si:")) {
        char *struct_def = strchr(line, ':') + 1;
        trim_whitespace(struct_def);
        printf("Estructura condicional 'si' configurada: %s\n", struct_def);
    }
    else if (strstr(line, "definir_estructura_mientras:")) {
        char *struct_def = strchr(line, ':') + 1;
        trim_whitespace(struct_def);
        printf("Estructura de bucle 'mientras' configurada: %s\n", struct_def);
    }
    else if (strstr(line, "definir_estructura_repetir:")) {
        char *struct_def = strchr(line, ':') + 1;
        trim_whitespace(struct_def);
        printf("Estructura de bucle 'repetir' configurada: %s\n", struct_def);
    }
    else if (strstr(line, "definir_funcion_matematica:")) {
        char *func_def = strchr(line, ':') + 1;
        trim_whitespace(func_def);
        printf("Funciones matemáticas avanzadas habilitadas: %s\n", func_def);
        printf("Disponibles: sin, cos, tan, sqrt, abs, floor, ceil, round, log, exp, pow, factorial\n");
    }
    else if (strstr(line, "definir_tipos_datos:")) {
        char *types_def = strchr(line, ':') + 1;
        trim_whitespace(types_def);
        printf("Tipos de datos configurados: %s\n", types_def);
        printf("Soportados: números decimales, strings, arrays, booleanos\n");
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
        printf("%s%.15g\n", current_lang.output_prefix, result);
    }
    else if (strstr(line, "variable:") || strstr(line, "var:")) {
        char *var_start = strchr(line, ':') + 1;
        trim_whitespace(var_start);

        char *equal_pos = strchr(var_start, '=');
        if (equal_pos) {
            char var_name[MAX_TOKEN];
            int name_len = equal_pos - var_start;
            if (name_len > 0 && name_len < MAX_TOKEN) {
                strncpy(var_name, var_start, name_len);
                var_name[name_len] = '\0';
                trim_whitespace(var_name);

                char *value_str = equal_pos + 1;
                trim_whitespace(value_str);

                if (strlen(var_name) > 0 && strlen(value_str) > 0) {
                    // Detectar si es string (entre comillas)
                    if ((value_str[0] == '"' && value_str[strlen(value_str)-1] == '"') ||
                        (value_str[0] == '\'' && value_str[strlen(value_str)-1] == '\'')) {
                        char string_val[MAX_LINE];
                        int str_len = strlen(value_str) - 2;
                        if (str_len > 0 && str_len < MAX_LINE) {
                            strncpy(string_val, value_str + 1, str_len);
                            string_val[str_len] = '\0';
                            set_variable(var_name, 0, "string", string_val);
                            if (!debug_mode) {
                                printf("%sVariable '%s' = \"%s\"\n", current_lang.output_prefix, var_name, string_val);
                            }
                        }
                    } else {
                        double value = evaluate_expression(value_str);
                        set_variable(var_name, value, "number", NULL);
                        if (!debug_mode) {
                            printf("%sVariable '%s' = %.15g\n", current_lang.output_prefix, var_name, value);
                        }
                    }
                }
            }
        } else {
            // Variable sans valeur, initialiser à 0
            set_variable(var_start, 0, "number", NULL);
            if (!debug_mode) {
                printf("%sVariable '%s' = 0\n", current_lang.output_prefix, var_start);
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

                if (strlen(values_str) > 0) {
                    char *token = strtok(values_str, ",");
                    while (token && current_lang.arrays[current_lang.array_count].size < MAX_ARRAY_SIZE) {
                        trim_whitespace(token);
                        current_lang.arrays[current_lang.array_count].values[current_lang.arrays[current_lang.array_count].size] = evaluate_expression(token);
                        current_lang.arrays[current_lang.array_count].size++;
                        token = strtok(NULL, ",");
                    }
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
        
        current_lang.loop_active = 1;
        current_lang.break_flag = 0;
        current_lang.continue_flag = 0;
        
        if (debug_mode) {
            printf("[DEBUG] Iniciando bucle mientras con condición: %s\n", condition_start);
        }
        
        // Procesar líneas siguientes hasta encontrar 'hacer:'
        current_lang.loop_count = -1; // Indicador de bucle while
        
        // En esta implementación simplificada, buscaremos el 'hacer:' en las siguientes líneas
        // Para un bucle while completo, necesitaríamos un parser más sofisticado
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

        if (current_lang.loop_count == -1) {
            // Bucle while - ejecutar una vez por simplicidad
            if (debug_mode) {
                printf("[DEBUG] Ejecutando cuerpo de bucle while\n");
            }
            
            set_variable("iteration", 1, "number", NULL);
            current_lang.continue_flag = 0;
            
            parse_lilou_definition(do_start);
            
        } else {
            // Bucle for normal
            while (current_lang.loop_count < current_lang.loop_max && !current_lang.break_flag) {
                // Mettre à jour les variables de boucle
                set_variable("i", current_lang.loop_count, "number", NULL);
                set_variable("iteration", current_lang.loop_count + 1, "number", NULL);
                current_lang.continue_flag = 0;

                if (debug_mode) {
                    printf("[DEBUG] Boucle iteration %d/%d\n", current_lang.loop_count + 1, current_lang.loop_max);
                }

                parse_lilou_definition(do_start);

                if (current_lang.continue_flag) {
                    current_lang.loop_count++;
                    continue;
                }
                if (current_lang.break_flag) {
                    break;
                }

                current_lang.loop_count++;
            }
        }
        
        current_lang.loop_active = 0;
        current_lang.break_flag = 0;
        current_lang.continue_flag = 0;
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
            set_variable("aleatorio_resultado", random_num, "number", NULL);
            printf("%s%d\n", current_lang.output_prefix, random_num);
        } else {
            int max = (int)evaluate_expression(range_start);
            int random_num = rand() % (max + 1);
            set_variable("aleatorio_resultado", random_num, "number", NULL);
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
            set_variable("aleatorio_real_resultado", random_num, "number", NULL);
            printf("%s%.15g\n", current_lang.output_prefix, random_num);
        } else {
            double max = evaluate_expression(range_start);
            double random_num = ((double)rand() / RAND_MAX) * max;
            set_variable("aleatorio_real_resultado", random_num, "number", NULL);
            printf("%s%.15g\n", current_lang.output_prefix, random_num);
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
            char file_content[MAX_LINE * 10] = "";
            while (fgets(file_line, sizeof(file_line), file)) {
                file_line[strcspn(file_line, "\n")] = 0;
                printf("%s%s\n", current_lang.output_prefix, file_line);
                if (strlen(file_content) + strlen(file_line) < sizeof(file_content) - 2) {
                    strcat(file_content, file_line);
                    strcat(file_content, "\n");
                }
            }
            fclose(file);
            set_variable("archivo_contenido", 0, "string", file_content);
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
                if (strlen(params_str) > 0) {
                    char *param_token = strtok(params_str, ",");
                    while (param_token && current_lang.functions[current_lang.func_count].param_count < 10) {
                        trim_whitespace(param_token);
                        strcpy(current_lang.functions[current_lang.func_count].params[current_lang.functions[current_lang.func_count].param_count], param_token);
                        current_lang.functions[current_lang.func_count].param_count++;
                        param_token = strtok(NULL, ",");
                    }
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
            printf("[DEBUG] Función retorna: %.15g\n", return_value);
        }

        set_variable("retorno", return_value, "number", NULL);
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

            // Guardar entrada como string
            strncpy(current_lang.last_input, input, MAX_LINE - 1);
            current_lang.last_input[MAX_LINE - 1] = '\0';

            // Detectar si es número o string
            char *endptr;
            double num_value = strtod(input, &endptr);
            if (*endptr == '\0' && strlen(input) > 0) {
                current_lang.last_input_number = num_value;
                set_variable("entrada", num_value, "number", NULL);
                set_variable("input", num_value, "number", NULL);
            } else {
                current_lang.last_input_number = 0;
                set_variable("entrada", 0, "string", input);
                set_variable("input", 0, "string", input);
            }
        }
    }
    else if (strstr(line, "esperar:")) {
        char *time_start = strchr(line, ':') + 1;
        trim_whitespace(time_start);
        double seconds = evaluate_expression(time_start);
        int milliseconds = (int)(seconds * 1000);

        if (debug_mode) {
            printf("[DEBUG] Esperando %.3f segundos (%d ms)\n", seconds, milliseconds);
        }

        #ifdef _WIN32
            Sleep(milliseconds);
        #else
            usleep((useconds_t)(milliseconds * 1000));
        #endif
    }
    else if (strstr(line, "limpiar_pantalla") || strstr(line, "clear")) {
        #ifdef _WIN32
            if (system("cls") == -1) {
                printf("Erreur lors du nettoyage de l'écran\n");
            }
        #else
            if (system("clear") == -1) {
                printf("Erreur lors du nettoyage de l'écran\n");
            }
        #endif
        printf("%sPantalla limpiada\n", current_lang.output_prefix);
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
                    printf("%s%s (number) = %.15g%s\n", current_lang.output_prefix,
                           current_lang.variables[i].name, current_lang.variables[i].value,
                           current_lang.variables[i].is_constant ? " [CONST]" : "");
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
                printf("%s%s[%d elementos]: ", current_lang.output_prefix,
                       current_lang.arrays[i].name, current_lang.arrays[i].size);
                for (int j = 0; j < current_lang.arrays[i].size && j < 10; j++) {
                    printf("%.3g", current_lang.arrays[i].values[j]);
                    if (j < current_lang.arrays[i].size - 1 && j < 9) printf(", ");
                }
                if (current_lang.arrays[i].size > 10) printf("...");
                printf("\n");
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
    // Réinitialiser complètement le langage
    init_language();
    
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
                        
                        if (debug_mode) {
                            printf("[DEBUG] Mapeando '%s' -> '%s'\n", line, mapped_line);
                        }
                        
                        parse_lilou_definition(mapped_line);
                        found = 1;
                        break;
                    }
                }
            }

            // Si toujours pas trouvé, vérifier si c'est un mot-clé avec ':' quelconque
            if (!found && strlen(line) > 0 && line[0] != '/' && line[0] != '#') {
                char *colon_pos = strchr(line, ':');
                if (colon_pos) {
                    // Extraire le mot-clé avant ':'
                    char potential_keyword[MAX_TOKEN];
                    int keyword_len = colon_pos - line;
                    if (keyword_len > 0 && keyword_len < MAX_TOKEN) {
                        strncpy(potential_keyword, line, keyword_len);
                        potential_keyword[keyword_len] = '\0';
                        trim_whitespace(potential_keyword);
                        
                        // Traiter comme mot-clé personnalisé générique
                        char mapped_line[MAX_LINE];
                        map_custom_keyword_to_internal(line, mapped_line, potential_keyword);
                        
                        if (debug_mode) {
                            printf("[DEBUG] Mot-clé générique détecté '%s' -> '%s'\n", line, mapped_line);
                        }
                        
                        parse_lilou_definition(mapped_line);
                        found = 1;
                    }
                }
                
                if (!found) {
                    if (current_lang.strict_mode) {
                        printf("Error en línea %d: %s: '%s'\n", line_number, current_lang.error_messages[0], line);
                    } else {
                        if (debug_mode) {
                            printf("[DEBUG] Comando ignorado: %s\n", line);
                        }
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
            } else if (strstr(line, "definir_")) {
                printf("✓ Línea %d: Definición avanzada - %s\n", line_number, line);
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

    printf("✓ Variables globales definidas: %d\n", current_lang.var_count);

    // Mostrar comandos disponibles
    printf("\n=== Módulos activados ===\n");
    printf("🧮 Matemáticas Avanzadas: ✓ Trigonométricas, logarítmicas, exponenciales\n");
    printf("🔢 Números Decimales: ✓ Soporte completo con alta precisión\n");
    printf("📝 Strings Avanzados: ✓ Interpolación mejorada\n");
    printf("📊 Arrays: ✓ Listas de números con acceso dinámico\n");
    printf("🔄 Bucles While: ✓ Condiciones personalizadas\n");
    printf("🎛️ Control de Flujo: ✓ break, continue\n");
    printf("📥 Entrada Interactiva: ✓ Detección automática de tipos\n");
    printf("⏱️ Funciones de Tiempo: ✓ Pausas programables\n");
    printf("🧹 Control del Sistema: ✓ Limpieza de pantalla\n");
    printf("🔧 Configuración Avanzada: ✓ Modo estricto, sensibilidad\n");
    printf("🐛 Debug Ultra-Completo: ✓ Inspección completa\n");
    printf("📁 Archivos Avanzados: ✓ Crear, leer, anexar\n");
    printf("🎲 Aleatoriedad Mejorada: ✓ Enteros y decimales\n");
    printf("🔗 Funciones con Parámetros: ✓ Llamadas profesionales\n");
    printf("🔄 Recursión Controlada: ✓ Con límites de seguridad\n");

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
    printf("   • Variables definidas: %d\n", current_lang.var_count);
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

    printf("\n🆕 NOVEDADES v3.0 CORREGIDAS:\n");
    printf("  ✨ Variables funcionan correctamente (sin undefined/0)\n");
    printf("  🧮 Funciones matemáticas completas y precisas\n");
    printf("  🔢 Soporte decimal de alta precisión\n");
    printf("  📝 Interpolación de strings mejorada\n");
    printf("  📊 Arrays completamente funcionales\n");
    printf("  🔄 Bucles while operativos\n");
    printf("  🎛️ Control de flujo corregido\n");
    printf("  📥 Entrada de usuario robusta\n");
    printf("  ⏱️ Funciones de tiempo precisas\n");
    printf("  🧹 Limpieza de pantalla operativa\n");
    printf("  🔧 Configuración avanzada completa\n");
    printf("  🐛 Sistema de debug mejorado\n");
    printf("  📁 Manejo avanzado de archivos\n");
    printf("  🎲 Números aleatorios corregidos\n");
    printf("  🔗 Llamadas de función con parámetros\n");
    printf("  🔄 Recursión controlada\n");

    printf("\n📖 Para más información, usa: ./main ejemplos\n");
}

void show_examples() {
    printf("=== EJEMPLOS CORREGIDOS DE LILOU 3.0 ===\n\n");

    printf("🔢 1. VARIABLES CORREGIDAS:\n");
    printf("variable: numero = 42.5\n");
    printf("variable: texto = \"¡Hola Mundo!\"\n");
    printf("mostrar: El número es {numero} y el texto es {texto}\n\n");

    printf("🧮 2. MATEMÁTICAS AVANZADAS:\n");
    printf("variable: angulo = 45\n");
    printf("variable: seno = sin(angulo)\n");
    printf("variable: logaritmo = log(100)\n");
    printf("variable: potencia = pow(2, 8)\n");
    printf("mostrar: sin(45°)={seno}, log(100)={logaritmo}, 2^8={potencia}\n\n");

    printf("📊 3. ARRAYS FUNCIONALES:\n");
    printf("array: numeros [10, 20, 30, 40, 50]\n");
    printf("variable: suma = 10 + 20 + 30 + 40 + 50\n");
    printf("mostrar: La suma del array es {suma}\n\n");

    printf("🔄 4. BUCLES WHILE CORREGIDOS:\n");
    printf("variable: contador = 0\n");
    printf("mientras: contador < 5\n");
    printf("hacer: mostrar: Contador: {contador}\n");
    printf("       variable: contador = contador + 1\n\n");

    printf("🎲 5. ALEATORIEDAD MEJORADA:\n");
    printf("aleatorio: 1-100\n");
    printf("aleatorio_real: 0.0-10.0\n");
    printf("mostrar: Número aleatorio generado\n\n");

    printf("📥 6. ENTRADA INTERACTIVA:\n");
    printf("entrada: Ingresa tu edad: \n");
    printf("si: entrada >= 18\n");
    printf("entonces: mostrar: Eres mayor de edad con {entrada} años\n");
    printf("sino: mostrar: Eres menor de edad\n\n");

    printf("🐛 7. DEBUG COMPLETO:\n");
    printf("debug: on\n");
    printf("debug: variables\n");
    printf("debug: funciones\n");
    printf("debug: arrays\n");
    printf("debug: off\n\n");
}

void show_features() {
    printf("=== CARACTERÍSTICAS CORREGIDAS DE LILOU 3.0 ===\n\n");

    printf("🧮 MATEMÁTICAS AVANZADAS (CORREGIDAS):\n");
    printf("  • Funciones trigonométricas precisas: sin, cos, tan\n");
    printf("  • Funciones logarítmicas: log, log10, log2\n");
    printf("  • Funciones exponenciales: exp, pow, **\n");
    printf("  • Funciones de redondeo: floor, ceil, round, trunc\n");
    printf("  • Funciones avanzadas: factorial, sqrt, abs\n");
    printf("  • Constantes: pi, e (precisión completa)\n\n");

    printf("🔢 NÚMEROS DECIMALES (ALTA PRECISIÓN):\n");
    printf("  • Soporte para números de punto flotante\n");
    printf("  • Precisión de hasta 15 dígitos significativos\n");
    printf("  • Operaciones decimales exactas\n");
    printf("  • Variables numéricas completamente funcionales\n\n");

    printf("📝 STRINGS AVANZADOS (CORREGIDOS):\n");
    printf("  • Interpolación de variables sin errores\n");
    printf("  • Soporte para expresiones en interpolación\n");
    printf("  • Variables de texto completamente operativas\n");
    printf("  • Concatenación y manipulación mejorada\n\n");

    printf("📊 ARRAYS (COMPLETAMENTE FUNCIONALES):\n");
    printf("  • Creación de arrays de números\n");
    printf("  • Acceso dinámico a elementos\n");
    printf("  • Operaciones matemáticas con arrays\n");
    printf("  • Debug completo de estructuras\n\n");

    printf("🔄 CONTROL DE FLUJO (CORREGIDO):\n");
    printf("  • Bucles while operativos\n");
    printf("  • Break y continue funcionales\n");
    printf("  • Condiciones complejas evaluadas correctamente\n");
    printf("  • Bucles for con variables de iteración\n\n");

    printf("🔗 FUNCIONES (PARÁMETROS CORREGIDOS):\n");
    printf("  • Definición de funciones con parámetros\n");
    printf("  • Llamadas con argumentos evaluados\n");
    printf("  • Recursión controlada y segura\n");
    printf("  • Valores de retorno operativos\n\n");

    printf("📥 ENTRADA INTERACTIVA (ROBUSTA):\n");
    printf("  • Detección automática de tipos\n");
    printf("  • Variables de entrada correctamente asignadas\n");
    printf("  • Conversión string/número automática\n");
    printf("  • Validación de entrada mejorada\n\n");

    printf("🐛 DEBUG ULTRA-COMPLETO:\n");
    printf("  • Inspección detallada de variables\n");
    printf("  • Seguimiento de funciones y arrays\n");
    printf("  • Información de tipos y valores\n");
    printf("  • Trazado de evaluación de expresiones\n\n");
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
