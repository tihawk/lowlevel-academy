#include <stdio.h>

int main() {
  int temp;
  printf("what temperature is it?\n");
  scanf("%d", &temp);

  if (temp >= 70) {
    printf("dang bruther, it's hot\n");
  } else if (temp >= 30) {
    printf("dang bruther, it's mild\n");
  } else {
    printf("dang bruther, it's cold\n");
  }

  return 0;
}
