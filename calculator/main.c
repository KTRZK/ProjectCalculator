#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma warning(disable : 4996)

// Function prototypes
char* add_strings(const char* num1, const char* num2, int base);
char* multiply_strings(const char* num1, const char* num2, int base);
char* power_strings(const char* num1, const char* num2, int base);
char* base_to_decimal(const char* num, int base);
char* decimal_to_base(const char* num, int base);
char* integer_division_strings(const char* num1, const char* num2, int base);
char* modulo_strings(const char* num1, const char* num2, int base);

// Bezpieczne zwalnianie pamici, aby unikn wska nik w wisz cych
void safe_free(void* ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
        ptr = NULL;
    }
}

// Konwertuje liczb  z jednego systemu liczbowego do innego
char* perform_base_conversion(const char* num, int from_base, int to_base)
{
    char* decimal = base_to_decimal(num, from_base);
    if (decimal == NULL)
    {
        return NULL;
    }
    char* result = decimal_to_base(decimal, to_base);
    safe_free(decimal);
    return result;
}

// Sprawdza, czy linia zawiera poprawne   danie konwersji mi dzy bazami
int is_base_conversion_request(const char* line, int* base1, int* base2)
{
    char extra;
    return (sscanf(line, "%d %d %c", base1, base2, &extra) == 2);
}

// Sprawdza, czy linia zawiera poprawne   danie operacji arytmetycznej
int is_arithmetic_operation_request(const char* line, char* operation, int* base)
{
    char extra;
    return (sscanf(line, " %c %d %c", operation, base, &extra) == 2);
}

// Sprawdza, czy podana baza jest poprawna (od 2 do 16)
int is_valid_base(int base)
{
    return base >= 2 && base <= 16;
}
// Weryfikuje, czy liczba jest zgodna z podanym systemem liczbowym
int is_valid_number_for_base(const char* number, int base)
{
    for (int i = 0; number[i] != '\0'; i++)
    {
        char digit = number[i];
        int value;

        if (digit >= '0' && digit <= '9')
        {
            value = digit - '0';
        }
        else if (digit >= 'A' && digit <= 'F')
        {
            value = digit - 'A' + 10;
        }
        else if (digit >= 'a' && digit <= 'f')
        {
            value = digit - 'a' + 10;
        }
        else
        {
            return 0;
        }

        if (value >= base)
        {
            return 0;
        }
    }
    return 1;
}


// Funkcja do dynamicznego odczytu linii o dowolnej d³ugoœci
char* read_line(FILE* file) {
    size_t size = 1024; // Pocz¹tkowy rozmiar
    size_t len = 0;
    char* buffer = malloc(size);
    if (!buffer) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2; // Zwiêkszamy rozmiar
            char* temp = realloc(buffer, size);
            if (!temp) {
                free(buffer);
                perror("realloc failed");
                exit(EXIT_FAILURE);
            }
            buffer = temp;
        }
        buffer[len++] = (char)c;
    }
    buffer[len] = '\0';

    if (len == 0 && c == EOF) {
        free(buffer);
        return NULL; // Koniec pliku
    }
    return buffer;
}

void parse_and_execute(FILE* input, FILE* output)
{
    int empty_line_count = 0;
    int base1 = 0, base2 = 0;
    int is_base_conversion_mode = 0;
    int error_flag = 0;
    char operation = '\0';
    int base = 10;
    int start_of_file = 1;
    int operation_started = 0;

    char* line = NULL; // WskaŸnik do dynamicznie alokowanej linii

    while ((line = read_line(input)) != NULL) {
        line[strcspn(line, "\n")] = 0;  // Usuwanie pustych linii

        // Liczenie pustych linii
        if (strlen(line) == 0)
        {
            empty_line_count++;
            if (empty_line_count == 3)
            {
                // Reset flagi konwersji
                is_base_conversion_mode = 0;
                operation = '\0';
                start_of_file = 1;
                empty_line_count = 0;
                if (error_flag)
                {
                    fprintf(output, "ERROR\n\n");
                    error_flag = 0;  // Reset flagi error
                }
                operation_started = 0;
                free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
                continue;
            }
            free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
            continue;
        }
        else
        {
            // Reset licznika linii
            empty_line_count = 0;
        }

        if (is_base_conversion_mode == 1) {
            line[strcspn(line, "\n")] = 0;
            if (!is_valid_number_for_base(line, base1))
            {
                error_flag = 1;
                free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
                continue;
            }

            // Dokonywanie konwersji
            char* result = perform_base_conversion(line, base1, base2);
            if (result)
            {
                fprintf(output, "%s\n\n%s\n\n", line, result);
                safe_free(result);
            }
            else
            {
                error_flag = 1;
            }
            empty_line_count = 0;
            free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
            continue;
        }

        // W acznie flagi is_base_conversion_mode je li wyst puj konwersja
        if ((is_base_conversion_request(line, &base1, &base2) || (start_of_file && is_base_conversion_mode)))
        {
            start_of_file = 0;
            is_base_conversion_mode = 1;
            operation_started = 1;
            fprintf(output, "%d %d\n\n", base1, base2);
            // Sprawdzanie poprawnoœci zmiennych
            if (!is_valid_base(base1) || !is_valid_base(base2))
            {
                error_flag = 1;
                free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
                continue;
            }
            free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
            continue;
        }

        // Dokonywanie operacji arytmetycznych
        else if (is_arithmetic_operation_request(line, &operation, &base) || (start_of_file && operation))
        {
            start_of_file = 0;

            is_base_conversion_mode = 0;
            operation_started = 1;

            // Sprawdzanie poprawnoœci zmiennych
            if (!is_valid_base(base))
            {
                error_flag = 1;
                free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
                continue;
            }

            char* result = NULL;
            int contains_valid_number = 0;

            fprintf(output, "%c %d\n\n", operation, base);

            // Wczytywanie zmiennych do operacji arytmetycznych
            while ((line = read_line(input)) != NULL)
            {
                line[strcspn(line, "\n")] = 0;

                // Jeœli linia jest pusta, powiêkszanie wartoœci empty_line_count
                if (strlen(line) == 0)
                {
                    empty_line_count++;
                    if (empty_line_count == 3)
                    {
                        empty_line_count = 0;
                        break;  // Koniec obecnej operacji
                    }
                    free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
                    continue;
                }

                // Reset empty_line_count na niepustej linii
                empty_line_count = 0;

                // Sprawdzanie poprawnoœci zmiennych
                if (!is_valid_number_for_base(line, base))
                {
                    error_flag = 1;
                    free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
                    break;
                }

                fprintf(output, "%s\n\n", line);
                contains_valid_number = 1;

                if (!result)
                {
                    result = strdup(line);
                }
                else
                {
                    char* temp_result = NULL;

                    // Dokonywanie odpowiedniej operacji w zale¿noœci od znaku
                    switch (operation)
                    {
                    case '+':
                        temp_result = add_strings(result, line, base);
                        break;
                    case '*':
                        temp_result = multiply_strings(result, line, base);
                        break;
                    case '^':
                        temp_result = power_strings(result, line, base);
                        break;
                    case '/':
                        temp_result = integer_division_strings(result, line, base);
                        break;
                    case '%':
                        temp_result = modulo_strings(result, line, base);
                        break;
                    default:
                        error_flag = 1;
                        break;
                    }

                    safe_free(result);
                    result = temp_result;
                }

                free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
            }

            if (contains_valid_number && result && !error_flag)
            {
                fprintf(output, "%s\n\n", result);
                safe_free(result);
            }
            else
            {
                error_flag = 1;
            }
        }
        else
        {
            error_flag = 1;
        }

        if (error_flag)
        {
            fprintf(output, "ERROR\n\n");
            error_flag = 0;
        }

        free(line); // Zwolnienie pamiêci po zakoñczeniu przetwarzania linii
    }

    if (error_flag)
    {
        fprintf(output, "ERROR\n\n");
        error_flag = 0;
    }
}



// Funkcja do por wnywania dw ch ci g w znak w reprezentuj cych liczby
int compare_strings(const char* num1, const char* num2, int base)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    if (len1 > len2) return 1;
    if (len1 < len2) return -1;
    return strcmp(num1, num2);
}

// Funkcja konwertuj ca ci g znak w z systemu o podanej podstawie na liczb  w systemie dziesi tnym
char* base_to_decimal(const char* num, int base_from)
{
    int len = strlen(num);
    char* result = strdup("0");
    if (!result) return NULL;
    char* base = strdup("1");
    if (!base)
    {
        safe_free(result);
        return NULL;
    }

    for (int i = len - 1; i >= 0; i--)
    {
        int digit = isdigit(num[i]) ? num[i] - '0' : toupper(num[i]) - 'A' + 10;
        if (digit >= base_from)
        {
            safe_free(result);
            safe_free(base);
            return NULL;
        }

        char digit_str[2] = { '0' + digit, '\0' };
        char* temp1 = multiply_strings(base, digit_str, 10);
        if (!temp1)
        {
            safe_free(result);
            safe_free(base);
            return NULL;
        }

        char* temp2 = add_strings(result, temp1, 10);
        safe_free(temp1);
        safe_free(result);
        result = temp2;

        char base_from_str[3];
        sprintf(base_from_str, "%d", base_from);
        char* temp_base = multiply_strings(base, base_from_str, 10);
        safe_free(base);
        base = temp_base;
    }

    safe_free(base);
    return result;
}

// Konwertuje liczb  z systemu dziesi tnego na dowolny system liczbowy
char* decimal_to_base(const char* num, int base_to)
{
    if (strcmp(num, "0") == 0) return strdup("0");

    char* result = strdup("");
    if (!result) return NULL;
    char* quotient = strdup(num);
    if (!quotient)
    {
        safe_free(result);
        return NULL;
    }
    char base_to_str[3];
    sprintf(base_to_str, "%d", base_to);

    while (compare_strings(quotient, "0", 10) > 0)
    {
        char* remainder = modulo_strings(quotient, base_to_str, 10);
        if (!remainder)
        {
            safe_free(result);
            safe_free(quotient);
            return NULL;
        }

        int rem_digit = atoi(remainder);
        char rem_char = rem_digit < 10 ? rem_digit + '0' : rem_digit - 10 + 'A';

        char temp[2] = { rem_char, '\0' };
        char* new_result = (char*)malloc(strlen(result) + 2);
        if (!new_result)
        {
            safe_free(remainder);
            safe_free(result);
            safe_free(quotient);
            return NULL;
        }
        strcpy(new_result + 1, result);
        new_result[0] = rem_char;

        safe_free(result);
        result = new_result;

        char* temp_quotient = quotient;
        quotient = integer_division_strings(quotient, base_to_str, 10);
        safe_free(temp_quotient);
        safe_free(remainder);
    }

    safe_free(quotient);
    return result;
}


// Funkcja do odejmowania dw ch du ych liczb zapisanych jako ci gi znak w
char* subtract_strings(const char* num1, const char* num2, int base)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    char* result = (char*)malloc(len1 + 1);
    if (!result) return NULL;

    int carry = 0, i = len1 - 1, j = len2 - 1, k = len1;
    result[k--] = '\0';

    while (i >= 0 || j >= 0)
    {
        int digit1 = i >= 0 ? (isdigit(num1[i]) ? num1[i] - '0' : num1[i] - 'A' + 10) : 0;
        int digit2 = j >= 0 ? (isdigit(num2[j]) ? num2[j] - '0' : num2[j] - 'A' + 10) : 0;
        int sub = digit1 - digit2 - carry;
        if (sub < 0)
        {
            sub += base;
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        result[k--] = sub < 10 ? sub + '0' : sub - 10 + 'A';
        i--; j--;
    }

    char* final_result = result + k + 1;
    while (*final_result == '0' && *(final_result + 1)) final_result++;
    char* trimmed_result = strdup(final_result);
    safe_free(result);
    return trimmed_result;
}




// Funkcja do dodawania dw ch liczb w systemie o podanej podstawie
char* add_strings(const char* num1, const char* num2, int base)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int max_len = len1 > len2 ? len1 : len2;
    char* result = (char*)malloc(max_len + 2);
    if (!result) return NULL;

    int carry = 0, i = len1 - 1, j = len2 - 1, k = max_len;
    result[k + 1] = '\0';

    while (i >= 0 || j >= 0 || carry > 0)
    {
        int digit1 = i >= 0 ? (isdigit(num1[i]) ? num1[i] - '0' : num1[i] - 'A' + 10) : 0;
        int digit2 = j >= 0 ? (isdigit(num2[j]) ? num2[j] - '0' : num2[j] - 'A' + 10) : 0;
        int sum = digit1 + digit2 + carry;
        carry = sum / base;
        sum = sum % base;
        result[k--] = sum < 10 ? sum + '0' : sum - 10 + 'A';
        i--; j--;
    }

    char* trimmed_result = strdup(result + k + 1);
    safe_free(result);
    return trimmed_result;
}

// Funkcja do mno enia dw ch du ych liczb zapisanych jako ci gi znak w
char* multiply_strings(const char* num1, const char* num2, int base)
{
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int result_len = len1 + len2;
    int* temp_result = (int*)calloc(result_len, sizeof(int));
    if (!temp_result) return NULL;

    for (int i = len1 - 1; i >= 0; i--)
    {
        int digit1 = isdigit(num1[i]) ? num1[i] - '0' : num1[i] - 'A' + 10;
        for (int j = len2 - 1; j >= 0; j--)
        {
            int digit2 = isdigit(num2[j]) ? num2[j] - '0' : num2[j] - 'A' + 10;
            int product = digit1 * digit2 + temp_result[i + j + 1];
            temp_result[i + j + 1] = product % base;
            temp_result[i + j] += product / base;
        }
    }

    char* result = (char*)malloc(result_len + 1);
    if (!result)
    {
        safe_free(temp_result);
        return NULL;
    }

    int k = 0;
    while (k < result_len && temp_result[k] == 0) k++;
    for (int i = k; i < result_len; i++)
    {
        result[i - k] = temp_result[i] < 10 ? temp_result[i] + '0' : temp_result[i] - 10 + 'A';
    }
    result[result_len - k] = '\0';

    safe_free(temp_result);
    return strlen(result) == 0 ? strdup("0") : strdup(result);
}

// Funkcja do pot gowania du ych liczb zapisanych jako ci gi znak w
char* power_strings(const char* num1, const char* num2, int base)
{
    char* result = strdup("1");
    char* base_num = strdup(num1);
    int exponent = atoi(num2);

    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        {
            char* temp = result;
            result = multiply_strings(result, base_num, base);
            safe_free(temp);
        }
        char* temp_base = base_num;
        base_num = multiply_strings(base_num, base_num, base);
        safe_free(temp_base);
        exponent /= 2;
    }

    safe_free(base_num);
    return result;
}

// Funkcja do dzielenia ca kowitoliczbowego dw ch du ych liczb zapisanych jako ci gi znak w
char* integer_division_strings(const char* num1, const char* num2, int base)
{
    if (compare_strings(num2, "0", base) == 0) return strdup("DIV_BY_ZERO");

    char* result = strdup("0");
    char* remainder = strdup(num1);

    while (compare_strings(remainder, num2, base) >= 0)
    {
        char* temp = remainder;
        remainder = subtract_strings(remainder, num2, base);
        safe_free(temp);

        char* temp_result = result;
        result = add_strings(result, "1", base);
        safe_free(temp_result);
    }

    safe_free(remainder);
    return result;
}

// Funkcja do obliczania reszty z dzielenia dw ch du ych liczb zapisanych jako ci gi znak w
char* modulo_strings(const char* num1, const char* num2, int base)
{
    if (compare_strings(num2, "0", base) == 0) return strdup("DIV_BY_ZERO");

    char* remainder = strdup(num1);

    while (compare_strings(remainder, num2, base) >= 0)
    {
        char* temp = remainder;
        remainder = subtract_strings(remainder, num2, base);
        safe_free(temp);
    }

    return remainder;
}


//G  wna funkcja obs uguj ca otwieranie i zamykanie plik w txt oraz uruchamiaj ca funkcj  parse_and_execute
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input)
    {
        perror("Error opening input file");
        return 1;
    }

    FILE* output = fopen(argv[2], "w");
    if (!output)
    {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    parse_and_execute(input, output);

    fclose(input);
    fclose(output);

    return 0;
}