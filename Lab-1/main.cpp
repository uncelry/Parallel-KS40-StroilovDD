// Строилов Денис Дмитриевич - КС-40 - Вариант 19 - Лаба 1

// 19. Пусть задана система уравнений в виде
// x1 = a11x1 + a12x2 + ...
// x2 = a12x1 + a22x2 + ...
// или, в матричном виде, x = Ax
// Найти решение этой системы модификацией метода Гаусса - Зейделя, используя 1, 2
// или 4 потока. Модификация состоит в том, что рассчитанные на новой итерации
// значения x используются только в пределах потока, которыми они рассчитаны. Найти
// время исполнения программы в каждом случае.
// Матрица A состоит из случайных целых чисел размером 500х500. Начальное
// приближение для x определить самостоятельно(например, все единицы).Требуемая
// точность решения по xi = 10 - 2
// В случае отсутствия сходимости, заканчивать процесс
// через 10000 итераций с соответствующим сообщением.

#include <cmath>
#include <thread>
#include <vector>
#include <ctime>
#include <iostream>
#include <mutex>


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
const int THREADS_AMOUNT = 4;

std::mutex mute;

void gaussZeidel(vector<vector<int>> matrix, int size, int matr_size, int first, double x_zero, int max_iterations, vector<double>& X)
{

	// Создаем вектор под иксы (для двух итераций)
	vector<double> X_PREV(matr_size, x_zero);

	// Создаем флаг для определения окончания расчета
	bool eps_reached = false;

	// Локальная переменная под последние расчитанные иксы
	vector<double> X_LOCAL(matr_size, x_zero);

	// Начинаем расчет
	for (int k = 0; k < max_iterations; k++)
	{
		int i, j;

		// В цикле проходимся по каждой строке матрицы для данного потока (каждому уравнению)
		for (i = 0; i < size; i++)
		{
			// Создаем временную переменную для подсчета суммы
			double tmp_val = 0;

			// Проходимся по всем слагаемым данного уравнения и суммируем их
			for (j = 0; j < i + first; j++)
				tmp_val += (matrix[i + first][j] * X_PREV[j]);

			for (j = i + 1 + first; j < matr_size; j++)
				tmp_val += (matrix[i + first][j] * X_LOCAL[j]);

			// Вычитаем из свободного члена сумму и делим результат на соответствующий коэф матрицы
			X_LOCAL[i + first] = (X_LOCAL[i + first] - tmp_val) / matrix[i + first][i + first];

		}

		// Считаем погрешность (сумма квадратов разниц текущих иксов и иксов предыдущей итерации)
		double norm = 0;
		for (int l = 0; l < size; l++)
			norm += pow((abs(X_LOCAL[l + first]) - abs(X_PREV[l + first])), 2);

		// Считаем достигнута ли нужная точность
		if (sqrt(norm) < EPS)
			eps_reached = true;

		mute.lock();

		// Копируем значения иксов текущей итерации
		for (int m = 0; m < size; m++) {
			X[m + first] = X_LOCAL[m + first];
		}

		X_LOCAL = X;
		X_PREV = X;

		mute.unlock();

		// Если достигли точности, то выходим из цикла
		if (eps_reached)
			break;
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

	// По сколько иксов будет дано на поиск каждому потоку
	int size = N / THREADS_AMOUNT;

	// Замер времени начала
	clock_t start_time = clock();

	// Флаг того, что из функции вернулся заполненный вектор
	bool is_not_empty = true;

	// Создаем результирующий вектор для иксов
	vector<double> X(N, zero_iteration);

	// Начиная с какого икса нужно исать решения каждому потоку
	int first = 0;

	// Проходимся по всем потокам
	for (int thread_num = 0; thread_num < THREADS_AMOUNT; thread_num++)
	{

		// Для каждого потока через лямбду записываем рассчитанные данные
		threads[thread_num] = thread([A, start_equasion, size, zero_iteration, &X, first] {

			// Записываем результат функции во временный вектор
			gaussZeidel(A, size, N, first, zero_iteration, MAX_ITER_AMOUNT, X);
			});

		first += size;
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
	if (X.size() == N)
	{
		cout << "Result:" << endl;

		for (auto el : X)
			cout << el << endl;
	}

	// Если значения не всех иксов, значит какие-то иксы рассчитать не получилось (не хватило итераций)
	else
		cout << "Some values were not counted because we ran out of iterations!";

	// Выводим итоговое время расчета
	cout << endl << "Time of execution: " << time_of_execution << endl;

	system("pause");
}