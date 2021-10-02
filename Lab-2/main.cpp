// �������� ����� ���������� - ������� 2
#include <cmath>
#include <thread>
#include <vector>
#include <ctime>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

// ���-�� �����������
const int SMOKERS_AMOUNT = 3;

// ���-�� ���, ������� ��������� ����� ������ �������� �� ���� (�� ������ ������� ����)
const int N = 8;

// ����������� ����
class SmokeTable
{
	private:
		// ���-�� ������� �� �����
		int paper;
		
		// ���-�� ������ �� �����
		int tobacco;

		// ���-�� ������ �� �����
		int match;

		// �������
		mutex mute;

		// �������� ���������� - �������� �� ����������
		condition_variable conditional_smokers_have_smoked;

		// �������� ���������� - �������� �� ����� �������� �� ����
		condition_variable conditional_new_items;

		// ������� �� ���������, ������� ��������� ��� ���� �� ����� ��������
		bool is_smoked = false;

	public:
		// ����������� �� ���������
		SmokeTable()
		{
			// ��������� ��� �������� �� 0 - ��� ��� ���� � ����� ������ ������
			paper = 0;
			tobacco = 0;
			match = 0;
		}

		// �������� ������� �� ���������, ������� ��������� ��� ���� �� ����� ��������
		bool isSmoked()
		{
			return is_smoked;
		}

		// ����� �������� ����, ����� �� �� ����� ������ ��� �������� ��������
		bool isRightItemsOnTheTable(int item_id)
		{
			switch (item_id)
			{
			case 0:
				// ���� � ���������� ���� ������
				if ((tobacco > 0) && (match > 0))
					return true;
				else
					return false;

			case 1:
				// ���� � ���������� ���� �����
				if ((paper > 0) && (match > 0))
					return true;
				else
					return false;

			case 2:
				// ���� � ���������� ���� ������
				if ((paper > 0) && (tobacco > 0))
					return true;
				else
					return false;

			default:
				return false;
			}
		}

		// ����� �� ����� 2 �������� � �������� �������� (����������� �� ���� �����������). ��������: 0 - ������, 1 - �����, 2 - ������
		void takeTwoThingsAndSmoke(int smoker_item)
		{
			// ������ ����������� ������
			unique_lock<mutex> un_lock(mute);

			// ���� �� ����� ���� �� ��������, ������� ����� ����������, �� �� �� �����. ����� - ����, ���� �� �������
			conditional_new_items.wait(un_lock, bind(&SmokeTable::isRightItemsOnTheTable, this, smoker_item));

			// ���� �������� ���� �� �����, ��������� ����� ��, ������ ��������, � �������� ������ � (�������� ��������)
			paper = 0;
			tobacco = 0;
			match = 0;

			// ������� ��������� � �������
			if (smoker_item == 0)
				cout << "Smoker with an item - paper - took needed items, made a cigarette and smoked it out!" << endl;
			else if (smoker_item == 1)
				cout << "Smoker with an item - tobacco - took needed items, made a cigarette and smoked it out!" << endl;
			else if (smoker_item == 2)
				cout << "Smoker with an item - matches - took needed items, made a cigarette and smoked it out!" << endl;
			
			// ��������, ��� �������� ��������
			is_smoked = true;

			// ���� ������ ���������� � ���, ��� ���-�� �� ����������� ������� ��������
			conditional_smokers_have_smoked.notify_all();
		}

		// �������� �� ���� 2 �������� � ����� ������. ���� ����� ���������� 1 ���, ����� ��������� ������ ��� �� ���� (����������� �� ���� ����������)
		void putTwoThingsBegin()
		{
			// ���������� ��������� ����� �� 0 �� 2
			int rand_number = rand() % 3;

			// ������ ����������� ������
			unique_lock<mutex> un_lock(mute);

			// � ����������� �� �����, ����������� �� ���� �����-�� 2 ��������
			switch (rand_number)
			{
			case 0:
				// ���� 0, �� ������ � �����
				paper++;
				tobacco++;
				cout << "The mediator has put two things on the table. These are: paper, tobacco!" << endl;
				break;

			case 1:
				// ���� 1, �� ������ � ������
				paper++;
				match++;
				cout << "The mediator has put two things on the table. These are: paper, matches!" << endl;
				break;

			case 2:
				// ���� 2, �� ����� � ������
				tobacco++;
				match++;
				cout << "The mediator has put two things on the table. These are: tobacco, matches!" << endl;
				break;
			}

			// ����� ����, ��� �������� �������� �� ����, ��������� �� ���� ���� �����������
			conditional_new_items.notify_all();
		}

		// �������� �� ���� 2 �������� (����������� �� ���� ����������)
		void putTwoThings()
		{
			// ���������� ��������� ����� �� 0 �� 2
			int rand_number = rand() % 3;

			// ������ ����������� ������
			unique_lock<mutex> un_lock(mute);

			// ����, ���� ���-�� �� ����������� �������
			conditional_smokers_have_smoked.wait(un_lock, bind(&SmokeTable::isSmoked, this));

			// ���������� ����� ����, ������� ������� ���� ������������
			is_smoked = false;

			// � ����������� �� �����, ����������� �� ���� �����-�� 2 ��������
			switch (rand_number)
			{
			case 0:
				// ���� 0, �� ������ � �����
				paper++;
				tobacco++;
				cout << "The mediator has put two things on the table. These are: paper, tobacco!" << endl;
				break;

			case 1:
				// ���� 1, �� ������ � ������
				paper++;
				match++;
				cout << "The mediator has put two things on the table. These are: paper, matches!" << endl;
				break;

			case 2:
				// ���� 2, �� ����� � ������
				tobacco++;
				match++;
				cout << "The mediator has put two things on the table. These are: tobacco, matches!" << endl;
				break;
			}

			// ����� ����, ��� �������� �������� �� ����, ��������� �� ���� ���� �����������
			conditional_new_items.notify_all();
		}

		// �������� �� ���� 2 �������� �� ������ - ������������ � ����� ����� ��������, ����� ���������� ����� ��-�� ����� (����������� �� ���� ����������)
		void putTwoSpecificThings(int items_for)
		{
			// ������ ����������� ������
			unique_lock<mutex> un_lock(mute);

			// ����, ���� ���-�� �� ����������� �������
			conditional_smokers_have_smoked.wait(un_lock, bind(&SmokeTable::isSmoked, this));

			// ���������� ����� ����, ������� ������� ���� ������������
			is_smoked = false;

			// � ����������� �� �����, ����������� �� ���� �����-�� 2 ��������
			switch (items_for)
			{
			case 0:
				// ���� 0, �� ������ � �����
				paper++;
				tobacco++;
				cout << "The mediator has put two things on the table. These are: paper, tobacco!" << endl;
				break;

			case 1:
				// ���� 1, �� ������ � ������
				paper++;
				match++;
				cout << "The mediator has put two things on the table. These are: paper, matches!" << endl;
				break;

			case 2:
				// ���� 2, �� ����� � ������
				tobacco++;
				match++;
				cout << "The mediator has put two things on the table. These are: tobacco, matches!" << endl;
				break;
			}

			// ����� ����, ��� �������� �������� �� ����, ��������� �� ���� ���� �����������
			conditional_new_items.notify_all();
		}
};

int main()
{
	// ���������� ��������� �����
	srand(time(NULL));

	// ������� ������ ����
	SmokeTable table;

	// ������� �����������
	thread smokers[SMOKERS_AMOUNT];

	// ���������� �� ���� �� �� �� ����� ��������� (���� ��������� �����, �� ���������� ���� ������ - ����� ���������)
	bool is_mediator_at_the_table = true;

	// ������� ����������
	thread mediator = thread([&table, &is_mediator_at_the_table] {

			// ������ ��� ��������� ������ �� ���� �������� �� ���������, ���� ������� ��������� (��� ��� ����� ��� �� �����)
			table.putTwoThingsBegin();

			// ������ ��������� ������ �� ���� n ��� ��������� ��������
			for (int i = 0; i < N; i++)
				table.putTwoThings();

			// ��������� ����� ������ ��������� ������� ���������� �� 2 ��������, ����� ��� ��������� ��� �������� � ��������� ����� ��-�� �����
			for (int i = 0; i < SMOKERS_AMOUNT; i++)
			{
				table.putTwoSpecificThings(i);

				// ��������� ������� ����, ��� ������� �������
				is_mediator_at_the_table = false;
			}
		});

	// ���� ����������� ������� - ������
	for (int i = 0; i < SMOKERS_AMOUNT; i++)
	{
		smokers[i] = thread([&table, &is_mediator_at_the_table, i] {
				
				// ���������� ���� ����� �������� �� ��� ���, ���� ��������� �� �����
				while(is_mediator_at_the_table)
					// ������� ����������� ����� � ��� �� ����� ������� - ������� i (0 - ������, 1 - �����, 2 - ������)
					table.takeTwoThingsAndSmoke(i);

			});
	}

	// ���������� ���� �������
	for (int i = 0; i < SMOKERS_AMOUNT; i++)
		smokers[i].join();

	// ���������� ���� �������
	mediator.join();

	// ������� ��������� � ���, ��� �� ������ ������� - ������ �������
	cout << endl << "Done!";
}