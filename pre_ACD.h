#pragma once
#include <vector>
#include <set>
#include "chord_diagram_base_class.h"


class pre_ACD : public chord_diagram_base {
private:
	//Technical class for storing an alpha-string character along with an associated linear function
	class letter {
	public:
		int alpha_number;							//If it is -1, then letter is a part of simple curve J
		int id;										//Unique id of the letter;
		linear_function closest_linear_function;	//This is the linear function associated with the letter

	public:
		letter(letter&&) = default;
		letter(const letter&) = default;
		letter& operator=(letter&&) = default;
		letter& operator=(const letter&) = default;

		letter() : alpha_number(-1), id(-1), closest_linear_function() {}
		letter(int num, int _id, int num_of_variables) : alpha_number(num), id(_id), closest_linear_function(_id, 1, num_of_variables) {}

		letter& operator+=(const letter& letter) {
			closest_linear_function += letter.closest_linear_function;
			return *this;
		}
	};

private:
	std::vector<letter> lhs;			//List of letters on the left hand side from J (the closest point has the maximal index)
	std::vector<letter> rhs;			//List of letters on the right hand side from J (the closest point has the maximal index)

private:
	//Technical function. Checks if a letter has a pair. 
	//If there is, then the function returns a pointer to the side, and the num variable stores the index of the paired letter.
	std::vector<letter>* find_letter(letter& pair_letter, int& num) noexcept {
		for (size_t i = 0; i < lhs.size(); ++i) {
			if ((lhs[i].alpha_number == pair_letter.alpha_number) && (lhs[i].id != pair_letter.id)) {
				num = i;
				return &lhs;
			}
		}
		for (size_t i = 0; i < rhs.size(); ++i) {
			if ((rhs[i].alpha_number == pair_letter.alpha_number) && (rhs[i].id != pair_letter.id)) {
				num = i;
				return &rhs;
			}
		}
		return nullptr;
	}
public:
	pre_ACD() = default;
	pre_ACD(const std::vector<int>& _chords) noexcept : chord_diagram_base(_chords.size()+1){
		int half_size = _chords.size() / 2;
		lhs.resize(half_size);
		rhs.resize(half_size);
		for (int id_counter = 0; id_counter < half_size; ++id_counter) {
			lhs[id_counter] = letter(_chords[id_counter], 1 + id_counter, 2 * half_size + 1);
			rhs[id_counter] = letter(_chords[2*half_size - 1 - id_counter], 1 + id_counter + half_size, 2 * half_size + 1);
		}
	}

	//Removes the closest letter from the selected side. 
	//If the function returns false, then there are no letters left on the corresponding side.
	// is_limit_case shows whether we consider the limit case, i.e. when m->inf (and thus adding one more crossing is insignificant)
	bool eliminate_from_side(bool is_right, bool is_limit_case) noexcept {
		std::vector<letter>& side = is_right ? rhs : lhs;
		//Check if we can eliminate letter
		if (side.empty())
			return false;
		//Update increasing function
		++number_of_increases;
		total_increaser += 2 * side[side.size() - 1].closest_linear_function;
		if (!is_limit_case)
			total_increaser += 1;
		//Increase the linear function corresponding to the next letter
		if (side.size() != 1) 
			side[side.size() - 2] += side[side.size() - 1];
		//Check if there is a paired letter
		int paired_letter_index = -1;
		std::vector<letter>* paired_letter_side = find_letter(side[side.size() - 1], paired_letter_index);
		//If there is a paired letter, then delete it
		if (paired_letter_side != nullptr) {
			std::vector<letter>& pl_side = *paired_letter_side;
			//If the paired letter is not the last then increase the linear function corresponding to the next letter
			if (paired_letter_index > 0) {
				pl_side[paired_letter_index - 1].closest_linear_function +=
					pl_side[paired_letter_index].closest_linear_function + 2 * side[side.size() - 1].closest_linear_function;
				if (!is_limit_case)
					pl_side[paired_letter_index - 1].closest_linear_function += 1;
			}
			pl_side.erase(std::next(pl_side.begin(), paired_letter_index));
		}
		side.pop_back();
		return true;
	}

	//Use second method to eliminate crossing
	bool eliminate_turn(int index) noexcept {
		//If index is < 0, then the symbol is on the left hand side
		std::vector<letter>& side = index > 0 ? rhs : lhs;
		int ind = std::abs(index);
		total_increaser += side[ind - 1].closest_linear_function;
		++number_of_increases;

		//If two paired letters are not the last, then increase the linear function corresponding to the next letter
		if (ind > 1) 
			side[ind - 2].closest_linear_function += 
			/*2 **/ side[ind - 1].closest_linear_function + side[ind].closest_linear_function;
		
		side.erase(std::next(side.begin(), ind - 1), std::next(side.begin(), ind + 1));
		return true;
	}

	//Return vector with all indices where we can apply the second method. 
	//The index is greater than zero for the right hand side element and less than zero otherwise
	//Notice: the index can not be zero.
	std::vector<int> get_all_possible_turns() const noexcept {
		std::vector<int> indecies;
		for (int i = lhs.size() - 1; i > 0; --i) {
			if (lhs[i].alpha_number == lhs[i - 1].alpha_number)
				indecies.push_back(-i);
		}
		for (int i = rhs.size() - 1; i > 0; --i) {
			if (rhs[i].alpha_number == rhs[i - 1].alpha_number)
				indecies.push_back(i);
		}
		return indecies;
	}

	friend std::ostream& operator<<(std::ostream& stream_out, const pre_ACD& str) {
		for (size_t i = 0; i < str.lhs.size(); ++i)
			stream_out << str.lhs[i].alpha_number << " ";
		stream_out << "| ";
		for (int i = str.rhs.size() - 1; i > -1; --i)
			stream_out << str.rhs[i].alpha_number << " ";
		return stream_out;
	}
};