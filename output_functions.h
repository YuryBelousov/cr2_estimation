#pragma once
#include <iomanip>

#include "ACD.h"
#include "pre_ACD.h"

/*
Several technical functions required for convenient data output.
*/

// Technical function. It returns a string containing the time passed.
std::string get_time(const std::clock_t& start) {
	std::stringstream stream;
	double time = (double)(clock() - start) / CLOCKS_PER_SEC;
	stream << std::fixed << std::setprecision(2);
	std::string time_type(" sec.");
	if (time > 60) {
		time /= 60.;
		time_type = " min.";
		if (time > 60) {
			time /= 60.;
			time_type = " hours.";
			if (time > 24) {
				time /= 24.;
				time_type = " days.";
			}
		}
	}
	stream << time;
	return std::string(stream.str() + time_type);
}

// Technical function. It writes information into the stream.
template<class T>
void make_simple_output(std::ostream& stream_out, int length, int num_of_diag, double max_value_8, double max_value_limit, const std::clock_t& start) {
	std::string class_name = typeid(T) == typeid(ACD) ? "ACD" : "preACD";
	stream_out
		<< "Number of " << class_name << " of length  " << length
		<< " is " << num_of_diag
		<< ". It takes "
		<< get_time(start) << " Worst values are "
		<< std::fixed << std::setprecision(8)
		<< max_value_8 / 8. + 1. << " and " << max_value_limit + 1. << std::endl;
}
