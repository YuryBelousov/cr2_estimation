#include <ostream>
#include <fstream>

#include "brute_force_functions.h"

int main() {
	std::cout << "Write number of chords (it should be less than " << n_max << "): "; // See linear_function_coefficients for details.
	int n = 1;
	std::cin >> n;

	if (n > n_max || n < 0) {
		std::cout << "The entered value is incorrect! Assume it is equal to " << n_max << std::endl;
		n = n_max;
	}

	std::ofstream file_out_ACD("Files with numbers//the_output_1-" + std::to_string(n) + "(ACD).txt");
	std::ofstream file_out_pre_ACD("Files with numbers//the_output_1-" + std::to_string(n) + "(pre_ACD).txt");

	get_main_estimates(file_out_ACD, file_out_pre_ACD, 1, n);
	return 0;
}