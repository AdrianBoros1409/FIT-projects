/**
 *  Adrian Boros
 *  xboros03@fit.vutbr.cz
 *  Projekt: Praca s textem
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_SIZE 101    // max size of array
#define CHARACTER_SIZE 26   // max size of enable letters
/*
 * Funkcia na vymazanie duplikatov z pola
 * size = dlzka pola
 */
void removeDuplicate(char *arr)
{
    int size = (int)strlen(arr);
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size;) {
            /* Ak sa nejaky duplikatny znak najde */
            if (arr[j] == arr[i]) {
            /* Vymaze duplikatny znak */
                for (int k = j; k < size; k++) {
                    arr[k] = arr[k + 1];
                }
            /* Dekrementuje size po vymazani duplikatneho znaku */
                size--;
            } else
                j++;
        }
    }
}

/*
 * Funkcia na zoradenie najdenych pismen abecedne
 */
void sort_Enable(char *a)
{
    int length = strlen(a);
    char temp;
    for(int j=1; j<length; j++)
    {
        for(int k=0; k<length-j; k++)
        {
            if(a[k]>=a[k+1])
            {
                temp=a[k];
                a[k]=a[k+1];
                a[k+1]=temp;
            }
        }
    }
}

/*
 * Funkcia na vycistenie pola
 */
void clean(char *a)
{
    for(int i = 0; i != '\n'; i++){     // vynuluje postupne pole kym nedojde na znak s hodnotou '\n'
        a[i] = '\0';
    }
}

/*
 * Funkcia na zistenie ci adresa alebo argument nie je dlhsia ako 100 znakov
 * Ak adresa alebo argument ma viac ako 100 znakov, vypise sa chybove hlasenie a program skonci
 */
int test_if_longer(int i)
{
    if(i > (MAX_SIZE - 1)){
        fprintf(stderr,"ADDRESS OR ARGUMENT IS LONGER THAN MAX_SIZE (100)! \n");
        return EXIT_FAILURE;
    }
    return 0;
}

/*
 * Funkcia na zistenie ci pocet adries v adresy.txt nie je viac ako 42
 * pocet = pocet adries v adresy.txt
 * ak je viac ako 42 adries vypise sa chybove hlasenie a program konci
 * ak nie je v databaze ziadna adresa, program vypise chybove hlasenie a konci
 */
int number_of_adresses(int pocet)
{
    if(pocet > 42){
        fprintf(stderr, "MANY CITIES IN ADRESS.TXT \n");
        return EXIT_FAILURE;
    }
    if(pocet == 0){
        fprintf(stderr, "DATABASE IS BLANK \n");
        return EXIT_FAILURE;
    }
    return 0;
}

/*
 * Funkcia na kontrolu nepovolenych znakov a argumente alebo v databaze
 * ak sa najde nepovoleny znak, program konci s chybovym vypisom
 */
int notAllowed(char *arr)
{
    for(int i = 0; i < (int)strlen(arr); i++){
        if(isalpha(arr[i]) == 0 && isspace(arr[i]) == 0){
            fprintf(stderr, "NOT ALLOWED CHARACTER IN ADDRESS OR ARGUMENT \n");
            return EXIT_FAILURE;
        }
    }
    return 0;
}

/*
 * Funkcia ked nie je zadany ziadny argument
 * words = pole do ktoreho sa ukladaju nacitane mesta
 * letters = pole do ktoreho sa ukladaju prve pismena miest
 */
int noArg(char *words, char *letters)
{
    int i = 0, j = 0, c, num=0;
    while((c = toupper(getchar())) != EOF)
    {
        words[i]=c;
        if(words[0] == '\n')
            continue;
        if(c == '\n'){
            num++;
            letters[j] = words[0];
            j++;
            clean(words);
            i=0;
            continue;
        }
        i++;
        if(test_if_longer(i))
            return EXIT_FAILURE;
    }
    if(number_of_adresses(num))
        return EXIT_FAILURE;
    sort_Enable(letters);
    removeDuplicate(letters);
    printf("Enable: %s \n", letters);
    return EXIT_SUCCESS;
}

/*
 * Funkcia na prevod argumentu na velke pismena
 */
void To_Upper(char *arr)
{
    for(int i = 0; i < (int)strlen(arr); i++){
        arr[i] = toupper(arr[i]);
    }
}

/*
 * Funkcia na vypis najdenych miest, povolenych znakov alebo na vypis ked sa nic nenaslo
 */
void vypis(bool foundAddr, bool enableFlag, char* letters, char *match, char *found)
{
    if(foundAddr){
        if(letters[1] == '\0'){
            printf("Found: %s\n", match);
        }
        else if(letters[1] != '\0'){
            sort_Enable(letters);
            removeDuplicate(letters);
            printf("Enable: %s \n", letters);
        }
    }
    if(enableFlag){
        if(letters[0] != '\0' && letters[1] == '\0' && !foundAddr)
        {
            printf("Found: %s\n", found);
        }
        else{
            sort_Enable(letters);
            removeDuplicate(letters);
            printf("Enable: %s \n", letters);
        }
    }
    else if(!foundAddr && !enableFlag){
        printf("Not found \n");
    }
}

int main(int argc, char *argv[])
{
    bool foundAddr = false;
    bool enableFlag = false;
    int c;
    int i = 0;
    int j = 0;
    int num_adress = 0;
    char words[MAX_SIZE] = {0};
    char letters[CHARACTER_SIZE] = {0};
    char match[MAX_SIZE] = {0};
    char found[MAX_SIZE] = {0};

    if(argc == 1){
        noArg(words,letters);
        return 0;
    }
    if(argc > 2){
        fprintf(stderr, "YOU ENTER MANY ARGUMENTS \n");
        return EXIT_FAILURE;
    }
    else
    {
        To_Upper(argv[1]);
        int length_of_argv = strlen(argv[1]);
        if(test_if_longer(length_of_argv)){
            return EXIT_FAILURE;
        }
        if(notAllowed(argv[1]))
            return 1;
        while((c = toupper(getchar())) != EOF)
        {
            if(c == '\n'){
                num_adress++;
                int length_of_arr = strlen(words);
                if(strncmp(argv[1], words, length_of_argv) == 0){
                    if (length_of_arr == length_of_argv){
                        strncpy(match, words,length_of_arr);
                        foundAddr = true;
                    }
                    else{
                        enableFlag = true;
                        strncpy(found, words, length_of_arr);
                        letters[j] = words[length_of_argv];
                        j++;
                    }
                }
                clean(words);
                i = 0;
                continue;
            }
            words[i] = c;
            i++;
            if(test_if_longer(i)){
                return EXIT_FAILURE;
            }
            if(notAllowed(words)){
                return EXIT_FAILURE;
            }
        }
        if(number_of_adresses(num_adress))
            return EXIT_FAILURE;
        vypis(foundAddr, enableFlag, letters, match, found);
        return 0;
    }
}
