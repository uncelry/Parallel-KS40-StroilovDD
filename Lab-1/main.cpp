// �������� ����� ���������� - ��-40 - ������� 19 - ���� 1

// 19. ����� ������ ������� ��������� � ����
// x1 = a11x1 + a12x2 + ...
// x2 = a12x1 + a22x2 + ...
// ���, � ��������� ����, x = Ax
// ����� ������� ���� ������� ������������ ������ ������ - �������, ��������� 1, 2
// ��� 4 ������. ����������� ������� � ���, ��� ������������ �� ����� ��������
// �������� x ������������ ������ � �������� ������, �������� ��� ����������. �����
// ����� ���������� ��������� � ������ ������.
// ������� A ������� �� ��������� ����� ����� �������� 500�500. ���������
// ����������� ��� x ���������� ��������������(��������, ��� �������).���������
// �������� ������� �� xi = 10 - 2
// � ������ ���������� ����������, ����������� �������
// ����� 10000 �������� � ��������������� ����������.

#include <cmath>
#include <thread>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

// ��������
const double EPS = 0.01;

// ������������ ���-�� ��������
const int MAX_ITER_AMOUNT = 10000;

// ������ �������
const int N = 500;

// ����������� � ������������ ����� ��� ���������� �������
const int MIN = 1;
const int MAX = 10;

// ����������� � ������������ ����� ��� ���������� ������� ��������� ������� (��� ����, ����� � ������� ���� ������������ ������������ - ��� ����������)
const int MIN_DIAG = (N * MAX) + 1;
const int MAX_DIAG = MIN_DIAG + 100;

// ������� �������
const int THREADS_AMOUNT = 4;


void gaussZeidel(vector<vector<int>> matrix, long long int start_equasion, int amount_of_equasions, double x_zero, int x_amount, int max_iterations, vector<double>& X)
{

	
	// ������� ������ ��� ���� (��� ���� ��������)
	vector<double> X_PREV(amount_of_equasions, x_zero);

	// ������� ���� ��� ����������� ��������� �������
	bool eps_reached = false;

	// �������� ������
	for (int i = 0; i < max_iterations; i++)
	{

		// �������� �������� ����� ������� ��������
		for (int i = 0; i < amount_of_equasions; i++) {
			X_PREV[i] = X[start_equasion + i];
		}

		// � ����� ���������� �� ������ ������ ������� (������� ���������)
		for (int n = 0; n < amount_of_equasions; n++)
		{
			// ������� ��������� ���������� ��� �������� �����
			double tmp_val = 0;

			// ���������� �� ���� ��������� ������� ��������� � ��������� ��
			for (int k = 0; k < amount_of_equasions; k++)
				if (k + start_equasion != n + start_equasion)
					tmp_val += (matrix[n + start_equasion][k + start_equasion] * X[k + start_equasion]);

			// �������� �� ���������� ����� ����� � ����� ��������� �� ��������������� ���� �������
			X[start_equasion + n] = (X[start_equasion + n] - tmp_val) / matrix[n + start_equasion][n + start_equasion];
		}

		// ������� ����������� (����� ��������� ������ ������� ����� � ����� ���������� ��������)
		double norm = 0;
		for (int i = 0; i < amount_of_equasions; i++)
			norm += (X[i + start_equasion] - X_PREV[i]) * (X[i + start_equasion] - X_PREV[i]);

		// ������� ���������� �� ������ ��������
		if (sqrt(norm) < EPS)
			eps_reached = true;

		// ���� �������� ��������, �� ������� �� �����
		if (eps_reached)
			break;
	}
}


int main()
{
	// ������� ����� ��������� �����
	srand(time(NULL));

	// ������� ������� A
	vector<vector<int>> A(N, vector<int>(N));

	// ��������� �
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			// �������� ����� �� �������
			if (i != j)
				A[i][j] = MIN + rand() % (MAX - MIN + 1);
			else
				A[i][j] = MIN_DIAG + rand() % (MAX_DIAG - MIN_DIAG + 1);
		}
	}

	// ������� ������
	thread threads[THREADS_AMOUNT];

	// ���������� ��� ������ ��������� � �������
	long long int start_equasion = 0;

	// ��������� �����������
	double zero_iteration = 1;

	// �� ������� ��������� ���� ������� ������
	int amount_of_equasions = N / THREADS_AMOUNT;

	// ����� ������� ������
	clock_t start_time = clock();

	// ���� ����, ��� �� ������� �������� ����������� ������
	bool is_not_empty = true;

	// ������� �������������� ������ ��� �����
	vector<double> X(N, zero_iteration);

	// ���������� �� ���� �������
	for (int thread_num = 0; thread_num < THREADS_AMOUNT; thread_num++)
	{

		// ��� ������� ������ ����� ������ ���������� ������������ ������
		threads[thread_num] = thread([A, start_equasion, amount_of_equasions, zero_iteration, &X] {

			// ���������� ��������� ������� �� ��������� ������
			 gaussZeidel(A, start_equasion, amount_of_equasions, zero_iteration, N, MAX_ITER_AMOUNT, X);
			});

		// ��������� � ��������� ���������� �������
		start_equasion += amount_of_equasions;
	}

	// ���������� ��������� ������ ���� �������
	for (int i = 0; i < THREADS_AMOUNT; i++)
	{
		threads[i].join();
	}

	// ����� ����� ������� � ����� ������� ��������
	clock_t end_time = clock();
	float time_of_execution = float(end_time - start_time) / CLOCKS_PER_SEC;

	// ���� � ������ ���������, �� �����, ��� �������� �����, �� ������� ��
	if (X.size() == N)
	{
		cout << "Result:" << endl;

		for (auto el : X)
			cout << el << endl;
	}

	// ���� �������� �� ���� �����, ������ �����-�� ���� ���������� �� ���������� (�� ������� ��������)
	else
		cout << "Some values were not counted because we ran out of iterations!";

	// ������� �������� ����� �������
	cout << endl << "Time of execution: " << time_of_execution << endl;

	system("pause");
}