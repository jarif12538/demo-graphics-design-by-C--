#include <stdio.h>
int main() {
  char firstName[100];
  printf("Enter your first name and press enter: \n");
  if (scanf(" %99[^\n]", firstName) != 1) {
    return 1;
  }
  printf("%s\n", firstName);

  return 0;
}