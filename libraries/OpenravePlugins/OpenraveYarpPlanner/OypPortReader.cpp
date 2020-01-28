// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>

#include <ColorDebug.h>

#include "OpenraveYarpPlanner.hpp"

#include "OypPortReader.hpp"

// -----------------------------------------------------------------------------

const yarp::conf::vocab32_t roboticslab::OypPortReader::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t roboticslab::OypPortReader::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

// -----------------------------------------------------------------------------

bool roboticslab::OypPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    CD_DEBUG("Request: %s\n", request.toString().c_str());
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if ( request.get(0).asString() == "help" ) //-- help
    {
        response.addString("Available commands: help, plan, goto");
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "plan" ) //-- plan
    {
        OpenRAVE::ModuleBasePtr pbasemanip = RaveCreateModule(openraveYarpPlannerPtr->GetEnv(),"basemanipulation");
        openraveYarpPlannerPtr->GetEnv()->Add(pbasemanip,true,"BarrettWAM"); // load the module
        {
            OpenRAVE::EnvironmentMutex::scoped_lock lock(openraveYarpPlannerPtr->GetEnv()->GetMutex()); // lock environment

            std::stringstream cmdin, cmdout;
            cmdin << "MoveManipulator goal ";
            cmdin << request.tail().toString();
            CD_DEBUG("%s\n", cmdin.str().c_str());

            if( !pbasemanip->SendCommand(cmdout,cmdin) )
            {
                response.addVocab(VOCAB_FAILED);
                response.write(*out);
                return true;
            }
        }

        response.addVocab(VOCAB_OK);
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "goto" ) //-- goto
    {

        response.addVocab(VOCAB_OK);
        return response.write(*out);
    }

    response.addVocab(VOCAB_FAILED);
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------
