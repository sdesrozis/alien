/*
 * Copyright 2020 IFPEN-CEA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hypre_matrix.h"
#include "hypre_vector.h"

#include <boost/timer.hpp>

#include <HYPRE_parcsr_ls.h>
#include <HYPRE_parcsr_mv.h>

#include <arccore/message_passing_mpi/MpiMessagePassingMng.h>

#include <alien/core/backend/LinearSolverT.h>
#include <alien/expression/solver/SolverStater.h>

#include <alien/hypre/backend.h>
#include <alien/hypre/export.h>
#include <alien/hypre/options.h>

namespace Alien {
// Compile HypreLinearSolver.
template class ALIEN_HYPRE_EXPORT LinearSolver<BackEnd::tag::hypre>;

} // namespace Alien

namespace Alien::Hypre {
class InternalLinearSolver : public IInternalLinearSolver<Matrix, Vector>,
                             public ObjectWithTrace
{
 public:
  typedef SolverStatus Status;

  InternalLinearSolver();

  InternalLinearSolver(const Options& options);

  virtual ~InternalLinearSolver() {}

 public:
  // Nothing to do
  void updateParallelMng(Arccore::MessagePassing::IMessagePassingMng* pm) {}

  bool solve(const Matrix& A, const Vector& b, Vector& x);

  bool hasParallelSupport() const { return true; }

  //! Etat du solveur
  const Status& getStatus() const;

  const SolverStat& getSolverStat() const { return m_stat; }

  std::shared_ptr<ILinearAlgebra> algebra() const;

 private:
  Status m_status;

  Arccore::Real m_init_time;
  Arccore::Real m_total_solve_time;
  Arccore::Integer m_solve_num;
  Arccore::Integer m_total_iter_num;

  SolverStat m_stat;
  Options m_options;

 private:
  void checkError(const Arccore::String& msg, int ierr, int skipError = 0) const;
};

InternalLinearSolver::InternalLinearSolver()
{
  boost::timer tinit;
  m_init_time += tinit.elapsed();
}

InternalLinearSolver::InternalLinearSolver(const Options& options)
: m_options(options)
{
  boost::timer tinit;
  m_init_time += tinit.elapsed();
}

void
InternalLinearSolver::checkError(
    const Arccore::String& msg, int ierr, int skipError) const
{
  if (ierr != 0 and (ierr & ~skipError) != 0) {
    char hypre_error_msg[256];
    HYPRE_DescribeError(ierr, hypre_error_msg);
    alien_fatal([&] {
      cout() << msg << " failed : " << hypre_error_msg << "[code=" << ierr << "]";
    });
  }
}

bool
InternalLinearSolver::solve(const Matrix& A, const Vector& b, Vector& x)
{
  auto ij_matrix = A.internal();
  auto bij_vector = b.internal();
  auto xij_vector = x.internal();

  // Clear all Hypre error for this session
  HYPRE_ClearAllErrors();

  // Macro "pratique" en attendant de trouver mieux
  boost::timer tsolve;

  int output_level = m_options.verbose() ? 1 : 0;

  HYPRE_Solver solver = nullptr;
  HYPRE_Solver preconditioner = nullptr;

  // acces aux fonctions du preconditionneur
  HYPRE_PtrToParSolverFcn precond_solve_function = nullptr;
  HYPRE_PtrToParSolverFcn precond_setup_function = nullptr;
  int (*precond_destroy_function)(HYPRE_Solver) = nullptr;

  MPI_Comm comm = MPI_COMM_WORLD;
  auto* mpi_comm_mng = dynamic_cast<Arccore::MessagePassing::Mpi::MpiMessagePassingMng*>(
      A.distribution().parallelMng());
  if (mpi_comm_mng)
    comm = *(mpi_comm_mng->getMPIComm());

  std::string precond_name = "undefined";
  switch (m_options.preconditioner()) {
  case OptionTypes::NoPC:
    precond_name = "none";
    // precond_destroy_function = nullptr;
    break;
  case OptionTypes::DiagPC:
    precond_name = "diag";
    // checkError("Hypre diagonal preconditioner",HYPRE_BoomerAMGCreate(&preconditioner));
    precond_solve_function = HYPRE_ParCSRDiagScale;
    precond_setup_function = HYPRE_ParCSRDiagScaleSetup;
    break;
  case OptionTypes::AMGPC:
    precond_name = "amg";
    checkError("Hypre AMG preconditioner", HYPRE_BoomerAMGCreate(&preconditioner));
    precond_solve_function = HYPRE_BoomerAMGSolve;
    precond_setup_function = HYPRE_BoomerAMGSetup;
    precond_destroy_function = HYPRE_BoomerAMGDestroy;
    break;
  case OptionTypes::ParaSailsPC:
    precond_name = "parasails";
    checkError(
        "Hypre ParaSails preconditioner", HYPRE_ParaSailsCreate(comm, &preconditioner));
    precond_solve_function = HYPRE_ParaSailsSolve;
    precond_setup_function = HYPRE_ParaSailsSetup;
    precond_destroy_function = HYPRE_ParaSailsDestroy;
    break;
  case OptionTypes::EuclidPC:
    precond_name = "euclid";
    checkError("Hypre Euclid preconditioner", HYPRE_EuclidCreate(comm, &preconditioner));
    precond_solve_function = HYPRE_EuclidSolve;
    precond_setup_function = HYPRE_EuclidSetup;
    precond_destroy_function = HYPRE_EuclidDestroy;
    break;
  default:
    alien_fatal([&] { cout() << "Undefined Hypre preconditioner option"; });
    break;
  }

  // acces aux fonctions du solveur
  // int (*solver_set_logging_function)(HYPRE_Solver,int) = nullptr;
  int (*solver_set_print_level_function)(HYPRE_Solver, int) = nullptr;
  int (*solver_set_tol_function)(HYPRE_Solver, double) = nullptr;
  int (*solver_set_precond_function)(HYPRE_Solver, HYPRE_PtrToParSolverFcn,
      HYPRE_PtrToParSolverFcn, HYPRE_Solver) = nullptr;
  int (*solver_setup_function)(
      HYPRE_Solver, HYPRE_ParCSRMatrix, HYPRE_ParVector, HYPRE_ParVector) = nullptr;
  int (*solver_solve_function)(
      HYPRE_Solver, HYPRE_ParCSRMatrix, HYPRE_ParVector, HYPRE_ParVector) = nullptr;
  int (*solver_get_num_iterations_function)(HYPRE_Solver, int*) = nullptr;
  int (*solver_get_final_relative_residual_function)(HYPRE_Solver, double*) = nullptr;
  int (*solver_destroy_function)(HYPRE_Solver) = nullptr;

  int max_it = m_options.numIterationsMax();
  double rtol = m_options.stopCriteriaValue();

  std::string solver_name = "undefined";
  switch (m_options.solver()) {
  case OptionTypes::AMG:
    solver_name = "amg";
    checkError("Hypre AMG solver", HYPRE_BoomerAMGCreate(&solver));
    if (output_level > 0)
      checkError("Hypre AMG SetDebugFlag", HYPRE_BoomerAMGSetDebugFlag(solver, 1));
    checkError("Hypre AMG solver SetMaxIter", HYPRE_BoomerAMGSetMaxIter(solver, max_it));
    // solver_set_logging_function = HYPRE_BoomerAMGSetLogging;
    solver_set_print_level_function = HYPRE_BoomerAMGSetPrintLevel;
    solver_set_tol_function = HYPRE_BoomerAMGSetTol;
    // solver_set_precond_function = nullptr;
    solver_setup_function = HYPRE_BoomerAMGSetup;
    solver_solve_function = HYPRE_BoomerAMGSolve;
    solver_get_num_iterations_function = HYPRE_BoomerAMGGetNumIterations;
    solver_get_final_relative_residual_function =
        HYPRE_BoomerAMGGetFinalRelativeResidualNorm;
    solver_destroy_function = HYPRE_BoomerAMGDestroy;
    break;
  case OptionTypes::GMRES:
    solver_name = "gmres";
    checkError("Hypre GMRES solver", HYPRE_ParCSRGMRESCreate(comm, &solver));
    checkError(
        "Hypre GMRES solver SetMaxIter", HYPRE_ParCSRGMRESSetMaxIter(solver, max_it));
    // solver_set_logging_function = HYPRE_ParCSRGMRESSetLogging;
    solver_set_print_level_function = HYPRE_ParCSRGMRESSetPrintLevel;
    solver_set_tol_function = HYPRE_ParCSRGMRESSetTol;
    solver_set_precond_function = HYPRE_ParCSRGMRESSetPrecond;
    solver_setup_function = HYPRE_ParCSRGMRESSetup;
    solver_solve_function = HYPRE_ParCSRGMRESSolve;
    solver_get_num_iterations_function = HYPRE_ParCSRGMRESGetNumIterations;
    solver_get_final_relative_residual_function =
        HYPRE_ParCSRGMRESGetFinalRelativeResidualNorm;
    solver_destroy_function = HYPRE_ParCSRGMRESDestroy;
    break;
  case OptionTypes::CG:
    solver_name = "cg";
    checkError("Hypre CG solver", HYPRE_ParCSRPCGCreate(comm, &solver));
    checkError(
        "Hypre BiCGStab solver SetMaxIter", HYPRE_ParCSRPCGSetMaxIter(solver, max_it));
    // solver_set_logging_function = HYPRE_ParCSRPCGSetLogging;
    solver_set_print_level_function = HYPRE_ParCSRPCGSetPrintLevel;
    solver_set_tol_function = HYPRE_ParCSRPCGSetTol;
    solver_set_precond_function = HYPRE_ParCSRPCGSetPrecond;
    solver_setup_function = HYPRE_ParCSRPCGSetup;
    solver_solve_function = HYPRE_ParCSRPCGSolve;
    solver_get_num_iterations_function = HYPRE_ParCSRPCGGetNumIterations;
    solver_get_final_relative_residual_function =
        HYPRE_ParCSRPCGGetFinalRelativeResidualNorm;
    solver_destroy_function = HYPRE_ParCSRPCGDestroy;
    break;
  case OptionTypes::BiCGStab:
    solver_name = "bicgs";
    checkError("Hypre BiCGStab solver", HYPRE_ParCSRBiCGSTABCreate(comm, &solver));
    checkError("Hypre BiCGStab solver SetMaxIter",
        HYPRE_ParCSRBiCGSTABSetMaxIter(solver, max_it));
    // solver_set_logging_function = HYPRE_ParCSRBiCGSTABSetLogging;
    solver_set_print_level_function = HYPRE_ParCSRBiCGSTABSetPrintLevel;
    solver_set_tol_function = HYPRE_ParCSRBiCGSTABSetTol;
    solver_set_precond_function = HYPRE_ParCSRBiCGSTABSetPrecond;
    solver_setup_function = HYPRE_ParCSRBiCGSTABSetup;
    solver_solve_function = HYPRE_ParCSRBiCGSTABSolve;
    solver_get_num_iterations_function = HYPRE_ParCSRBiCGSTABGetNumIterations;
    solver_get_final_relative_residual_function =
        HYPRE_ParCSRBiCGSTABGetFinalRelativeResidualNorm;
    solver_destroy_function = HYPRE_ParCSRBiCGSTABDestroy;
    break;
  case OptionTypes::Hybrid:
    solver_name = "hybrid";
    checkError("Hypre Hybrid solver", HYPRE_ParCSRHybridCreate(&solver));
    // checkError("Hypre Hybrid solver
    // SetSolverType",HYPRE_ParCSRHybridSetSolverType(solver,1)); // PCG
    checkError("Hypre Hybrid solver SetSolverType",
        HYPRE_ParCSRHybridSetSolverType(solver, 2)); // GMRES
    // checkError("Hypre Hybrid solver
    // SetSolverType",HYPRE_ParCSRHybridSetSolverType(solver,3)); // BiCGSTab
    checkError("Hypre Hybrid solver SetDiagMaxIter",
        HYPRE_ParCSRHybridSetDSCGMaxIter(solver, max_it));
    checkError("Hypre Hybrid solver SetPCMaxIter",
        HYPRE_ParCSRHybridSetPCGMaxIter(solver, max_it));
    // solver_set_logging_function = HYPRE_ParCSRHybridSetLogging;
    solver_set_print_level_function = HYPRE_ParCSRHybridSetPrintLevel;
    solver_set_tol_function = HYPRE_ParCSRHybridSetTol;
    solver_set_precond_function = nullptr; // HYPRE_ParCSRHybridSetPrecond; // SegFault si
                                           // utilise un préconditionneur !
    solver_setup_function = HYPRE_ParCSRHybridSetup;
    solver_solve_function = HYPRE_ParCSRHybridSolve;
    solver_get_num_iterations_function = HYPRE_ParCSRHybridGetNumIterations;
    solver_get_final_relative_residual_function =
        HYPRE_ParCSRHybridGetFinalRelativeResidualNorm;
    solver_destroy_function = HYPRE_ParCSRHybridDestroy;
    break;
  default:
    alien_fatal([&] { cout() << "Undefined solver option"; });
    break;
  }

  if (solver_set_precond_function) {
    if (precond_solve_function) {
      checkError("Hypre " + solver_name + " solver SetPreconditioner",
          (*solver_set_precond_function)(
              solver, precond_solve_function, precond_setup_function, preconditioner));
    }
  } else {
    if (precond_solve_function) {
      alien_fatal([&] {
        cout() << "Hypre " << solver_name << " solver cannot accept preconditioner";
      });
    }
  }

  checkError("Hypre " + solver_name + " solver SetStopCriteria",
      (*solver_set_tol_function)(solver, rtol));

  if (output_level > 0) {
    checkError("Hypre " + solver_name + " solver Setlogging",
        (*solver_set_print_level_function)(solver, 1));
    checkError("Hypre " + solver_name + " solver SetPrintLevel",
        (*solver_set_print_level_function)(solver, 3));
  }

  HYPRE_ParCSRMatrix par_a;
  HYPRE_ParVector par_rhs, par_x;
  checkError(
      "Hypre Matrix GetObject", HYPRE_IJMatrixGetObject(ij_matrix, (void**)&par_a));
  checkError("Hypre RHS Vector GetObject",
      HYPRE_IJVectorGetObject(bij_vector, (void**)&par_rhs));
  checkError("Hypre Unknown Vector GetObject",
      HYPRE_IJVectorGetObject(xij_vector, (void**)&par_x));

  checkError("Hypre " + solver_name + " solver Setup",
      (*solver_setup_function)(solver, par_a, par_rhs, par_x));
  m_status.succeeded = ((*solver_solve_function)(solver, par_a, par_rhs, par_x) == 0);

  if (m_status.succeeded) {
      checkError("Hypre " + solver_name + " solver GetNumIterations",
                 (*solver_get_num_iterations_function)(solver, &m_status.iteration_count));
      checkError("Hypre " + solver_name + " solver GetFinalResidual",
                 (*solver_get_final_relative_residual_function)(solver, &m_status.residual));

      m_status.succeeded = (m_status.iteration_count < max_it)
        || (m_status.succeeded == max_it && m_status.residual <= rtol);
  } else {
      // Solver is not converged. Clear Hypre errors for subsequent calls.
      HYPRE_ClearAllErrors();
  }

  checkError(
      "Hypre " + solver_name + " solver Destroy", (*solver_destroy_function)(solver));
  if (precond_destroy_function)
    checkError("Hypre " + precond_name + " preconditioner Destroy",
        (*precond_destroy_function)(preconditioner));

  ++m_solve_num;
  m_total_iter_num += m_status.iteration_count;
  m_total_solve_time += tsolve.elapsed();
  return m_status.succeeded;
}

const Alien::SolverStatus&
InternalLinearSolver::getStatus() const
{
  return m_status;
}

ALIEN_HYPRE_EXPORT
std::shared_ptr<ILinearAlgebra>
InternalLinearSolver::algebra() const
{
  return std::make_shared<LinearAlgebra>();
}

ALIEN_HYPRE_EXPORT
IInternalLinearSolver<Matrix, Vector>*
InternalLinearSolverFactory(const Options& options)
{
  return new InternalLinearSolver(options);
}

ALIEN_HYPRE_EXPORT
IInternalLinearSolver<Matrix, Vector>*
InternalLinearSolverFactory()
{
  return new InternalLinearSolver();
}
} // namespace Alien::Hypre
