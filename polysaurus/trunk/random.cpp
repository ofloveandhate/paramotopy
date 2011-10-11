#include "random.h"
#include "mtrand.h"

std::vector<std::pair<double,double> > MakeRandomValues(int size){
  std::vector< std::pair<double,double> > RandomValues;
  MTRand drand(time(0));
  for (int i = 0; i < size;++i){
    double creal = double(drand());
    double cimaginary = double(drand());
    RandomValues.push_back(std::pair<double,double>(creal,cimaginary));
  }
  
  
  return RandomValues;
}

std::vector<std::pair<double, double> > 
  MakeRandomValues(std::vector< std::pair< std::pair< double, double >, 
		       std::pair< double, double > > > RandomRanges){

  std::vector<std::pair<double, double> > RandomValues;

  MTRand drand(time(0));
  for (int i = 0; i < RandomRanges.size(); ++i){
    double crandreal = drand();
    double crandimaginary = drand();
    crandreal*=(RandomRanges[i].first.second 
		- RandomRanges[i].first.first);
    crandreal+=RandomRanges[i].first.first;
    crandimaginary*=(RandomRanges[i].second.second
		     -RandomRanges[i].second.first);
    crandimaginary+=RandomRanges[i].second.first;
    RandomValues.push_back(std::pair<double,double>(crandreal,
						  crandimaginary));

  }


  return RandomValues;


}
					      

void PrintRandom(std::vector<std::pair<double,double> > RandomValues,
		 std::vector<std::string> ParamStrings){
  //  std::cout << "RandomValues.size() = " << RandomValues.size();
  //  std::cout << "\nParamStrings.size() = " << ParamStrings.size();
  // std::cout << "\n";
  std::cout << "The random initial values for the parameters are : \n";
  for (int i = 0; i < ParamStrings.size();++i){
    std::cout << ParamStrings[i] << " = " << RandomValues[i].first
	      << " + " << RandomValues[i].second << "*I\n";

  }
  std::cout << "\n";
  std::cout << "ParamStrings.size() = " << ParamStrings.size() << "\n";
}
