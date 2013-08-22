#ifndef PARSER_H
#define PARSER_H

/**
 * @file Parser.h
 * @brief This abstract class represents the interface of a parser to load agents from files with different formats
 */

#include <string>
#include "Agent.h"

/**
 * @class Parser
 * @brief This abstract class represents the interface of a parser to load agents from files with different formats
 */

class Parser
{

public:
    /**
     * @brief Load agents from a file
     */
    virtual bool loadAgents(std::string _fileName, const std::map<std::string,ngl::Obj*> &_dummies, std::vector<Agent*> &_agents) = 0;

};

#endif // PARSER_H
