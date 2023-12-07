#pragma once
#include <ostream>

#include "linear_function_coefficients.h"

// Base class for annotated chord diagrams. It has two derived classesL ACD and pre_ACD.

class chord_diagram_base {
protected:
	linear_function total_increaser;	// If some of the chords have been eliminated, the resulting linear function is stored here.
	int number_of_increases;			// The number of chords that have been eliminated.

public:
	explicit chord_diagram_base(int num_of_variables) : total_increaser(0, 0, num_of_variables), number_of_increases(0) {}

	// Removes the outermost chord from the selected side. 
	// If the function returns false, then this cannot e done.
	// is_limit_case indicates whether we are considering the limit case, i.e. when m->inf (and thus adding a single crossing is insignificant).
	virtual bool eliminate_from_side(bool is_right, bool is_limit_case) noexcept = 0;

	// Use second method to eliminate chords with a given index.
	virtual bool eliminate_turn(int index) noexcept = 0;

	// Return vector with all indices where we can apply the second method. 
	// Note: the index cannot be zero.
	virtual std::vector<int> get_all_possible_turns() const noexcept = 0;

	linear_function get_increaser() const noexcept {
		return total_increaser;
	}

	int get_number_of_increases() const noexcept {
		return number_of_increases;
	}
};