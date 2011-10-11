#define timingstep2



void GetStartConfig(std::string base_dir,
					std::vector< std::string > & startvector,
					std::vector< std::string > & configvector);

void GetLastNumSent(std::string base_dir,
					std::vector< int > & lastnumsent,
					int numprocs);

void GetRandomValues(std::string base_dir,
					 std::vector< std::pair<double,double> > & RandomValues);

void ReadDotOut(std::vector<std::string> & Numoutvector, 
				std::vector<std::string> & arroutvector,
				std::vector<std::string> & degoutvector,
				std::vector<std::string> & namesoutvector,
				std::vector<std::string> & configvector,
				std::vector<std::string> & funcinputvector);

void WriteDotOut(std::vector<std::string> & arroutvector,
				 std::vector<std::string> & degoutvector,
				 std::vector<std::string> & namesoutvector,
				 std::vector<std::string> & configvector,
				 std::vector<std::string> & funcinputvector);

void WriteNumDotOut(std::vector<std::string> Numoutvector,
					std::vector<std::pair<double,double> > AllParams,
					int numparam);

