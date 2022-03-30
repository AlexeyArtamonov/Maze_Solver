#include "Maze_Walker.h"

#include <windows.h>
#include <thread>
#include <chrono>
#include <utility>

namespace leart
{
	Maze_Walker::Maze_Walker(Fairyland* _world, size_t _matrix_size , bool full_exploration, bool real_time_output)
	:
	matrix_size(_matrix_size * 2 + 1),
		x_start(matrix_size),
		y_start(matrix_size),
		full_exploration_mode(full_exploration),
		real_time_output_mode(real_time_output)
	{
		world = _world;
		matrix = std::vector<std::vector<Cell>>(matrix_size, std::vector<Cell>(matrix_size, { ' ', -1 }));
	}

	int Maze_Walker::opposite_dir(int direction)
	{
		return (direction + 2) % 4;
	}

	Maze_Walker::Info Maze_Walker::solve()
	{
		bool forward = true; // Направление движение
		bool init = true;
		matrix[current_x][current_y].state = '.';
		int turns_until_meet = -1;

		while (true)
		{
			if (real_time_output_mode)
			{
				print();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			int direction = -1;
			int ways_count = 0;


			// Поиск стен и доступных путей
			for (size_t i = 0; i < 4; i++)
			{
				if (!world->canGo(Character::Ivan, dirs[i]))
					matrix[current_x + xMove[i]][current_y + yMove[i]].state = '#';
				else
				{
					ways_count++;
					if (matrix[current_x + xMove[i]][current_y + yMove[i]].state != '.')
						direction = i;
				}
			}

			// not a typo
			if (ways_count == 1)
			{
				matrix[current_x][current_y].full_explored = true;
			}
			if (forward && !init && ways_count == 2 && matrix[current_x + xMove[matrix[current_x][current_y].from]][current_y + yMove[matrix[current_x][current_y].from]].full_explored)
			{
				matrix[current_x][current_y].full_explored = true;
			}
			init = false;


			forward = true;
			// Если доступных путей нет, то идем назад
			if (direction == -1)
			{
				forward = false;
				direction = matrix[current_x][current_y].from;
			}

			if (world->go(dirs[direction], Direction::Pass))
			{
				turns_until_meet = world->getTurnCount();
				final_x = current_x + xMove[direction];
				final_y = current_y + yMove[direction];
			}

			current_x += xMove[direction];
			current_y += yMove[direction];

			// Если при возврате назад, мы вернулись на тупиковую ветвь или мы вернулись в исходную точку, и в этой точке нет доступных путей
			if (matrix[current_x][current_y].full_explored || (current_x == x_start && current_y == y_start && !forward))
			{
				break;
			}

			if (forward)
			{
				matrix[current_x][current_y].state = '.';
				matrix[current_x][current_y].from = opposite_dir(direction);
			}

		}

		INFO.has_solution = turns_until_meet != -1;
		INFO.result_matrix = cut();
		INFO.turns_until_solution = turns_until_meet;
		INFO.turns_until_solution = world->getTurnCount();
		return INFO;
	}

	std::vector<std::vector<char>> Maze_Walker::cut()
	{
		std::pair<size_t, size_t> left_upper_corner_position = {-1,-1};
		std::pair<size_t, size_t> right_bottom_corner_position = {-1,-1};

		for (size_t i = 0; i < matrix.size(); i++)
		{
			for (size_t j = 0; j < matrix.size(); j++)
			{
				if (matrix[j][i].state == (char)Symbols::Wall)
				{
					if (left_upper_corner_position.first == -1)
						left_upper_corner_position = {j, i};
					right_bottom_corner_position = {j, i};
				}
			}

		}

		std::vector<std::vector<char>> result
		(
			right_bottom_corner_position.first - left_upper_corner_position.first + 1,
			std::vector<char>(right_bottom_corner_position.second - left_upper_corner_position.second + 1)
		);
		for (size_t i = 0; i < result[0].size(); i++)
		{
			for (size_t j = 0; j < result.size(); j++)
			{
				result[j][i] = matrix[left_upper_corner_position.first + j][left_upper_corner_position.second + i].state;
			}

		}
		return result;
	}
	void Maze_Walker::print()
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, 0});
		std::cout << "----------------------------------------------\n";
		for (size_t i = 0; i <  matrix.size(); i++)
		{
			for (size_t j = 0; j < matrix.size(); j++)
				std::cout << matrix[j][i].state;

			std::cout << std::endl;
		}
		std::cout << "----------------------------------------------\n";
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(short)current_x, (short)current_y  + 1});
		std::cout << '@';
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, (short)matrix.size() + 2 });
	}
	void Maze_Walker::print_info()
	{
		std::cout << "Turns until meet: " << INFO.turns_until_solution << std::endl;
		std::cout << "Turns until ";
			if (full_exploration_mode)
				std::cout << "full exploration";
			else
				std::cout << "partial exploration";
		std::cout<<": " << INFO.turns_until_full_exploration;
		for (size_t i = 0; i < INFO.result_matrix[0].size(); i++)
		{
			for (size_t j = 0; j < INFO.result_matrix.size(); j++)
				std::cout << INFO.result_matrix[j][i];

			std::cout << std::endl;
		}
	}
}

