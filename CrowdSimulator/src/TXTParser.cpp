#include "TXTParser.h"

bool TXTParser::loadAgents(std::string _filename, const std::map<std::string, ngl::Obj *> &_dummies, std::vector<Agent*> &_agents)
{

    std::ifstream agentsFile;

    agentsFile.open(_filename.c_str(),std::ifstream::in);

    if (!agentsFile.is_open())
    {
        std::cout << "TXTParser: ERROR: " << _filename << " not found" << std::endl;
        return false;
    }

    std::string lineBuffer;
    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> >::iterator currentToken;

    Agent *agent;
    ngl::Vec4 position;
    ngl::Vec4 heading;
    std::string attributeKey, attributeValue;
    int numberOfAttributes;
    int lineNumber = 1;
    while (!agentsFile.eof())
    {
        std::getline(agentsFile,lineBuffer,'\n');
        //std::cout << lineNumber << " " << lineBuffer << std::endl;
        if (lineBuffer.size()!=0)
        {
            try
            {
                boost::tokenizer<boost::char_separator<char> > tokens(lineBuffer,sep);
                currentToken = tokens.begin();

                if (*currentToken!="#")
                {
                    // CREATING AGENT
                    agent = new Agent();

                    //POSITION
                    position.m_x = boost::lexical_cast<float>(*currentToken);
                    ++currentToken;
                    position.m_y = boost::lexical_cast<float>(*currentToken);
                    ++currentToken;
                    position.m_z = boost::lexical_cast<float>(*currentToken);
                    ++currentToken;
                    agent->setPosition(position);

                    //MASS
                    agent->setMass(boost::lexical_cast<float>(*currentToken));
                    ++currentToken;

                    //MAXSTRENGTH
                    agent->setMaxStrength(boost::lexical_cast<float>(*currentToken));
                    ++currentToken;

                    //MAXSPEED
                    agent->setMaxSpeed(boost::lexical_cast<float>(*currentToken));
                    ++currentToken;

                    //VISIONRADIUS
                    agent->setVisionRadius(boost::lexical_cast<float>(*currentToken));
                    ++currentToken;

                    //STATE
                    agent->setState(boost::lexical_cast<std::string>(*currentToken));
                    ++currentToken;

                    //BRAIN
                    agent->setBrain(boost::lexical_cast<std::string>(*currentToken));
                    ++currentToken;

                    //HEADING
                    heading.m_x = boost::lexical_cast<float>(*currentToken);
                    ++currentToken;
                    heading.m_y = boost::lexical_cast<float>(*currentToken);
                    ++currentToken;
                    heading.m_z = boost::lexical_cast<float>(*currentToken);
                    agent->setHeading(heading);
                    ++currentToken;

                    //DUMMY INDEX
                    std::string dummyName = boost::lexical_cast<std::string>(*currentToken);
                    if (_dummies.count(dummyName)==1)
                        agent->setDummy(_dummies.at(dummyName));
                    else
                        std::cout << "TXTParser: Dummy " << dummyName << " not found" << std::endl;

                    //NUMBER OF ATTRIBUTES
                    std::getline(agentsFile,lineBuffer,'\n');
                    ++lineNumber;
                    numberOfAttributes = atoi(lineBuffer.c_str());

                    for(int i=0; i<numberOfAttributes; ++i)
                    {
                        ++lineNumber;

                        std::getline(agentsFile,lineBuffer,'\n');
                        //std::cout << lineNumber << " " << lineBuffer << std::endl;
                        boost::tokenizer<boost::char_separator<char> > attributeTokens(lineBuffer,sep);

                        currentToken = attributeTokens.begin();

                        attributeKey = boost::lexical_cast<std::string>(*currentToken);
                        ++currentToken;
                        attributeValue = boost::lexical_cast<std::string>(*currentToken);

                        agent->addAttribute(attributeKey,attributeValue);
                    }

                    _agents.push_back(agent);
                }
            }
            catch (...)
            {
                // this line might be dangerous...
                delete agent;
                std::cout << "TXTParser: " << _filename << ": line " << lineNumber << ": EXCEPTION" << std::endl;
                break;
            }
        }
        ++lineNumber;
    }

    agentsFile.close();

    return true;

}
