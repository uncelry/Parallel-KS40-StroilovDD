// Строилов Денис - КС-40 - Лаба 5 - Вариант 5

#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>

using namespace std;

// Команда для запуска программы - используем 3 потока, так как 1 под писателя и 2 под издательства
// mpiexec -n 3 Lab-5.exe

// Сколько времени будут ждать издательства
const int MIN_SLEEP = 500;
const int MAX_SLEEP = 1000;

// Гонорар
const int MIN_FEE = 10000;
const int MAX_FEE = 100000;

int main(int argc, char** argv)
{
	srand(time(0));

	// Ранг потока (номер потока среди всех потоков)
	int rank;

	// Инициализируем MPI
	MPI_Init(&argc, &argv);

	// Получаем в переменную rank ранг текущего потока в глобальном коммуникаторе (т.е. среди всех потоков программы)
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Пусть у потока "Писатель" будет ранг 0
	if (rank == 0)
	{
		// Объект класса MPI_Status - необходим для получения сообщений
		MPI_Status status;

		// Номера книг
		int book_1 = 1;
		int book_2 = 2;
		int book_3 = 3;

		// Сумма гонорара (для упрощения - целое число)
		int fee;

		// Сперва, писатель отправляет первую книгу в первое издательство
		MPI_Send(&book_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);

		// Затем, писатель отправляет вторую книгу во второе издательство
		MPI_Send(&book_2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD);

		// Затем, писатель отправляет третью книгу в первое издательство
		MPI_Send(&book_3, 1, MPI_INT, 1, 3, MPI_COMM_WORLD);

		// После того, как все сообщения отправлены, ждем ответа по 3 книгам (от любых потоков с любыми тегами сообщений)
		for (int i = 0; i < 3; i++) {
			MPI_Recv(&fee, 6, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			cout << "I got a fee " << fee << ", from publisher " << status.MPI_SOURCE << " for a book " << status.MPI_TAG << endl;
		}
	}

	// Пусть у потоков Издательств будут ранги 1 и 2
	if ((rank == 1) || (rank == 2))
	{

		// Генерируем сумму гонорара
		int fee = MIN_FEE + rand() % (MAX_FEE - MIN_FEE + 1);

		// Генериурем новый гонорар
		int fee_1 = MIN_FEE + rand() % (MAX_FEE - MIN_FEE + 1);

		// Генериурем новый гонорар
		int fee_2 = MIN_FEE + rand() % (MAX_FEE - MIN_FEE + 1);

		// Переменная под прием данных
		int recieved;

		// Если это первое издательство, то делаем всё из расчета на 2 сообщения (т.к. оно принимает информацию о 2-х книгах). Можно сделать и в цикле, но в задании написано строго 2
		if (rank == 1) {

			// Объект класса MPI_Status - необходим для получения сообщений
			MPI_Status msg_1;
			MPI_Status msg_2;

			// Сперва, ждем сообщения от писателя
			MPI_Recv(&recieved, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &msg_1);

			// Ждем 2-е сообщения от писателя
			MPI_Recv(&recieved, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &msg_2);

			// После получения сообщения, ждем какое-то время
			Sleep(MIN_SLEEP + rand() % (MAX_SLEEP - MIN_SLEEP + 1));

			

			// И формируем ответ
			if (rand() == 0) {
				// Если 0, то сначала по первой книге
				MPI_Send(&fee, 6, MPI_INT, msg_1.MPI_SOURCE, msg_1.MPI_TAG, MPI_COMM_WORLD);

				// Ждем
				Sleep(MIN_SLEEP + rand() % (MAX_SLEEP - MIN_SLEEP + 1));

				// Отправляем вторую
				MPI_Send(&fee_1, 6, MPI_INT, msg_2.MPI_SOURCE, msg_2.MPI_TAG, MPI_COMM_WORLD);
			}
			else
			{
				// Если 1, то сначала по второй книге
				MPI_Send(&fee_1, 6, MPI_INT, msg_2.MPI_SOURCE, msg_2.MPI_TAG, MPI_COMM_WORLD);

				// Ждем
				Sleep(MIN_SLEEP + rand() % (MAX_SLEEP - MIN_SLEEP + 1));

				// Отправляем первую
				MPI_Send(&fee, 6, MPI_INT, msg_1.MPI_SOURCE, msg_1.MPI_TAG, MPI_COMM_WORLD);
			}
			
		}

		// Иначе - без цикла
		else
		{
			// Объект класса MPI_Status - необходим для получения сообщений
			MPI_Status msg_1;

			// Сперва, ждем сообщения от писателя
			MPI_Recv(&recieved, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &msg_1);

			// После получения сообщения, ждем какое-то время
			Sleep(MIN_SLEEP + rand() % (MAX_SLEEP - MIN_SLEEP + 1));

			// Отправляем гонорар
			MPI_Send(&fee_2, 6, MPI_INT, msg_1.MPI_SOURCE, msg_1.MPI_TAG, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
}