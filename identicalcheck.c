#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Hardcode the file names here
    const char *file1 = "sample.c";
    const char *file2 = "htmlcode.c";

    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (!f1 || !f2) {
        printf("Error opening files.\n");
        return 1;
    }

    int ch1, ch2;
    long pos = 0;
    long int size1=0;
    long int size2=0;
    int identical = 1;
    int linecount1 =0;
    int linecount2=0;

    while (1) {
        ch1 = fgetc(f1);
        ch2 = fgetc(f2);
        pos++;
        if (ch1 != EOF) {
            size1++;
            if (ch1 == '\n') linecount1++;
        }
            if (ch2 != EOF) {
                size2++;
                if (ch2 == '\n') linecount2++;
            }

        if (ch1 == EOF && ch2 == EOF) {
            break; // both reached end together
        }
        if (ch1 != ch2) {
            printf("Files differ at byte %ld: %d vs %d\n", pos, ch1, ch2);
            identical = 0;
            break;
        }
    }

    if (identical) {
        printf("Files are IDENTICAL.\n");
        printf("size of sample.c is %ld\n",size1);
        printf("size of htmlcode.c is %ld\n",size2);
         printf("linecount of sample.c is %d\n",linecount1+1);
        printf("linecount of htmlcode.c is %d\n",linecount2+1);
    }

    fclose(f1);
    fclose(f2);
    return 0;
}
