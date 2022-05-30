#include "maze_solver.h"

int main()
{
	const size_t matrix_size = 10;
	Fairyland world;

	Result res = Walk_Through(&world, matrix_size, false, false);

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