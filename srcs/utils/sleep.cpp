#include "../../includes/utils.hpp"

void	utils::sleep(int seconds) {
	clock_t start = std::clock();
	clock_t target = start + seconds * CLOCKS_PER_SEC;

	while (std::clock() < target) ;
}
