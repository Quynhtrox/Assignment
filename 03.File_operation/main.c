#include <stdio.h>
#include <string.h>

#define MAX 100
#define Q   "README.txt"

int main() {
    FILE *q;
    char sentence[] = "Hello world!\n";

    // **********************************
    /* fopen(): open or create a file */
    q = fopen(Q, "w");
    // "r"  open file for read only (file created)
    // "w"  open file for write only (clear data in file/create a new file)
    // "a"  continous write at the end
    // "r+" read and write (file created)
    // "w+" read and write (clear data in file)
    // "a+" read and write, continous write at the end

    fwrite(sentence, sizeof(char), strlen(sentence), q);
    fclose(q);

    // **********************************
    char buffer[MAX];
    q = fopen(Q, "r");
    fread(buffer, sizeof(char), MAX, q);
    fclose(q);
    printf("Read from file: %s", buffer);

    // ************************************
    q = fopen(Q, "w+");
    fprintf(q, "Name: %s , Age: %d\n", "Troc", 22);

    char line[MAX], name[MAX];
    int age;

    fseek(q, 0, SEEK_SET);
    // SEEK_SET     Counted from the beginning of the file
    // SEEK_CUR     from current location
    // SEEK_END     from the end of the file

    // fgets(line, sizeof(line), q);
    
    fscanf(q, "Name: %s , Age: %d", name, &age);
    fclose(q);
    printf("%s, %d\n", name, age);

    return 0;
}