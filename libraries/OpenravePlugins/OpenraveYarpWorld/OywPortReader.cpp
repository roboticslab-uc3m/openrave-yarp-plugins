// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <algorithm> // std::equal
#include <string>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ResourceFinder.h>

#include <ColorDebug.h>

#include "OpenraveYarpWorld.hpp"

#include "OywPortReader.hpp"

namespace
{
    inline bool endsWith(const std::string & base, const std::string & suffix)
    {
        // https://stackoverflow.com/a/2072890/10404307
        if (suffix.size() > base.size()) return false;
        return std::equal(suffix.rbegin(), suffix.rend(), base.rbegin());
    }
}

// -----------------------------------------------------------------------------

const yarp::conf::vocab32_t roboticslab::OywPortReader::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t roboticslab::OywPortReader::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

// -----------------------------------------------------------------------------

bool roboticslab::OywPortReader::checkIfString(yarp::os::Bottle& request, int index, yarp::os::Bottle& response)
{
    if (request.get(index).isString())
        return true;
    response.addVocab(VOCAB_FAILED);
    std::stringstream ss;
    ss << "expected type string but got wrong data type at ";
    ss << index;
    response.addString(ss.str());
    ss << "\n";
    CD_ERROR(ss.str().c_str(), index);
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::OywPortReader::tryToSetActiveManipulator(const std::string& robot, const std::string& manipulator, yarp::os::Bottle& response)
{
    OpenRAVE::RobotBasePtr wantActiveRobotPtr = openraveYarpWorldPtr->GetEnv()->GetRobot(robot);
    if(!wantActiveRobotPtr)
    {
        CD_ERROR("Could not find robot: %s.\n", robot.c_str());
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    try
    {
        wantActiveRobotPtr->SetActiveManipulator(manipulator);
    }
    catch (const std::exception& ex)
    {
        CD_ERROR("Caught openrave_exception: %s\n", ex.what());
        response.addVocab(VOCAB_FAILED);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::OywPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    CD_DEBUG("Request: %s\n", request.toString().c_str());
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if (!checkIfString(request, 0, response))
        return response.write(*out);
    std::string choice = request.get(0).asString();

    if (choice=="help") //-- help
    {
        response.addString("Available commands: \
help, \
list, \
world delall created, \
world del (objName), \
world fk (robotName) (manipulatorName), \
world get (objName), \
world grab (robotName) (manipulatorName) (objName) 0/1, \
world mk box/sbox (three params for size) (three params for pos), \
world mk cyl/scyl (radius height) (three params for pos), \
world mk sph/ssph (radius) (three params for pos), \
world mk mesh (no params yet), \
world mk file (fileName) (three params for pos) [four params for axis angle (deg)]], \
world set (objName) (three params for pos), \
world draw 0/1 (radius r g b).");
        return response.write(*out);
    }
    else if (choice == "list") //-- list
    {
        openraveYarpWorldPtr->addWorldInfo(response);
        return response.write(*out);
    }
    else if (choice=="world") //-- world
    {
        if (!checkIfString(request, 1, response))
            return response.write(*out);
        if (request.get(1).asString()=="delall")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            if (request.get(2).asString()=="created")
            {
                for (unsigned int i=0; i<objKinBodyPtrs.size(); i++)
                {
                    openraveYarpWorldPtr->GetEnv()->Remove(objKinBodyPtrs[i]);
                }
                objKinBodyPtrs.clear();
                response.addVocab(VOCAB_OK);
            }
        }
        if (request.get(1).asString()=="del")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            OpenRAVE::KinBodyPtr objPtr = openraveYarpWorldPtr->GetEnv()->GetKinBody(request.get(2).asString());
            if(objPtr)
            {
                openraveYarpWorldPtr->GetEnv()->Remove(objPtr);
                response.addVocab(VOCAB_OK);
            }
            else
            {
                CD_ERROR("object %s does not exist.\n", request.get(2).asString().c_str());
                response.addVocab(VOCAB_FAILED);
                response.addString("object does not exist");
            }
        }
        else if (request.get(1).asString()=="fk")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            if (!checkIfString(request, 3, response))
                return response.write(*out);
            OpenRAVE::RobotBasePtr fkRobotPtr = openraveYarpWorldPtr->GetEnv()->GetRobot(request.get(2).asString());
            if(!fkRobotPtr)
            {
                CD_ERROR("Could not find robot: %s.\n", request.get(2).asString().c_str());
                response.addVocab(VOCAB_FAILED);
                return response.write(*out);
            }
            OpenRAVE::RobotBase::ManipulatorPtr pRobotManip = fkRobotPtr->GetManipulator(request.get(3).asString());
            if(!pRobotManip)
            {
                CD_ERROR("Could not find manipulator: %s.\n", request.get(3).asString().c_str());
                response.addVocab(VOCAB_FAILED);
                return response.write(*out);
            }
            OpenRAVE::Transform ee = pRobotManip->GetEndEffector()->GetTransform();
            OpenRAVE::Transform tool;
            //tool.trans = Vector(0.0,0.0,1.3);
            tool.rot = OpenRAVE::geometry::quatFromAxisAngle(OpenRAVE::Vector(0,0,0)); //-- Converts an axis-angle rotation into a quaternion.
            tool.rot = ee.rot;
            OpenRAVE::Transform tcp = ee * tool;
            //Transform tcp = ee;
            CD_SUCCESS("TCP at %f, %f, %f.\n", tcp.trans.x, tcp.trans.y, tcp.trans.z);
            yarp::os::Bottle& trans = response.addList();
            trans.addFloat64(tcp.trans.x);
            trans.addFloat64(tcp.trans.y);
            trans.addFloat64(tcp.trans.z);
            response.addVocab(VOCAB_OK);
        }
        else if (request.get(1).asString()=="get")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            OpenRAVE::KinBodyPtr objPtr = openraveYarpWorldPtr->GetEnv()->GetKinBody(request.get(2).asString());
            if(objPtr)
            {
                //Transform t = objPtr->GetTransform();
                OpenRAVE::Vector tr = objPtr->GetTransform().trans;
                CD_SUCCESS("object %s at %f, %f, %f.\n", objPtr->GetName().c_str(), tr.x,tr.y,tr.z);
                yarp::os::Bottle& trans = response.addList();
                trans.addFloat64(tr.x);
                trans.addFloat64(tr.y);
                trans.addFloat64(tr.z);
                response.addVocab(VOCAB_OK);
            }
            else // null pointer
            {
                CD_ERROR("object %s does not exist.\n", request.get(3).asString().c_str());
                response.addVocab(VOCAB_FAILED);
                response.addString("object does not exist");
            }
        }
        else if (request.get(1).asString()=="grab")
        {
            // -- (0)world + (1)grab + (2)robotName + (3)manipulatorName + (4)objectName + (5)0/1
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            if (!checkIfString(request, 3, response))
                return response.write(*out);
            if (!checkIfString(request, 4, response))
                return response.write(*out);
            if (!tryToSetActiveManipulator(request.get(2).asString(), request.get(3).asString(), response))
                return response.write(*out);
            OpenRAVE::KinBodyPtr objPtr = openraveYarpWorldPtr->GetEnv()->GetKinBody(request.get(4).asString().c_str());
            if(objPtr)
            {
                CD_INFO("object %s exists.\n", request.get(4).asString().c_str());
                if (request.get(5).asInt32()==1)
                {
                    CD_SUCCESS("object grabbed!!\n");
                    openraveYarpWorldPtr->GetEnv()->GetRobot(request.get(2).asString())->Grab(objPtr); // robot was found at tryToSetActiveManipulator
                    response.addVocab(VOCAB_OK);
                }
                else if (request.get(5).asInt32()==0)
                {
                    CD_SUCCESS("object released!!\n");
                    openraveYarpWorldPtr->GetEnv()->GetRobot(request.get(2).asString())->Release(objPtr); // robot was found at tryToSetActiveManipulator
                    response.addVocab(VOCAB_OK);
                }
                else response.addVocab(VOCAB_FAILED);
            }
            else
            {
                CD_WARNING("object %s does not exist.\n", request.get(4).asString().c_str());
                response.addVocab(VOCAB_FAILED);
            }
        }
        else if (request.get(1).asString()=="mk")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            {
                // lock the environment!
                OpenRAVE::EnvironmentMutex::scoped_lock lock(openraveYarpWorldPtr->GetEnv()->GetMutex());
                OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(openraveYarpWorldPtr->GetEnv(),"");

                std::string objName;
                bool objIsStatic = true; // this is the OpenRAVE default
                if ((request.get(2).asString() == "box")||(request.get(2).asString() == "sbox"))
                {
                    std::vector<OpenRAVE::AABB> boxes(1);
                    boxes[0].extents = OpenRAVE::Vector(request.get(3).asFloat64(), request.get(4).asFloat64(), request.get(5).asFloat64());
                    boxes[0].pos = OpenRAVE::Vector(request.get(6).asFloat64(), request.get(7).asFloat64(), request.get(8).asFloat64());
                    objKinBodyPtr->InitFromBoxes(boxes,true);
                    if(request.get(2).asString() == "box")
                    {
                        objIsStatic = false;
                        objName.append("box_");
                        std::ostringstream s;
                        s << boxCount;
                        objName.append(s.str());
                        boxCount++;
                    }
                    else if (request.get(2).asString() == "sbox")
                    {
                        objName.append("sbox_");
                        std::ostringstream s;
                        s << sboxCount;
                        objName.append(s.str());
                        sboxCount++;
                    }
                }
                else if((request.get(2).asString() == "cyl")||(request.get(2).asString() == "scyl"))
                {
                    std::list<OpenRAVE::KinBody::GeometryInfo> scylInfoList;
                    OpenRAVE::KinBody::GeometryInfo scylInfo;
                    scylInfo._type = OpenRAVE::GeometryType::GT_Cylinder;
                    OpenRAVE::Transform pose(OpenRAVE::Vector(1,0,0,0),OpenRAVE::Vector(request.get(5).asFloat64(),request.get(6).asFloat64(),request.get(7).asFloat64()));
                    scylInfo._t = pose;
                    OpenRAVE::Vector volume;
                    volume.x = request.get(3).asFloat64();
                    volume.y = request.get(4).asFloat64();
                    scylInfo._vGeomData = volume;
                    scylInfo._bVisible = true;
                    //scylInfo._vDiffuseColor = [1,0,0];
                    scylInfoList.push_back(scylInfo);
                    objKinBodyPtr->InitFromGeometries(scylInfoList);
                    if(request.get(2).asString() == "cyl")
                    {
                        objIsStatic = false;
                        objName.append("cyl_");
                        std::ostringstream s;
                        s << cylCount;
                        objName.append(s.str());
                        cylCount++;
                    }
                    else if(request.get(2).asString() == "scyl")
                    {
                        objName.append("scyl_");
                        std::ostringstream s;
                        s << scylCount;
                        objName.append(s.str());
                        scylCount++;
                    }
                }
                else if((request.get(2).asString() == "sph")||(request.get(2).asString() == "ssph"))
                {
                    std::vector<OpenRAVE::Vector> spheres(1);
                    spheres.push_back( OpenRAVE::Vector(request.get(4).asFloat64(), request.get(5).asFloat64(), request.get(6).asFloat64(), request.get(3).asFloat64() ));
                    objKinBodyPtr->InitFromSpheres(spheres,true);
                    if(request.get(2).asString() == "sph")
                    {
                        objIsStatic = false;
                        objName.append("sph_");
                        std::ostringstream s;
                        s << sphCount;
                        objName.append(s.str());
                        sphCount++;
                    }
                    else if(request.get(2).asString() == "ssph")
                    {
                        objName.append("ssph_");
                        std::ostringstream s;
                        s << ssphCount;
                        objName.append(s.str());
                        ssphCount++;
                    }
                }
                else if (request.get(2).asString() == "mesh")
                {
                    objIsStatic = false;

                    const auto * vertices = request.get(3).asList();
                    const auto * indices = request.get(4).asList();

                    OpenRAVE::TriMesh raveMesh;
                    raveMesh.vertices.reserve(vertices->size() / 3);
                    raveMesh.indices.reserve(indices->size());

                    for (auto i = 0; i < vertices->size(); i += 3)
                    {
                        OpenRAVE::Vector point(
                            vertices->get(i).asFloat32(),
                            vertices->get(i + 1).asFloat32(),
                            vertices->get(i + 2).asFloat32()
                        );

                        raveMesh.vertices.push_back(point);
                    }

                    for (auto i = 0; i < indices->size(); i++)
                    {
                        raveMesh.indices.push_back(indices->get(i).asInt32());
                    }

                    OpenRAVE::Transform T;

                    if (request.size() > 5)
                    {
                        T.trans.x = request.get(5).asFloat64(); // [m]
                        T.trans.y = request.get(6).asFloat64(); // [m]
                        T.trans.z = request.get(7).asFloat64(); // [m]
                    }

                    if (request.size() > 8)
                    {
                        OpenRAVE::Vector vaxis(request.get(8).asFloat64(), request.get(9).asFloat64(), request.get(10).asFloat64());
                        OpenRAVE::dReal fangle = request.get(11).asFloat64();
                        T.rot = quatFromAxisAngle(vaxis, fangle * M_PI / 180.0f);
                    }

                    raveMesh.ApplyTransform(T);

                    objKinBodyPtr->InitFromTrimesh(raveMesh,true);

                    objName.append("mesh_");
                    std::ostringstream s;
                    s << meshCount;
                    objName.append(s.str());
                    meshCount++;
                }
                else if (request.get(2).asString() == "file")
                {
                    objIsStatic = false;
                    if (!checkIfString(request, 3, response))
                        return response.write(*out);
                    std::string fileName = request.get(3).asString();
                    yarp::os::ResourceFinder & rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
                    std::string fullFileName = rf.findFileByName(fileName);
                    if (fullFileName.empty())
                    {
                        CD_ERROR("Could not find '%s' file via yarp::os::ResourceFinder.\n", fileName.c_str());
                        response.addVocab(VOCAB_FAILED);
                        response.addString("could not load file");
                        return response.write(*out);
                    }
                    CD_INFO("Loading '%s' file.\n", fullFileName.c_str());

                    if (endsWith(fileName, ".ply"))
                    {
                        auto objTriMeshPtr = openraveYarpWorldPtr->GetEnv()->ReadTrimeshURI(nullptr, fullFileName);
                        if (objTriMeshPtr) objKinBodyPtr->InitFromTrimesh(*objTriMeshPtr);
                    }
                    else
                    {
                        objKinBodyPtr = openraveYarpWorldPtr->GetEnv()->ReadKinBodyURI(fullFileName);
                    }

                    if (!objKinBodyPtr)
                    {
                        CD_ERROR("Could not load '%s' file.\n", fullFileName.c_str());
                        response.addVocab(VOCAB_FAILED);
                        response.addString("could not load file");
                        return response.write(*out);
                    }

                    OpenRAVE::Transform T = objKinBodyPtr->GetTransform();
                    T.trans.x = request.get(4).asFloat64(); // [m]
                    T.trans.y = request.get(5).asFloat64(); // [m]
                    T.trans.z = request.get(6).asFloat64(); // [m]
                    if(request.size() > 7)
                    {
                        OpenRAVE::Vector vaxis(request.get(7).asFloat64(), request.get(8).asFloat64(), request.get(9).asFloat64());
                        OpenRAVE::dReal fangle = request.get(10).asFloat64();
                        T.rot = quatFromAxisAngle(vaxis, fangle * M_PI / 180.0f);
                    }
                    objKinBodyPtr->SetTransform(T);

                    objName.append("file_");
                    std::ostringstream s;
                    s << fileCount;
                    objName.append(s.str());
                    fileCount++;
                }
                else
                {
                    response.addVocab(VOCAB_FAILED);
                    return response.write(*out);
                }

                objKinBodyPtr->GetLinks()[0]->SetMass(1);
                OpenRAVE::Transform localMassFrame(OpenRAVE::Vector(1,0,0,0),OpenRAVE::Vector(0,0,0));
                objKinBodyPtr->GetLinks()[0]->SetLocalMassFrame(localMassFrame);
                OpenRAVE::Vector inertia(1,1,1);
                objKinBodyPtr->GetLinks()[0]->SetPrincipalMomentsOfInertia(inertia);

                objKinBodyPtr->SetName(objName);
                openraveYarpWorldPtr->GetEnv()->Add(objKinBodyPtr,true);
                if(!objIsStatic)
                {
                    objKinBodyPtr->GetLinks()[0]->SetStatic(false);
                }
                objKinBodyPtrs.push_back(objKinBodyPtr);

                CD_SUCCESS("Created: %s\n", objName.c_str());
                response.addVocab(VOCAB_OK);
                response.addString(objName);
            } // the environment is not locked anymore
        }
        else if (request.get(1).asString()=="set")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            OpenRAVE::KinBodyPtr objPtr = openraveYarpWorldPtr->GetEnv()->GetKinBody(request.get(2).asString().c_str());
            if(!objPtr)
            {
                CD_ERROR("object %s does not exist.\n", request.get(2).asString().c_str());
                response.addVocab(VOCAB_FAILED);
                response.addString("object does not exist");
                return response.write(*out);
            }
            OpenRAVE::Transform T = objPtr->GetTransform();
            T.trans.x = request.get(3).asFloat64();  // [m]
            T.trans.y = request.get(4).asFloat64();  // [m]
            T.trans.z = request.get(5).asFloat64();  // [m]
            objPtr->SetTransform(T);
            response.addVocab(VOCAB_OK);
        }
        else if (request.get(1).asString()=="draw")
        {
            if (request.get(2).asInt32() == 0)
            {
                CD_SUCCESS("Turning draw OFF.\n");
                robotDraw = 0;
                response.addVocab(VOCAB_OK);
            }
            else
            {
                CD_SUCCESS("Turning draw ON.\n");
                robotDraw = request.get(2).asInt32();
                if (request.size() >= 4) drawRadius = request.get(3).asFloat64();
                if (request.size() >= 7)
                {
                    drawR = request.get(4).asFloat64();
                    drawG = request.get(5).asFloat64();
                    drawB = request.get(6).asFloat64();
                }
                response.addVocab(VOCAB_OK);
            }

        }
        else
        {
            response.addVocab(VOCAB_FAILED);
        }
        return response.write(*out);
    }
    else
    {
        CD_ERROR("Command not understood, try 'help'.\n");
        response.addVocab(VOCAB_FAILED);
        response.addString("Command not understood, try 'help'");
    }
    return response.write(*out);
}

// -----------------------------------------------------------------------------
