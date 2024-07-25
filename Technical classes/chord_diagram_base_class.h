#pragma once
#include <ostream>

#include "Technical classes/linear_function_coefficients.h"

/*
This file contains base class for chord diagrams. It has two derived classes: ACD and preACD.
*/

class chord_diagram_base {
protected:
	linear_function total_increaser;	// If some of the chords have been eliminated, the resulting linear function is stored here.
	int number_of_increases;			// The number of chords that have been eliminated.

public:
	explicit chord_diagram_base(int num_of_variables) : total_increaser(0, 0, num_of_variables), number_of_increases(0) {}

	//Return vector with all indices where we can apply Transformation II. 
	virtual std::vector<int> get_all_possible_turns() const noexcept = 0;

	//Use Transformation I to eliminate the chord closest to the basepoint. 
	//If the function returns false, then there are no letters left on the corresponding side.
	// is_limit_case shows whether we consider the limit case, i.e. when m->inf (and thus adding one more crossing is insignificant)
	virtual bool eliminate_from_side(bool is_right, bool is_limit_case) noexcept = 0;

	//Use Transformation II to eliminate chord. 
	virtual bool eliminate_turn(int index) noexcept = 0;

	const linear_function& get_increaser() const noexcept {
		return total_increaser;
	}

	int get_number_of_increases() const noexcept {
		return number_of_increases;
	}
};