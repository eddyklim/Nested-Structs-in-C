/*
 * eduardk_hw5.c
 *
 *  Created on: Feb 10, 2018
 *  Author: Eduard Klimenko
 *
 *  This program takes in an input file of customers and the items they bought.
 *  Then stores the data into structures. The data is then sorted buy value of
 *  items bought. Two output files are created one for chronological purchases
 *  and the second based on purchase value. Input and output files are .txt
 *  extension.
 *
 *  NOTE: Some editors do not register the end of line character! For example,
 *  some versions of notepad do not while wordpad do!
 */

// Constants
#define NAME_LEN 30
#define MAX_CUST 20
#define MAX_PURCHASES 10

// Headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Structures
struct purchase_tag {
    char itemName[NAME_LEN];
    int quantity;
    double price;
};
typedef struct purchase_tag Purchase;

struct customer_tag {
    char name[NAME_LEN];
    Purchase *order[MAX_PURCHASES];
    int orderSize;
    double total;
};
typedef struct customer_tag Customer;

// Prototypes
int totalValueCompare(const void *a1, const void *a2);
int itemValueCompare(const void *w1, const void *w2);
void bubbleSort(void *ptrs[], int size,
        int (*compare)(const void *, const void *));
int loadCustomers(Customer *p[], FILE *in);
void printChronological(Customer *ptrs[], int size);
void printFinancial(Customer *ptrs[], int size);

/*
 * This is the main function. Looks for a specific input file, if not located
 * return 1. Else, creates an array of customer pointers to store information.
 * After information is loaded, a chronological output file is created. The
 * customers are then sorted and a new financial output file is created.
 */
int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    FILE *infile = fopen("hw5input.txt", "r");
    if (infile == NULL) {
        printf("File did not open.");
        return 1;
    }
    Customer *arr[20];
    int arrCount = 0;
    arrCount = loadCustomers(arr, infile);
    printChronological(arr, arrCount);
    bubbleSort((void *) arr, arrCount, totalValueCompare);
    for (int i = 0; i < arrCount; i++) {
        bubbleSort((void *) (arr[i]->order), arr[i]->orderSize,
                itemValueCompare);
    }
    printFinancial(arr, arrCount);
    fclose(infile);
    return 0;
}

/*
 * This function takes in a pointer array of customers and the input file.
 * It then parses line by line filling in information about the purchase.
 * If the name is new, creates a new struct to store the customer.
 */
int loadCustomers(Customer *p[], FILE *in) {
    char stuName[NAME_LEN], stuffName[NAME_LEN];
    int i, count = 0;
    float w = 0;
    int q;
    while (count < MAX_CUST
            && fscanf(in, "%s %d %s $%f", stuName, &q, stuffName, &w) != EOF) {
        for (i = 0; i < count && strcmp(stuName, p[i]->name); i++)
            ;
        if (i == count) {
            p[i] = (Customer *) malloc(sizeof(Customer));
            p[i]->orderSize = 0;
            count++;
        }
        p[i]->order[p[i]->orderSize] = (Purchase *) malloc(sizeof(Purchase));
        strcpy(p[i]->name, stuName);
        strcpy(p[i]->order[p[i]->orderSize]->itemName, stuffName);
        p[i]->order[p[i]->orderSize]->quantity = q;
        p[i]->order[p[i]->orderSize]->price = w;
        p[i]->total += w * q;
        p[i]->orderSize++;
    }
    fclose(in);
    return count;
}

/*
 * This function takes in a pointer array of customers and the size of the array.
 * Creates a new file, then prints information using a specific format to a text file.
 */
void printChronological(Customer *ptrs[], int size) {
    FILE *f = fopen("hw5time.txt", "w");
    int i, j, q;
    double p;
    for (i = 0; i < size; i++) {
        fprintf(f, "Customer: %s\n \tOrders\n", ptrs[i]->name);
        for (j = 0; j < ptrs[i]->orderSize; j++) {
            q = ptrs[i]->order[j]->quantity;
            p = ptrs[i]->order[j]->price;
            fprintf(f, "%16s", "");
            fprintf(f, "%-16s %-7d %8.2f %8.2f\n", ptrs[i]->order[j]->itemName,
                    q, p, q * p);
        }
        fprintf(f, "%50s", "Total: ");
        fprintf(f, "%8.2f\n", ptrs[i]->total);
    }
    fclose(f);
}

/*
 * This function takes in a pointer array of customers and the size of the array.
 * Creates a new file, then prints information using a specific format to a text file.
 * This is done after the customers are sorted.
 */
void printFinancial(Customer *ptrs[], int size) {
    FILE *f = fopen("hw5money.txt", "w");
    int i, j, q;
    double p;
    for (i = 0; i < size; i++) {
        fprintf(f, "%s, Total Order = $%.2f\n", ptrs[i]->name, ptrs[i]->total);
        for (j = 0; j < ptrs[i]->orderSize; j++) {
            q = ptrs[i]->order[j]->quantity;
            p = ptrs[i]->order[j]->price;
            fprintf(f, "%s %d $%.2f Item Value = $%.2f\n",
                    ptrs[i]->order[j]->itemName, q, p, q * p);
        }
    }
    fclose(f);
}

/*
 * This function takes in an array of pointers, it's size, and another function,
 * and bubble sorts them based on what is being compared.
 *
 * NOTE: This code was provided in the example files!
 */
void bubbleSort(void *ptrs[], int size,
        int (*compare)(const void *, const void *)) {
    void *s;
    int topBubble, bubbles, notSorted = 1;
    for (topBubble = 0; topBubble < size - 1 && notSorted; topBubble++)
        for (bubbles = size - 1, notSorted = 0; bubbles > topBubble;
                bubbles--) {
            if (compare(ptrs[bubbles], ptrs[bubbles - 1]) > 0) {
                s = ptrs[bubbles];
                ptrs[bubbles] = ptrs[bubbles - 1];
                ptrs[bubbles - 1] = s;
                notSorted = 1;
            }
        }
}

/*
 * Compares the item value of a purchase.
 */
int itemValueCompare(const void *w1, const void *w2) {
    int p1, p2;
    p1 = ((Purchase *) w1)->quantity * ((Purchase *) w1)->price;
    p2 = ((Purchase *) w2)->quantity * ((Purchase *) w2)->price;
    return (p1 - p2);
}

/*
 * Compares the total value of all purchases.
 */
int totalValueCompare(const void *a1, const void *a2) {
    return ((Customer *) a1)->total - ((Customer *) a2)->total;
}
