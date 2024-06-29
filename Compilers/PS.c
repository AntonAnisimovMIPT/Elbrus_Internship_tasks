#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct {
    int coefficient; 
    char *symbols; 
} Multiplier;

Multiplier** parse_string(char* str, int* token_count) {
    const char* delimiters = "+-";
    Multiplier** multipliers = malloc(sizeof(Multiplier*) * strlen(str));

    int signed_flag = 0;
    if (str[0] == '-') {
        signed_flag = -1; 
    } else {
        signed_flag = 1; 
    }

    char* separators = malloc(strlen(str) + 1);

    int counter1 = 0;
    if (str[0] != '-')
        {
            separators[0] = '+';
            counter1 += 1;
        }
    for (size_t i = 0; i < strlen(str); i++) {

        if (strchr(delimiters, str[i]) != NULL) {
            separators[counter1++] = str[i];
        }
    }

    separators[counter1] = '\0';

    char* token = strtok(str, delimiters);
    int count = 0;
    
    while (token != NULL) {
        Multiplier* multiplier = malloc(sizeof(Multiplier));

        int coefficient = 1;
        if (sscanf(token, "%d", &coefficient) == 1) {

            token = strchr(token, '*');
            if (token != NULL) {
                token++;
            }
            coefficient *= signed_flag; 
        } else {
            coefficient = signed_flag; 
        }

        multiplier->coefficient = coefficient;

        char* trimmed_token = malloc(strlen(token) + 1); 
        int j = 0;
        for (int i = 0; token[i] != '\0'; i++) {
            if (!isspace(token[i])) { 
                trimmed_token[j++] = token[i]; 
            }
        }
        trimmed_token[j] = '\0'; 
        multiplier->symbols = strdup(trimmed_token);

        free(trimmed_token);

        multipliers[count++] = multiplier;
        token = strtok(NULL, delimiters);

        if (separators[count] == '-') {
            signed_flag = -1; 
        } else {
            signed_flag = 1;
        }
    }

    *token_count = count;
    free(separators);
    return multipliers;
}

void remove_spaces(char *str) {
    int len = strlen(str);
    int i, j = 0;
    for (i = 0; i < len; i++) {
        if (str[i] != ' ') {
            str[j++] = str[i];
        }
        
    }
    str[j] = '\0';
}

void add_spaces_around_operators(char *str) {
    size_t len = strlen(str);
    char *result = (char*) malloc((2 * len + 1) * sizeof(char)); 

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '+' || str[i] == '-') {

            if (i != 0) {
                result[j++] = ' '; 
                result[j++] = str[i]; 
                result[j++] = ' '; 
            }
            else {
                result[j++] = str[i]; 
                result[j++] = ' '; 
            }
        } else {
            result[j++] = str[i]; 
        }
    }
    result[j] = '\0'; 
    strcpy(str, result);
    free(result);
}

void remove_trivial_coef(char *str) {
    size_t len = strlen(str);

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '1' && str[i + 1] != '\0') {
            i++; 
        } else {
            str[j++] = str[i]; 
        }
    }
    str[j] = '\0'; 
}

void remove_from_zero(char *str) {
    char *zero_pos = strchr(str, '0'); 
    if (zero_pos != NULL) {
        char *space_pos = strchr(zero_pos, ' '); 
        char *end_pos = strchr(zero_pos, '\0'); 

        char *remove_pos = (space_pos != NULL) ? space_pos : end_pos;
        *remove_pos = '\0';
    }
}

void free_multipliers(Multiplier** multipliers, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(multipliers[i]); 
    }
    free(multipliers); 
}

bool check_symbols_eq(Multiplier* multiplier1, Multiplier* multiplier2) {

    int count1[256] = {0}; 
    int count2[256] = {0}; 

    for (int i = 0; multiplier1->symbols[i] != '\0'; i++) {
        count1[(int)multiplier1->symbols[i]]++;
    }

    for (int i = 0; multiplier2->symbols[i] != '\0'; i++) {
        count2[(int)multiplier2->symbols[i]]++;
    }

    for (int i = 0; i < 256; i++) {
        if (count1[i] != count2[i]) {
            return false;
        }
    }
    return true;
}

Multiplier** sum_forms(char* str1, char* str2, int* token_count) {
    int token_count1, token_count2;
    Multiplier** multipliers1 = parse_string(str1, &token_count1); 
    Multiplier** multipliers2 = parse_string(str2, &token_count2); 

    int total_token_count = token_count1 + token_count2;
    Multiplier** combined_multipliers = malloc(sizeof(Multiplier*) * total_token_count);

    int counter = token_count1;
    for (int i = 0; i < token_count1; i++) {
        combined_multipliers[i] = multipliers1[i];
    }

    for (int i = 0; i < token_count2; i++) {
        int add_flag = 0;
        int add_pos = 0;
        for (int pos = 0; pos < counter; pos++) {
            if (check_symbols_eq(multipliers2[i], combined_multipliers[pos])) {
                add_flag = 1;
                add_pos = pos;
                break;
            }
            
        }
        if (add_flag == 1) {
            combined_multipliers[add_pos]->coefficient += multipliers2[i]->coefficient;
        }
        else {
            combined_multipliers[counter++] = multipliers2[i];
        }
    }

    *token_count = counter;
    return combined_multipliers;
}

Multiplier** substract_forms(char* str1, char* str2, int* token_count) {
    int token_count1, token_count2;
    Multiplier** multipliers1 = parse_string(str1, &token_count1); 
    Multiplier** multipliers2 = parse_string(str2, &token_count2); 

    int total_token_count = token_count1 + token_count2;
    Multiplier** combined_multipliers = malloc(sizeof(Multiplier*) * total_token_count);

    int counter = token_count1;

    for (int i = 0; i < token_count1; i++) {
        combined_multipliers[i] = multipliers1[i];
    }

    for (int i = 0; i < token_count2; i++)
    {
        int add_flag = 0;
        int add_pos = 0;
        for (int pos = 0; pos < counter; pos++) {
            if (check_symbols_eq(multipliers2[i], combined_multipliers[pos]))
            {
                add_flag = 1;
                add_pos = pos;
                break;
            }
        
        }
        if (add_flag == 1) {
            combined_multipliers[add_pos]->coefficient -= multipliers2[i]->coefficient;
        }
        else {
            multipliers2[i]->coefficient *= -1;
            combined_multipliers[counter++] = multipliers2[i];
        }
    }
    *token_count = counter;
    return combined_multipliers;
}

Multiplier** multiply_forms(char* str1, char* str2, int* token_count) {
    int token_count1, token_count2;
    Multiplier** multipliers1 = parse_string(str1, &token_count1); 
    Multiplier** multipliers2 = parse_string(str2, &token_count2); 

    int total_token_count = token_count1 * token_count2;
    Multiplier** combined_multipliers = malloc(sizeof(Multiplier*) * total_token_count);

    int counter = 0;
    for (int first_index = 0; first_index < token_count1; first_index++) {
        for (int second_index = 0; second_index < token_count2; second_index++) {
            int coef = multipliers1[first_index]->coefficient * multipliers2[second_index]->coefficient;
            char* symb = malloc(strlen(multipliers1[first_index]->symbols) + strlen("*") + strlen(multipliers2[second_index]->symbols) + 1);
            
            strcpy(symb, multipliers1[first_index]->symbols);
            strcat(symb, "*");
            strcat(symb, multipliers2[second_index]->symbols);
            remove_spaces(symb);

            Multiplier* mt = malloc(sizeof(Multiplier));
            
            mt->coefficient = coef;
            mt->symbols = symb;

            bool found = false;
            for (int pos = 0; pos < counter; pos++) {
                if (check_symbols_eq(mt, combined_multipliers[pos])) {
                    combined_multipliers[pos]->coefficient += coef;
                    found = true;
                    break;
                }
            }

            if (!found) {
                combined_multipliers[counter++] = mt;
            }
        }
    }
    *token_count = counter;
    return combined_multipliers;
}

bool comparison_forms(Multiplier** multipliers1, Multiplier** multipliers2, int token_count1, int token_count2) {
    if (token_count1 != token_count2) {
        return false; 
    }

    bool* visited = calloc(token_count2, sizeof(bool)); 

    for (int i = 0; i < token_count1; i++) {
        bool found = false; 
        for (int j = 0; j < token_count2; j++) {
            if (!visited[j] && check_symbols_eq(multipliers1[i], multipliers2[j]) && multipliers1[i]->coefficient == multipliers2[j]->coefficient) {
                visited[j] = true; 
                break;
            }
        }
        if (!found) {
            free(visited); 
            return false;
        }
    }

    for (int i = 0; i < token_count2; i++) {
        if (!visited[i]) {
            free(visited); 
            return false; 
        }
    }
    free(visited); 
    return true; 
}

char* create_combination(Multiplier** multipliers, int token_count) {
    
    char* combination_str = malloc(sizeof(char) * 1000);
    combination_str[0] = '\0';

    int position = 0;
    for (int i = 0; i < token_count; i++) {

        Multiplier* current = multipliers[i];

        if (current->coefficient != 0) {

            if (current->coefficient >= 0 && position > 0) {
                
                position += sprintf(combination_str + position, " + ");
            }
            position += sprintf(combination_str + position, "%d*%s", current->coefficient, current->symbols);
        }
    }

    return combination_str;
}

int main() {
    char mode;
    char str1[1000];
    char str2[1000];

    scanf(" %c", &mode);
    getchar();

    fgets(str1, sizeof(str1), stdin);
    str1[strcspn(str1, "\n")] = '\0';

    fgets(str2, sizeof(str2), stdin);
    str2[strcspn(str2, "\n")] = '\0';

    if (mode == '+') {
        int token_count;
        Multiplier** sum = sum_forms(str1, str2, &token_count);

        char* combination = create_combination(sum, token_count);
        remove_trivial_coef(combination);
        //remove_from_zero(combination); 
        remove_spaces(combination);
        add_spaces_around_operators(combination);
        printf("%s\n", combination);
        
        free(combination);
        free_multipliers(sum, token_count);
    }

    if (mode == '-') {
        int token_count;
        Multiplier** substract = substract_forms(str1, str2, &token_count);

        char* combination = create_combination(substract, token_count);
        remove_trivial_coef(combination);
        //remove_from_zero(combination); 
        remove_spaces(combination);
        add_spaces_around_operators(combination); 
        printf("%s\n", combination);
        
        free(combination);
        free_multipliers(substract, token_count);
    }

    if (mode == '=')
    {
        int token_count1;
        int token_count2;
        Multiplier** mt1 = parse_string(str1, &token_count1);
        Multiplier** mt2 = parse_string(str2, &token_count2);

        if (comparison_forms(mt1, mt2, token_count1, token_count2)) {
            printf("equal");
        }
        else {
            printf("not equal");
        }
    }

    if (mode == '*')
    {
        int token_count;
        Multiplier** mult = multiply_forms(str1, str2, &token_count);

        char* combination = create_combination(mult, token_count);
        remove_trivial_coef(combination);
        //remove_from_zero(combination); 
        remove_spaces(combination);
        add_spaces_around_operators(combination);
        printf("%s\n", combination);

        free(combination);
        free_multipliers(mult, token_count);
    }
}
