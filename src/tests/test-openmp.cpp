/**
 @file -testopenmp.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <omp.h>

int main()
{
#pragma omp parallel
  {
  int id = omp_get_thread_num();
  std::cout << "Hello " << id << " World\n" ;
  }
  return EXIT_SUCCESS;
}

