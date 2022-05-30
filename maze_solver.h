#pragma once

#include <utility>
#include <vector>

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
	std::pair<unsigned int, unsigned int>	ivan_pos;
	std::pair<unsigned int, unsigned int>	elena_pos;
	int										turns_until_full_research = -1;
	int										turns_until_meet = -1;
	std::vector<std::vector<Cell>>			matrix;
};

// Для итерации по направлениям
extern Direction	dirs[4];

// Смещения при движении в направлении с соответсвующим индексом
extern int	yMove[4];
extern int	xMove[4];

inline int opposite_dir(int direction)
{
	return (direction + 2) % 4;
}

// Выводит матрицу
void print(std::vector<std::vector<Cell>>& m, std::pair<int,int> ivan);
void print(std::vector<std::vector<Cell>>& m);

Result Walk_Through(Fairyland* land, unsigned int matrix_size, bool full_exploration_mode = true, bool interactive_mode = false, int interactive_mode_speed = 50);


bool SetCursorPosition(short x, short y);
void ThreadWait(unsigned int ms);
