// �������� ����� - ��-40 - ���� 4 - ������� 5
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

// ���-�� ���������
const int N = 1000;

// ����� �������
const double A = 1;

// ������ �������
const double B = 2;

// ��������������� �������
double integFunc(double x)
{
	// �� ��������� ������� �����
	return sin(x);
}

int main()
{
	// ��������� ������� �� n ������ (������ ����� �������)
	double h = (B - A) / N;

	// ��������� �������� ������� �� ��������� ������
	double sum = integFunc(A) + integFunc(B);

	// ������� ��� ����������� ��������
	int k;

	double start_time, end_time;

	// ���������������� �������
	#pragma omp parallel private(k)
	{

		// �������� ����� ������
		start_time = omp_get_wtime();

		// � ����� ������� ������� ������� �������
		#pragma omp for
		for (int i = 1; i < N; i++)
		{
			// ������� �������� �������� ������� (��� ��������� �� ��������)
			k = 2 + 2 * (i % 2);

			// ��������� ������ �������
			#pragma omp atomic
			sum += k * integFunc(A + i * h);
		}

		// ��������� ������ �� �������
		end_time = omp_get_wtime();
	}
	// ����� ��������� �� 1/3 ����� �������
	sum *= h / 3;

	// ������� ���������
	cout << "Result = " << sum << endl;

	// ������� �����
	cout << "Time is " << (end_time - start_time) << endl;
}