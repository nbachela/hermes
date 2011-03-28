// This file is part of Hermes3D
//
// Copyright (c) 2009 hp-FEM group at the University of Nevada, Reno (UNR).
// Email: hpfem-group@unr.edu, home page: http://hpfem.org/.
//
// Hermes3D is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
//
// Hermes3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hermes3D; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef __HERMES_COMMON_UMFPACK_SOLVER_H_
#define __HERMES_COMMON_UMFPACK_SOLVER_H_

#include "solver.h"
#include "../matrix.h"


// General CSC Matrix class (can be used in umfpack, in that case use the
// UMFPackMatrix subclass, or with EigenSolver, or anything else)

template <typename Scalar> class UMFPackLinearSolver;

template <typename Scalar>
class HERMES_API CSCMatrix : public SparseMatrix<Scalar> {
public:
  CSCMatrix();
  CSCMatrix(unsigned int size);
  virtual ~CSCMatrix();

  virtual void alloc();
  virtual void free();
  virtual Scalar get(unsigned int m, unsigned int n);
  virtual void zero();
  virtual void add(unsigned int m, unsigned int n, Scalar v);
  virtual void add_to_diagonal(Scalar v);
  // TODO: implement this for other matrix types.
  virtual void add_matrix(CSCMatrix<Scalar>* mat);
  // TODO: implement this for other matrix types.
  virtual void add_to_diagonal_blocks(int num_stages, CSCMatrix<Scalar>* mat);
  // TODO: implement this for other matrix types.
  virtual void add_as_block(unsigned int i, unsigned int j, CSCMatrix<Scalar>* mat);
  virtual void add(unsigned int m, unsigned int n, Scalar **mat, int *rows, int *cols);
  virtual bool dump(FILE *file, const char *var_name, EMatrixDumpFormat fmt = DF_MATLAB_SPARSE);
  virtual unsigned int get_matrix_size() const;
  unsigned int get_nnz() {return this->nnz;}
  virtual double get_fill_in() const;

  // Applies the matrix to vector_in and saves result to vector_out.
  void multiply_with_vector(Scalar* vector_in, Scalar* vector_out);
  // Multiplies matrix with a Scalar.
  void multiply_with_scalar(Scalar value);
  // Creates matrix in CSC format using size, nnz, and the three arrays.
  void create(unsigned int size, unsigned int nnz, int* ap, int* ai, Scalar* ax);
  // Duplicates a matrix (including allocation).
  CSCMatrix* duplicate();
  // Exposes pointers to the CSC arrays.
  int *get_Ap() {
      return this->Ap;
  }
  int *get_Ai() {
      return this->Ai;
  }
  Scalar *get_Ax() {
      return this->Ax;
  }

protected:
  // UMFPack specific data structures for storing the system matrix (CSC format).
  Scalar *Ax;            // Matrix entries (column-wise).
  int *Ai;               // Row indices of values in Ax.
  int *Ap;               // Index to Ax/Ai, where each column starts.
  unsigned int nnz;      // Number of non-zero entries (= Ap[size]).

};

// This class is to be used with UMFPack solver only:
template <typename Scalar>
class HERMES_API UMFPackMatrix : public CSCMatrix<Scalar> {
  friend class UMFPackLinearSolver<Scalar>;
};

template <typename Scalar>
class HERMES_API UMFPackVector : public Vector<Scalar> {
public:
  UMFPackVector();
  UMFPackVector(unsigned int size);
  virtual ~UMFPackVector();

  virtual void alloc(unsigned int ndofs);
  virtual void free();
  virtual Scalar get(unsigned int idx) { return v[idx]; }
  virtual void extract(Scalar *v) const { memcpy(v, this->v, size * sizeof(Scalar)); }
  virtual void zero();
  virtual void change_sign();
  virtual void set(unsigned int idx, Scalar y);
  virtual void add(unsigned int idx, Scalar y);
  virtual void add(unsigned int n, unsigned int *idx, Scalar *y);
  virtual void add_vector(Vector* vec) {
    assert(this->length() == vec->length());
    for (unsigned int i = 0; i < this->length(); i++) this->v[i] += vec->get(i);
  };
  virtual void add_vector(Scalar* vec) {
    for (unsigned int i = 0; i < this->length(); i++) this->v[i] += vec[i];
  };
  virtual bool dump(FILE *file, const char *var_name, EMatrixDumpFormat fmt = DF_MATLAB_SPARSE);

  Scalar *get_c_array() {
      return this->v;
  }

protected:
  //UMFPack specific data structures for storing the rhs.
  Scalar *v;
  friend class UMFPackLinearSolver<Scalar>;
};


/// Encapsulation of UMFPACK linear solver
///
/// @ingroup solvers
template <typename Scalar>
class HERMES_API UMFPackLinearSolver : public LinearSolver<Scalar> {
public:
  UMFPackLinearSolver(UMFPackMatrix<Scalar> *m, UMFPackVector<Scalar> *rhs);
  virtual ~UMFPackLinearSolver();

  virtual bool solve();
    
protected:
  UMFPackMatrix<Scalar> *m;
  UMFPackVector<Scalar> *rhs;
  
  // Reusable factorization information (A denotes matrix represented by the pointer 'm').
  void *symbolic; // Reordering of matrix A to reduce fill-in during factorization.
  void *numeric;  // LU factorization of matrix A.
  
  bool setup_factorization();
  void free_factorization_data();
};



/*** UMFPack matrix iterator ****/
template <typename Scalar>
class UMFPackIterator {
public:
  UMFPackIterator(CSCMatrix<Scalar>* mat) 
  {
    this->size = mat->get_size();
    this->nnz = mat->get_nnz();
    this->Ai = mat->get_Ai();
    this->Ap = mat->get_Ap();
    this->Ax = mat->get_Ax();
    this->Ai_pos = 0;
    this->Ap_pos = 0;
  };
  bool init();
  void get_current_position(int& i, int& j, Scalar& val);
  bool move_to_position(int i, int j);
  bool move_ptr();
  void add_to_current_position(Scalar val);

protected:
  int size;
  int nnz;
  int* Ai;
  int* Ap;
  Scalar* Ax;
  int Ai_pos;
  int Ap_pos;
};
#endif
