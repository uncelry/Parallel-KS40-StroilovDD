// Строилов Денис Дмитриевич - Вариант 2
#include <cmath>
#include <thread>
#include <vector>
#include <ctime>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

// Кол-во курильщиков
const int SMOKERS_AMOUNT = 3;

// Кол-во раз, которое курильщик будет класть предметы на стол (не считая первого раза)
const int N = 8;

// Курительный стол
class SmokeTable
{
	private:
		// Кол-во бумажек на столе
		int paper;
		
		// Кол-во табака на столе
		int tobacco;

		// Кол-во спичек на столе
		int match;

		// Мьютекс
		mutex mute;

		// Условная переменная - докурили ли курильщики
		condition_variable conditional_smokers_have_smoked;

		// Условная переменная - выложили ли новые предметы на стол
		condition_variable conditional_new_items;

		// Докурил ли курильщик, который последний раз взял со стола предметы
		bool is_smoked = false;

	public:
		// Конструктор по умолчанию
		SmokeTable()
		{
			// Выставить все предметы на 0 - так как стол в самом начале пустой
			paper = 0;
			tobacco = 0;
			match = 0;
		}

		// Проверка докурил ли курильщик, который последний раз взял со стола предметы
		bool isSmoked()
		{
			return is_smoked;
		}

		// Метод проверки того, лежат ли на столе нужные для сигареты предметы
		bool isRightItemsOnTheTable(int item_id)
		{
			switch (item_id)
			{
			case 0:
				// Если у курильщика есть бумага
				if ((tobacco > 0) && (match > 0))
					return true;
				else
					return false;

			case 1:
				// Если у курильщика есть табак
				if ((paper > 0) && (match > 0))
					return true;
				else
					return false;

			case 2:
				// Если у курильшика есть спички
				if ((paper > 0) && (tobacco > 0))
					return true;
				else
					return false;

			default:
				return false;
			}
		}

		// Взять со стола 2 предмета и выкурить сигарету (выполняется от лица курильщиков). Параметр: 0 - бумага, 1 - табак, 2 - спичка
		void takeTwoThingsAndSmoke(int smoker_item)
		{
			// Начало критической секции
			unique_lock<mutex> un_lock(mute);

			// Если на столе есть те предметы, которые нужны курильщику, то он их берет. Иначе - ждет, пока их положат
			conditional_new_items.wait(un_lock, bind(&SmokeTable::isRightItemsOnTheTable, this, smoker_item));

			// Если предметы есть на столе, курильщик берет их, делает сигарету, и начинает курить её (обнуляем предметы)
			paper = 0;
			tobacco = 0;
			match = 0;

			// Выводим сообщение в консоль
			if (smoker_item == 0)
				cout << "Smoker with an item - paper - took needed items, made a cigarette and smoked it out!" << endl;
			else if (smoker_item == 1)
				cout << "Smoker with an item - tobacco - took needed items, made a cigarette and smoked it out!" << endl;
			else if (smoker_item == 2)
				cout << "Smoker with an item - matches - took needed items, made a cigarette and smoked it out!" << endl;
			
			// Отмечаем, что сигарета докурена
			is_smoked = true;

			// Даем сигнал посреднику о том, что кто-то из курильщиков выкурил сигарету
			conditional_smokers_have_smoked.notify_all();
		}

		// Выложить на стол 2 предмета в самом начале. Этот метод вызывается 1 раз, когда посредник только сел за стол (выполняется от лица посредника)
		void putTwoThingsBegin()
		{
			// Генерируем случайное число от 0 до 2
			int rand_number = rand() % 3;

			// Начало критической секции
			unique_lock<mutex> un_lock(mute);

			// В зависимости от числа, выкладываем на стол какие-то 2 предмета
			switch (rand_number)
			{
			case 0:
				// Если 0, то бумага и табак
				paper++;
				tobacco++;
				cout << "The mediator has put two things on the table. These are: paper, tobacco!" << endl;
				break;

			case 1:
				// Если 1, то бумага и спичка
				paper++;
				match++;
				cout << "The mediator has put two things on the table. These are: paper, matches!" << endl;
				break;

			case 2:
				// Если 2, то табак и спичка
				tobacco++;
				match++;
				cout << "The mediator has put two things on the table. These are: tobacco, matches!" << endl;
				break;
			}

			// После того, как предметы положили на стол, оповещаем об этом всех курильщиков
			conditional_new_items.notify_all();
		}

		// Выложить на стол 2 предмета (выполняется от лица посредника)
		void putTwoThings()
		{
			// Генерируем случайное число от 0 до 2
			int rand_number = rand() % 3;

			// Начало критической секции
			unique_lock<mutex> un_lock(mute);

			// Ждем, пока кто-то из курильщиков докурит
			conditional_smokers_have_smoked.wait(un_lock, bind(&SmokeTable::isSmoked, this));

			// Начинается новый круг, поэтому убираем флаг докуренности
			is_smoked = false;

			// В зависимости от числа, выкладываем на стол какие-то 2 предмета
			switch (rand_number)
			{
			case 0:
				// Если 0, то бумага и табак
				paper++;
				tobacco++;
				cout << "The mediator has put two things on the table. These are: paper, tobacco!" << endl;
				break;

			case 1:
				// Если 1, то бумага и спичка
				paper++;
				match++;
				cout << "The mediator has put two things on the table. These are: paper, matches!" << endl;
				break;

			case 2:
				// Если 2, то табак и спичка
				tobacco++;
				match++;
				cout << "The mediator has put two things on the table. These are: tobacco, matches!" << endl;
				break;
			}

			// После того, как предметы положили на стол, оповещаем об этом всех курильщиков
			conditional_new_items.notify_all();
		}

		// Выложить на стол 2 предмета по выбору - используется в самом конце перекура, чтобы курильщики вышли из-за стола (выполняется от лица посредника)
		void putTwoSpecificThings(int items_for)
		{
			// Начало критической секции
			unique_lock<mutex> un_lock(mute);

			// Ждем, пока кто-то из курильщиков докурит
			conditional_smokers_have_smoked.wait(un_lock, bind(&SmokeTable::isSmoked, this));

			// Начинается новый круг, поэтому убираем флаг докуренности
			is_smoked = false;

			// В зависимости от числа, выкладываем на стол какие-то 2 предмета
			switch (items_for)
			{
			case 0:
				// Если 0, то бумага и табак
				paper++;
				tobacco++;
				cout << "The mediator has put two things on the table. These are: paper, tobacco!" << endl;
				break;

			case 1:
				// Если 1, то бумага и спичка
				paper++;
				match++;
				cout << "The mediator has put two things on the table. These are: paper, matches!" << endl;
				break;

			case 2:
				// Если 2, то табак и спичка
				tobacco++;
				match++;
				cout << "The mediator has put two things on the table. These are: tobacco, matches!" << endl;
				break;
			}

			// После того, как предметы положили на стол, оповещаем об этом всех курильщиков
			conditional_new_items.notify_all();
		}
};

int main()
{
	// Генерируем случайные числа
	srand(time(NULL));

	// Создаем объект стол
	SmokeTable table;

	// Создаем курильщиков
	thread smokers[SMOKERS_AMOUNT];

	// Определяет не ушел ли из за стола посредник (если посредник уйдет, то курильщики тоже уходят - конец программы)
	bool is_mediator_at_the_table = true;

	// Создаем посредника
	thread mediator = thread([&table, &is_mediator_at_the_table] {

			// Первый раз посредник кладет на стол предметы не дожидаясь, пока докурит курильщик (так как никто ещё не курит)
			table.putTwoThingsBegin();

			// Дальше посредник кладет на стол n раз случайные предметы
			for (int i = 0; i < N; i++)
				table.putTwoThings();

			// Посредник перед уходом оставляет каждому курильщику по 2 предмета, чтобы все последний раз докурили и спокойной вышли из-за стола
			for (int i = 0; i < SMOKERS_AMOUNT; i++)
			{
				table.putTwoSpecificThings(i);

				// Посредник говорит всем, что перекур окончен
				is_mediator_at_the_table = false;
			}
		});

	// Даем курильщикам задание - курить
	for (int i = 0; i < SMOKERS_AMOUNT; i++)
	{
		smokers[i] = thread([&table, &is_mediator_at_the_table, i] {
				
				// Курильщики ждут новые предметы до тех пор, пока посредник не уйдет
				while(is_mediator_at_the_table)
					// Говорим курильщикам какой у них на руках предмет - счетчик i (0 - бумага, 1 - табак, 2 - спички)
					table.takeTwoThingsAndSmoke(i);

			});
	}

	// Дожидаемся всех потоков
	for (int i = 0; i < SMOKERS_AMOUNT; i++)
		smokers[i].join();

	// Дожидаемся всех потоков
	mediator.join();

	// Выводим сообщение о том, что всё прошло успешно - перкур окончен
	cout << endl << "Done!";
}