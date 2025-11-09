#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// --- Configuration ---
#define MAX_STACK_SIZE 100
#define MAX_EXPRESSION_LENGTH 256

// --- Global Stack Definitions ---
// Note: We use global stacks and reset 'top' for simplicity in a single file project.
double value_stack[MAX_STACK_SIZE]; // Used for Postfix EVALUATION
char op_stack[MAX_STACK_SIZE];      // Used for Infix to Postfix CONVERSION
int top_val = -1;
int top_op = -1;

// --- Stack Utility Functions ---

void reset_stacks() {
    top_val = -1;
    top_op = -1;
}

// Value Stack (for Doubles) Operations
void push_val(double val) { value_stack[++top_val] = val; }
double pop_val() { return value_stack[top_val--]; }
int is_empty_val() { return top_val == -1; }

// Operator Stack (for Chars) Operations
void push_op(char op) { op_stack[++top_op] = op; }
char pop_op() { return op_stack[top_op--]; }
char peek_op() { return op_stack[top_op]; }
int is_empty_op() { return top_op == -1; }

// --- Precedence and Operation Logic ---

// Returns precedence of an operator
int precedence(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0; // For parentheses
}

// Applies an operator to two floating-point operands
double apply_op(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0.0) { fprintf(stderr, "Error: Division by zero\n"); exit(1); }
            return a / b;
        case '^': return pow(a, b);
    }
    return 0.0;
}

// -------------------------------------------------------------------
// --- 1. INFIX TO POSTFIX CONVERSION ---
// -------------------------------------------------------------------

void infix_to_postfix(char *infix, char *postfix) {
    int i, j = 0;
    reset_stacks();

    for (i = 0; i < strlen(infix); i++) {
        char token = infix[i];

        if (token == ' ' || token == '\n' || token == '\r') continue;

        // Operand (Digit or Decimal Point)
        if (isdigit(token) || token == '.') {
            // Copy the entire number (multi-digit or float) to the postfix string
            int start_i = i;
            while (i < strlen(infix) && (isdigit(infix[i]) || infix[i] == '.')) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' '; // Add space delimiter after number
            i--; // Decrement to re-check the character after the number
        }
        // Opening Parenthesis
        else if (token == '(') {
            push_op(token);
        }
        // Closing Parenthesis
        else if (token == ')') {
            while (!is_empty_op() && peek_op() != '(') {
                postfix[j++] = pop_op();
                postfix[j++] = ' ';
            }
            if (is_empty_op() || pop_op() != '(') {
                fprintf(stderr, "Error: Mismatched parentheses in conversion.\n");
                exit(1);
            }
        }
        // Operator
        else {
            while (!is_empty_op() && precedence(peek_op()) >= precedence(token)) {
                if (peek_op() == '(') break;
                postfix[j++] = pop_op();
                postfix[j++] = ' ';
            }
            push_op(token);
        }
    }

    // Pop any remaining operators from the stack
    while (!is_empty_op()) {
        if (peek_op() == '(') {
             fprintf(stderr, "Error: Unclosed parenthesis in conversion.\n");
             exit(1);
        }
        postfix[j++] = pop_op();
        postfix[j++] = ' ';
    }
    postfix[j] = '\0'; // Null-terminate the postfix string
}

// -------------------------------------------------------------------
// --- 2. POSTFIX EVALUATION ---
// -------------------------------------------------------------------

double evaluate_postfix(char *postfix) {
    char *token;

    // Use strtok to tokenize the postfix string by spaces
    token = strtok(postfix, " ");
    reset_stacks();

    while (token != NULL) {
        // Operand (Number)
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            push_val(atof(token));
        }
        // Operator
        else {
            // Note: Operands are popped in reverse order (val2 then val1)
            double val2 = pop_val();
            double val1 = pop_val();
            push_val(apply_op(val1, val2, token[0]));
        }
        token = strtok(NULL, " "); // Get next token
    }

    // The result is the single value remaining on the stack
    if (top_val == 0) {
        return pop_val();
    } else {
        fprintf(stderr, "Error: Invalid postfix format (final check).\n");
        exit(1);
    }
}

// -------------------------------------------------------------------
// --- Main Function for User Input ---
// -------------------------------------------------------------------

int main() {
    char infix_expression[MAX_EXPRESSION_LENGTH];
    char postfix_expression[MAX_EXPRESSION_LENGTH];
    double result;

    printf("--- Infix Expression Evaluator (Stack-based) ---\n");
    printf("Supports +, -, *, /, ^, parentheses, and floating-point numbers.\n\n");

    printf("Enter INFIX expression: ");

    if (fgets(infix_expression, MAX_EXPRESSION_LENGTH, stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }


    infix_to_postfix(infix_expression, postfix_expression);
    printf("\n--> Postfix Notation: %s\n", postfix_expression);

    char postfix_copy[MAX_EXPRESSION_LENGTH];
    strcpy(postfix_copy, postfix_expression);

    result = evaluate_postfix(postfix_copy);

    printf("--> Evaluation Result: %.4lf\n", result);

    return 0;
}
