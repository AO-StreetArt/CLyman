#include "logging.h"

log4cpp::Category *logging;

void init_log()
{
	std::string initFileName = "log4cpp.properties";
	try {
        	log4cpp::PropertyConfigurator::configure(initFileName);
	}
	catch ( log4cpp::ConfigureFailure &e ) {
        	std::cout << "[log4cpp::ConfigureFailure] caught while reading" << initFileName << std::endl;
        	std::cout << e.what();
        	exit(1);
	}

	log4cpp::Category& root = log4cpp::Category::getRoot();

	log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

	log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));
	logging = &log;
}

void end_log()
{
log4cpp::Category::shutdown();
}
