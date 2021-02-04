/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Adrian Boros [xboros03]
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->capacity = cap;
    c->size = 0;
    c->obj = malloc(cap * sizeof(struct obj_t));        // alokuje sa pamat velkosti kapacity clusteru

    if (c->obj == NULL)                                 // ak NULL, tak kapacita je 0
        c->capacity = 0;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    if(c->obj)
        free(c->obj);       // uvolnenie pamate

    c->capacity = 0;        // inicializacia na prazdny zhluk
    c->size = 0;
    c->obj = NULL;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->capacity <= c->size)                          // ak je kapacita mensia (rovna) ako velkost zhluku tak sa kapacita zhluku zvysi
        resize_cluster(c,c->capacity + CLUSTER_CHUNK);

    c->obj[c->size] = obj;                              // prida objekt na zhluk s indexom c->size cize na posledny index
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // TODO
    for(int i = 0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]);     // prida objekt c2 do zhluku c1 a zvacsi kapacita ak je to potrebne

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);              // vymaze zhluk z pola na indexe IDX

    for(int i = idx; i < narr - 1; i++)
        carr[i] = carr[i+1];                // posunie indexy pola carr po odstraneni

    return (narr - 1);                      // vrati narr - 1 poloziek lebo 1 polozka sa odstranila
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // TODO
    float dx = o1->x - o2->x;
    float dy = o1->y - o2->y;

    return sqrtf((dx * dx) + (dy * dy));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float distance = 0.0;
    int counter = 0;
    for (int i = 0; i < c1->size; i++)          // porovna sa kazdy zhluk s kazdym
    {
        for (int j = i; j < c2->size; j++)
        {
            distance += obj_distance(&c1->obj[i], &c2->obj[j]);     // pocita celkovu vzdialenost objektov v zhlukoch
            counter++;                                              // pocitadlo porovnanych objektov
        }
    }
    return distance / counter;                                      // vrati priemer vzdialenosti
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float distance;
    float min_dist = INT_MAX;
    for (int i = 0; i < narr; i++)
    {
        for (int j = i+1; j < narr; j++)
        {
            distance = cluster_distance(&carr[i], &carr[j]);        // vypocet vzdialenosti zhlukov
            if (distance < min_dist) {                              // hladanie najmensej vzdialenosti
                min_dist = distance;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *fr = fopen(filename, "r");                            // otvorenie suboru na citanie
    if(fr == NULL){                                             // kontrola spravneho otvorenia, v pripade chyby sa vypise chybove hlasenie
        fprintf(stderr, "FAILED TO OPEN %s\n", filename);
        *arr = NULL;
        return EXIT_FAILURE;
    }

    int count;
    struct obj_t obj;
    if(fscanf(fr, "count=%d", &count) == 1){                    // nacitanie poctu objektov v subore, ulozi sa do premennej count
        *arr = malloc(count * sizeof(struct cluster_t));        // alokacia pamate
        if(count <= 0){                                         // kontrola ci count v subore je kladne cislo
            fprintf(stderr, "COUNT IS LESS THAN 0 OR IS 0 \n");
            *arr = NULL;
            return EXIT_FAILURE;
        }
        if(*arr == NULL){                                       // kontrola ci sa podarilo naalokovat pamat
            fprintf(stderr, "MEMORY ALLOCATION ERROR \n");
            return EXIT_FAILURE;
        }
    }
    else {
        fprintf(stderr, "INCORRECT FORMAT OF %s, COUNT IS INCORRECT \n", filename);     // ak je chyba v subore na riadku s count
        *arr = NULL;
        return EXIT_FAILURE;
    }

    char endLine;
    for(int i = 0; i < count; i++)
    {
        int load = fscanf(fr, "%d %f %f%c", &obj.id, &obj.x, &obj.y, &endLine);         // nacitanie hodnot zo suboru
        if(load == 3 || load == 4)
        {
            if(obj.x < 0 || obj.x > 1000){                                              // kontrola ci su suradnice v rozsahu
                fprintf(stderr, "X WITH INDEX %d MUST BE IN RANGE 0 <= X <= 1000 \n", obj.id);
                *arr = NULL;
                return EXIT_FAILURE;
            }
            else if(obj.y < 0 || obj.y > 1000){                                          // kontrola ci su suradnice v rozsahu
                fprintf(stderr, "Y WITH INDEX %d MUST BE IN RANGE 0 <= Y <= 1000 \n", obj.id);
                *arr = NULL;
                return EXIT_FAILURE;
            }
            else{
                if(endLine == '\n' || endLine == '\0'){      // ak je na konci riadku Enter alebo nie je nic,
                    init_cluster(&(*arr)[i], 1);             // inicializuje sa zhluk a prida sa do pola zhlukov
                    append_cluster(&(*arr)[i], obj);
                }
                else{
                    fprintf(stderr, "INCORRECT FORMAT OF %s \n", filename);     // ak je chybny riadok v subore
                    *arr = NULL;
                    return EXIT_FAILURE;
                }
            }
        }
        else{
            fprintf(stderr, "THERE IS NOT ENOUGH OBJECTS IN %s \n", filename);      // ak count je vacsi ako pocet objektov v subore, vypise sa chyba
            *arr = NULL;
            return EXIT_FAILURE;
        }
    }

    if(fclose(fr) == EOF)                                   // kontrola zatvorenia suboru
        fprintf(stderr, "FAILED TO CLOSE %s \n", filename);
    return count;                                           // vrati pocet objektov
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{

    int number;
    char *pEnd;
    if(argc == 2)
        number = 1;                                     // ak sa nezada pocet zhlukov, defaultne je 1
    else if(argc == 3){
        number = strtol(argv[2], &pEnd, 10);            // kontrola ci argument je cislo a ci je vacsie ako 0
        if(*pEnd != '\0' || number <= 0){
            fprintf(stderr, "ARGUMENT IS NOT A NUMBER OR IS <= 0 \n");
            return EXIT_FAILURE;
        }
    }
    else{
        fprintf(stderr, "INCORRECT FORMAT OF INPUT \n");
        return EXIT_FAILURE;
    }
    struct cluster_t *clusters;
    int loaded = load_clusters(argv[1], &clusters);     // do premennej loaded sa ulozi nacitany pocet zhlukov
    if(loaded == EXIT_FAILURE)                                     // ak je nejaka chyba v nacitani objektov zo suboru
        return EXIT_FAILURE;
    if(number > loaded){                                // ak sa zada vacsie cislo ako je pocet objektov v subore
        fprintf(stderr, "THERE IS LESS OBJECT IN %s THAN %d \n", argv[1],number);
        return EXIT_FAILURE;
    }
    for(int i = number; i < loaded; loaded--){
        int c1, c2;
        find_neighbours(clusters, loaded, &c1, &c2);    // najdenie najblizsieho zhluku
        merge_clusters(&clusters[c1], &clusters[c2]);   // zjednotenie zhlukov
        remove_cluster(clusters, loaded, c2);           // odstranenie clustera
    }

    print_clusters(clusters, loaded);
    for(int i = 0; i < loaded; i++){                    // inicializacia na prazdny zhluk
        clear_cluster(&clusters[i]);
    }
    free(clusters);                                     // uvolnenie pamate
    return 0;
}
