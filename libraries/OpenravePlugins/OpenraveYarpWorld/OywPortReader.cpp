// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>

#include <ColorDebug.h>

#include "OpenraveYarpWorld.hpp"

#include "OywPortReader.hpp"

// -----------------------------------------------------------------------------

const yarp::conf::vocab32_t OywPortReader::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t OywPortReader::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

// -----------------------------------------------------------------------------

bool OywPortReader::checkIfString(yarp::os::Bottle& request, int index, yarp::os::Bottle& response)
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

bool OywPortReader::read(yarp::os::ConnectionReader& in)
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
info, \
world del all, \
world mk box/sbox (three params for size) (three params for pos), \
world mk cyl/scyl (radius height) (three params for pos), \
world mk sph/ssph (radius) (three params for pos), \
world mk mesh (no params yet), \
world mk file (absolute path), \
world mv (objName) (three params for pos), \
world grab (manipulatorName) (objName) 0/1, \
world whereis obj (objName), \
world whereis tcp (manipulator), \
world draw 0/1 (radius r g b).");
        return response.write(*out);
    }
    else if (choice == "info") //-- info
    {
        openraveYarpWorldPtr->addWorldInfo(response);
        return response.write(*out);
    }
    else if (choice=="world") //-- world
    {
        if (!checkIfString(request, 1, response))
            return response.write(*out);
        if (request.get(1).asString() == "mk")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            {
                // lock the environment!
                OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");

                std::string objName;
                bool objIsStatic = true; // this is the OpenRAVE default
                if ((request.get(2).asString() == "box")||(request.get(2).asString() == "sbox"))
                {
                    std::vector<OpenRAVE::AABB> boxes(1);
                    boxes[0].extents = OpenRAVE::Vector(request.get(3).asFloat64(), request.get(4).asFloat64(), request.get(5).asFloat64());
                    boxes[0].pos = OpenRAVE::Vector(request.get(6).asFloat64(), request.get(7).asFloat64(), request.get(8).asFloat64());
                    objKinBodyPtr->InitFromBoxes(boxes,true);
                    objKinBodyPtr->GetLinks()[0]->SetMass(1);
                    if(request.get(2).asString() == "box")
                    {
                        objIsStatic = false;
                        OpenRAVE::Vector inertia(1,1,1);
                        objKinBodyPtr->GetLinks()[0]->SetPrincipalMomentsOfInertia(inertia);
                        OpenRAVE::Transform pose(OpenRAVE::Vector(1,0,0,0),OpenRAVE::Vector(0,0,0));
                        objKinBodyPtr->GetLinks()[0]->SetLocalMassFrame(pose);

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
                    std::list<OpenRAVE::KinBody::Link::GeometryInfo> scylInfoList;
                    OpenRAVE::KinBody::Link::GeometryInfo scylInfo;
                    scylInfo._type = OpenRAVE::KinBody::Link::GeomCylinder;
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
                    OpenRAVE::KinBody::Link::TRIMESH raveMesh;
                    raveMesh.indices.resize(6);
                    raveMesh.indices[0]=0;
                    raveMesh.indices[1]=1;
                    raveMesh.indices[2]=2;
                    raveMesh.indices[3]=3;
                    raveMesh.indices[4]=4;
                    raveMesh.indices[5]=5;
                    raveMesh.vertices.resize(6);
                    raveMesh.vertices[0] = OpenRAVE::Vector(1.0,1.0,1.0);
                    raveMesh.vertices[1] = OpenRAVE::Vector(1.0,1.5,1.0);
                    raveMesh.vertices[2] = OpenRAVE::Vector(1.5,1.0,1.0);
                    raveMesh.vertices[3] = OpenRAVE::Vector(1.0,1.5,1.0);
                    raveMesh.vertices[4] = OpenRAVE::Vector(1.5,1.0,1.0);
                    raveMesh.vertices[5] = OpenRAVE::Vector(1.5,1.5,1.5);
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
                    pEnv->ReadKinBodyXMLFile(objKinBodyPtr, fileName);
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

                objKinBodyPtr->SetName(objName);
                pEnv->Add(objKinBodyPtr,true);
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
        else if (request.get(1).asString()=="mv")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(request.get(2).asString().c_str());
            if(!objPtr)
            {
                CD_ERROR("object %s does not exist.\n", request.get(3).asString().c_str());
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
        else if ((request.get(1).asString()=="del")&&(request.get(2).asString()=="all"))
        {
            for (unsigned int i=0; i<objKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(objKinBodyPtrs[i]);
            }
            objKinBodyPtrs.clear();
            response.addVocab(VOCAB_OK);
        }
        else if (request.get(1).asString()=="grab")
        {
            // -- rpc command to write: world + grab + "part of robot" + name object + 0
            // --                         0       1           2              3         4
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            try
            {
                pRobot->SetActiveManipulator(request.get(2).asString()); // <in.get(2).asString()> will have to be the robot manipulator used in XML file. E.g: rigthArm for TEO"
            }
            catch (const std::exception& ex)
            {
                CD_ERROR("Caught openrave_exception: %s\n", ex.what());
                response.addVocab(VOCAB_FAILED);
                return response.write(*out);
            }
            if (!checkIfString(request, 3, response))
                return response.write(*out);
            OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(request.get(3).asString().c_str());
            if(objPtr)
            {
                CD_SUCCESS("object %s exists.\n", request.get(3).asString().c_str());
                if (request.get(4).asInt32()==1)
                {
                    CD_INFO("The object is grabbed!!\n");
                    pRobot->Grab(objPtr);
                    response.addVocab(VOCAB_OK);
                }
                else if (request.get(4).asInt32()==0)
                {
                    CD_INFO("The object is released!!\n");
                    pRobot->Release(objPtr);
                    response.addVocab(VOCAB_OK);
                }
                else response.addVocab(VOCAB_FAILED);
            }
            else // null pointer
            {
                CD_WARNING("object %s does not exist.\n", request.get(3).asString().c_str());
                response.addVocab(VOCAB_FAILED);
            }
        }
        else if (request.get(1).asString()=="whereis")
        {
            if (!checkIfString(request, 2, response))
                return response.write(*out);
            if (request.get(2).asString()=="obj")
            {
                if (!checkIfString(request, 3, response))
                    return response.write(*out);
                OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(request.get(3).asString().c_str());
                if(objPtr)
                {
                    //Transform t = objPtr->GetTransform();
                    OpenRAVE::Vector tr = objPtr->GetTransform().trans;
                    CD_SUCCESS("object %s at %f, %f, %f.\n", objPtr->GetName().c_str(), tr.x,tr.y,tr.z);
                    yarp::os::Bottle trans;
                    trans.addFloat64(tr.x);
                    trans.addFloat64(tr.y);
                    trans.addFloat64(tr.z);
                    response.addList() = trans;
                    response.addVocab(VOCAB_OK);
                }
                else // null pointer
                {
                    CD_ERROR("object %s does not exist.\n", request.get(3).asString().c_str());
                    response.addVocab(VOCAB_FAILED);
                    response.addString("object does not exist");
                }
            }
            else if (request.get(2).asString()=="tcp")
            {
                std::vector<OpenRAVE::RobotBasePtr> robots;
                pEnv->GetRobots(robots);
                OpenRAVE::RobotBasePtr robotPtr = robots.at(0);  //-- For now, we use only the first robot
                if (!checkIfString(request, 3, response))
                    return response.write(*out);
                pRobotManip = robotPtr->GetManipulator(request.get(3).asString()); //-- <in.get(3).asString()> will have to be the robot manipulator used in XML file. E.g: rigthArm for TEO"
                OpenRAVE::Transform ee = pRobotManip->GetEndEffector()->GetTransform();
                OpenRAVE::Transform tool;
                //tool.trans = Vector(0.0,0.0,1.3);
                tool.rot = OpenRAVE::geometry::quatFromAxisAngle(OpenRAVE::Vector(0,0,0)); //-- Converts an axis-angle rotation into a quaternion.
                tool.rot = ee.rot;
                OpenRAVE::Transform tcp = ee * tool;
                //Transform tcp = ee;
                CD_SUCCESS("TCP at %f, %f, %f.\n", tcp.trans.x, tcp.trans.y, tcp.trans.z);
                yarp::os::Bottle trans;
                trans.addFloat64(tcp.trans.x);
                trans.addFloat64(tcp.trans.y);
                trans.addFloat64(tcp.trans.z);
                response.addList() = trans;
                response.addVocab(VOCAB_OK);
            }
            else
            {
                CD_WARNING("where is what?\n");
                response.addVocab(VOCAB_FAILED);
            }

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
        else response.addVocab(VOCAB_FAILED);
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
