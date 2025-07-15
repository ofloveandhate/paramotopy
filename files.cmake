set(common_src
	src/body/datagatherer.cpp 
	src/body/exit_funcs.cpp 
	src/body/failed_paths.cpp 
	src/body/menu_cases.cpp 
	src/body/mtrand.cpp 
	src/body/para_aux_funcs.cpp 
	src/body/point.cpp 
	src/body/random.cpp 
	src/body/runinfo.cpp 
	src/body/timing.cpp 
	src/body/xml_preferences.cpp 
	src/body/step1_funcs.cpp 
)

set(common_headers
	include/paramotopy/datagatherer.hpp
	include/paramotopy/exit_funcs.hpp
	include/paramotopy/failed_paths.hpp
	include/paramotopy/menu_cases.hpp
	include/paramotopy/mtrand.hpp
	include/paramotopy/para_aux_funcs.hpp
	include/paramotopy/point.hpp
	include/paramotopy/random.hpp
	include/paramotopy/runinfo.hpp
	include/paramotopy/timing.hpp
	include/paramotopy/xml_preferences.hpp
	include/paramotopy/step1_funcs.hpp
)

set(common_headers
	include/paramotopy/paramotopy_enum.hpp
)

set(para_src
	src/paramotopy/paramotopy.cpp
)

set(para_headers
	include/paramotopy/paramotopy_enum.hpp
	include/paramotopy/paramotopy.hpp
)

set(step2_src
	src/step2/step2.cpp 
	src/step2/controller.cpp 
	src/step2/worker.cpp 
	src/step2/step2_funcs.cpp 
	src/step2/step2readandwrite.cpp 
)

set(step2_headers
	include/paramotopy/step2.hpp
	include/paramotopy/controller.hpp
	include/paramotopy/worker.hpp
	include/paramotopy/step2_funcs.hpp
	include/paramotopy/step2readandwrite.hpp
	)

set(libtinyxml_src
	src/tinyxml/tinystr.cpp 
	src/tinyxml/tinyxml.cpp 
	src/tinyxml/tinyxmlerror.cpp 
	src/tinyxml/tinyxmlparser.cpp 
)

set(libtinyxml_headers
	include/paramotopy/tinystr.h 
	include/paramotopy/tinyxml.h
)

