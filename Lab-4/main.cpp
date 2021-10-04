// Строилов Денис - КС-40 - Лаба 4 - Вариант 5
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

// Кол-во разбиений
const int N = 1000;

// Левая граница
const double A = 1;

// Правая граница
const double B = 2;

// Подинтегральная функция
double integFunc(double x)
{
	// По умолчанию возьмем синус
	return sin(x);
}

int main()
{
	// Разбиваем отрезок на n частей (узнаем длину деления)
	double h = (B - A) / N;

	// Суммируем значения функции на граничных точках
	double sum = integFunc(A) + integFunc(B);

	// Счетчик для определения четности
	int k;

	double start_time, end_time;

	// Распараллеливаем расчеты
	#pragma omp parallel
	{

		// Замеряем время работы
		start_time = omp_get_wtime();

		// В цикле считаем площадь каждого отрезка
		#pragma omp for
		for (int i = 1; i < N; i++)
		{
			// Считаем четность текущего отрезка (для разбиения на пораболы)
			k = 2 + 2 * (i % 2);

			// Суммируем каждый отрезок
			sum += k * integFunc(A + i * h);
		}

		// Окончание работы по времени
		end_time = omp_get_wtime();
	}
	// Сумму домножаем на 1/3 длины отрезка
	sum *= h / 3;

	// Выводим результат
	cout << "Result = " << sum << endl;

	// Выводим время
	cout << "Time is " << (end_time - start_time) << endl;
}