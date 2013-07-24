#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Agent.h"

class Parser
{

protected:
    static const std::string s_crowdsPath;

public:
    virtual bool loadCrowd(std::string _fileName, std::vector<Agent*> &_agents) = 0;

};

#endif // PARSER_H
