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
 * \brief Implementation of the Solver class.
 */

#ifndef ROBOPTIM_CORE_SOLVER_HXX
# define ROBOPTIM_CORE_SOLVER_HXX

namespace roboptim
{
  template <typename F, typename C>
  Solver<F, C>::Solver (const problem_t& pb) throw ()
    : GenericSolver (),
      problem_ (pb)
  {
  }

  template <typename F, typename C>
  template <typename F_, typename C_>
  Solver<F, C>::Solver (const Problem<F_, C_>& pb) throw ()
    : GenericSolver (),
      problem_ (pb)
  {
  }

  template <typename F, typename C>
  Solver<F, C>::~Solver () throw ()
  {
  }

  template <typename F, typename C>
  const typename Solver<F, C>::problem_t&
  Solver<F, C>::problem () const throw ()
  {
    return problem_;
  }

  template <typename F, typename C>
  std::ostream&
  Solver<F, C>::print (std::ostream& o) const throw ()
  {
    return o << problem_;
  }
}; // end of namespace roboptim

#endif //! ROBOPTIM_CORE_SOLVER_HH
