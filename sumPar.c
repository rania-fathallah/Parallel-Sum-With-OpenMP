#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

// Function to check if the sum of two inputs equals 9
bool isPropagate(int input1, int input2) {
    return (input1 + input2 == 9);
}

// Function to check if the sum of two inputs is greater than 9
bool isGenerate(int input1, int input2) {
    return (input1 + input2 > 9);
}

// Function to calculate carry-out
bool hasCout(bool g, bool p, bool cin) {
    return g || (p && cin);
}

// Function to perform carry-lookahead addition
char* carryLookaheadAddition(char* num1, char* num2) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int maxSize = (len1 > len2 ? len1 : len2) + 2;
    int C[maxSize-1]; // Array to store carry-out for each digit position
    bool G[maxSize-2]; // Array to store Generate signals for each digit position
    bool P[maxSize-2]; // Array to store Propagate signals for each digit position
    int digit1[maxSize-1];
    int digit2[maxSize-1]; 
    C[0] = 0; // Initial carry-in is 0
    char* result = (char*)malloc(maxSize * sizeof(char));

    // Parallel section using OpenMP
#pragma omp parallel
{
    // Calculate carry-out for each digit position in parallel
#pragma omp for
    for (int i = 0; i < maxSize - 2; i++) {
        // Extract digits from input strings and convert them to integers
        digit1[i] = (i < len1) ? num1[len1 - 1 - i] - '0' : 0;
        digit2[i] = (i < len2) ? num2[len2 - 1 - i] - '0' : 0;
        // Calculate Generate and Propagate signals for each digit
        G[i] = isGenerate(digit1[i], digit2[i]);
        P[i] = isPropagate(digit1[i], digit2[i]);
    }

    // Calculate carry-out for each digit position
    for (int i = 0; i < maxSize - 2; i++) {
        C[i + 1] = hasCout(G[i], P[i], C[i]);
    }     

    // Compute sum digit by digit in parallel
#pragma omp for
    for (int i = 0; i < maxSize - 1; i++) {
        result[i] = (digit1[i] + digit2[i] + C[i]) % 10 + '0'; // Calculate digit sum
    }
}
    if(result[maxSize-2]-'0'==0){
        result[maxSize-2] = '\0';
    }else{
        result[maxSize-1] = '\0'; // Null-terminate the string
    }


    // Reverse the result string
    int start = 0;
    int end = strlen(result) - 1;
    while (start < end) {
        char temp = result[start];
        result[start] = result[end];
        result[end] = temp;
        start++;
        end--;
    }

    return result;
}

// Main function
int main() {
    char num1[100], num2[100];
    scanf("%s %s", num1, num2); // Input two numbers

    struct timeval start, stop;
    gettimeofday(&start, NULL); // Start timing

    char* sum = carryLookaheadAddition(num1, num2); // Perform carry-lookahead addition

    gettimeofday(&stop, NULL); // End timing
    unsigned long long duration = (stop.tv_sec - start.tv_sec) * 1000000LL + stop.tv_usec - start.tv_usec; // Calculate duration in microseconds

    // Output the sum and the computation time
    printf("La somme est : %s\n", sum);
    printf("Temps de calcul : %llu microsecondes\n", duration);
    
    free(sum); // Free allocated memory
    return 0;
}