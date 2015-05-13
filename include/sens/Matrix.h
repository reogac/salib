/**
 @file Matrix.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  Matrix_INC
#define  Matrix_INC

#include <cassert>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <memory>
#include <ostream>
#include <stdexcept>

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

template <typename T> class Matrix
{
  public:
    /**
     * @brief Matrix constructor
     *
     *@param nrows The number of rows
     @param ncols The number of columns
     * */
    Matrix(const int nrows, const int ncols)
      : m_Rows(nrows)
        , m_Cols(ncols)
        , m_Data(nullptr)
        , data_(nullptr)
        , orig_(true)
    {
      assert( nrows>0
              && ncols>0
              );

      std::unique_ptr<T*[]> dataptr(new T*[m_Rows]);
      std::unique_ptr<T[]> data(new T[m_Rows*m_Cols]);

      data_ = data.release();
      m_Data = dataptr.release();

      for (int i=0; i<m_Rows; ++i)
        m_Data[i] = &data_[i*m_Cols];
    }

    virtual ~Matrix() 
    {
      this->free();
    }
    /**
     * @brief Copy constructor
     * @param other The matrix to be copied 
     * */
    Matrix(const Matrix& other)
      : m_Rows(other.m_Rows)
        , m_Cols(other.m_Cols)
        , orig_(true)
    {
     std::unique_ptr<T[]> data(new T[m_Rows*m_Cols]);
      std::unique_ptr<T*[]> dataptr(new T*[m_Rows]);
     data_ = data.release();
     m_Data = dataptr.release();
      std::memcpy(data_, other.data_, m_Rows*m_Cols*sizeof(T));
      for (int i=0; i<m_Rows; ++i)
        m_Data[i] = &data_[i*m_Cols];
    }
    
    /**
     * @brief Assignment constructor
     *
     * @param other The matrix to be copied
     * */
    Matrix& operator=(const Matrix& other)
    {
      if (this != &other && orig_)
      {
        if (m_Rows*m_Cols != other.m_Rows*other.m_Cols)
        {
          std::unique_ptr<T[]> data(new T[other.m_Rows*other.m_Cols]);
          delete [] data_;
          data_ = data.release(); 
        }

        if (m_Rows != other.m_Rows)
        {
          std::unique_ptr<T*[]> dataptr(new T*[other.m_Rows]);
          delete [] m_Data;
          m_Data = dataptr.release();
        }

        m_Rows = other.m_Rows;
        m_Cols = other.m_Cols;
        std::memcpy(data_, other.data_, m_Rows*m_Cols*sizeof(T));

        for (int i=0; i<m_Rows; ++i)
          m_Data[i] = &data_[i*m_Cols];
      }
      return *this;
    }

    /**
     * @brief Returns a submatrix
     *
     * @param starRow Starting row of the sub-matrix
     * @param nrows Number of rows of the sub matrix.
     * */
    std::unique_ptr<Matrix> subMatrix(const int startRow, const int nrows)
    {
      assert( startRow>=0
              && nrows>0
              && startRow+nrows<=m_Rows
              );
      std::unique_ptr<Matrix> ret(new Matrix());
      ret->m_Cols = m_Cols;
      ret->m_Rows = nrows;
      ret->m_Data = &m_Data[startRow];
      ret->data_ = m_Data[startRow];
      ret->orig_ = false;
      return ret;
    }
    /**
     * @brief Gets the number of rows
     * */
    int getNumRows() const 
    { 
      return m_Rows; 
    }
    /**
    * @brief Gets the number of columns
    *
    * */
    int getNumCols() const 
    { 
      return m_Cols; 
    }
    
    /** 
     * @brief Returns a pointer to a row
     *
     * @param index Index of the wanted row
     * */
    T* getRow(const int index) 
    { 
      return m_Data[index]; 
    }
    /**
     * @brief Returns a const pointer to a row
     *
     * @param index Index of the wanted row
     * */
    const T* getRow(const int index) const 
    { 
      return m_Data[index]; 
    }
    
    /**
     * @brief Returns the row vector
     * */
    T** getData() 
    { 
      return m_Data; 
    }

    /**
     * @brief Copies content
     *
     * @param mat Matrix to which content is copied. It must be the same size as
     * the source matrix
     * */
    void copy(Matrix& mat)
    {
      if (m_Rows*m_Cols == mat.m_Rows*mat.m_Cols)
      {
        std::memcpy(mat.data_, data_, m_Rows*m_Cols*sizeof(T));
      }
    }
    /**
     * @brief Copies content to a row
     *
     * @param rowId Index of the row to receive content
     * @param values Content to be copied
     *
     * */
    void fillRow(const int rowId, const T* values)
    {
      std::memcpy(m_Data[rowId], values, m_Cols*sizeof(T));
    }

     /**
     * @brief Copies content to a column
     *
     * @param colId Index of the column to receive content
     * @param values Content to be copied
     *
     * */
    void fillCol(const int colId, const T* values)
    {
      for (int i=0; i<m_Rows; ++i)
        m_Data[i][colId] = values[i];
    }
    /**
     * @brief Copies content from a column
     * @param colId Index of column to be copied
     * @param values Vector (allocated by callers) to hold the copied content
     * */
    void copyCol(const int colId, T* values) const
    {
      for (int iRow=0; iRow < m_Rows; ++iRow)
        values[iRow]= m_Data[iRow][colId];
    }
    /** 
     * @brief Returns a copy of a column
     *
     * @param colId Index of the column to be copied
     * */
    std::unique_ptr<T[]> copyCol(const int colId) const
    {
      std::unique_ptr<T[]> ret(new T[m_Rows]);
      T* retcore = ret.get();
      for (int iRow=0; iRow<m_Rows; ++iRow)
        retcore[iRow] = m_Data[iRow][colId];
      return ret;
    }

    /**
     * @brief Applies an operation on a column.
     *
     * Values of the column will be assigned according to return values of the
     * operation
     *
     * @param colId Column where the operation will be execute on
     * @param f The operation
     *
     * */
    void colEval(const int col, std::function<T (T&)> f)
    {
      for (int row=0; row<m_Rows; ++row)
        m_Data[row][col] = f(m_Data[row][col]);
    }

  protected:
    Matrix()
      : m_Rows(0)
      , m_Cols(0)
      , m_Data(nullptr)
      , data_(nullptr)
      , orig_(false)
    {
    }
    /**
     * @brief Gets a submatrix
     *
     * @param mat a Matrix reference to hole submatrix content
     * @param starRow Starting row of the sub-matrix
     * @param nrows Number of rows of the sub matrix.
     * 
     * */
    void subMatrix(Matrix& mat, const int startRow, const int nrows)
    {
      assert( startRow>=0
              && nrows>0
              && startRow+nrows<=m_Rows
              );

      mat.free();

      mat.m_Cols = m_Cols;
      mat.m_Rows = nrows;
      mat.m_Data = &m_Data[startRow];
      mat.data_ = m_Data[startRow];
      mat.orig_ = false;
    }



    T** m_Data;                                 /* vector of pointer of matrix rows */
    int m_Rows;                                 /* number of rows */
    int m_Cols;                                 /* number of columns */

    bool orig_;                                 /* is the matrix origial (need memory delocation in the destructor) */
  private:
    void free()
    {
      if (orig_)                              /* delete if the matrix is original */
      {
        delete [] data_;
        delete [] m_Data;
        data_ = nullptr;
        m_Data = nullptr;
      }
    }

    T* data_;                                   /* internal data */

    friend std::ostream& operator<< (std::ostream& out, const Matrix& mat)
    {
      out << std::endl;
      for (int iRow=0; iRow<mat.getNumRows(); ++iRow)
      {
        const T* row = mat.getRow(iRow);
        for (int iCol=0; iCol<mat.getNumCols(); ++iCol)
        out << std::setw(10) << row[iCol] << " ";
        out << std::endl;
      }
      return out;
    }
};

typedef class Matrix<double> DMatrix;           /* Matrix of double values */

typedef class Matrix<int> IMatrix;           /* Matrix of integer values */

BIO_NAMESPACE_END


#endif   /* ----- #ifndef Matrix_INC  ----- */


