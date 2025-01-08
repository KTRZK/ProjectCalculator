#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable : 4996)

// Function prototypes
char* add_strings(const char* num1, const char* num2, int base);
char* multiply_strings(const char* num1, const char* num2, int base);
char* power_strings(const char* num1, const char* num2, int base);
char* base_to_decimal(const char* num, int base);
char* decimal_to_base(const char* num, int base);
char* integer_division_strings(const char* num1, const char* num2, int base);
char* modulo_strings(const char* num1, const char* num2, int base);

// Utility function to free dynamically allocated memory
void safe_free(void* ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
}

// Integer division of numbers as strings in the specified base
char* integer_division_strings(const char* num1, const char* num2, int base) {
    // Convert both numbers to decimal
    char* dec_num1 = base_to_decimal(num1, base);
    char* dec_num2 = base_to_decimal(num2, base);

    // Perform the division in decimal
    long long int n1 = strtoll(dec_num1, NULL, 10);
    long long int n2 = strtoll(dec_num2, NULL, 10);

    if (n2 == 0) {
        safe_free(dec_num1);
        safe_free(dec_num2);
        return strdup("DIV_BY_ZERO");
    }

    long long int result = n1 / n2;

    // Convert the result back to the original base
    char result_str[1024];
    sprintf(result_str, "%lld", result);

    char* final_result = decimal_to_base(result_str, base);

    // Free temporary strings
    safe_free(dec_num1);
    safe_free(dec_num2);

    return final_result;
}

// Modulo operation of numbers as strings in the specified base
char* modulo_strings(const char* num1, const char* num2, int base) {
    // Convert both numbers to decimal
    char* dec_num1 = base_to_decimal(num1, base);
    char* dec_num2 = base_to_decimal(num2, base);

    // Perform the modulo operation in decimal
    long long int n1 = strtoll(dec_num1, NULL, 10);
    long long int n2 = strtoll(dec_num2, NULL, 10);

    if (n2 == 0) {
        safe_free(dec_num1);
        safe_free(dec_num2);
        return strdup("MOD_BY_ZERO");
    }

    long long int result = n1 % n2;

    // Convert the result back to the original base
    char result_str[1024];
    sprintf(result_str, "%lld", result);

    char* final_result = decimal_to_base(result_str, base);

    // Free temporary strings
    safe_free(dec_num1);
    safe_free(dec_num2);

    return final_result;
}


// Function to handle arithmetic operations from the file
void parse_and_execute(FILE* input, FILE* output) {
    char operation;
    int base;

    while (fscanf(input, " %c %d", &operation, &base) != EOF) {
        char num1[1024], num2[1024];
        char* result = NULL;

        // Read the two numbers for the operations
        fscanf(input, "%s %s", num1, num2);

        switch (operation) {
        case '+': {
            result = add_strings(num1, num2, base);
            break;
        }

        case '*': {
            result = multiply_strings(num1, num2, base);
            break;
        }

        case '^': {
            result = power_strings(num1, num2, base);
            break;
        }

        case '/': {
            result = integer_division_strings(num1, num2, base);
            break;
        }

        case '%': {
            result = modulo_strings(num1, num2, base);
            break;
        }
        }

        if (result != NULL) {
            // Write the operation, arguments, and result to the output file
            fprintf(output, "%c %d\n%s\n%s\n%s\n\n", operation, base, num1, num2, result);
            safe_free(result);  // Free the dynamically allocated result string
        }
        else {
            // If something went wrong, just copy the arguments without a result
            fprintf(output, "%c %d\n%s\n%s\nERROR\n\n", operation, base, num1, num2);
        }
    }
}

// Addition of numbers as strings in the specified base
char* add_strings(const char* num1, const char* num2, int base) {
    // Convert both numbers to decimal
    char* dec_num1 = base_to_decimal(num1, base);
    char* dec_num2 = base_to_decimal(num2, base);

    // Perform the addition in decimal
    long long int n1 = strtoll(dec_num1, NULL, 10);
    long long int n2 = strtoll(dec_num2, NULL, 10);
    long long int result = n1 + n2;

    // Convert the result back to the original base
    char result_str[1024];
    sprintf(result_str, "%lld", result);

    char* final_result = decimal_to_base(result_str, base);

    // Free temporary strings
    safe_free(dec_num1);
    safe_free(dec_num2);

    return final_result;
}

// Multiplication of numbers as strings in the specified base
char* multiply_strings(const char* num1, const char* num2, int base) {
    // Convert both numbers to decimal
    char* dec_num1 = base_to_decimal(num1, base);
    char* dec_num2 = base_to_decimal(num2, base);

    // Perform the multiplication in decimal
    long long int n1 = strtoll(dec_num1, NULL, 10);
    long long int n2 = strtoll(dec_num2, NULL, 10);
    long long int result = n1 * n2;

    // Convert the result back to the original base
    char result_str[1024];
    sprintf(result_str, "%lld", result);

    char* final_result = decimal_to_base(result_str, base);

    // Free temporary strings
    safe_free(dec_num1);
    safe_free(dec_num2);

    return final_result;
}

// Exponentiation of numbers as strings in the specified base
char* power_strings(const char* num1, const char* num2, int base) {
    // Convert both numbers to decimal
    char* dec_num1 = base_to_decimal(num1, base);
    char* dec_num2 = base_to_decimal(num2, base);

    // Perform the power operation in decimal
    long long int n1 = strtoll(dec_num1, NULL, 10);
    long long int n2 = strtoll(dec_num2, NULL, 10);
    long long int result = 1;

    for (long long int i = 0; i < n2; i++) {
        result *= n1;
    }

    // Convert the result back to the original base
    char result_str[1024];
    sprintf(result_str, "%lld", result);

    char* final_result = decimal_to_base(result_str, base);

    // Free temporary strings
    safe_free(dec_num1);
    safe_free(dec_num2);

    return final_result;
}

// Conversion from any base to decimal
char* base_to_decimal(const char* num, int base) {
    long long int result = 0;
    for (int i = 0; num[i] != '\0'; i++) {
        char digit = num[i];
        if (digit >= '0' && digit <= '9') {
            result = result * base + (digit - '0');
        }
        else if (digit >= 'A' && digit <= 'F') {
            result = result * base + (digit - 'A' + 10);
        }
    }

    // Convert result to string
    char* result_str = (char*)malloc(1024);
    sprintf(result_str, "%lld", result);

    return result_str;
}

// Conversion from decimal to any base
char* decimal_to_base(const char* num, int base) {
    long long int n = strtoll(num, NULL, 10);
    char* result = (char*)malloc(1024);
    char* ptr = result;

    // Convert decimal to the specified base
    do {
        int remainder = n % base;
        if (remainder < 10) {
            *ptr++ = '0' + remainder;
        }
        else {
            *ptr++ = 'A' + (remainder - 10);
        }
        n /= base;
    } while (n > 0);

    *ptr = '\0';

    // Reverse the result string
    for (int i = 0, j = strlen(result) - 1; i < j; i++, j--) {
        char temp = result[i];
        result[i] = result[j];
        result[j] = temp;
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: calc.exe <input_file> [output_file]\n");
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        printf("Error: Cannot open input file.\n");
        return 1;
    }

    FILE* output;
    if (argc >= 3) {
        output = fopen(argv[2], "w");
    }
    else {
        char output_filename[256];
        sprintf(output_filename, "out_%s", argv[1]);
        output = fopen(output_filename, "w");
    }

    if (!output) {
        printf("Error: Cannot open output file.\n");
        fclose(input);
        return 1;
    }

    parse_and_execute(input, output);

    fclose(input);
    fclose(output);
    return 0;
}
