// Строилов Денис Дмитриевич - Вариант 19
#include <cmath>
#include <thread>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

// Точность
const double EPS = 0.01;

// Максимальное кол-во итераций
const int MAX_ITER_AMOUNT = 10000;

// Размер матрицы
const int N = 500;

// Минимальное и максимальное числа для заполнения матрицы
const int MIN = 1;
const int MAX = 10;

// Минимальное и максимальное числа для заполнения главной диагонали матрицы (для того, чтобы у матрицы было диагональное преобладание - для сходимости)
const int MIN_DIAG = (N * MAX) + 1;
const int MAX_DIAG = MIN_DIAG + 100;

// Сколько потоков
const int THREADS_AMOUNT = 1;


vector<double> gaussZeidel(vector<vector<int>> matrix, long long int start_equasion, int amount_of_equasions, double x_zero, int x_amount, int max_iterations)
{
	// Создаем вектор под иксы (для двух итераций)
	vector<double> X(amount_of_equasions, x_zero);
	vector<double> X_PREV(amount_of_equasions, x_zero);

	// Создаем флаг для определения окончания расчета
	bool eps_reached = false;

	// Начинаем расчет
	for (int i = 0; i < max_iterations; i++)
	{

		// Копируем значения иксов текущей итерации
		X_PREV = X;

		// В цикле проходимся по каждой строке матрицы (каждому уравнению)
		for (int n = 0; n < amount_of_equasions; n++)
		{
			// Создаем временную переменную для подсчета суммы
			double tmp_val = 0;

			// Проходимся по всем слагаемым данного уравнения и суммируем их
			for (int k = 0; k < amount_of_equasions; k++)
				if (k != n)
					tmp_val += (matrix[n + start_equasion][k] * X[k]);

			// Вычитаем из свободного члена сумму и делим результат на соответствующий коэф матрицы
			X[n] = (X[n] - tmp_val) / matrix[n + start_equasion][n + start_equasion];
		}

		// Считаем погрешность (сумма квадратов разниц текущих иксов и иксов предыдущей итерации)
		double norm = 0;
		for (int i = 0; i < amount_of_equasions; i++)
			norm += (X[i] - X_PREV[i]) * (X[i] - X_PREV[i]);

		// Считаем достигнута ли нужная точность
		if (sqrt(norm) < EPS)
			eps_reached = true;

		// Если достигли точности, то выходим из цикла
		if (eps_reached)
			break;
	}


	// Если точность достигнута, то возвращаем найденные иксы
	if (eps_reached)
	{
		return X;
	}

	// Если точность не достигнута, то возвращаем пустой вектор
	else
	{
		return vector<double>();
	}
}


int main()
{
	// Создаем новые рандомные числа
	srand(time(NULL));

	// Создаем матрицу A
	vector<vector<int>> A(N, vector<int>(N));

	// Заполняем А
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			// Случаное число из отрезка
			if (i != j)
				A[i][j] = MIN + rand() % (MAX - MIN + 1);
			else
				A[i][j] = MIN_DIAG + rand() % (MAX_DIAG - MIN_DIAG + 1);
		}
	}

	// Создаем потоки
	thread threads[THREADS_AMOUNT];

	// Переменная для номера уравнения в функцию
	long long int start_equasion = 0;

	// Начальное приближение
	double zero_iteration = 1;

	// По сколько уравнений даем каждому потоку
	int amount_of_equasions = N / THREADS_AMOUNT;

	// Замер времени начала
	clock_t start_time = clock();

	// Создаем результирующий вектор для иксов
	vector<double> result_vector(N);

	// Флаг того, что из функции вернулся заполненный вектор
	bool is_not_empty = true;

	// Проходимся по всем потокам
	for (int thread_num = 0; thread_num < THREADS_AMOUNT; thread_num++)
	{

		// Для каждого потока через лямбду записываем рассчитанные данные
		threads[thread_num] = thread([A, start_equasion, amount_of_equasions, zero_iteration, &result_vector] {

				// Записываем результат функции во временный вектор
				vector<double> tmp_vect = gaussZeidel(A, start_equasion, amount_of_equasions, zero_iteration, N, MAX_ITER_AMOUNT);

				// Если вернуля не пустой вектор, то добавляем из него элементы в результирующий вектор
				if (tmp_vect.size() != 0)
					for (int i = 0; i < tmp_vect.size(); i++)
						result_vector[i + start_equasion] = tmp_vect[i];
			});

		// Переходим к следующим уравнениям системы
		start_equasion += amount_of_equasions;
	}

	// Дожидаемся окончания работы всех потоков
	for (int i = 0; i < THREADS_AMOUNT; i++)
	{
		threads[i].join();
	}

	// Замер конца времени и вывод времени рассчета
	clock_t end_time = clock();
	float time_of_execution = float(end_time - start_time) / CLOCKS_PER_SEC;

	// Если в вектор записаный, по итогу, все значения иксов, то выводим их
	if (result_vector.size() == N)
	{
		cout << "Result:" << endl;

		for (auto el : result_vector)
			cout << el << endl;
	}

	// Если значения не всех иксов, значит какие-то иксы рассчитать не получилось (не хватило итераций)
	else
		cout << "Some values were not counted because we ran out of iterations!";

	// Выводим итоговое время расчета
	cout << endl << "Time of execution: " << time_of_execution << endl;
}