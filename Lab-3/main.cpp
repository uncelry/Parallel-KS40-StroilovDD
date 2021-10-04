// Строилов Денис - КС-40 - Лаба 3 - Вариант 4
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

// Размер массива
const int ARRAY_SIZE = 5;

// Диапазон случайных чисел
const int MIN = -10;
const int MAX = 10;

int main()
{
	srand(time(NULL));

	// Создаем массив
	int** B = new int* [ARRAY_SIZE];

	for (int i = 0; i < ARRAY_SIZE; i++)
		B[i] = new int[ARRAY_SIZE];

	// Заполняем массив случайными числами и выводим его
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			B[i][j] = MIN + rand() % (MAX - MIN + 1);
			std::cout << B[i][j] << '\t';
		}
		std::cout << endl;
	}

	// Замер времени начала
	clock_t start_time = clock();

	// Распараллеливаем рассчеты
	#pragma omp parallel
	{
		// В цикле проходимся по каждой строке
		#pragma omp for
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			int min_el = MAX;
			int min_pos;

			for (int j = 0; j < ARRAY_SIZE; j++)
			{
				// Ищем минимальный элемент строки
				if (B[i][j] < min_el)
				{
					min_el = B[i][j];
					min_pos = j;
				}
			}

			// Меняем местами минимальный элемент с диагональным
			swap(B[i][i], B[i][min_pos]);
		}
	}

	// Замер конца времени и вывод времени рассчета
	clock_t end_time = clock();
	float time_of_execution = float(end_time - start_time) / CLOCKS_PER_SEC;

	// В последовательном режиме выводим измененный массив в консоль
	std::cout << endl;
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			std::cout << B[i][j] << '\t';
		}
		std::cout << endl;
	}

	// Очищаем память
	for (int i = 0; i < ARRAY_SIZE; i++)
		delete[] B[i];

	delete[] B;

	// Выводим итоговое время расчета
	std::cout << endl << "Time of execution: " << time_of_execution << endl;

	#pragma omp parallel num_threads(1)
	{
		// Замеряем время работы функции omp_get_wtick()
		start_time = omp_get_wtime();

		// Вызываем функцию
		omp_get_wtick();

		// Останавливаем таймер
		end_time = omp_get_wtime();
	}
	// Выводи сообщение в консоль
	std::cout << endl << "omp_get_wtick() function time is: " << (end_time - start_time) << endl;

	// Точности системного таймера НЕ хватает для замера времени выполнения функции omp_get_wtick()
}