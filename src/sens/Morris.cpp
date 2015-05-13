/**
 @file Morris.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "Morris.h"

#include "ModelInput.h"
#include "ModelOutput.h"

BIO_NAMESPACE_BEGIN

Morris::Morris()
  : SASimple(SA_MORRIS)
  , perm_(nullptr)
  , r_(20)
  , p_(4)
{
}


void Morris::setR(const int r)
{
  if (r<5)
    throw SAException(ERROR_MORRIS_TOO_SMALL_R);
  r_ = r;
}

void Morris::setP(const int p)
{
  if (p<3)
    throw SAException(ERROR_MORRIS_TOO_SMALL_P);
  p_ = p;
}

int Morris::getNumSens() const
{
  return 3*m_NumOutputs;                /* each output has 3 sensitivity measues, muy, muystar and std */
}

int Morris::getNumSamples() const
{
  return r_ * ( m_InputList->size() + 1 );     /* Morris method needs r*(k+1) samples */
}

void Morris::sample()
{  
  int k = m_InputList->size();                 /* number of outputs */
  double delta = 0.5*p_/(p_-1);

 /* Allocates the base vector xstar */
  std::unique_ptr<double[]> xstar_ptr(new double[k]);
  double* xstar = xstar_ptr.get();             

  /* Allocates a vector to hold randomized indexes in order to generate xstar
   * randomly*/
  std::unique_ptr<int[]> xstar_indexes_ptr(new int[k]);
  int* xstar_indexes = xstar_indexes_ptr.get(); 

 /* Creates a vector that holds all feasible attibute values of xstar*/
  int num_xstar_values = (int) floor((1-delta)*(p_-1)); /* size of the vector */
  std::unique_ptr<double[]> xstar_values_ptr(new double[num_xstar_values+1]);

  double* xstar_values = xstar_values_ptr.get();
  for (int i=0; i<= num_xstar_values; ++i)
    xstar_values[i] = (double)i/(p_-1);  /* x_star_values = {0, 1/(p-1), ... ,1-delta} */

 /* Creates a temporary row vector that later will be used for column permutation of Bstar matrix*/
  std::unique_ptr<double[]> tmprow_ptr(new double[k]);
  double* tmprow = tmprow_ptr.get();           

  /* Allocates array of index permutation vector */
  perm_.reset(new IMatrix(r_, k));

  /* Creates trajectories */
  for (int iR=0; iR < r_; ++iR) 
  {

    /* 1. Randomly generate a base vector xstar */

    m_RNG.rand(0, num_xstar_values, xstar_indexes, k); /* randomly generate indexes */
    for (int iK=0; iK<k; ++iK)
    {
      xstar[iK] = xstar_values[xstar_indexes[iK]];
    }

    /* 2. Generate sammpling matrix Bstar */

    /* 2.1 Generate B matrix */
    /*  Instead of allocate Bstar, lets it points to internal data of m_InputData */
    double** Bstar = &(m_InputData->getData()[iR*(k+1)]);
    /* Make it lower triangle matrix of value ones */
    for (int iRow=0; iRow<k+1; ++iRow)
    {
      for (int iCol=0; iCol<k; ++iCol)
      {
        Bstar[iRow][iCol] = (iRow > iCol) ? delta : 0.0;
      }
    }
    
    /* 2.2 Change signs of the values of about half of the colums. These colums are chosen
     * randomly with equal probability */
    for (int iCol = 0; iCol<k; ++iCol)          /* for each column */
    {
      if  (m_RNG.rand() < 0.5)                  /* change or not change? */
      {
        for (int iRow=0; iRow<k+1; ++iRow)
        {
          if (Bstar[iRow][iCol]>0.0)              /* Bstar[iRow][iCol] = delta */
          {
            Bstar[iRow][iCol] = 0.0;
          } else                                /* Bstar[iRow][iCol] = 0.0 */
          {
            Bstar[iRow][iCol] = delta;
          }
        }
      }        
    }

    /* 2.3 Randomly permutate colum of B, the result is Bstar matrix 
     * NOTE: this is might be not neccessary*/

    /* create permuation indexes */
    int* indexRow = perm_->getRow(iR);
    for (int iCol=0; iCol<k; ++iCol)
      indexRow[iCol] = iCol;
    m_RNG.shuffle(indexRow, k);            /* shuffle */
    
    /* permutation on every rows */
    for (int iRow = 0; iRow <= k; ++iRow)
    {
      /* copy the row to temporary vector */
      for (int iCol=0; iCol<k; ++iCol)
      {
        tmprow[iCol] = Bstar[iRow][iCol];
      }
      /* now copy it back with permuted indexes  */
      for (int iCol=0; iCol<k; ++iCol)
      {
        Bstar[iRow][indexRow[iCol]] = tmprow[iCol];
      }     
    }

    /* 2.4 Add the base vector to make samples */
    for (int iRow=0; iRow<k+1; ++iRow)
    {
      for (int iCol=0; iCol<k; ++iCol)
      {
        Bstar[iRow][iCol] += xstar[iCol];
      }
    }
  }
}

void Morris::estimate()
{
  int k = m_InputList->size();
  double delta = 0.5*p_/(p_-1);
  
  std::unique_ptr<DMatrix> stats_ptr(new DMatrix(k, 3)); /* temporary matrix to hold statistic values */
  double** stats = stats_ptr->getData();
  std::unique_ptr<int[]> cnts(new int[k]);      /* vector to hold the number of valid simulation results */

  int* labels = m_OutputData->getLabels();

  for (int iOut = 0; iOut<m_NumOutputs; ++iOut)
  {
    for (int iK=0; iK<k; ++iK)
    {
      cnts[iK] = 0;
      stats[iK][0] = 0;
      stats[iK][1] = 0;
      stats[iK][2] = 0;
    }

    for (int iR=0; iR<r_; ++iR)
    {
      int* indexRow = perm_->getRow(iR);
      int currentRow = iR*(k+1);
      for (int iRow=0; iRow<k; ++iRow)
      {
        int curEfIndex = indexRow[iRow];
        if (labels[currentRow] == SIM_SUCCESS
            && labels[currentRow+1] == SIM_SUCCESS)
        {
          double ef = m_OutputData->getRow(currentRow+1)[iOut]
              - m_OutputData->getRow(currentRow)[iOut];
          stats[curEfIndex][0] += ef;           /* for mean estimation*/
          stats[curEfIndex][1] += abs(ef);      /* for absolute mean estimation */
          stats[curEfIndex][2] += ef*ef;        /* for variance estimation*/
          cnts[curEfIndex]++;
        }
        currentRow++;
      }
    }
    
    for (int iK=0; iK<k; ++iK)
    {
      if (cnts[iK] < (1-m_FailureRate) * r_)
        throw SAException(ERROR_EXCEEDING_FAILURE_RATE);

      stats[iK][0] /= cnts[iK];
      stats[iK][1] /= cnts[iK]*delta;
      stats[iK][2] == sqrt(stats[iK][2]/cnts[iK]-stats[iK][0]*stats[iK][0])/delta;  
      
      m_Sens->getRow(iK)[3*iOut] = stats[iK][0]/delta;
      m_Sens->getRow(iK)[3*iOut+1] = stats[iK][1];
      m_Sens->getRow(iK)[3*iOut+2] = stats[iK][2];
    }
  }
}
BIO_NAMESPACE_END

