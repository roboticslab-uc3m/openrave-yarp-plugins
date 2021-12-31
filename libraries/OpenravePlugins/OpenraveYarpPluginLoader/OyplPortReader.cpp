// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Vocab.h>

#include "LogComponent.hpp"
#include "OpenraveYarpPluginLoader.hpp"
#include "OyplPortReader.hpp"

using namespace roboticslab;

// -----------------------------------------------------------------------------

constexpr auto VOCAB_OK = yarp::os::createVocab32('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab32('f','a','i','l');

// -----------------------------------------------------------------------------

bool OyplPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    yCDebug(ORYPL) << "Request:" << request.toString();
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if ( request.get(0).asString() == "help" ) //-- help
    {
        response.addString("Available commands: help, list, open (device ...) ...");
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "list" ) //-- list
    {
        openraveYarpPluginLoaderPtr->addYarpPluginsLists(response);
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "open" ) //-- open
    {
        std::string cmdStr;
        for(size_t i=1; i<request.size(); i++)
        {
            if(!request.get(i).isList())
            {
                yCError(ORYPL) << "Expected list at" << i;
                response.addVocab32(VOCAB_FAILED);
                response.addString("Expected list");
                return response.write(*out);
            }
            yarp::os::Bottle* elem = request.get(i).asList();
            cmdStr.append("--");
            cmdStr.append(elem->toString());
            cmdStr.append(" ");
        }
        yCDebug(ORYPL) << cmdStr;

        std::stringstream sout;
        std::stringstream sinput(cmdStr);

        if(!openraveYarpPluginLoaderPtr->Open(sout, sinput))
        {
            response.addVocab32(VOCAB_FAILED);
            response.addString("Open failed");
            return response.write(*out);
        }
        response.addVocab32(VOCAB_OK);
        int value;
        while(sout >> value)
        {
           response.addInt32(value);
        }
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "getPenv" ) //-- getPenv
    {
        OpenRAVE::EnvironmentBasePtr penv = openraveYarpPluginLoaderPtr->GetEnv();
        yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
        yCDebug(ORYPL) << "penvValue:" << v.toString();
        response.add(v); // penvValue.isBlob()
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "close" ) //-- close
    {
        if(request.size() < 2)
        {
            response.addVocab32(VOCAB_FAILED);
            response.addString("close requires at least 1 argument");
            return response.write(*out);
        }
        for(size_t i=1; i<request.size(); i++)
        {
            if(!openraveYarpPluginLoaderPtr->close(request.get(i).asInt32()))
            {
                response.addVocab32(VOCAB_FAILED);
                response.addString("close failed");
                response.addInt32(request.get(i).asInt32());
                return response.write(*out);
            }
        }
        response.addVocab32(VOCAB_OK);
        return response.write(*out);
    }

    response.addVocab32(VOCAB_FAILED);
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------
