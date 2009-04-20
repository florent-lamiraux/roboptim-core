// Copyright (C) 2009 by Thomas Moulard, AIST, CNRS, INRIA.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

/**
 * \brief Declaration of the NumericLinearFunction class.
 */

#ifndef ROBOPTIM_CORE_NUMERIC_LINEAR_FUNCTION_HH
# define ROBOPTIM_CORE_NUMERIC_LINEAR_FUNCTION_HH

# include <roboptim-core/linear-function.hh>

namespace roboptim
{
  /// Define a quadratic function.
  class NumericLinearFunction : public LinearFunction
  {
  public:
    NumericLinearFunction (const matrix_t&, const vector_t&) throw ();

    virtual vector_t operator () (const vector_t&) const throw ();

    virtual gradient_t gradient (const vector_t&, int) const throw ();
    virtual jacobian_t jacobian (const vector_t&) const throw ();

    virtual std::ostream& print (std::ostream&) const throw ();

  private:
    matrix_t a_;
    vector_t b_;
  };

} // end of namespace roboptim

#endif //! ROBOPTIM_CORE_QUADRATIC_FUNCTION_HH
