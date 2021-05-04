#include <sstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>
#include "partition-comparison.hxx"

int main(int argc, char* argv[])
{

  if (argc < 3){
    fprintf(stderr, "Invalid number of arguments for computing accuracy\n");
  }
  
  int c;

  std::vector<int> indexes_madc;
  std::ifstream index_madc_file(argv[1]);
  while( index_madc_file >> c){
    indexes_madc.push_back(c);
  }
  
  std::vector<int> indexes_baseline;
  std::ifstream index_baseline_file(argv[2]);
  while( index_baseline_file >> c){
    indexes_baseline.push_back(c);
  }
  
  
  //  size_t p0[6] = {1, 1, 0, 0, 0, 0};
  //  size_t p1[6] = {0, 0, 0, 1, 0, 1};

  //  std::cout << andres::RandError<>(p0, p0+6, p1).index() << std::endl;
  //  std::cout << "Accuracy: " << andres::RandError<>(indexes_madc.begin(), indexes_madc.begin()+indexes_madc.size(), indexes_baseline.begin()).index() << std::endl;

  std::ofstream profileStream;

  if (argc == 3){
    profileStream.open("stats/accuracy.txt", std::ofstream::out | std::ofstream::app);
  }
  else{
    profileStream.open(argv[3], std::ofstream::out | std::ofstream::app);
  }

  
  profileStream << andres::RandError<>(indexes_madc.begin(), indexes_madc.begin()+indexes_madc.size(), indexes_baseline.begin()).index() << std::endl;
  profileStream.close();
  
  return 0;
}
