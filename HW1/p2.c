#include <stdio.h>
#include <string.h>

int main(){
    char sentence[128];
    fgets(sentence, 128, stdin);
    char delimiter[128];
    fgets(delimiter, 128, stdin);
    char sentence_copy[128];
    strncpy(sentence_copy, sentence, 128);
    char *mytok = strtok(sentence, delimiter);
    while(mytok != NULL){
        printf("%s\n", mytok);
        mytok = strtok(NULL, delimiter);
    }
    printf("%s", sentence_copy);
    return 0;
}
