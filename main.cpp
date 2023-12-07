#include <set>
#include <ios>
#include <vector>
#include <ostream>
#include <fstream>
#include <iostream>

#include "brute_force_functions.h"

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cout << "Write number of chords (it should be less than " << n_max << "): "; // See linear_function_coefficients for details.
	int n = 1;
	std::cin >> n;

	if (n > n_max) {
		std::cout << "The entered value is greater than the permissible value!" << std::endl;
		n = n_max;
	}

	std::ofstream file_out_ACD("the_output_1-" + std::to_string(n) + "(ACD).txt");
	std::ofstream file_out_pre_ACD("the_output_1-" + std::to_string(n) + "(pre_ACD).txt");

	get_main_estimates(file_out_ACD, file_out_pre_ACD, 1, n);

	return 0;
}