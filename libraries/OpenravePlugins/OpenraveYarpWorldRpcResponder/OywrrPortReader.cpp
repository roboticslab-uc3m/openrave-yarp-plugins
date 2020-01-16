// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>

#include <ColorDebug.h>

#include "OywrrPortReader.hpp"

// -----------------------------------------------------------------------------

const yarp::conf::vocab32_t OywrrPortReader::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t OywrrPortReader::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

// -----------------------------------------------------------------------------

bool OywrrPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    CD_DEBUG("Request: %s\n", request.toString().c_str());
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if (!request.get(0).isString())
    {
        CD_ERROR("expected type string at 0 but got wrong data type!\n");
        response.addVocab(VOCAB_FAILED);
        response.write(*out);
        return true;
    }
    std::string choice = request.get(0).asString();

    if (choice=="help") //-- help
    {
        response.addString("Available commands: help, info (robots and environment information), world del all, world mk box/sbox (three params for size) (three params for pos), world mk ssph (radius) (three params for pos), world mk scyl (radius height) (three params for pos), world mk mesh (no params yet), world mk obj (absolute path), world mv (name) (three params for pos), world grab (manipulator) (obj) (num) 0/1, world whereis obj (name), world whereis tcp (manipulator),  world draw 0/1 (radius r g b).");
        response.write(*out);
        return true;
    }
    else if (choice == "info") //-- info
    {
        //-- Variable to save info
        std::stringstream info;

        //-- Get robots
        std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
        pEnv->GetRobots(vectorOfRobotPtr);

        //-- For each robot
        for(size_t robotPtrIdx=0; robotPtrIdx<vectorOfRobotPtr.size(); robotPtrIdx++)
        {
            info << "Robot ["<< robotPtrIdx <<"]"<<" named ["<<vectorOfRobotPtr[robotPtrIdx]->GetName().c_str()<<"]";
            info <<" with manipulators: ";

            //-- Get manipulators
            std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[robotPtrIdx]->GetManipulators();

            //-- For each manipulator
            for(size_t manipulatorPtrIdx=0; manipulatorPtrIdx<vectorOfManipulatorPtr.size(); manipulatorPtrIdx++)
            {
                info <<"("<<manipulatorPtrIdx<<")"<<vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName().c_str()<<" ";
            }
        }

        info << " // "; // -- to separate information!!

        // -- Get bodies
        std::vector<OpenRAVE::KinBodyPtr> vectorOfBodiesPtr;
        pEnv->GetBodies(vectorOfBodiesPtr);

        info << "Total bodies in the environment: ";

        // -- For each body
        for(size_t bodiesPtrIdx=0; bodiesPtrIdx<vectorOfBodiesPtr.size(); bodiesPtrIdx++)
        {
            info << "("<< bodiesPtrIdx <<")"<<vectorOfBodiesPtr[bodiesPtrIdx]->GetName().c_str()<<" ";
        }

        response.addString(info.str());
        response.write(*out);
        return true;
    }
    else if (choice=="world") //-- world
    {
        if (request.get(1).asString() == "mk")
        {
            if (request.get(2).asString() == "box")
            {
                {
                    // lock the environment!
                    OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                    OpenRAVE::KinBodyPtr boxKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                    std::string boxName("box_");
                    std::ostringstream s;  // boxName += std::to_string(boxKinBodyPtrs.size()+1);  // C++11 only
                    s << boxKinBodyPtrs.size()+1;
                    boxName += s.str();
                    boxKinBodyPtr->SetName(boxName.c_str());
                    //
                    std::vector<OpenRAVE::AABB> boxes(1);
                    boxes[0].extents = OpenRAVE::Vector(request.get(3).asFloat64(), request.get(4).asFloat64(), request.get(5).asFloat64());
                    boxes[0].pos = OpenRAVE::Vector(request.get(6).asFloat64(), request.get(7).asFloat64(), request.get(8).asFloat64());
                    boxKinBodyPtr->InitFromBoxes(boxes,true);
                    boxKinBodyPtr->GetLinks()[0]->SetMass(1);
                    OpenRAVE::Vector inertia(1,1,1);
                    boxKinBodyPtr->GetLinks()[0]->SetPrincipalMomentsOfInertia(inertia);
                    OpenRAVE::Transform pose(OpenRAVE::Vector(1,0,0,0),OpenRAVE::Vector(0,0,0));
                    boxKinBodyPtr->GetLinks()[0]->SetLocalMassFrame(pose);
                    //
                    pEnv->Add(boxKinBodyPtr,true);
                    boxKinBodyPtrs.push_back(boxKinBodyPtr);
                }  // the environment is not locked anymore
                boxKinBodyPtrs[boxKinBodyPtrs.size()-1]->GetLinks()[0]->SetStatic(false);
                response.addVocab(VOCAB_OK);
            }
            else if (request.get(2).asString() == "sbox")
            {
                {
                    // lock the environment!
                    OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                    OpenRAVE::KinBodyPtr sboxKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                    std::string sboxName("sbox_");
                    std::ostringstream s;  // sboxName += std::to_string(sboxKinBodyPtrs.size()+1);  // C++11 only
                    s << sboxKinBodyPtrs.size()+1;
                    sboxName += s.str();
                    sboxKinBodyPtr->SetName(sboxName.c_str());
                    //
                    std::vector<OpenRAVE::AABB> boxes(1);
                    boxes[0].extents = OpenRAVE::Vector(request.get(3).asFloat64(), request.get(4).asFloat64(), request.get(5).asFloat64());
                    boxes[0].pos = OpenRAVE::Vector(request.get(6).asFloat64(), request.get(7).asFloat64(), request.get(8).asFloat64());
                    sboxKinBodyPtr->InitFromBoxes(boxes,true);
                    //
                    pEnv->Add(sboxKinBodyPtr,true);
                    sboxKinBodyPtrs.push_back(sboxKinBodyPtr);
                }  // the environment is not locked anymore
                response.addVocab(VOCAB_OK);
            }
            else if (request.get(2).asString() == "ssph")
            {
                {
                    // lock the environment!
                    OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                    OpenRAVE::KinBodyPtr ssphKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                    std::string ssphName("ssph_");
                    std::ostringstream s;  // ssphName += std::to_string(ssphKinBodyPtrs.size()+1);  // C++11 only
                    s << ssphKinBodyPtrs.size()+1;
                    ssphName += s.str();
                    ssphKinBodyPtr->SetName(ssphName.c_str());
                    //
                    std::vector<OpenRAVE::Vector> spheres(1);
                    spheres.push_back( OpenRAVE::Vector(request.get(4).asFloat64(), request.get(5).asFloat64(), request.get(6).asFloat64(), request.get(3).asFloat64() ));
                    ssphKinBodyPtr->InitFromSpheres(spheres,true);
                    //
                    pEnv->Add(ssphKinBodyPtr,true);
                    ssphKinBodyPtrs.push_back(ssphKinBodyPtr);
                }  // the environment is not locked anymore
                response.addVocab(VOCAB_OK);
            }
            else if (request.get(2).asString() == "scyl")
            {
                {
                    // lock the environment!
                    OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                    OpenRAVE::KinBodyPtr scylKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                    std::string scylName("scyl_");
                    std::ostringstream s;  // scylName += std::to_string(scylKinBodyPtrs.size()+1);  // C++11 only
                    s << scylKinBodyPtrs.size()+1;
                    scylName += s.str();
                    scylKinBodyPtr->SetName(scylName.c_str());
                    //
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
                    scylKinBodyPtr->InitFromGeometries(scylInfoList);
                    //
                    pEnv->Add(scylKinBodyPtr,true);
                    scylKinBodyPtrs.push_back(scylKinBodyPtr);
                }  // the environment is not locked anymore
                response.addVocab(VOCAB_OK);

            }
            else if (request.get(2).asString() == "mesh")
            {
                {
                    // lock the environment!
                    OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                    OpenRAVE::KinBodyPtr meshKinBodyPtr = RaveCreateKinBody(pEnv,"");
                    std::string meshName("mesh_");
                    std::ostringstream s;  // meshName += std::to_string(meshKinBodyPtrs.size()+1);  // C++11 only
                    s << meshKinBodyPtrs.size()+1;
                    meshName += s.str();
                    meshKinBodyPtr->SetName(meshName.c_str());
                    //
                    //std::vector<AABB> boxes(1);
                    //boxes[0].extents = Vector(in.get(3).asFloat64(), in.get(4).asFloat64(), in.get(5).asFloat64());
                    //boxes[0].pos = Vector(in.get(6).asFloat64(), in.get(7).asFloat64(), in.get(8).asFloat64());
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
                    meshKinBodyPtr->InitFromTrimesh(raveMesh,true);
                    //
                    pEnv->Add(meshKinBodyPtr,true);
                    meshKinBodyPtrs.push_back(meshKinBodyPtr);
                }  // the environment is not locked anymore
                response.addVocab(VOCAB_OK);

            }
            else if (request.get(2).asString() == "obj")
            {
                {
                    // lock the environment!
                    OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                    OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                    pEnv->ReadKinBodyXMLFile(objKinBodyPtr, request.get(3).asString().c_str());
                    pEnv->Add(objKinBodyPtr,true);
                    objKinBodyPtrs.push_back(objKinBodyPtr);
                }  // the environment is not locked anymore
                response.addVocab(VOCAB_OK);
            }
            else response.addVocab(VOCAB_FAILED);

        }
        else if (request.get(1).asString()=="mv")
        {
            OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(request.get(2).asString().c_str());
            OpenRAVE::Transform T = objPtr->GetTransform();
            T.trans.x = request.get(3).asFloat64();  // [m]
            T.trans.y = request.get(4).asFloat64();  // [m]
            T.trans.z = request.get(5).asFloat64();  // [m]
            objPtr->SetTransform(T);
            response.addVocab(VOCAB_OK);
        }
        else if ((request.get(1).asString()=="del")&&(request.get(2).asString()=="all"))
        {
            for (unsigned int i=0; i<boxKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(boxKinBodyPtrs[i]);
            }
            boxKinBodyPtrs.clear();
            for (unsigned int i=0; i<sboxKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(sboxKinBodyPtrs[i]);
            }
            sboxKinBodyPtrs.clear();
            for (unsigned int i=0; i<ssphKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(ssphKinBodyPtrs[i]);
            }
            ssphKinBodyPtrs.clear();
            for (unsigned int i=0; i<scylKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(scylKinBodyPtrs[i]);
            }
            scylKinBodyPtrs.clear();
            for (unsigned int i=0; i<meshKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(meshKinBodyPtrs[i]);
            }
            meshKinBodyPtrs.clear();
            for (unsigned int i=0; i<objKinBodyPtrs.size(); i++)
            {
                pEnv->Remove(objKinBodyPtrs[i]);
            }
            objKinBodyPtrs.clear();
            response.addVocab(VOCAB_OK);
        }
        else if (request.get(1).asString()=="grab")
        {
            // -- rpc command to write: world + grab + "part of robot" + name object + index + 0
            // --                         0       1           2              3           4     5

            if(request.get(3).asString()=="box")
            {
                int inIndex = (request.get(4).asInt32()); // -- index of the object
                if ( (inIndex>=1) && (inIndex<=(int)boxKinBodyPtrs.size()) )
                {
                    if (request.get(5).asInt32()==1)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString()); // <in.get(2).asString()> will have to be the robot manipulator used in XML file. E.g: rigthArm for TEO"
                        pRobot->Grab(boxKinBodyPtrs[inIndex-1]);
                        CD_INFO("The box is grabbed!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else if (request.get(5).asInt32()==0)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Release(boxKinBodyPtrs[inIndex-1]);
                        CD_INFO("The box is released!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else response.addVocab(VOCAB_FAILED);
                }
                else response.addVocab(VOCAB_FAILED);
            }
            else if(request.get(3).asString()=="sbox")
            {
                int inIndex = (request.get(4).asInt32());
                if ( (inIndex>=1) && (inIndex<=(int)sboxKinBodyPtrs.size()) )
                {
                    if (request.get(5).asInt32()==1)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Grab(sboxKinBodyPtrs[inIndex-1]);
                        CD_INFO("The sbox is grabbed!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else if (request.get(5).asInt32()==0)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Release(sboxKinBodyPtrs[inIndex-1]);
                        CD_INFO("The sbox is released!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else response.addVocab(VOCAB_FAILED);
                }
                else response.addVocab(VOCAB_FAILED);
            }
            else if(request.get(3).asString()=="ssph")
            {
                int inIndex = (request.get(4).asInt32());
                if ( (inIndex>=1) && (inIndex<=(int)ssphKinBodyPtrs.size()) )
                {
                    if (request.get(5).asInt32()==1)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Grab(ssphKinBodyPtrs[inIndex-1]);
                        CD_INFO("The sphere is grabbed!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else if (request.get(5).asInt32()==0)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Release(ssphKinBodyPtrs[inIndex-1]);
                        CD_INFO("The sphere is released!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else response.addVocab(VOCAB_FAILED);
                }
                else response.addVocab(VOCAB_FAILED);
            }
            else if(request.get(3).asString()=="scyl")
            {
                int inIndex = (request.get(4).asInt32());
                if ( (inIndex>=1) && (inIndex<=(int)scylKinBodyPtrs.size()) )
                {
                    if (request.get(5).asInt32()==1)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Grab(scylKinBodyPtrs[inIndex-1]);
                        CD_INFO("The cylinder is grabbed!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else if (request.get(5).asInt32()==0)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        pRobot->Release(scylKinBodyPtrs[inIndex-1]);
                        CD_INFO("The cylinder is released!!\n");
                        response.addVocab(VOCAB_OK);
                    }
                    else response.addVocab(VOCAB_FAILED);
                }
                else response.addVocab(VOCAB_FAILED);
            }
            else if (request.get(3).asString()=="obj")
            {
                OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(request.get(4).asString().c_str());
                if(objPtr)
                {
                    CD_SUCCESS("object %s exists.\n", request.get(4).asString().c_str());
                    if (request.get(5).asInt32()==1)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        CD_INFO("The cylinder is grabbed!!\n");
                        pRobot->Grab(objPtr);
                        response.addVocab(VOCAB_OK);
                    }
                    else if (request.get(5).asInt32()==0)
                    {
                        pRobot->SetActiveManipulator(request.get(2).asString());
                        CD_INFO("The cylinder is released!!\n");
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
            else response.addVocab(VOCAB_FAILED);
        }
        else if (request.get(1).asString()=="whereis")
        {
            if (request.get(2).asString()=="obj")
            {
                OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(request.get(3).asString().c_str());
                CD_INFO("We want to know where is ->> %s\n", objPtr->GetName().c_str());
                if(objPtr)
                {
                    //Transform t = objPtr->GetTransform();
                    OpenRAVE::Vector tr = objPtr->GetTransform().trans;
                    CD_SUCCESS("object %s at %f, %f, %f.\n", request.get(3).asString().c_str(), tr.x,tr.y,tr.z);
                    yarp::os::Bottle trans;
                    trans.addFloat64(tr.x);
                    trans.addFloat64(tr.y);
                    trans.addFloat64(tr.z);
                    response.addList() = trans;
                    response.addVocab(VOCAB_OK);
                }
                else // null pointer
                {
                    CD_WARNING("object %s does not exist.\n", request.get(3).asString().c_str());
                    response.addVocab(VOCAB_FAILED);
                }
            }
            else if (request.get(2).asString()=="tcp")
            {
                std::vector<OpenRAVE::RobotBasePtr> robots;
                pEnv->GetRobots(robots);
                OpenRAVE::RobotBasePtr robotPtr = robots.at(0);  //-- For now, we use only the first robot
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
        response.write(*out);
        return true;
    }
    response.addVocab(VOCAB_FAILED);
    response.write(*out);
    return true;
}

// -----------------------------------------------------------------------------
