#include <windows.h>
#include <thread>
#include <chrono>
#include <utility>

#include "fairy_tail.hpp"

// Представляет клетку "Лабиринта"
struct Cell
{
	char state; //	Состояния:
				//	стена - #
				//	неизвестно - ?
				//	проход - .
				//	Иван - @
				//	Елена - &
	int from;   // Направление из которого мы пришли в клетку
	bool full_explored = false;
};

struct Result
{
	std::pair<int, int>				ivan_pos;
	std::pair<int, int>				elena_pos;
	int								turns_until_full_research = - 1;
	int								turns_until_meet = -1;
	std::vector<std::vector<Cell>>	matrix;
};

Direction	dirs[4] { Direction::Up, Direction::Right, Direction::Down, Direction::Left }; // Для итерации по направлениям

// Смещения при движении в направлении с соответсвующим индексом
int			yMove[4]{ -1, 0, 1, 0 };
int			xMove[4]{ 0, 1, 0, -1 };

int opposite_dir(int direction)
{
	return (direction + 2) % 4;
}

// Выводит часть матрицы, содержащию полезную информацию
void print(std::vector<std::vector<Cell>>& m, COORD ivan = { 0,0 });
Result Walk_Through(Fairyland* land, size_t matrix_size, bool full_exploration_mode = true, bool interactive_mode = false, int interactive_mode_speed = 50);


int main()
{
	size_t matrix_size = 10;
	Fairyland world;

	Result res = Walk_Through(&world, matrix_size, true, true);
	
	if (res.turns_until_meet == -1)
	{
		std::cout << "Cannot find path" << std::endl;
	}
	else
	{
		res.matrix[res.elena_pos.first][res.elena_pos.second].state = '&';
	}
	res.matrix[res.ivan_pos.first][res.ivan_pos.second].state = '@';

	print(res.matrix);

	std::cout << "Turns until meet = " << res.turns_until_meet << std::endl;
	std::cout << "Turns until full exploration = " << res.turns_until_full_research << std::endl;

}
void print(std::vector<std::vector<Cell>>& m, COORD ivan)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	std::cout << "----------------------------------------------\n";
	for (int i = 0; i < m.size(); i++)
	{
		for (int j = 0; j < m[0].size(); j++)
			std::cout << m[j][i].state;

		std::cout << std::endl;
	}
	std::cout << "----------------------------------------------\n";
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { ivan.X, short(ivan.Y + 1) });
	std::cout << '@';
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, (short)(m.size() + 2) });
}
Result Walk_Through(Fairyland* land, size_t matrix_size, bool full_exploration_mode, bool interactive_mode, int interactive_mode_speed)
{
	Result result;

	// Матрица создаётся размером 2 * n + 1, что бы была возможность пройти в любую сторону
	std::vector<std::vector<Cell>> matrix(2 * matrix_size + 1, std::vector<Cell>(2 * matrix_size + 1, { ' ', -1 }));

	// Принимается что Иван находится в центре матрицы
	const int x_start = matrix_size;
	const int y_start = matrix_size;


	// Координаты текущей позиции Ивана в матрице
	int x = x_start;
	int y = y_start;

	result.turns_until_meet = -1;


	bool forward = true; // Направление движение

	matrix[x][y].state = '.';

	while (true)
	{
		if (interactive_mode)
		{
			print(matrix, { short(x),short(y) });
			std::this_thread::sleep_for(std::chrono::milliseconds(interactive_mode_speed));
		}

		int direction = -1;
		int ways_count = 0;


		// Поиск стен и доступных путей
		for (size_t i = 0; i < 4; i++)
		{
			if (!land->canGo(Character::Ivan, dirs[i]))
				matrix[x + xMove[i]][y + yMove[i]].state = '#';
			else
			{
				if (matrix[x + xMove[i]][y + yMove[i]].state != '.')
				{

					ways_count++;
					direction = i;
				}
			}
		}


		forward = true;
		// Если доступных путей нет, то идем назад
		if (direction == -1)
		{
			forward = false;
			direction = matrix[x][y].from;
		}

		if (land->go(dirs[direction], Direction::Pass))
		{
			result.turns_until_meet = land->getTurnCount();

			result.elena_pos.first = x + xMove[direction];
			result.elena_pos.second = y + yMove[direction];

			if (!full_exploration_mode)
				break;
		}

		// Если при возврате назад  мы вернулись в исходную точку, и в этой точке нет доступных путей
		if (x == x_start && y == y_start && ways_count == 0)
		{
			break;
		}

		x += xMove[direction];
		y += yMove[direction];


		if (forward)
		{
			matrix[x][y].state = '.';
			matrix[x][y].from = opposite_dir(direction);
		}
	}
	

	result.ivan_pos.first = x;
	result.ivan_pos.second = y;
	
	result.matrix = matrix;
	result.turns_until_full_research = land->getTurnCount();

	return result;
}