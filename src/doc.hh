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

   \mainpage User manual

   \section intro Introduction

   RobOptim Core defines a C++ common interface to several non linear problem
   solvers.

   The package is composed of a base class called
   optimization::GenericSolver and several sub-classes which transform
   the problem in order to pass it to an external solver.


   The interface relies heavily on Boost for:
   <ul>
   <li>advanced typing (Boost.Optional and Boost.Variant),</li>
   <li>and meta-programming
   (Boost.StaticAssert, Boost.TypeTraits, Boost.MPL).</li>
   </ul>

   Matrix manipulation is handled by default by the Eigen matrix.

   New users might want to check out the idea behind Boost.Optional
   and Boost.Variant are these types are present in the public
   interface.

   Users do not have to know about meta-programming to use this library.


   To get basic knowledge about the library, you might want to check out the
   \ref quickstart page.


   Table of content:
   - \subpage quickstart
   - \subpage faq
   - \subpage design
   - \subpage bridge


   \section reporting Reporting bugs

   As this package is still in its early development steps, bugs report
   and enhancement proposals are highly welcomed.

   To report a bug, please go this package's
   <a href="http://www.roboptim.net/">web page</a>.

   To discuss about this package, please use <a
   href="https://groups.google.com/forum/?fromgroups#!forum/roboptim"
   >roboptim@googlegroups.com</a>.
*/


/**
   \page quickstart Quick start

   Solving a problem is done through several steps:

   - Define your cost function by deriving one kind of function, depending
   on whether or not you want to provide a jacobian and/or a hessian.
   - Define your constraints functions in the same manner.
   - Build an instance of problem matching your requirements.
   - Use one of the solvers to solve your problem.


   The following example defines a cost function F and two constraints
   G0 and G1.

   \section problem Problem definition

   The problem that will be solved in this tutorial is the 71th
   problem of Hock-Schittkowski:

   \f$min_{x \in \mathbb{R}^4} x_1 x_4 (x_1 + x_2 + x_3) + x_3\f$

   with the following constraints:

   - \f$x_1 x_2 x_3 x_4 \geq 25\f$
   - \f$x_1^2 + x_2^2 + x_3^2 + x_4^2 = 40\f$
   - \f$1 \leq x_1, x_2, x_3, x_4 \leq 5\f$

   \section cost Defining the cost function.

   The library contains the following hierarchy of functions:
   - #roboptim::Function
   - #roboptim::DerivableFunction
   - #roboptim::TwiceDerivableFunction
   - #roboptim::QuadraticFunction
   - #roboptim::LinearFunction

   These types correspond to dense vectors and matrices relying on
   Eigen. RobOptim also support dense matrices and you can even extend
   the framework to support your own types.

   When defining a new function, you have to derive your new function from one
   of those classes. Depending on the class you derive from, you will be have
   to implement one or several methods:

   - impl_compute that returns the function's result has to be defined for all
   functions.
   - impl_gradient which returns the function's gradient is to be
   defined for DerivableFunction and its subclasses.
   - impl_hessian for TwiceDerivableFunction functions and its subclasses.

   It is usually recommended to derive from the deepest possible class of the
   hierarchy (deriving from TwiceDerivableFunction is better than
   DerivableFunction).

   Keep in mind that the type of the function represents the amount of
   information the solver will get, not the real nature of a function (it is
   possible to avoid defining a hessian by deriving from DerivableFunction,
   even if you function can be derived twice).

   In the following sample, a TwiceDifferentiableFunction will be defined.

   \code
struct F : public TwiceDifferentiableFunction
{
  F () : TwiceDifferentiableFunction (4, 1, "x₀ * x₃ * (x₀ + x₁ + x₂) + x₃")
  {
  }

  void
  impl_compute (result_t& result, const argument_t& x) const throw ()
  {
    result (0) = x[0] * x[3] * (x[0] + x[1] + x[2]) + x[3];
  }

  void
  impl_gradient (gradient_t& grad, const argument_t& x, size_type) const throw ()
  {
    grad[0] = x[0] * x[3] + x[3] * (x[0] + x[1] + x[2]);
    grad[1] = x[0] * x[3];
    grad[2] = x[0] * x[3] + 1;
    grad[3] = x[0] * (x[0] + x[1] + x[2]);
  }

  void
  impl_hessian (hessian_t& h, const argument_t& x, size_type) const throw ()
  {
    h (0, 0) = 2 * x[3];
    h (0, 1) = x[3];
    h (0, 2) = x[3];
    h (0, 3) = 2 * x[0] + x[1] + x[2];

    h (1, 0) = x[3];
    h (1, 1) = 0.;
    h (1, 2) = 0.;
    h (1, 3) = x[0];

    h (2, 0) = x[3];
    h (2, 1) = 0.;
    h (2, 2) = 0.;
    h (2, 3) = x[1];

    h (3, 0) = 2 * x[0] + x[1] + x[2];
    h (3, 1) = x[0];
    h (3, 2) = x[0];
    h (3, 3) = 0.;
  }
};
   \endcode

   \section constraints Defining the constraints.

   A constraint is no different from a cost function and
   can be defined in the same way than a cost function.

   The following sample defines two constraints which are
   twice derivable functions.


   \code
struct G0 : public TwiceDifferentiableFunction
{
  G0 () : TwiceDifferentiableFunction (4, 1, "x₀ * x₁ * x₂ * x₃")
  {
  }

  void
  impl_compute (result_t& result, const argument_t& x) const throw ()
  {
    result (0) = x[0] * x[1] * x[2] * x[3];
  }

  void
  impl_gradient (gradient_t& grad, const argument_t& x, size_type) const throw ()
  {
    grad[0] = x[1] * x[2] * x[3];
    grad[1] = x[0] * x[2] * x[3];
    grad[2] = x[0] * x[1] * x[3];
    grad[3] = x[0] * x[1] * x[2];
  }

  void
  impl_hessian (hessian_t& h, const argument_t& x, size_type) const throw ()
  {
    h (0, 0) = 0.;
    h (0, 1) = x[2] * x[3];
    h (0, 2) = x[1] * x[3];
    h (0, 3) = x[1] * x[2];

    h (1, 0) = x[2] * x[3];
    h (1, 1) = 0.;
    h (1, 2) = x[0] * x[3];
    h (1, 3) = x[0] * x[2];

    h (2, 0) = x[1] * x[3];
    h (2, 1) = x[0] * x[3];
    h (2, 2) = 0.;
    h (2, 3) = x[0] * x[1];

    h (3, 0) = x[1] * x[2];
    h (3, 1) = x[0] * x[2];
    h (3, 2) = x[0] * x[1];
    h (3, 3) = 0.;
  }
};

struct G1 : public TwiceDifferentiableFunction
{
  G1 () : TwiceDifferentiableFunction (4, 1, "x₀² + x₁² + x₂² + x₃²")
  {
  }

  void
  impl_compute (result_t& result, const argument_t& x) const throw ()
  {
    result (0) = x[0]*x[0] + x[1]*x[1] + x[2]*x[2] + x[3]*x[3];
  }

  void
  impl_gradient (gradient_t& grad, const argument_t& x, size_type) const throw ()
  {
    grad[0] = 2 * x[0];
    grad[1] = 2 * x[1];
    grad[2] = 2 * x[2];
    grad[3] = 2 * x[3];
  }

  void
  impl_hessian (hessian_t& h, const argument_t& x, size_type) const throw ()
  {
    h (0, 0) = 2.;
    h (0, 1) = 0.;
    h (0, 2) = 0.;
    h (0, 3) = 0.;

    h (1, 0) = 0.;
    h (1, 1) = 2.;
    h (1, 2) = 0.;
    h (1, 3) = 0.;

    h (2, 0) = 0.;
    h (2, 1) = 0.;
    h (2, 2) = 2.;
    h (2, 3) = 0.;

    h (3, 0) = 0.;
    h (3, 1) = 0.;
    h (3, 2) = 0.;
    h (3, 3) = 2.;
  }
};
   \endcode

   \section problem Building the problem and solving it.

   The last part of this tutorial covers how to build a problem and
   solve it. The steps are:

   - Instanciate your functions (cost functions and constraints).
   - Pass them to the problem.
   - Optional: set a starting point.
   - Instanciate a solver which solves your class of problem.
   - Solve the problem by calling minimum.

   \code
int run_test ()
{
  // Create cost function.
  F f;

  // Create problem.
  solver_t::problem_t pb (f);

  // Set bounds for all optimization parameters.
  // 1. < x_i < 5. (x_i in [1.;5.])
  for (Function::size_type i = 0; i < pb.function ().inputSize (); ++i)
    pb.argumentBounds ()[i] = Function::makeInterval (1., 5.);

  // Set the starting point.
  Function::vector_t start (pb.function ().inputSize ());
  start[0] = 1., start[1] = 5., start[2] = 5., start[3] = 1.;

  // Create constraints.
  boost::shared_ptr<G0> g0 (new G0 ());
  boost::shared_ptr<G1> g1 (new G1 ());

  F::intervals_t bounds;
  solver_t::problem_t::scales_t scales;

  // Add constraints
  bounds.push_back(Function::makeLowerInterval (25.));
  scales.push_back (1.);
  pb.addConstraint
    (boost::static_pointer_cast<TwiceDifferentiableFunction> (g0),
     bounds, scales);

  bounds.clear ();
  scales.clear ();

  bounds.push_back(Function::makeInterval (40., 40.));
  scales.push_back (1.);
  pb.addConstraint
    (boost::static_pointer_cast<TwiceDifferentiableFunction> (g1),
     bounds, scales);

  // Initialize solver.

  // Here we are relying on a dummy solver.
  // You may change this string to load the solver you wish to use:
  //  - Ipopt: "ipopt", "ipopt-sparse", "ipopt-td"
  //  - Eigen: "eigen-levenberg-marquardt"
  //  etc.
  // The plugin is built for a given solver type, so chose it adequately.
  SolverFactory<solver_t> factory ("dummy-td", pb);
  solver_t& solver = factory ();

  // Compute the minimum and retrieve the result.
  solver_t::result_t res = solver.minimum ();

  // Display solver information.
  std::cout << solver << std::endl;

  // Check if the minimization has succeeded.

  // Process the result
  switch (res.which ())
    {
    case solver_t::SOLVER_VALUE:
      {
        // Get the result.
        Result& result = boost::get<Result> (res);

        // Display the result.
        std::cout << "A solution has been found: " << std::endl
                  << result << std::endl;

        return 0;
      }

    case solver_t::SOLVER_VALUE_WARNINGS:
      {
        // Get the result.
        ResultWithWarnings& result = boost::get<ResultWithWarnings> (res);

        // Display the result.
        std::cout << "A solution has been found: " << std::endl
                  << result << std::endl;

        return 0;
      }

    case solver_t::SOLVER_NO_SOLUTION:
    case solver_t::SOLVER_ERROR:
      {
        std::cout << "A solution should have been found. Failing..."
                  << std::endl
                  << boost::get<SolverError> (res).what ()
                  << std::endl;

        return 2;
      }
    }

  // Should never happen.
  assert (0);
  return 42;
}
   \endcode

   This is the last piece of code needed to instantiate and resolve
   an optimization problem with this package.

   To see more usage examples, consider looking at the test directory of the
   project which contains the project's test suite.
*/

/**
   \page faq Frequently Asked Questions (FAQ)

   \section avl_solvers What solvers are currently available?

   Supported solvers are available on seperated packages, please check
   <a href="http://www.roboptim.net/">the project webpage</a> of the
   supported solvers list.

   \section boost_optvar What are those Optional and Variant types?

   An optional type is a templated class which modelize the possibility
   in C++ to have "no value" in a variable.
   It is safer than passing a null pointer as an exception will be
   launched if you try to dereference a variable which has no content.

   See the following example:
   \code
   // Declare an empty optional int.
   boost::optional<int> optInt;

   // There is nothing in our optional int.
   // Check if this is really the case.
   if (optInt)
      assert (false);

   // Set a value in our optional int.
   optInt = 42;

   // Access our optional int.
   std::cout << *optInt << std::endl;
   \endcode


   A variant is an object-oriented way to define a C enum.
   Here is a small example:

   \code
   // x can be a string or an int.
   boost::variant<int, std::string> x;

   // Set x to 42.
   x = 42;

   // Which allows to determine what is the current type of x
   // First type (int) is zero, second (string) is one, etc.
   assert (x.which () == 0);

   // Set x to "hello world".
   x = "hello world";

   // Test again.
   assert (x.which () == 1);
   \endcode

   Please note that a variant can never be empty.


   See the <a href="http://www.boost.org/">Boost.Optional and Boost.Variant
   documentation</a> for more information.
*/


/**
   \page design Design choices and libraries' internals

   This package is divided in three parts:
   <ul>
   <li>Functions which represent different kind of mathematical functions and
   theirs associated features (gradient, hessian...).</li>
   <li>A problem class defines a whole optimization problem including some
   technical details (scales).</li>
   <li>Solvers hierarchy defines solvers that are working on a class of
   optimizations problem (for instance QP solves problems where the
   objective function is quadratic and the constraints are linear).</li>
   </ul>

   \section fct The function hierarchy

   The function hierarchy is an abstract hierarchy which defines
   <b>meta-function</b> in the sense that the user has to derive from
   these classes to define its <b>real</b> function.

   \code
   struct MyLinearFunction : public LinearFunction {
   //FIXME: some code is missing
   };

   struct MyDerivableFunction : public DerivableFunction {
   //FIXME: some code is missing
   };
   // etc...
   \endcode

   The user can defines as many functions as he wants as long as he respects
   the constraints defined by the classes he inherits from:
   <ul>
   <li>Any function must be able to be evaluated (through operator()).</li>
   <li>A derivable function has a gradient/jacobian.</li>
   <li>A twice derivable function (\f$C^2\f$) has a hessian.</li>
   <li>A quadratic function has the sames constraints than a \f$C^2\f$
   function.</li>
   <li>A linear function has the same constraints except its hessian which
   is null and computed automatically by the abstract class.</li>
   </ul>

   It is important to note that all these functions are
   \f$\mathbb{R}^n \rightarrow \mathbb{R}^m\f$ functions.

   These functions have to be considered as m functions from to
   \f$\mathbb{R}^n \rightarrow \mathbb{R}\f$.
   In particular, it means that in the gradient/hessian methods, the
   second argument (integer) refers to which function you want to get
   the gradient/hessian.

   It avoids useless computations when the whole Jacobian is not needed.
   Hence, it also avoid the use of a tensor as this structure is particularly
   costly and not built-in in Eigen (the matrix library the package rely on).


   This set of meta-functions is completed by two generic implementations
   of quadratic and linear functions in respectively NumericQuadraticFunction
   and NumericLinearFunction.
   Those are real functions as the user is meant to instantiate them, not
   derive from them.


   \section pb The problem class

   The problem class defines a complete optimization problem.
   It is composed of:
   <ul>
   <li>A \f$\mathbb{R}^n \rightarrow \mathbb{R}\f$ cost
   (or objective) function.</li>
   <li>A set of functions constraints.</li>
   <li>Bounds on the input arguments.</li>
   <li>Bounds on the constraints functions output.</li>
   <li>Scales on the input arguments.</li>
   <li>Scales on the constraints functions output.</li>
   </ul>

   The problem class is templated by two parameters:
   <ul>
   <li>The objective's function type (F).</li>
   <li>The set of constraint's types (C).</li>
   </ul>

   Those two parameters defines a class of problem.
   For instance, QP solves problems where the objective function
   is quadratic and the constraints are linear.

   To a class of problem matches a set of solvers designed to
   exactly those problems.

   The copy constructor of the Problem class allows to make
   the problem more generic by replacing F or C by a more
   general type.

   Examples:
   <ul>
   <li>It is valid to convert a linear problem into a non-linear
   problem and make a QP solve it.</li>
   <li>But, the opposite is not valid.</li>
   </ul>

   Ie: you can always go higher in the function hierarchy
   but you can not go deeper.

   However, this transformation as any dynamic typing makes
   <b>forget</b> specific information and might slow-down the
   process. Ie: solving a linear problem as a non-linear one
   is slower than directly solving it.


   \section slv The solver hierarchy


   The solver hierarchy is divided in three parts:
   <ul>
   <li>The generic solver.</li>
   <li>The solver for a class of problems.</li>
   <li>The bridges (the leafs in the hierarchy's tree).</li>
   </ul>

   \subsection gs Generic solver

   The generic solver is an abstract class which is the root of the
   hierarchy. All the solvers have to inherit from this class directly or
   indirectly.

   It only defines three methods:
   <ul>
   <li><code>solve</code> (virtual pure)</li>
   <li><code>getMinimum</code></li>
   <li><code>reset</code></li>
   </ul>

   The method <code>solve</code> has to fill the protected attribute
   <code>result_</code>. It is implemented by the bridges at the bottom of the
   hierarchy.

   By default <code>result_</code> contains an instance the
   <code>No_Solution</code> class.  It has to be changed to an instance of
   <code>Result</code>, <code>ResultWithWarnings</code> or
   <code>SolverError</code>.

   <ul>
   <li><code>No_Solution</code>: represents the fact that solve has never been
   called. A user should never retrieve an instance of <code>No_Solution</code>.
   </li>
   <li><code>Result</code>, <code>ResultWithWarnings</code>: represents a
   <strong>valid</strong> result.  If <code>ResultWithWarnings</code> is used,
   some non-critical problem might have happened and are specified in the object
   (minor numerical instabilities for instance).</li>
   <li><code>SolverError</code>: indicate that the optimization has
   <strong>failed</strong>.</li>
   </ul>

   The <code>solve</code> method should rarely be called by the user.
   Instead, the user calls <code>getMinimum</code> which calls
   <code>solve</code> if required.
   If <code>getMinimum</code> is called several time, the problem is only
   solved once.

   The <code>reset</code> method should be called when one wants to manually
   force the recomputation of the solution.


   \subsection solver Solver

   The <code>Solver</code> class is parametrized by two parameters as the
   <code>Problem</code> class.
   It defines an abstract solver for a class of optimization problems.

   For instance, a QP should inherit from
   <code>Solver<QuadraticFunction, LinearFunction></code>.

   This level of the hierarchy integrates the notion of <code>Problem</code>,
   hence the <code>problem</code> method is defined to access the problem. Once
   a solver is created with a problem, the problem can never be modified again.

   \subsection briges Bridges

   The leafs of the hierarchy are bridges.  The role of a bridge is to convert
   the generic representation of the problem into the solver's representation.
   It might also emulate some missing features of the back-end solver.

   A bridge mainly implements the <code>solve</code> method. It may also
   exposes the underlying solver's internal mechanism for fine tuning.
*/


/**
   \page bridge How to link the library with a new solver?


   This part assumes the reader went through the \ref design page and is
   familiar with the user interface.

   \section tutorial Intregrating a solver step by step.

   \subsection prereq Pre-requesite

   The first step is determining if there is any interest in integrating
   the solver.

   The solver should target one of the following optimization problem's:
   <ul>
   <li>Non-linear optimization</li>
   <li>Linear optimization</li>
   </ul>
   ...with or without constraints.

   It should also support bounds and scales. If not, emulating them
   should be possible.

   Integrating solvers that solve very different problems will likely
   not match the interface and lead to major changement in the package.
   These changes are beyond the scope of this document and should be
   discussed on the mailing-list.


   \subsection pkg Modifying the package.

   Basically, adding a new solver means adding a new leaf in the solver
   hierarchy.

   The new header and source files should be added in the <code>src</code>
   directory.

   The naming convention is CamlCase, the name should be the solver's name
   suffixed by <code>Solver</code>.

   This plug-in must be provided as a separate package to avoid
   cluttering RobOptim Core with useless dependencies.

   \subsection impl Implementing the bridge.

   The first step is choosing from which <code>Solver</code> you want to
   inherit from. The <code>F</code> parameters should be straight-forward: it
   is the kind of objective function you expect.
   For the constraints, there is two cases:
   <ul>
   <li>Constraint's type are unique: set <code>C</code> to
   <code>const MyConstraintType*</code> where <code>MyConstraintType</code>
   is the solver's constraints type.</li>
   <li>If you want to discriminate your constriants into different categories,
   you will have to use a Boost.Variant. <code>C</code> should be:
   <code>boost::variant<const MyConstraintType1*, const MyConstraintType2*>
   </code>
   </ul>

   Then, the <code>solve</code> method should be implemented in the bridge.


   Following guidelines should be respected by any bridge:
   <ul>
   <li>Be sure to call <code>reset</code> if the result has to be recomputed
   the next time <code>getMinimum</code> is called.</li>
   <li>Be sure that <code>solve</code> put the computed value in the
   <code>result_</code> attribute.</li>
   <li>Solver's arguments should be exposed to user through the class.</li>
   <li>The solver should not display anything by default. If possible, use
   the <code>print</code> method to display internal state.</li>
   </ul>


   \subsection ccl Conclusion

   The dummy solver can be a good starting point for anyone who wants to add
   a new solver as the code can be considered as the minimum stub to get a
   compilable bridge. It should also be quite easy to understand.

   If you write a new bridge, please post an annoucement to the RobOptim
   Google group.
*/


/// \namespace roboptim
/// \brief Meta-functions, functions and solvers related classes.


/// \namespace roboptim::visualization
/// \brief Graphic visualization
///
/// Visualization related code. Only Gnuplot is supported currently.


/// \namespace roboptim::visualization::gnuplot
/// \brief Gnuplot rendering
///
/// Gnuplot display classes.


/// \defgroup roboptim_meta_function Mathematical abstract functions
/// \defgroup roboptim_function Mathematical functions
/// \defgroup roboptim_filter Function filters
/// \defgroup roboptim_problem Optimization problems
/// \defgroup roboptim_solver Optimization solvers
/// \defgroup roboptim_visualization Visualization
