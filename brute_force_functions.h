#pragma once
#include <unordered_set>
#include <iomanip>
#include <thread>
#include <sstream>

#include "alglib/optimization.h"	// This is a part of ALGLIB library, see https://www.alglib.net/
#include "output_functions.h"

/*
Main functions for finding estimates.
*/


// Check if (pre)ACD constructed from this vector would give a small estimate. There are several possible possibilities: 
// (1) the number of chords is greater than 3, and there is a chord of length 1;
// (2) the number of chords is greater than 5, and we can eliminate all chords without passing through a chord that has already been eliminated.
template<class T>
bool not_interesting(const std::vector<int>& chords, int required_k) {
	if (required_k < 4)
		return false;
	// Check if there a chord of length 1. 
	// Note that preACD is not a cyclic string.
	for (int i = 1; i < chords.size(); ++i)
		if ((chords[i] == chords[i - 1]) 
			&& ((typeid(T) == typeid(ACD)) || (typeid(T) == typeid(pre_ACD)) && (i != required_k)))
			return true;
	// Check if we can eliminate all chords without passing through a chord that has already been eliminated.
	// Note that we only need to check the last k chords, as the previous sequences have already been checked.
	if (required_k > 5 && chords.size() >= required_k) {
		std::unordered_set<int> chords_names;
		for (int i = 0; i < required_k; ++i)
			chords_names.insert(chords[chords.size() - i - 1]);
		return chords_names.size() == required_k;
	}
	return false;
}


// This function recursively constructs a set of all linear functions obtained after number_of_moves consecutive eliminations. 
template <class chord_like_type>
void all_linear_functions_chord_diagram(chord_like_type start, std::set<linear_function>& results, int number_of_moves, bool is_limit_case) {
	if (start.get_number_of_increases() == number_of_moves) {
		results.insert(start.get_increaser());
		return;
	}
	// Considering all the possible ways to make a turn.
	chord_like_type start_copy(start);
	auto all_turns = start.get_all_possible_turns();
	for (auto ind : all_turns) {
		start_copy = start;
		start_copy.eliminate_turn(ind);
		all_linear_functions_chord_diagram(start_copy, results, number_of_moves, is_limit_case);
	}
	// Try to eliminate a chord from the left.
	start_copy = start;
	if (start_copy.eliminate_from_side(false, is_limit_case))
		all_linear_functions_chord_diagram(start_copy, results, number_of_moves, is_limit_case);
	// Try to eliminate a chord from the right.
	if (start.eliminate_from_side(true, is_limit_case))
		all_linear_functions_chord_diagram(start, results, number_of_moves, is_limit_case);
}


// This function returns the solution to the linear programming problem. See details inside.
double create_solver(const std::set<linear_function>& all_linear_functions, bool is_limit_case, bool print_solution = false) {
	/*
	We solve the following optimization problem:
	-x[n+1] -> min
	0 <= x[i] <= m,			where i=1,2,...,n
	0 <= x[1] + x[2] + ... + x[n] <= m
	f[j](x) - x[n+1] >= 0,	where all f[j](x) are linear functions

	Here x[0] is constant.
	*/
	double m = (is_limit_case) ? 1. : 8.;	// If we consider non limit case, we always can start with a simple curve with 8 crossings 
	int num_of_variables =					// Additional variable for the minimization function
		all_linear_functions.begin()->get_number_of_variables() + 1;					
	int num_of_inequalities =				// Each linear function gives an inequality and also one more inequality, namely
		all_linear_functions.size() + 1;	// x[1] + x[2] + ... + x[n] <= m

	alglib::real_1d_array bounds_lower;		// Lower bounds on the variables value
	alglib::real_1d_array bounds_upper;		// Upper bound on the variables value
	
	alglib::real_1d_array cost;		// Minimization function

	alglib::real_1d_array scales;	// Scales is a technical parameter, see https://www.alglib.net/optimization/scaling.php

	alglib::real_2d_array a;		// Matrix for the main set of inequalities
	alglib::real_1d_array a_lower;	// Lower bounds for each inequality
	alglib::real_1d_array a_upper;	// Upper bounds for each inequality

	double temp[2 * n_max + 3];	//Technical array, used to specify all other objects.

	// Set lower bounds for variables. 
	// In our case x[0] = 1 (it is a constant), x[i]=>0 (for i=1,2,...,n) and x[n+1] > -inf.
	std::fill_n(temp, num_of_variables, 0.);
	bounds_lower.setcontent(num_of_variables, temp);
	bounds_lower[0] = 1;
	bounds_lower[num_of_variables - 1] = alglib::fp_neginf;

	// Set upper bounds for variables. 
	// In our case x[0] = 1 (it is a constant), x[i]<=1 (for i=1,2,...,n) and x[n+1] < inf.
	std::fill_n(temp, num_of_variables, m);
	bounds_upper.setcontent(num_of_variables, temp);
	bounds_upper[0] = 1;
	bounds_upper[num_of_variables - 1] = alglib::fp_posinf;

	// Set minimization function coefficients.
	// In our case its -1*x[n+1].
	std::fill_n(temp, num_of_variables, 0.);
	cost.setcontent(num_of_variables, temp);
	cost[num_of_variables - 1] = -1;

	// Scales is a technical function, see https://www.alglib.net/optimization/scaling.php
	std::fill_n(temp, num_of_variables, 1);
	scales.setcontent(num_of_variables, temp);
	scales[num_of_variables - 1] = 1;

	// Set bounds for main inequalities.
	// In our case they have the form 0 <= f_j(x) - x[n+1] < inf 
	a_lower.setlength(num_of_inequalities);
	a_upper.setlength(num_of_inequalities);
	for (size_t i = 0; i < num_of_inequalities; ++i) {
		a_lower[i] = 0;
		a_upper[i] = alglib::fp_posinf;
	}
	// Set bounds for the last inequality: 0 <= x[1] + x[2] + ... + x[n] <= m
	a_lower[num_of_inequalities - 1] = 0;
	a_upper[num_of_inequalities - 1] = m;

	// Set coefficient based on linear functions f[j].
	// They have the form f[j](x) - x[n+1] >= 0,
	a.setlength(num_of_inequalities, num_of_variables);
	int i_ineq = -1;
	for (const auto& p : all_linear_functions) {
		++i_ineq;
		for (size_t i_var = 0; i_var < num_of_variables - 1; ++i_var) 
			a[i_ineq][i_var] = p.get_coef_under_variable(i_var);
		a[i_ineq][num_of_variables - 1] = -1;
	}

	// Set coefficient for the last inequality: 0 <= x[1] + x[2] + ... + x[n] <= m
	for (size_t i_var = 1; i_var < num_of_variables - 1; ++i_var)
		a[num_of_inequalities - 1][i_var] = 1;
	a[num_of_inequalities - 1][0] = 0;
	a[num_of_inequalities - 1][num_of_variables - 1] = 0;

	alglib::real_1d_array solution;
	alglib::minlpstate state;
	alglib::minlpreport rep;

	alglib::minlpcreate(num_of_variables, state);

	alglib::minlpsetcost(state, cost);
	alglib::minlpsetbc(state, bounds_lower, bounds_upper);
	alglib::minlpsetscale(state, scales);

	alglib::minlpsetlc2dense(state, a, a_lower, a_upper, num_of_inequalities);
	alglib::minlpoptimize(state);

	alglib::minlpresults(state, solution, rep);

	if (print_solution)
		std::cout << solution.tostring(3) << std::endl;
	return solution[num_of_variables - 1];
}


// This function creates the linear programming problem for a (pre)ACD and returns its solution.
template <class chord_like_type>
double get_estimates_for_one_chord_diagram(const chord_like_type& s, int num_of_eleminations, bool is_limit_case) {
	std::set<linear_function> results;
	all_linear_functions_chord_diagram(s, results, num_of_eleminations, is_limit_case);
	return create_solver(results, is_limit_case);
}


// This function recursively traverses all possible interesting (pre)ACD starting with rhs_lhs by adding symbol new_v.
template<class chord_like_type>
void walk_trough_all_diagrams(std::vector<int>& rhs_lhs, double& value_8, double& value_limit, int& number_of_diag, int length, int new_v) {
	// Add new symbol.
	rhs_lhs.push_back(new_v);
	// Check if it can result in an interesting (pre)ACD
	if (not_interesting<chord_like_type>(rhs_lhs, length)) {
		rhs_lhs.pop_back();
		return;
	}
	// Check that the size of rhs_lhs is sufficient to create the (pre)ACD.
	if (rhs_lhs.size() == 2 * length) {
		chord_like_type new_chord_diag = chord_like_type(rhs_lhs);
		++number_of_diag;

		value_8 = std::max(value_8, get_estimates_for_one_chord_diagram<chord_like_type>(new_chord_diag, length, false));
		value_limit = std::max(value_limit, get_estimates_for_one_chord_diagram<chord_like_type>(new_chord_diag, length, true));

		rhs_lhs.pop_back();
		return;
	}

	// Find all possible ways to add another symbol to rhs_lhs.
	std::unordered_set<int> possible_values;
	int max_v = -1;
	for (auto c : rhs_lhs) {
		auto iter_c = possible_values.find(c);
		if (iter_c == possible_values.end())
			possible_values.insert(c);
		else
			possible_values.erase(iter_c);
		max_v = std::max(c, max_v);
	}
	if ((max_v < length) || typeid(chord_like_type) == typeid(pre_ACD)) 
		possible_values.insert(max_v + 1);
	
	for (const auto& c : possible_values) 
		walk_trough_all_diagrams< chord_like_type>(rhs_lhs, value_8, value_limit, number_of_diag, length, c);
	rhs_lhs.pop_back();
}


// This is the main function for finding estimates. It returns the total number of traversed (pre)ACD.
template<class chord_like_type>
int calculate_estimate(int length, double& max_value_8, double& max_value_limit) {
	// If tength is small, we do not use threads
	if (length < 6) {
		std::vector<int> rhs_lhs({});
		int number_of_diag = 0;
		walk_trough_all_diagrams<chord_like_type>(rhs_lhs, max_value_8, max_value_limit, number_of_diag, length, 1);
		return number_of_diag;
	}

	// Otherwise we use 11 threads.
	const int number_of_threads = 11;

	std::thread calculation[number_of_threads];

	int number_of_diag[number_of_threads];			// The number of diagrams traversed by each of the threads.
	double estimate_8[number_of_threads],			// The maximum estimate obtained by each of the threads.
		estimate_limit[number_of_threads];

	std::fill_n(number_of_diag, number_of_threads, 0);
	std::fill_n(estimate_8, number_of_threads, -1.);
	std::fill_n(estimate_limit, number_of_threads, -1.);

	// We have found that this partitioning results in the most uniform distribution of work between threads.
	// Where lhs_rhs[i] is the starting lhs_rhs for the walk_trough_all_diagrams function, 
	// and last_element[i] is the corresponding new_v symbol.
	std::vector<int> lhs_rhs[number_of_threads];
	lhs_rhs[0] = { 1, 2, 3, 4, 5 };
	lhs_rhs[1] = { 1, 2, 3, 4, 5 };
	lhs_rhs[2] = { 1, 2, 3, 4, 5 };
	lhs_rhs[3] = { 1, 2, 3, 4, 5 };
	lhs_rhs[4] = { 1, 2, 3, 4, 5 };
	lhs_rhs[5] = { 1, 2, 3, 4 };
	lhs_rhs[6] = { 1, 2, 3, 4 };
	lhs_rhs[7] = { 1, 2, 3, 4 };
	lhs_rhs[8] = { 1, 2, 3 };
	lhs_rhs[9] = { 1, 2, 3 };
	lhs_rhs[10] = { 1, 2};

	int last_elements[number_of_threads] = { 6, 4, 3, 2, 1, 3, 2, 1, 2, 1, 1 };

	for (int i = 0; i < number_of_threads; ++i) {
		calculation[i] = std::thread(
			walk_trough_all_diagrams<chord_like_type>,
			std::ref(lhs_rhs[i]),
			std::ref(estimate_8[i]),
			std::ref(estimate_limit[i]),
			std::ref(number_of_diag[i]),
			length, last_elements[i]);
	}

	for (int i = 0; i < number_of_threads; ++i)
		calculation[i].join();

	// Merge the results of the work of the threads.
	int total_num_of_diag = 0;
	for (int i = 0; i < number_of_threads; ++i) {
		max_value_8 = std::max(max_value_8, estimate_8[i]);
		max_value_limit = std::max(max_value_limit, estimate_limit[i]);
		total_num_of_diag += number_of_diag[i];
	}
	return total_num_of_diag;
}


// This function finds all the estimates, print them and saves the results to files. 
void get_main_estimates(std::ofstream& file_out_ACD, std::ofstream& file_out_pre_ACD, int start_length = 1, int max_length = 6) {	
	for (int k = start_length; k <= max_length; ++k) {
		std::cout
			<< "##################\n"
			<< "      Size " << k
			<< "\n##################\n";

		// Find estimates for preACD.
		std::clock_t start = std::clock();
		double max_value_8 = -1,
			max_value_limit = -1;
		int num_of_diag = calculate_estimate<pre_ACD>(k, max_value_8, max_value_limit);
		
		make_simple_output<pre_ACD>(std::cout, k, num_of_diag, max_value_8, max_value_limit, start);
		make_simple_output<pre_ACD>(file_out_pre_ACD, k, num_of_diag, max_value_8, max_value_limit, start);

		// Find estimates for ACD.
		start = std::clock();
		max_value_8 = -1;
		max_value_limit = -1;

		num_of_diag = calculate_estimate<ACD>(k, max_value_8, max_value_limit);

		make_simple_output<ACD>(std::cout, k, num_of_diag, max_value_8, max_value_limit, start);
		make_simple_output<ACD>(file_out_ACD, k, num_of_diag, max_value_8, max_value_limit, start);
	}
}

