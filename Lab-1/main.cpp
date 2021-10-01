// �������� ����� ���������� - ������� 19
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
const int THREADS_AMOUNT = 1;


vector<double> gaussZeidel(vector<vector<int>> matrix, long long int start_equasion, int amount_of_equasions, double x_zero, int x_amount, int max_iterations)
{
	// ������� ������ ��� ���� (��� ���� ��������)
	vector<double> X(amount_of_equasions, x_zero);
	vector<double> X_PREV(amount_of_equasions, x_zero);

	// ������� ���� ��� ����������� ��������� �������
	bool eps_reached = false;

	// �������� ������
	for (int i = 0; i < max_iterations; i++)
	{

		// �������� �������� ����� ������� ��������
		X_PREV = X;

		// � ����� ���������� �� ������ ������ ������� (������� ���������)
		for (int n = 0; n < amount_of_equasions; n++)
		{
			// ������� ��������� ���������� ��� �������� �����
			double tmp_val = 0;

			// ���������� �� ���� ��������� ������� ��������� � ��������� ��
			for (int k = 0; k < amount_of_equasions; k++)
				if (k != n)
					tmp_val += (matrix[n + start_equasion][k] * X[k]);

			// �������� �� ���������� ����� ����� � ����� ��������� �� ��������������� ���� �������
			X[n] = (X[n] - tmp_val) / matrix[n + start_equasion][n + start_equasion];
		}

		// ������� ����������� (����� ��������� ������ ������� ����� � ����� ���������� ��������)
		double norm = 0;
		for (int i = 0; i < amount_of_equasions; i++)
			norm += (X[i] - X_PREV[i]) * (X[i] - X_PREV[i]);

		// ������� ���������� �� ������ ��������
		if (sqrt(norm) < EPS)
			eps_reached = true;

		// ���� �������� ��������, �� ������� �� �����
		if (eps_reached)
			break;
	}


	// ���� �������� ����������, �� ���������� ��������� ����
	if (eps_reached)
	{
		return X;
	}

	// ���� �������� �� ����������, �� ���������� ������ ������
	else
	{
		return vector<double>();
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

	// ������� �������������� ������ ��� �����
	vector<double> result_vector(N);

	// ���� ����, ��� �� ������� �������� ����������� ������
	bool is_not_empty = true;

	// ���������� �� ���� �������
	for (int thread_num = 0; thread_num < THREADS_AMOUNT; thread_num++)
	{

		// ��� ������� ������ ����� ������ ���������� ������������ ������
		threads[thread_num] = thread([A, start_equasion, amount_of_equasions, zero_iteration, &result_vector] {

				// ���������� ��������� ������� �� ��������� ������
				vector<double> tmp_vect = gaussZeidel(A, start_equasion, amount_of_equasions, zero_iteration, N, MAX_ITER_AMOUNT);

				// ���� ������� �� ������ ������, �� ��������� �� ���� �������� � �������������� ������
				if (tmp_vect.size() != 0)
					for (int i = 0; i < tmp_vect.size(); i++)
						result_vector[i + start_equasion] = tmp_vect[i];
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
	if (result_vector.size() == N)
	{
		cout << "Result:" << endl;

		for (auto el : result_vector)
			cout << el << endl;
	}

	// ���� �������� �� ���� �����, ������ �����-�� ���� ���������� �� ���������� (�� ������� ��������)
	else
		cout << "Some values were not counted because we ran out of iterations!";

	// ������� �������� ����� �������
	cout << endl << "Time of execution: " << time_of_execution << endl;
}