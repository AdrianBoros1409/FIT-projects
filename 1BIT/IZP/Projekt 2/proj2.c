/**
 *  Projekt c.2: Iteracne vypocty
 *  Adrian Boros
 *  xboros03@fit.vutbr.cz
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define EPSILON 1.0e-10     // presnost na 10 desatinnych miest
#define MAX_HEIGHT 100      // maximalna vyska
#define IMPL_HEIGHT 1.5     // implicitna vyska
#define MAX_ANGLE 1.4       // maximalny uhol
#define MAX_ITTERATION 13   // maximalny pocet iteracii
#define PRINT_TAN "%d %e %e %e %e %e\n"     // format na vypis porovnani uhlov
#define PRINT_MEASURE "%.10e \n"            // format na vypis nameranych hodnot

/*
 * Funkcia na vypis napovedy v pripade zadania argumentu "--help"
 */
int help()
{
    printf("Program: Iteracne vypocty \n");
    printf("Autor: Adrian Boros [xboros03] \n \n");
    printf("Program implementuje funkce nutne pro vypocet vzdalenosti a vysky pomoci uhlu naklonu mereneho pristroje. \n");
    printf("Program sa spusti v nasledovnej podobe: \n \n ./proj2 --help \n ./proj2 --tan A N M \n alebo ./proj2 [-c X] -m A [B] \n \n");
    printf("--help sposobi ze program vytiskne napovedu pouzivani programu a skonci. \n");
    printf("--tan porovna presnosti vypoctu tangens uhlu A (v radianech) mezi volanim tan z matematicke knihovny, a vypoctu ");
    printf("tangens pomoci Taylorova polynomu a zretezeneho zlomku. Argumenty N a M udavaju, ve kterych iteracich iteracniho vypoctu ma porovnani probihat. 0 < N <= M < 14 \n");
    printf("-m vypocita a zmeri vzdalenosti  \n\n");
    printf("Uhel α (viz obrazek) je dan argumentem A v radianech. Program vypocita a vypise vzdalenost mereneho objektu. 0 < A <= 1.4 < π/2 \n");
    printf("Pokud je zadan, uhel β udava argument B v radianech. Program vypocita a vypise i vysku mereneho objektu. 0 < B <= 1.4 < π/2 \n");
    printf("Argument -c nastavuje vysku mericiho pristroje c pro vypocet. Vyske c je dana argumentem X (0 < X <= 100). Argument je volitelny - implicitny vyska je 1.5 metru. \n");
    return EXIT_SUCCESS;
}


/*
 * Funkcia na vypis chybovych hlaseni
 * int code = cislo chyby
 * po vypise chyby program konci
 */
int print_ERROR(int code)
{
    if(code == 1)
        fprintf(stderr, "INCORRECT NUMBER OF ITTERATION \n");
    else if(code == 2)
        fprintf(stderr, "INCORRECT ANGLE, MUST BE IN RANGE OF 0 < A <= 1.4 < π/2 \n");
    else if(code == 3)
        fprintf(stderr, "INCORRECT HEIGHT, MUST BE IN RANGE  0 < X <= 100 \n");
    else if(code == 4)
        fprintf(stderr, "MISSING ARGUMENTS, FOR MORE INFO ENTER \"--help\" \n");
    else if(code == 5)
        fprintf(stderr, "INVALID ARGUMENT, FOR MORE INFO ENTER \"--help\"\n");
    else if(code == 6)
        fprintf(stderr, "ARGUMENT IS NOT A NUMBER \n");
    return EXIT_FAILURE;
}

/*
 * Funkcia na kontrolu povolenej vysky
 * double c = zadana vyska
 * ak je zadana vyska mensia ako nula alebo vacsia ako 100 vrati sa false, inak sa vrati true
 */
bool ctrl_Height(double c)
{
    return ((c < 0) || (c > MAX_HEIGHT)) ? false : true;
}

/*
 * Funkcia na kontrolu zadaneho uhlu
 * double x = zadany uhol v radianoch
 */
bool ctrl_Angle(double x)
{
    return ((x <= 0) || (x > MAX_ANGLE)) ? false : true;
}

/*
 * Funkcia na kontrolu poctu iteracii
 * int from = od ktorej iteracie sa ma pocitat
 * int to = po ktoru iteraciu sa ma pocitat
 */
bool ctrl_Itter(int from, int to)
{
    return ((from < 0) || (to > MAX_ITTERATION) || (to < from)) ? false : true;
}

/*
 * Funkcia na vypocet Taylorovho polynomu
 * double x = zadany uhol v radianoch
 * unsigned int n = pocet clenov polynomu
 */
double taylor_tan(double x, unsigned int n)
{
    const double CITATEL[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    const double MENOVATEL[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
    double cf = 0.0;
    double exp = x;
    for(unsigned int i = 0; i < n; i++)
    {
        cf = cf + (CITATEL[i] * exp) / MENOVATEL[i];
        exp *= x * x;
    }
    return cf;
}

/*
 * Funkcia na vypocet zretazeneho zlomku
 * double x = zadany uhol v radianoch
 * unsigned int n = pocet clenov polynomu
 */
double cfrac_tan(double x, unsigned int n)
{
    double cf = 0.0;
    for (unsigned int k = n; k > 0; k--)
    {
        double b = 1.0;
        double a = (2*k-1)/x;
        cf = b/(a-cf);
    }
    return cf;
}

/*
 * Funkcia na absolutnu hodnotu
 * v pripade zaporneho cisla sa vrati -x cize sa z toho stane kladne
 */
double my_Absolute(double x)
{
    return (x < 0) ? -x : x;
}

/*
 * Funkcia na vypocet poctu iteracii
 * cyklus prebieha kym rozdiel medzi dvomi iteraciami nie je mensi alebo rovny zadanej presnosti
 */
double tan_eps(double x)
{
  double newSum = 0.;
  double oldSum = 0.;
  int i = 1;
  do
  {
    oldSum = newSum;
    newSum = cfrac_tan(x, i);
    i++;
  } while(my_Absolute(newSum - oldSum) >= EPSILON);
  return newSum;
}

/*
 * Funkcia na vypocet vzdialenosti meraneho objektu
 * double alfa = zadany uhol v radianoch
 * double c = vyska v metroch
 */
double width(double alfa, double c)
{
    return c / tan_eps(alfa);
}

/*
 * Funkcia na vypocet vysky meraneho objektu
 * double beta = zadany uhol v radianoch
 * double c = vyska v metroch
 * double width = dlzka
 */
double height(double beta, double c, double width)
{
    return tan_eps(beta) * width + c;
}

/*
 * Funkcia na vypis vypoctu tangens uhlu v radianoch
 * porovnava sa vypocet tangens uhla A medzo volanim tan z knihovne, a vypoctom tangens pomocou Taylorovho polynomu a zretazeneho zlomku
 */
void print_tan(char *angle, char *from, char *to)
{
    char *pEnd1, *pEnd2, *pEnd3;
    double x = strtod(angle, &pEnd1);
    int N = strtol(from, &pEnd2, 10);
    int M = strtol(to, &pEnd3, 10);
    if(*pEnd1 != '\0' || *pEnd2 != '\0' || *pEnd3 != '\0')
        print_ERROR(6);
    else if(ctrl_Itter(N, M) == 0)   // kontrola ci je zadany povoleny pocet iteracii, ak nie vypise sa chybove hlasenie
        print_ERROR(1);
    else if(isnan(x) || isinf(x))
        print_ERROR(6);
    else{
        for(int i = N; i <= M; i++)
        {
            printf(PRINT_TAN, i, tan(x), taylor_tan(x, i), my_Absolute(taylor_tan(x, i) - tan(x)),
                cfrac_tan(x ,i), my_Absolute(cfrac_tan(x ,i) - tan(x)));
        }
    }
}

int main(int argc, char **argv)
{
    char *pEnd1, *pEnd2, *pEnd3;
    switch(argc)
    {
        case 1: print_ERROR(4);     // ak nie je zadany ziadny argument vypise sa chybove hlasenie
                break;
        case 2: if(strcmp(argv[1], "--help") == 0)  // ak je zadany "--help" vypise sa napoveda, ak je zadane nieco ine vypise sa chybove hlasenie
                    help();
                else print_ERROR(5);
                break;
        case 3: if(strcmp(argv[1], "-m") == 0){     // ak bol zadany argument "-m" s jednym uhlom vypise sa vyska objektu
                    double alfa = strtod(argv[2], &pEnd1);
                    if(*pEnd1 != '\0')
                        print_ERROR(6);
                    else if(ctrl_Angle(alfa) == 0)      // kontrola ci uhol je v povolenom rozsahu
                        print_ERROR(2);
                    else printf(PRINT_MEASURE,width(alfa,IMPL_HEIGHT));
                }
                else print_ERROR(5);
                break;
        case 4: if(strcmp(argv[1], "-m") == 0){     // ak bol zadany argument "-m" s dvomi uhlami vypise sa vyska a vzdialenost objektu
                    double alfa = strtod(argv[2], &pEnd1);
                    double beta = strtod(argv[3], &pEnd2);
                    if(*pEnd1 != '\0' || *pEnd2 != '\0')
                        print_ERROR(6);
                    else if(ctrl_Angle(alfa) == 0 || ctrl_Angle(beta) == 0)         // kontrola ci uhly su v povolenom rozsahu
                        print_ERROR(2);
                    else{
                        printf(PRINT_MEASURE,width(alfa,IMPL_HEIGHT));
                        printf(PRINT_MEASURE,height(beta,IMPL_HEIGHT, width(alfa,IMPL_HEIGHT)));
                    }
                }
                else print_ERROR(5);
                break;
        case 5: if(strcmp(argv[1], "--tan") == 0){      // ak bol zadany argument "--tan"
                        print_tan(argv[2], argv[3], argv[4]);
                }
                else if((strcmp(argv[3],"-m") && strcmp(argv[1], "-c")) == 0)   //ak bol zadany argument "-c" a "-m"
                {
                    double alfa = strtod(argv[4], &pEnd1);
                    double height = strtod(argv[2], &pEnd2);      // nastavenie vysky
                    if(*pEnd1 != '\0' || *pEnd2 != '\0')
                        print_ERROR(6);
                    else if(ctrl_Height(height) == 0)       // kontrola vysky
                        print_ERROR(3);
                    else if(ctrl_Angle(alfa) == 0)          // kontrola uhla
                        print_ERROR(2);
                    else
                        printf(PRINT_MEASURE, width(alfa, height));
                }
                else print_ERROR(5);
                break;
        case 6: if((strcmp(argv[3],"-m") && strcmp(argv[1], "-c")) == 0)        //ak bol zadany argument "-c" a "-m"
                {
                    double alfa = strtod(argv[4], &pEnd1);
                    double beta = strtod(argv[5], &pEnd2);
                    double vyska = strtod(argv[2], &pEnd3);
                    if(*pEnd1 != '\0' || *pEnd2 != '\0' || *pEnd3 != '\0')
                        print_ERROR(6);
                    else if(ctrl_Angle(alfa) == 0 || ctrl_Angle(beta) == 0)     // kontrola ci je zadany uhol v povolenom rozsahu
                        print_ERROR(2);
                    else if(ctrl_Height(vyska) == 0)        // kontrola ci vyska je v povolenom rozsahu
                        print_ERROR(3);
                    else{
                        printf(PRINT_MEASURE, width(alfa, vyska));
                        printf(PRINT_MEASURE, height(beta, vyska, width(alfa, vyska)));
                    }
                }
                else print_ERROR(5);
                break;
        default:print_ERROR(5);
        break;
    }
    return EXIT_SUCCESS;
}


