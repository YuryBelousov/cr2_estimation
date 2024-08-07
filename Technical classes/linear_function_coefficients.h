#pragma once
#include <unordered_map>
#include <ostream>

/*
 This class represents multivariable linear functions with integer coefficients.
 A number of basic operations are defined: arithmetic and comparison operations.
 Note: a fixed-length array is used for storage, so make sure that the number of variables is not too large.
*/

constexpr int n_max = 10;

class linear_function {
private:
	int variable_and_coef[3 + 2 * n_max];
	int actual_num_of_variables;

public:
	linear_function() = default;
	linear_function(const linear_function&) = default;
	linear_function(linear_function&&) = default;

	linear_function& operator=(const linear_function&) = default;
	linear_function& operator=(linear_function&&) = default;

	linear_function(int variable, int value, int _num_of_v) noexcept : actual_num_of_variables(_num_of_v) {
		memset(variable_and_coef, 0, sizeof(int) * _num_of_v);
		variable_and_coef[variable] = value;
	}

	int get_coef_under_variable(int n) const noexcept {
		return variable_and_coef[n];
	}

	int get_number_of_variables() const noexcept {
		return actual_num_of_variables;
	}

	linear_function& operator*=(int mult) noexcept {
		for (int i = 0; i < actual_num_of_variables; ++i)
			variable_and_coef[i] *= mult;
		return *this;
	}

	linear_function& operator+=(const linear_function& p) noexcept {
		for (int i = 0; i < actual_num_of_variables; ++i) 
			variable_and_coef[i] += p.variable_and_coef[i];
		return *this;
	}

	linear_function& operator+=(int coef) noexcept {
		variable_and_coef[0];
		variable_and_coef[0] += coef;
		return *this;
	}

	bool operator==(const linear_function& _p) const noexcept {
		for (int i = 0; i < actual_num_of_variables; ++i) 
			if (variable_and_coef[i] != _p.variable_and_coef[i])
				return false;
		return true;
	}

	bool operator>=(const linear_function& p) const noexcept {
		for (int i = 0; i < actual_num_of_variables; ++i) {
			if (variable_and_coef[i] < p.variable_and_coef[i])
				return false;
			if (variable_and_coef[i] > p.variable_and_coef[i])
				return true;
		}
		return true;
	}

	bool operator<=(const linear_function& p) const noexcept {
		return p >= *this;
	}

	bool operator<(const linear_function& _p) const noexcept {
		for (int i = 0; i < actual_num_of_variables - 1; ++i) {
			if (variable_and_coef[i] < _p.variable_and_coef[i])
				return true;
			if (variable_and_coef[i] > _p.variable_and_coef[i])
				return false;
		}
		return variable_and_coef[actual_num_of_variables - 1] < _p.variable_and_coef[actual_num_of_variables - 1];
	}

	bool operator>(const linear_function& _p) const noexcept {
		return _p.operator<(*this);
	}

	friend linear_function operator*(int coef, linear_function p) noexcept {
		return p *= coef;
	}

	friend linear_function operator+(linear_function p, const linear_function& q) noexcept {
		return p += q;
	}

	friend linear_function operator+(linear_function p, int coef) noexcept {
		return p += coef;
	}

	friend std::ostream& operator<<(std::ostream& stream_out, const linear_function& _p) {
		bool first_thing = true;
		for (int i = 0; i < _p.actual_num_of_variables; ++i) {
			if (_p.variable_and_coef[i] == 0)
				continue;
			if (_p.variable_and_coef[i] > 0) 
				if (!first_thing)
					stream_out << "+";
			if (_p.variable_and_coef[i] != 1 || i == 0)
				stream_out << _p.variable_and_coef[i];
			if (i != 0)
				stream_out << "x[" << i << "]";
			first_thing = false;
		}
		return stream_out;
	}
};