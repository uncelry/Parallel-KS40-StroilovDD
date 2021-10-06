// Строилов Денис - КС-40 - Лаба 6 - Вариант 5

#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <time.h>

using namespace std;

// Команда для запуска программы (2 бригады по 20 рабочих каждая)
// mpiexec -n 40 Lab-7.exe

// Кол-во рабочих в каждой бригаде
const int WORKERS = 20;

// Кол-во кругов обмена сообщениями
const int ROUNDS = 5;

int main(int argc, char** argv)
{
	// Ранг потока (номер потока среди всех потоков)
	int rank;

	// Инициализируем MPI
	MPI_Init(&argc, &argv);

	// Инициализируем коммуникатор
	MPI_Comm comm = MPI_COMM_WORLD;

	// Получаем в переменную rank ранг текущего потока в глобальном коммуникаторе (т.е. среди всех потоков программы)
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Кол-во объектов в массиве MPI_Requests
	MPI_Request* reqs = new MPI_Request[size];

	// Объект класса MPI_Status - необходим для получения сообщений
	MPI_Status* status = new MPI_Status[size];

	// Создаем переменную для получения/отправки сообщений
	int msg_data = 0;

	// Переменная под кол-во отправленных вопросов
	int questions_sent = 0;

	// Переменная под кол-во отправленных ответов
	int answers_sent = 0;

	// Переменная под кол-во полученных вопросов
	int questions_recieved = 0;

	// Переменная под кол-во полученных ответов
	int answers_recieved = 0;

	// Задаем кол-во кругов обмена сообщениями
	for (int round = 0; round < ROUNDS; round++)
	{

		// Если текущий рабочий относится к первой бригаде
		if (rank < WORKERS)
		{
			// Рассылаем по очереди сообщение-вопрос каждому рабочему из другой бригады
			for (int i = WORKERS; i < WORKERS + WORKERS; i++)
			{
				MPI_Isend(&msg_data, 1, MPI_INT, i, i, MPI_COMM_WORLD, &reqs[i]);
				questions_sent++;
				MPI_Irecv(&msg_data, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[i]);
				answers_recieved++;
				std::cout << "I (" << rank << ") sent a question to the " << i << endl;
			}

			// Получаем ответы от всех рабочих другой бригады и отправляем ответы
			for (int i = WORKERS; i < WORKERS + WORKERS; i++)
			{
				MPI_Irecv(&msg_data, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[i]);
				questions_recieved++;
				MPI_Isend(&msg_data, 1, MPI_INT, i, i, MPI_COMM_WORLD, &reqs[i]);
				answers_sent++;
				std::cout << "I (" << rank << ") sent an answer to the " << i << endl;
			}
		}

		// Если текущий рабочий относится к первой бригаде
		else
		{
			// Рассылаем по очереди сообщение-вопрос каждому рабочему из другой бригады
			for (int i = 0; i < WORKERS; i++)
			{
				MPI_Isend(&msg_data, 1, MPI_INT, i, i, MPI_COMM_WORLD, &reqs[i]);
				questions_sent++;
				MPI_Irecv(&msg_data, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[i]);
				answers_recieved++;
				std::cout << "I (" << rank << ") sent a question to the " << i << endl;
			}

			// Получаем ответы от всех рабочих другой бригады и отправляем ответы
			for (int i = 0; i < WORKERS; i++)
			{
				MPI_Irecv(&msg_data, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[i]);
				questions_recieved++;
				MPI_Isend(&msg_data, 1, MPI_INT, i, i, MPI_COMM_WORLD, &reqs[i]);
				answers_sent++;
				std::cout << "I (" << rank << ") sent an answer to the " << i << endl;
			}
		}

		// Ждем получения данных от всех запросов
		MPI_Waitall(size, reqs, status);

	}

	// Очищаем память
	delete[] reqs;
	delete[] status;

	// Критическая секция
	MPI_Barrier(comm);

	// Выводим финальную статистику
	std::cout << "I " << rank << " sent " << questions_sent << " questions and " << answers_sent << " answers! Recieved questions " << questions_recieved << " and answers " << answers_recieved << endl;

	// Завершаем работу с MPI
	MPI_Finalize();
	
}