#include <stdio.h>

int main() {
    int size;

    // Запрашиваем у пользователя размер массива
    printf("Введите размер массива: ");
    scanf("%d", &size);

    // Создаем массив указанного размера
    int numbers[size];

    // Заполняем массив числами, введенными пользователем
    printf("Введите %d чисел:\n", size);
    for (int i = 0; i < size; i++) {
        scanf("%d", &numbers[i]);
    }

    // Выводим массив на экран
    printf("Введенные числа:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    return 0;
}
