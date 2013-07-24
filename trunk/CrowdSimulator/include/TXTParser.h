#ifndef TXTPARSER_H
#define TXTPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "Parser.h"

class TXTParser : public Parser
{

public:
    bool loadCrowd(std::string _filename, std::vector<Agent*> &_agents);

};

#endif // TXTPARSER_H
