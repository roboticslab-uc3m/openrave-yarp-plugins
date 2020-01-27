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
        response.addString("Available commands: help, list, open (device ...) ...");
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "list" ) //-- list
    {
        //openraveYarpPlannerPtr->addYarpPluginsLists(response);
        //response.addVocab(VOCAB_OK);
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "open" ) //-- open
    {
        std::string cmdStr;
        for(size_t i=1; i<request.size(); i++)
        {
            if(!request.get(i).isList())
            {
                CD_ERROR("Expected list at %d.\n",i);
                response.addVocab(VOCAB_FAILED);
                response.addString("Expected list");
                return response.write(*out);
            }
            yarp::os::Bottle* elem = request.get(i).asList();
            cmdStr.append("--");
            cmdStr.append(elem->toString());
            cmdStr.append(" ");
        }
        CD_DEBUG("%s\n", cmdStr.c_str());

        std::stringstream sout;
        std::stringstream sinput(cmdStr);

        if(!openraveYarpPlannerPtr->Open(sout, sinput))
        {
            response.addVocab(VOCAB_FAILED);
            response.addString("Open failed");
            return response.write(*out);
        }
        response.addVocab(VOCAB_OK);
        int value;
        while(sout >> value)
        {
           response.addInt32(value);
        }
        return response.write(*out);
    }

    response.addVocab(VOCAB_FAILED);
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------
