#include "maze_solver.h"

#include <thread>
#include <chrono>

#ifdef _WIN64
#include <windows.h>
#endif // _WIN64

int			yMove[4]{ -1, 0, 1, 0 };
int			xMove[4]{ 0, 1, 0, -1 };
Direction	dirs [4]{ Direction::Up, Direction::Right, Direction::Down, Direction::Left };

bool SetCursorPosition(short x, short y)
{
#ifdef _WIN64
	static HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	return SetConsoleCursorPosition(console, { x, y });
#else 
	return false;
#endif // _WIN64
}
void ThreadWait(unsigned int ms)
{
	std::this_thread::sleep_for(std::chrono::microseconds(ms));
}

void print(std::vector<std::vector<Cell>>& m, std::pair<int,int> ivan)
{
	SetCursorPosition(0, 0);
	std::cout << "----------------------------------------------\n";
	for (size_t i = 0; i < m.size(); i++)
	{
		for (size_t j = 0; j < m[0].size(); j++)
			std::cout << m[j][i].state;

		std::cout << std::endl;
	}
	std::cout << "----------------------------------------------\n";
	SetCursorPosition(ivan.first, (short)(ivan.second + 1));
	std::cout << '@';
	SetCursorPosition(0, short(m.size() + 2));
}
void print(std::vector<std::vector<Cell>>& m)
{
	SetCursorPosition(0, 0);
	std::cout << "----------------------------------------------\n";
	for (size_t i = 0; i < m.size(); i++)
	{
		for (size_t j = 0; j < m[0].size(); j++)
			std::cout << m[j][i].state;

		std::cout << std::endl;
	}
	std::cout << "----------------------------------------------\n";
	SetCursorPosition(0, short(m.size() + 2));
}

Result Walk_Through(Fairyland* land, unsigned int matrix_size, bool full_exploration_mode, bool realtime_mode, int realtime_mode_speed)
{
	// раздражает
#pragma warning(disable : 26451)

	Result result;
	// Матрица создаётся размером 2 * n + 1, что бы была возможность пройти в любую сторону
	std::vector<std::vector<Cell>> matrix(2 * matrix_size + 1, std::vector<Cell>(2 * matrix_size + 1, { ' ', -1 }));

	// Принимается что Иван находится в центре матрицы
	const unsigned int x_start = matrix_size;
	const unsigned int y_start = matrix_size;


	// Координаты текущей позиции Ивана в матрице
	unsigned int x = x_start;
	unsigned int y = y_start;

	// Если значение останется равно -1, то встреча не произошла
	result.turns_until_meet = -1;


	bool forward = true; // Направление движение

	matrix[x][y].state = '.';

	bool realtime_mode_compatibility = SetCursorPosition(0, 0);
	while (true)
	{
		if (realtime_mode && realtime_mode_compatibility)
		{
			print(matrix, { short(x),short(y) });
			ThreadWait(realtime_mode_speed);
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
					direction = (int)i;
				}
			}
		}
#pragma warning(default: 26451)

		forward = true;
		// Если доступных путей нет, то идем назад
		if (direction == -1)
		{
			forward = false;
			direction = matrix[x][y].from;
		}

		// Если встреча произошла
		if (land->go(dirs[direction], Direction::Pass))
		{
			result.turns_until_meet = land->getTurnCount();

			result.elena_pos.first = x + xMove[direction];
			result.elena_pos.second = y + yMove[direction];

			// Если full_exploration_mode == true, то алгоритм пройдет лабиринт до последней клетки
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