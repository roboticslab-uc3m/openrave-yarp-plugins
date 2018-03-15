// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>
#include <boost/bind.hpp>

#include "ColorDebug.hpp"

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveDumpManipulatorJointLinks
 *
 * @brief Contains roboticslab::OpenraveDumpManipulatorJointLinks.
 */

/**
 * @ingroup OpenraveDumpManipulatorJointLinks
 * @brief List joints and affected links.
 */
class OpenraveDumpManipulatorJointLinks : public OpenRAVE::ModuleBase
{
public:
    OpenraveDumpManipulatorJointLinks(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
    {
        __description = "OpenraveDumpManipulatorJointLinks plugin.";
        RegisterCommand("open",boost::bind(&OpenraveDumpManipulatorJointLinks::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveDumpManipulatorJointLinks()
    {
    }

    virtual void Destroy()
    {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    int main(const std::string& cmd)
    {
        CD_DEBUG("[%s]\n", cmd.c_str());
        std::stringstream ss(cmd);

        //-- Fill openStrings
        std::vector<std::string> openStrings;
        while( ! ss.eof() )
        {
            std::string tmp;
            ss >> tmp;
            if(tmp == "open")
            {
                std::string openString("open");
                openStrings.push_back(openString);
            }
            else
            {
                if(openStrings.size() == 0)
                {
                    CD_ERROR("args must start with open, sorry! Bye!\n");
                    return 1;
                }
                openStrings[openStrings.size()-1].append(" ");
                openStrings[openStrings.size()-1].append(tmp);
            }
        }

        //-- Open each openString
        for(int i=0;i<openStrings.size();i++)
        {
            CD_DEBUG("open[%d]: [%s]\n",i,openStrings[i].c_str());

            std::istringstream sinput( openStrings[i] );
            std::ostringstream sout;
            if( ! SendCommand(sout,sinput) )
                return 1;
        }
        return 0;
    }

    bool Open(std::ostream& sout, std::istream& sinput)
    {
        //-- Get and put pointer to environment
        CD_INFO("penv: %p\n",GetEnv().get());

        OpenRAVE::EnvironmentBasePtr penv = GetEnv();

        std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
        penv->GetRobots(vectorOfRobotPtr);

        for(size_t robotIndex=0;robotIndex<vectorOfRobotPtr.size();robotIndex++)
        {
            OpenRAVE::RobotBasePtr robotPtr = vectorOfRobotPtr[ robotIndex ];

            CD_INFO_NO_HEADER("Robot [%d]: %s\n",robotIndex,robotPtr->GetName().c_str());

            std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = robotPtr->GetManipulators();

            for(size_t manipulatorIndex=0;manipulatorIndex<vectorOfManipulatorPtr.size();manipulatorIndex++)
            {
                OpenRAVE::RobotBase::ManipulatorPtr manipulatorPtr = vectorOfManipulatorPtr[manipulatorIndex];

                CD_INFO_NO_HEADER("* Manipulator [%d,%d]: %s\n",robotIndex,manipulatorIndex,manipulatorPtr->GetName().c_str());

                std::vector<int> manipulatorJointIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

                std::vector<OpenRAVE::RobotBase::LinkPtr> vectorOfLinkPtr = robotPtr->GetLinks();

                for(size_t jointIndex=0;jointIndex<manipulatorJointIDs.size();jointIndex++)
                {
                    CD_INFO_NO_HEADER("** Joint [%d,%d,%d]: %s\n",robotIndex,manipulatorIndex,jointIndex,robotPtr->GetJointFromDOFIndex(manipulatorJointIDs[jointIndex])->GetName().c_str());

                    for(size_t linkIndex=0;linkIndex<vectorOfLinkPtr.size();linkIndex++)
                    {
                        OpenRAVE::RobotBase::LinkPtr linkPtr = vectorOfLinkPtr[linkIndex];
                        if( robotPtr->DoesAffect(manipulatorJointIDs[jointIndex],linkPtr->GetIndex()) )
                        {
                            CD_INFO_NO_HEADER("*** Affects: %s\n", linkPtr->GetName().c_str());
                        }
                    }
                }  // joint
            }  // manipulator
        }  // robot

        return true;
    }

private:
};

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openravedumpmanipulatorjointlinks")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveDumpManipulatorJointLinks(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveDumpManipulatorJointLinks");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
