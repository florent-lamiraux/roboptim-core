// Copyright (C) 2013 by Benjamin Chretien, CNRS.
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

#ifndef ROBOPTIM_CORE_SOLVER_STATE_HXX
# define ROBOPTIM_CORE_SOLVER_STATE_HXX
# include <boost/foreach.hpp>
# include <roboptim/core/io.hh>

namespace roboptim
{
  template <typename P>
  SolverState<P>::SolverState (const problem_t& pb) throw ()
    : boost::noncopyable (),
      cost_ (),
      constraintViolation_ ()
  {
    x_.resize (pb.function ().inputSize ());
    x_.setZero ();
  }

  template <typename P>
  SolverState<P>::~SolverState () throw ()
  {
  }

  template <typename P>
  const typename SolverState<P>::vector_t&
  SolverState<P>::x () const throw ()
  {
    return x_;
  }

  template <typename P>
  typename SolverState<P>::vector_t&
  SolverState<P>::x () throw ()
  {
    return x_;
  }

  template <typename P>
  const boost::optional<typename SolverState<P>::value_type>&
  SolverState<P>::cost () const throw ()
  {
    return cost_;
  }

  template <typename P>
  boost::optional<typename SolverState<P>::value_type>&
  SolverState<P>::cost () throw ()
  {
    return cost_;
  }

  template <typename P>
  const boost::optional<typename SolverState<P>::value_type>&
  SolverState<P>::constraintViolation () const throw ()
  {
    return constraintViolation_;
  }

  template <typename P>
  boost::optional<typename SolverState<P>::value_type>&
  SolverState<P>::constraintViolation () throw ()
  {
    return constraintViolation_;
  }

  template <typename P>
  const typename SolverState<P>::parameters_t&
  SolverState<P>::parameters () const throw ()
  {
    return parameters_;
  }

  template <typename P>
  typename SolverState<P>::parameters_t&
  SolverState<P>::parameters () throw ()
  {
    return parameters_;
  }

  template <typename P>
  template <typename T>
  const T&
  SolverState<P>::getParameter (const std::string& key) const
    throw (std::out_of_range)
  {
    typename parameters_t::const_iterator it = parameters_.find (key);
    if(it == parameters_.end())
      throw std::out_of_range("key "+ key +" not found");
    return boost::get<T> (it->second.value);
  }

  template <typename P>
  template <typename T>
  T&
  SolverState<P>::getParameter (const std::string& key)
    throw (std::out_of_range)
  {
    typename parameters_t::iterator it = parameters_.find (key);
    if (it == parameters_.end ())
      throw std::out_of_range ("key "+ key +" not found");
    return boost::get<T> (it->second.value);
  }

  template <typename P>
  std::ostream&
  SolverState<P>::print (std::ostream& o) const throw ()
  {
    o << "Solver state:" << incindent;

    o << iendl << "x: " << x_;

    if (cost_)
      o << iendl << "Cost: " << *cost_;

    if (constraintViolation_)
      o << iendl << "Constraint violation: " << *constraintViolation_;

    if (!parameters_.empty ())
      {
        o << iendl << "Parameters:" << incindent;
        typedef const std::pair<const std::string, StateParameter<function_t> >
          const_iterator_t;
        BOOST_FOREACH (const_iterator_t& it, parameters_)
          o << iendl << it.first << " (" << it.second.description << ")"
            << ": " << it.second.value;
        o << decindent;
      }
    o << decindent;
    return o;
  }

  template <typename F>
  std::ostream&
  operator<< (std::ostream& o, const StateParameter<F>& parameter)
  {
    return parameter.print (o);
  }

  template <typename P>
  std::ostream&
  operator<< (std::ostream& o, const SolverState<P>& state)
  {
    return state.print (o);
  }
} // end of namespace roboptim
#endif //! ROBOPTIM_CORE_SOLVER_STATE_HXX
