#pragma once
#include <vector>

#include "Technical classes/chord_diagram_base_class.h"

/*
 This class represents annotated chord diagrams (ACD).
 It contains implementations of virtual functions from the base class, as well as a function for output.
*/

class ACD : public chord_diagram_base {
private:
	std::vector<int> chords;
	std::vector<linear_function> weights;

public:
	ACD() = default;
	ACD(const ACD&) = default;
	ACD(ACD&&) = default;

	ACD& operator=(const ACD&) = default;
	ACD& operator=(ACD&&) = default;

	ACD(const std::vector<int>& _chords) noexcept : chord_diagram_base(_chords.size() + 2){
		int counter = 0;
		for (const auto& elt : _chords) {
			chords.insert(chords.end(), elt);
			weights.insert(weights.end(), linear_function(++counter, 1, _chords.size() + 2));
		}
		weights.insert(weights.end(), linear_function(++counter, 1, _chords.size() + 2));
	}

	//Return vector with all indices where we can apply Transformation II. 
	std::vector<int> get_all_possible_turns() const noexcept override {
		std::vector<int> results;
		for (auto iter = std::next(chords.cbegin()); iter != chords.cend(); ++iter)
			if (*iter == *std::next(iter, -1))
				results.push_back(*iter);
		return results;
	}

	//Use Transformation I to eliminate the chord closest to the basepoint.
	//If ACD is not empty, the chord can be eliminated on either the left or right side.
	// is_limit_case indicates whether we consider the limit case, i.e. when m->inf (and thus adding one more crossing is insignificant)
	bool eliminate_from_side(bool is_right, bool is_limit_case) noexcept override {
		if (chords.empty())
			return false;

		++number_of_increases;
		linear_function p;
		int chord_name = -1;
		if (!is_right) {
			p = *weights.begin();
			chord_name = *chords.begin();

			weights.erase(weights.begin());
			chords.erase(chords.begin());						
			weights.begin()->operator+=(p);

			//Find another chord endpoint.
			auto iter_weights = weights.begin();
			auto iter_chords = chords.begin();
			for (; *iter_chords != chord_name; ++iter_chords, ++iter_weights);
			iter_weights->operator+=(2 * p + *std::next(iter_weights));
			if (!is_limit_case)
				iter_weights->operator+=(1);
			weights.erase(std::next(iter_weights));
			chords.erase(iter_chords);
		}
		else {
			p = *weights.rbegin();
			chord_name = *chords.rbegin();

			weights.pop_back();
			chords.pop_back();
			weights.rbegin()->operator+=(p);

			//Find the another chord endpoint.
			auto iter_weights = weights.rbegin();
			auto iter_chords = chords.rbegin();
			for (; *iter_chords != chord_name; ++iter_chords, ++iter_weights);
			iter_weights->operator+=(2 * p + *std::next(iter_weights));
			if (!is_limit_case)
				iter_weights->operator+=(1);
			weights.erase(std::next(iter_weights, 2).base());
			chords.erase(std::next(iter_chords).base());
		}
		total_increaser += 2 * p;
		if (!is_limit_case)
			total_increaser += 1;
		return true;
	}

	//Use Transformation II to eliminate chord. 
	bool eliminate_turn(int chord_name) noexcept override {
		++number_of_increases;
		auto iter_chords = chords.begin();
		auto iter_weights = weights.begin();
		for (; *iter_chords != chord_name; ++iter_chords, ++iter_weights) {
			if (iter_chords == chords.end()) // If Transformation II cannot be applied, return false.
				return false;
		}
		total_increaser += *std::next(iter_weights);
		iter_weights->operator+=(*std::next(iter_weights) + *std::next(iter_weights, 2));

		weights.erase(std::next(iter_weights), std::next(iter_weights, 3));
		chords.erase(iter_chords, std::next(iter_chords, 2));
		return true;
	}

	friend std::ostream& operator<<(std::ostream& stream_out, const ACD& _old) {
		auto iter_weights = _old.weights.cbegin();
		auto iter_chords = _old.chords.cbegin();

		for (; iter_chords != _old.chords.cend(); ++iter_chords, ++iter_weights)
			stream_out << *iter_chords << " ";
		return stream_out;
	}
};