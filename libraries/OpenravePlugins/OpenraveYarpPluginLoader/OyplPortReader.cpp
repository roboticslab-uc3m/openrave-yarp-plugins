// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/conf/version.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Vocab.h>

#include "OpenraveYarpPluginLoader.hpp"

#include "OyplPortReader.hpp"

using namespace roboticslab;

// -----------------------------------------------------------------------------

#if YARP_VERSION_MINOR >= 5
constexpr auto VOCAB_OK = yarp::os::createVocab32('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab32('f','a','i','l');
#else
constexpr auto VOCAB_OK = yarp::os::createVocab('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');
#endif

// -----------------------------------------------------------------------------

bool OyplPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    yDebug() << "Request:" << request.toString();
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
                yError() << "Expected list at" << i;
#if YARP_VERSION_MINOR >= 5
                response.addVocab32(VOCAB_FAILED);
#else
                response.addVocab(VOCAB_FAILED);
#endif
                response.addString("Expected list");
                return response.write(*out);
            }
            yarp::os::Bottle* elem = request.get(i).asList();
            cmdStr.append("--");
            cmdStr.append(elem->toString());
            cmdStr.append(" ");
        }
        yDebug() << cmdStr;

        std::stringstream sout;
        std::stringstream sinput(cmdStr);

        if(!openraveYarpPluginLoaderPtr->Open(sout, sinput))
        {
#if YARP_VERSION_MINOR >= 5
            response.addVocab32(VOCAB_FAILED);
#else
            response.addVocab(VOCAB_FAILED);
#endif
            response.addString("Open failed");
            return response.write(*out);
        }
#if YARP_VERSION_MINOR >= 5
        response.addVocab32(VOCAB_OK);
#else
        response.addVocab(VOCAB_OK);
#endif
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
        yDebug() << "penvValue:" << v.toString();
        response.add(v); // penvValue.isBlob()
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "close" ) //-- close
    {
        if(request.size() < 2)
        {
#if YARP_VERSION_MINOR >= 5
            response.addVocab32(VOCAB_FAILED);
#else
            response.addVocab(VOCAB_FAILED);
#endif
            response.addString("close requires at least 1 argument");
            return response.write(*out);
        }
        for(size_t i=1; i<request.size(); i++)
        {
            if(!openraveYarpPluginLoaderPtr->close(request.get(i).asInt32()))
            {
#if YARP_VERSION_MINOR >= 5
                response.addVocab32(VOCAB_FAILED);
#else
                response.addVocab(VOCAB_FAILED);
#endif
                response.addString("close failed");
                response.addInt32(request.get(i).asInt32());
                return response.write(*out);
            }
        }
#if YARP_VERSION_MINOR >= 5
        response.addVocab32(VOCAB_OK);
#else
        response.addVocab(VOCAB_OK);
#endif
        return response.write(*out);
    }

#if YARP_VERSION_MINOR >= 5
    response.addVocab32(VOCAB_FAILED);
#else
    response.addVocab(VOCAB_FAILED);
#endif
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------
