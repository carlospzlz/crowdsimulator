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
    bool loadAgents(std::string _filename, const std::map<std::string,ngl::Obj*> &_dummies, std::vector<Agent*> &_agents);

};

#endif // TXTPARSER_H
