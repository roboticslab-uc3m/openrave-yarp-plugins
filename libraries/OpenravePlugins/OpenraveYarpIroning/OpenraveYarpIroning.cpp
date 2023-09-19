#include <cmath>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <openrave/config.h>
#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Property.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Value.h>

namespace
{
    YARP_LOG_COMPONENT(ORYPS, "rl.OpenraveYarpIroning")
}

constexpr auto DEFAULT_RATE_S = 0.1;
constexpr auto DEFAULT_IN_FILE_STR = "/home/yo/repos/roboticslab-uc3m/alma-playground/ironing/assets/map1.csv";

class OpenraveYarpIroning : public OpenRAVE::ModuleBase,
                            public yarp::os::PeriodicThread
{
public:
    OpenraveYarpIroning(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv), yarp::os::PeriodicThread(DEFAULT_RATE_S)
    {
        using namespace boost::placeholders;
        __description = "OpenraveYarpIroning plugin.";
        OpenRAVE::InterfaceBase::RegisterCommand("open", boost::bind(&OpenraveYarpIroning::Open, this, _1, _2), "opens OpenraveYarpIroning");

        yCInfo(ORYPS) << "Checking for yarp network... (1)";

        if (!yarp.checkNetwork())
        {
            yCError(ORYPS) << "Found no yarp network (try running \"yarpserver &\")";
        }

        yCInfo(ORYPS) << "Found yarp network";
    }

    ~OpenraveYarpIroning() override
    {
        //-- Note that we start on element 1, first elem was not via new!!
        for (size_t i = 1; i < argv.size(); i++)
        {
            delete[] argv[i];
            argv[i] = 0;
        }
    }

    void Destroy() override
    {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    int main(const std::string &cmd)
    {
        RAVELOG_INFO("module initialized with \"%s\"\n", cmd.c_str());
        // hard-coding "open"
        std::istringstream sinput("open");
        std::ostringstream sout;
        if (!OpenRAVE::InterfaceBase::SendCommand(sout, sinput))
            return 1;
        return 0;
    }

    bool Open(std::ostream &sout, std::istream &sinput)
    {
        yCInfo(ORYPS) << "Checking for yarp network... (2)";
        if (!yarp.checkNetwork())
        {
            yCError(ORYPS) << "Found no yarp network (try running \"yarpserver &\")";
            return false;
        }

        //-- Given "std::istream& sinput", create equivalent to "int argc, char *argv[]"
        //-- Note that char* != const char* given by std::string::c_str();
        const char *dummyProgramName = "dummyProgramName";
        argv.push_back(dummyProgramName);

        while (sinput)
        {
            std::string str;
            sinput >> str;
            if (str.empty()) //-- Omits empty string that is usually at end via openrave.
                continue;
            char *cstr = new char[str.length() + 1]; // pushed to member argv to be deleted in ~.
            std::strcpy(cstr, str.c_str());
            argv.push_back(cstr);
        }

        yarp::os::Property options;
        options.fromCommand(argv.size(), argv.data());

        yCDebug(ORYPS) << "Config:" << options.toString();

        std::string inFileStr = options.check("inFileStr", yarp::os::Value(DEFAULT_IN_FILE_STR), "input file").asString();
        yCInfo(ORYPS) << "Input file:" << inFileStr;

        file.open(inFileStr.c_str());
        if (!file.is_open())
        {
            printf("Not able to open file: %s\n", inFileStr.c_str());
            return false;
        }
        printf("Opened file: %s\n", inFileStr.c_str());
        std::vector<std::vector<int>> preIntMap;
        std::vector<int> intsOnFileLine;
        while (this->parseFileLine(intsOnFileLine))
        {
            if (intsOnFileLine.size() == 0)
                continue;
            preIntMap.push_back(intsOnFileLine);
        }
        intMap = transpose(preIntMap);

        this->dump();

        _penv = GetEnv();
        RAVELOG_INFO("penv: %p\n", _penv.get());

        /*_objPtr = _penv->GetKinBody("object");
        if (!_objPtr)
        {
            yCError(ORYPS) << "error: object \"object\" does not exist.";
            return false;
        }
        std::printf("sucess: object \"object\" exists.\n");*/

        std::vector<OpenRAVE::RobotBasePtr> robots;
        _penv->GetRobots(robots);
        if (robots.size() == 0)
        {
            yCError(ORYPS) << "No robot yet"; // default: teo
            return false;
        }
        OpenRAVE::RobotBasePtr probot = robots[0];
        yCInfo(ORYPS) << "Robot 0: " << probot->GetName(); // default: teo
        probot->SetActiveManipulator("rightArm");
        _pRobotManip = probot->GetManipulator("rightArm");

        {
// lock the environment!
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 68, 0)
            OpenRAVE::EnvironmentLock lock(_penv->GetMutex());
#else
            OpenRAVE::EnvironmentMutex::scoped_lock lock(_penv->GetMutex());
#endif

            double tableX = 0.4;
            double tableY = 0.4;
            //-- layer 1
            for (unsigned int sboxIdxX = 0; sboxIdxX < intMap.size(); sboxIdxX++)
            {
                for (unsigned int sboxIdxY = 0; sboxIdxY < intMap[0].size(); sboxIdxY++)
                {
                    if ((intMap[sboxIdxX][sboxIdxY] == 1) || (intMap[sboxIdxX][sboxIdxY] == 2))
                    {
                        OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(_penv, "");
                        std::vector<OpenRAVE::AABB> boxes(1);
                        boxes[0].extents = OpenRAVE::Vector(tableX / (2.0 * intMap.size()), tableY / (2.0 * intMap[0].size()), 0.005);
                        boxes[0].pos = OpenRAVE::Vector(0.6 + (0.5 + sboxIdxX) * tableX / intMap.size(), -(tableY / 2.0) + (0.5 + sboxIdxY) * tableY / intMap[0].size(), 0.005 - 0.1);
                        objKinBodyPtr->InitFromBoxes(boxes, true);
                        std::string objName("sbox_w_");
                        std::ostringstream s;
                        s << sboxIdxX;
                        s << "_";
                        s << sboxIdxY;
                        objName.append(s.str());
                        objKinBodyPtr->SetName(objName);
                        objKinBodyPtr->GetLinks()[0]->GetGeometry(0)->SetDiffuseColor(OpenRAVE::RaveVector<float>(1.0, 1.0, 1.0));
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
                        _penv->Add(objKinBodyPtr, OpenRAVE::IAM_AllowRenaming);
#else
                        _penv->Add(objKinBodyPtr, true);
#endif
                        _objKinBodyPtrs.push_back(objKinBodyPtr);
                    }
                }
            }
            //-- layer 2
            for (unsigned int sboxIdxX = 0; sboxIdxX < intMap.size(); sboxIdxX++)
            {
                for (unsigned int sboxIdxY = 0; sboxIdxY < intMap[0].size(); sboxIdxY++)
                {
                    if (intMap[sboxIdxX][sboxIdxY] == 2)
                    {
                        OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(_penv, "");
                        std::vector<OpenRAVE::AABB> boxes(1);
                        boxes[0].extents = OpenRAVE::Vector(tableX / (2.0 * intMap.size()), tableY / (2.0 * intMap[0].size()), 0.005);
                        boxes[0].pos = OpenRAVE::Vector(0.6 + (0.5 + sboxIdxX) * tableX / intMap.size(), -(tableY / 2.0) + (0.5 + sboxIdxY) * tableY / intMap[0].size(), 0.015 - 0.1);
                        objKinBodyPtr->InitFromBoxes(boxes, true);
                        std::string objName("sbox_");
                        std::ostringstream s;
                        s << sboxIdxX;
                        s << "_";
                        s << sboxIdxY;
                        objName.append(s.str());
                        objKinBodyPtr->SetName(objName);
                        objKinBodyPtr->GetLinks()[0]->GetGeometry(0)->SetDiffuseColor(OpenRAVE::RaveVector<float>(0.0, 0.0, 1.0));
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
                        _penv->Add(objKinBodyPtr, OpenRAVE::IAM_AllowRenaming);
#else
                        _penv->Add(objKinBodyPtr, true);
#endif
                        _objKinBodyPtrs.push_back(objKinBodyPtr);
                    }
                }
            }
        }

        /*OpenRAVE::KinBodyPtr eeKinBodyPtr = OpenRAVE::RaveCreateKinBody(_penv, "");
        OpenRAVE::Transform ee = _pRobotManip->GetEndEffector()->GetTransform();
        OpenRAVE::Transform tool;
        tool.trans = OpenRAVE::Vector(0.0,0.0,-0.3);
        tool.rot = OpenRAVE::geometry::quatFromAxisAngle(OpenRAVE::Vector(0,0,0)); //-- Converts an axis-angle rotation into a quaternion.
        tool.rot = ee.rot;
        OpenRAVE::Transform tcp = ee * tool;
        std::vector<OpenRAVE::AABB> boxes(1);
        boxes[0].extents = OpenRAVE::Vector(0.01, 0.01, 0.01);
        boxes[0].pos = OpenRAVE::Vector(tcp.trans.x, tcp.trans.y, tcp.trans.z);
        eeKinBodyPtr->InitFromBoxes(boxes,true);
        std::string objName("myee");
        eeKinBodyPtr->SetName(objName);
        #if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
            _penv->Add(eeKinBodyPtr, OpenRAVE::IAM_AllowRenaming);
        #else
            _penv->Add(eeKinBodyPtr, true);
        #endif*/

        this->start(); // start yarp::os::PeriodicThread (calls run periodically)

        return true;
    }

    void run() override
    {
        // yCInfo(ORYPS) << "thread";

        OpenRAVE::Transform ee = _pRobotManip->GetEndEffector()->GetTransform();
        OpenRAVE::Transform tool;
        tool.trans = OpenRAVE::Vector(0.0, 0.0, -0.3);
        tool.rot = OpenRAVE::geometry::quatFromAxisAngle(OpenRAVE::Vector(0, 0, 0)); //-- Converts an axis-angle rotation into a quaternion.
        tool.rot = ee.rot;
        OpenRAVE::Transform tcp = ee * tool;
        // Transform tcp = ee;
        // yCInfo(ORYPS) << "TCP at" << tcp.trans.x << tcp.trans.y << tcp.trans.z;

        for (unsigned int objKinBodyIdx = 0; objKinBodyIdx < _objKinBodyPtrs.size(); objKinBodyIdx++)
        {
            // yCInfo(ORYPS) << _objKinBodyPtrs[objKinBodyIdx]->GetName();
            OpenRAVE::Transform T_base_object = _objKinBodyPtrs[objKinBodyIdx]->GetLinks()[0]->GetGeometry(0)->GetTransform();
            // yCInfo(ORYPS) << T_base_object.trans.x << T_base_object.trans.y << T_base_object.trans.z << "| TCP" << tcp.trans.x << tcp.trans.y << tcp.trans.z;
            double dist = std::sqrt(std::pow(T_base_object.trans.x - tcp.trans.x, 2) + std::pow(T_base_object.trans.y - tcp.trans.y, 2) + std::pow(T_base_object.trans.z - tcp.trans.z, 2));

            if (dist < 0.05)
            {
                yCInfo(ORYPS) << "Erase" << _objKinBodyPtrs[objKinBodyIdx]->GetName();
                _penv->Remove(_objKinBodyPtrs[objKinBodyIdx]);
                _objKinBodyPtrs.erase(_objKinBodyPtrs.begin() + objKinBodyIdx);
            }
        }
    }

private:
    std::vector<const char *> argv;

    yarp::os::Network yarp;

    OpenRAVE::EnvironmentBasePtr _penv;
    std::vector<OpenRAVE::KinBodyPtr> _objKinBodyPtrs;
    OpenRAVE::RobotBase::ManipulatorPtr _pRobotManip;

    std::ifstream file;
    std::vector<std::vector<int>> intMap;

    bool parseFileLine(std::vector<int> &intsOnFileLine)
    {
        intsOnFileLine.clear();

        if (file.eof())
            return false;

        std::string csv;
        getline(file, csv);
        std::istringstream buffer(csv);
        std::string token;
        int d;

        while (std::getline(buffer, token, ','))
        {
            std::istringstream ss(token);
            ss >> d;
            intsOnFileLine.push_back(d);
        }

        return true;
    }

    void dump()
    {
        for (int i = 0; i < intMap.size(); i++)
        {
            for (int j = 0; j < intMap[0].size(); j++)
            {
                printf("%d ", intMap[i][j]);
            }
            printf("\n");
        }
    }

    std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>> data)
    {
        // this assumes that all inner vectors have the same size and
        // allocates space for the complete result in advance
        std::vector<std::vector<int>> result(data[0].size(), std::vector<int>(data.size()));
        for (std::vector<int>::size_type i = 0; i < data[0].size(); i++)
            for (std::vector<int>::size_type j = 0; j < data.size(); j++)
            {
                result[i][j] = data[j][i];
            }
        return result;
    }
};

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
class OpenraveYarpIroningPlugin : public RavePlugin
{
public:
    OpenRAVE::InterfaceBasePtr CreateInterface(OpenRAVE::InterfaceType type,
                                               const std::string &interfacename,
                                               std::istream &sinput,
                                               OpenRAVE::EnvironmentBasePtr penv) override
    {
        if (type == OpenRAVE::PT_Module && interfacename == "openraveyarpironing")
        {
            return OpenRAVE::InterfaceBasePtr(new OpenraveYarpIroning(penv));
        }

        return OpenRAVE::InterfaceBasePtr();
    }

    const InterfaceMap &GetInterfaces() const override
    {
        static const RavePlugin::InterfaceMap interfaces = {
            {OpenRAVE::PT_Module, {"OpenraveYarpIroning"}},
        };

        return interfaces;
    }

    const std::string &GetPluginName() const override
    {
        static const std::string pluginName = "OpenraveYarpIroningPlugin";
        return pluginName;
    }
};

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API RavePlugin *CreatePlugin()
{
    return new OpenraveYarpIroningPlugin();
}
#else  // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type,
                                                    const std::string &interfacename,
                                                    std::istream &sinput,
                                                    OpenRAVE::EnvironmentBasePtr penv)
{
    if (type == OpenRAVE::PT_Module && interfacename == "openraveyarpironing")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpIroning(penv));
    }

    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO &info)
{
    info.interfacenames[OpenRAVE::PT_Module].emplace_back("OpenraveYarpIroning");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
#endif // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
