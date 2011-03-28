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

#ifndef __HERMES_COMMON_SOLVER_EPETRA_H_
#define __HERMES_COMMON_SOLVER_EPETRA_H_

#include "../matrix.h"

#ifdef HAVE_EPETRA
  #include <Epetra_SerialComm.h>
  #include <Epetra_Map.h>
  #include <Epetra_Vector.h>
  #include <Epetra_CrsGraph.h>
  #include <Epetra_CrsMatrix.h>
#endif

template <typename Scalar> class AmesosSolver;
template <typename Scalar> class AztecOOSolver;
template <typename Scalar> class NoxSolver;
template <typename Scalar> class IfpackPrecond;
template <typename Scalar> class MlPrecond;

template <typename Scalar>
class HERMES_API EpetraMatrix : public SparseMatrix<Scalar> {
public:
  EpetraMatrix();
#ifdef HAVE_EPETRA
  EpetraMatrix(Epetra_RowMatrix &mat);
#endif
  virtual ~EpetraMatrix();

  virtual void prealloc(unsigned int n);
  virtual void pre_add_ij(unsigned int row, unsigned int col);
  virtual void finish();

  virtual void alloc();
  virtual void free();
  virtual Scalar get(unsigned int m, unsigned int n);
  virtual int get_num_row_entries(unsigned int row);
  virtual void extract_row_copy(unsigned int row, unsigned int len, unsigned int &n_entries, double *vals, unsigned int *idxs);
  virtual void zero();
  virtual void add(unsigned int m, unsigned int n, Scalar v);
  virtual void add_to_diagonal(Scalar v);
  virtual void add(unsigned int m, unsigned int n, Scalar **mat, int *rows, int *cols);
  virtual bool dump(FILE *file, const char *var_name, EMatrixDumpFormat fmt = DF_MATLAB_SPARSE);
  virtual unsigned int get_matrix_size() const;
  virtual unsigned int get_nnz() const;
  virtual double get_fill_in() const;

protected:
#ifdef HAVE_EPETRA
  Epetra_BlockMap *std_map;
  Epetra_CrsGraph *grph;
  Epetra_CrsMatrix *mat;
  Epetra_CrsMatrix *mat_im;		// imaginary part of the matrix, mat holds the real part
  bool owner;
#endif

  friend class AmesosSolver<Scalar>;
  friend class AztecOOSolver<Scalar>;
  friend class NoxSolver<Scalar>;
  friend class IfpackPrecond<Scalar>;
  friend class MlPrecond<Scalar>;
};

template <typename Scalar>
class HERMES_API EpetraVector : public Vector<Scalar> {
public:
  EpetraVector();
#ifdef HAVE_EPETRA
  EpetraVector(const Epetra_Vector &v);
#endif
  virtual ~EpetraVector();

  virtual void alloc(unsigned int ndofs);
  virtual void free();
#ifdef HAVE_EPETRA
  virtual Scalar get(unsigned int idx) { return (*vec)[idx]; }
  virtual void extract(Scalar *v) const { vec->ExtractCopy((double *)v); }
#else
  virtual Scalar get(unsigned int idx) { return 0.0; }
  virtual void extract(Scalar *v) const { }
#endif
  virtual void zero();
  virtual void change_sign();
  virtual void set(unsigned int idx, Scalar y);
  virtual void add(unsigned int idx, Scalar y);
  virtual void add(unsigned int n, unsigned int *idx, Scalar *y);
  virtual void add_vector(Vector* vec) {
    assert(this->length() == vec->length());
    for (unsigned int i = 0; i < this->length(); i++) this->add(i, vec->get(i));
  };
  virtual void add_vector(Scalar* vec) {
    for (unsigned int i = 0; i < this->length(); i++) this->add(i, vec[i]);
  };
  virtual bool dump(FILE *file, const char *var_name, EMatrixDumpFormat fmt = DF_MATLAB_SPARSE);

protected:
#ifdef HAVE_EPETRA
  Epetra_BlockMap *std_map;
  Epetra_Vector *vec;
  Epetra_Vector *vec_im;		// imaginary part of the vector, vec holds the real part
  bool owner;
#endif

  friend class AmesosSolver<Scalar>;
  friend class AztecOOSolver<Scalar>;
  friend class NoxSolver<Scalar>;
};

#endif
