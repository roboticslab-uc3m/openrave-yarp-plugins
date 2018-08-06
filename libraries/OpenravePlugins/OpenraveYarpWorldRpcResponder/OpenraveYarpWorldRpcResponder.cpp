// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <cstdio>

#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Vocab.h>

#define VOCAB_OK VOCAB2('o','k')
#define VOCAB_FAILED VOCAB4('f','a','i','l')

class DataProcessor : public yarp::os::PortReader
{

public:

    int robotDraw;
    double drawRadius, drawR, drawG, drawB;

    // Register an OpenRAVE environment.

    void setEnvironment(OpenRAVE::EnvironmentBasePtr _pEnv)
    {
        pEnv = _pEnv;
    }

    // ---------------------------------------------

    void setRobot(OpenRAVE::RobotBasePtr _pRobot)
    {
        pRobot = _pRobot;
    }

    // ----------------------------------------------

    void setRobotManip(OpenRAVE::RobotBase::ManipulatorPtr _pRobotManip)
    {
        pRobotManip = _pRobotManip;
    }

private:

    // -- variables
    std::vector<int>* psqPainted;
    yarp::os::Semaphore* psqPaintedSemaphore;

    // box/sbox/cyl/scyl/sph/ssph
    std::vector<OpenRAVE::KinBodyPtr> boxKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> sboxKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> cylKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> scylKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> sphKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> ssphKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> meshKinBodyPtrs;
    std::vector<OpenRAVE::KinBodyPtr> objKinBodyPtrs;

    OpenRAVE::EnvironmentBasePtr pEnv;  // set in setEnvironment
    OpenRAVE::RobotBasePtr pRobot;  // set in setRobot
    OpenRAVE::RobotBase::ManipulatorPtr pRobotManip;  // set in setRobot


    // Implement the actual responder (callback on RPC).

    virtual bool read(yarp::os::ConnectionReader& connection)
    {

        yarp::os::Bottle in, out;
        in.read(connection);
        std::printf("[WorldRpcResponder] Got %s\n", in.toString().c_str());
        yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
        if (returnToSender==NULL) return false;
        yarp::os::ConstString choice = in.get(0).asString();
        if (in.get(0).getCode() != BOTTLE_TAG_STRING) choice="";
        if (choice=="help")    ///////////////////////////////// help /////////////////////////////////
        {
            out.addString("Available commands: help, info (robots and environment information), world del all, world mk box/sbox (three params for size) (three params for pos), world mk ssph (radius) (three params for pos), world mk scyl (radius height) (three params for pos), world mk mesh (no params yet), world mk obj (absolute path), world mv (name) (three params for pos), world grab (manipulator) (obj) (num) 0/1, world whereis obj (name), world whereis tcp (manipulator),  world draw 0/1 (radius r g b).");
            out.write(*returnToSender);
            return true;
        }
        else if (choice == "info")
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

            info << "// "; // -- to separate information!!

            // -- Get bodies
            std::vector<OpenRAVE::KinBodyPtr> vectorOfBodiesPtr;
            pEnv->GetBodies(vectorOfBodiesPtr);

            info << "Total bodies in the environment: ";

            // -- For each body
            for(size_t bodiesPtrIdx=0; bodiesPtrIdx<vectorOfBodiesPtr.size(); bodiesPtrIdx++)
            {
                info << "("<< bodiesPtrIdx <<")"<<vectorOfBodiesPtr[bodiesPtrIdx]->GetName().c_str()<<" ";
            }

            out.addString(info.str());
            out.write(*returnToSender);
            return true;

        }

        else if (choice=="world")
        {
            if (in.get(1).asString() == "mk")
            {
                if (in.get(2).asString() == "box")
                {
                    {
                        // lock the environment!
                        OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                        OpenRAVE::KinBodyPtr boxKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                        yarp::os::ConstString boxName("box_");
                        std::ostringstream s;  // boxName += std::to_string(boxKinBodyPtrs.size()+1);  // C++11 only
                        s << boxKinBodyPtrs.size()+1;
                        boxName += s.str();
                        boxKinBodyPtr->SetName(boxName.c_str());
                        //
                        std::vector<OpenRAVE::AABB> boxes(1);
                        boxes[0].extents = OpenRAVE::Vector(in.get(3).asDouble(), in.get(4).asDouble(), in.get(5).asDouble());
                        boxes[0].pos = OpenRAVE::Vector(in.get(6).asDouble(), in.get(7).asDouble(), in.get(8).asDouble());
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
                    out.addVocab(VOCAB_OK);
                }
                else if (in.get(2).asString() == "sbox")
                {
                    {
                        // lock the environment!
                        OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                        OpenRAVE::KinBodyPtr sboxKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                        yarp::os::ConstString sboxName("sbox_");
                        std::ostringstream s;  // sboxName += std::to_string(sboxKinBodyPtrs.size()+1);  // C++11 only
                        s << sboxKinBodyPtrs.size()+1;
                        sboxName += s.str();
                        sboxKinBodyPtr->SetName(sboxName.c_str());
                        //
                        std::vector<OpenRAVE::AABB> boxes(1);
                        boxes[0].extents = OpenRAVE::Vector(in.get(3).asDouble(), in.get(4).asDouble(), in.get(5).asDouble());
                        boxes[0].pos = OpenRAVE::Vector(in.get(6).asDouble(), in.get(7).asDouble(), in.get(8).asDouble());
                        sboxKinBodyPtr->InitFromBoxes(boxes,true);
                        //
                        pEnv->Add(sboxKinBodyPtr,true);
                        sboxKinBodyPtrs.push_back(sboxKinBodyPtr);
                    }  // the environment is not locked anymore
                    out.addVocab(VOCAB_OK);
                }
                else if (in.get(2).asString() == "ssph")
                {
                    {
                        // lock the environment!
                        OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                        OpenRAVE::KinBodyPtr ssphKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                        yarp::os::ConstString ssphName("ssph_");
                        std::ostringstream s;  // ssphName += std::to_string(ssphKinBodyPtrs.size()+1);  // C++11 only
                        s << ssphKinBodyPtrs.size()+1;
                        ssphName += s.str();
                        ssphKinBodyPtr->SetName(ssphName.c_str());
                        //
                        std::vector<OpenRAVE::Vector> spheres(1);
                        spheres.push_back( OpenRAVE::Vector(in.get(4).asDouble(), in.get(5).asDouble(), in.get(6).asDouble(), in.get(3).asDouble() ));
                        ssphKinBodyPtr->InitFromSpheres(spheres,true);
                        //
                        pEnv->Add(ssphKinBodyPtr,true);
                        ssphKinBodyPtrs.push_back(ssphKinBodyPtr);
                    }  // the environment is not locked anymore
                    out.addVocab(VOCAB_OK);
                }
                else if (in.get(2).asString() == "scyl")
                {
                    {
                        // lock the environment!
                        OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                        OpenRAVE::KinBodyPtr scylKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                        yarp::os::ConstString scylName("scyl_");
                        std::ostringstream s;  // scylName += std::to_string(scylKinBodyPtrs.size()+1);  // C++11 only
                        s << scylKinBodyPtrs.size()+1;
                        scylName += s.str();
                        scylKinBodyPtr->SetName(scylName.c_str());
                        //
                        std::list<OpenRAVE::KinBody::Link::GeometryInfo> scylInfoList;
                        OpenRAVE::KinBody::Link::GeometryInfo scylInfo;
                        scylInfo._type = OpenRAVE::KinBody::Link::GeomCylinder;
                        OpenRAVE::Transform pose(OpenRAVE::Vector(1,0,0,0),OpenRAVE::Vector(in.get(5).asDouble(),in.get(6).asDouble(),in.get(7).asDouble()));
                        scylInfo._t = pose;
                        OpenRAVE::Vector volume;
                        volume.x = in.get(3).asDouble();
                        volume.y = in.get(4).asDouble();
                        scylInfo._vGeomData = volume;
                        scylInfo._bVisible = true;
                        //scylInfo._vDiffuseColor = [1,0,0];
                        scylInfoList.push_back(scylInfo);
                        scylKinBodyPtr->InitFromGeometries(scylInfoList);
                        //
                        pEnv->Add(scylKinBodyPtr,true);
                        scylKinBodyPtrs.push_back(scylKinBodyPtr);
                    }  // the environment is not locked anymore
                    out.addVocab(VOCAB_OK);

                }
                else if (in.get(2).asString() == "mesh")
                {
                    {
                        // lock the environment!
                        OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                        OpenRAVE::KinBodyPtr meshKinBodyPtr = RaveCreateKinBody(pEnv,"");
                        yarp::os::ConstString meshName("mesh_");
                        std::ostringstream s;  // meshName += std::to_string(meshKinBodyPtrs.size()+1);  // C++11 only
                        s << meshKinBodyPtrs.size()+1;
                        meshName += s.str();
                        meshKinBodyPtr->SetName(meshName.c_str());
                        //
                        //std::vector<AABB> boxes(1);
                        //boxes[0].extents = Vector(in.get(3).asDouble(), in.get(4).asDouble(), in.get(5).asDouble());
                        //boxes[0].pos = Vector(in.get(6).asDouble(), in.get(7).asDouble(), in.get(8).asDouble());
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
                    out.addVocab(VOCAB_OK);

                }
                else if (in.get(2).asString() == "obj")
                {
                    {
                        // lock the environment!
                        OpenRAVE::EnvironmentMutex::scoped_lock lock(pEnv->GetMutex());
                        OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(pEnv,"");
                        pEnv->ReadKinBodyXMLFile(objKinBodyPtr, in.get(3).asString().c_str());
                        pEnv->Add(objKinBodyPtr,true);
                        objKinBodyPtrs.push_back(objKinBodyPtr);
                    }  // the environment is not locked anymore
                    out.addVocab(VOCAB_OK);
                }
                else out.addVocab(VOCAB_FAILED);

            }
            else if (in.get(1).asString()=="mv")
            {
                OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(in.get(2).asString().c_str());
                OpenRAVE::Transform T = objPtr->GetTransform();
                T.trans.x = in.get(3).asDouble();  // [m]
                T.trans.y = in.get(4).asDouble();  // [m]
                T.trans.z = in.get(5).asDouble();  // [m]
                objPtr->SetTransform(T);
                out.addVocab(VOCAB_OK);

            }
            else if ((in.get(1).asString()=="del")&&(in.get(2).asString()=="all"))
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
                out.addVocab(VOCAB_OK);
            }
            else if (in.get(1).asString()=="grab")
            {

                // -- rpc command to write: world + grab + "part of robot" + name object + index + 0
                // --                         0       1           2              3           4     5

                if(in.get(3).asString()=="box")
                {
                    int inIndex = (in.get(4).asInt()); // -- index of the object
                    if ( (inIndex>=1) && (inIndex<=(int)boxKinBodyPtrs.size()) )
                    {
                        if (in.get(5).asInt()==1)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString()); // <in.get(2).asString()> will have to be the robot manipulator used in XML file. E.g: rigthArm for TEO"
                            pRobot->Grab(boxKinBodyPtrs[inIndex-1]);
                            std::printf("The box is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(boxKinBodyPtrs[inIndex-1]);
                            std::printf("The box is released!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else out.addVocab(VOCAB_FAILED);
                }
                else if(in.get(3).asString()=="sbox")
                {
                    int inIndex = (in.get(4).asInt());
                    if ( (inIndex>=1) && (inIndex<=(int)sboxKinBodyPtrs.size()) )
                    {
                        if (in.get(5).asInt()==1)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Grab(sboxKinBodyPtrs[inIndex-1]);
                            std::printf("The sbox is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(sboxKinBodyPtrs[inIndex-1]);
                            std::printf("The sbox is released!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else out.addVocab(VOCAB_FAILED);
                }
                else if(in.get(3).asString()=="ssph")
                {
                    int inIndex = (in.get(4).asInt());
                    if ( (inIndex>=1) && (inIndex<=(int)ssphKinBodyPtrs.size()) )
                    {
                        if (in.get(5).asInt()==1)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Grab(ssphKinBodyPtrs[inIndex-1]);
                            std::printf("The sphere is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(ssphKinBodyPtrs[inIndex-1]);
                            std::printf("The sphere is released!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else out.addVocab(VOCAB_FAILED);
                }
                else if(in.get(3).asString()=="scyl")
                {
                    int inIndex = (in.get(4).asInt());
                    if ( (inIndex>=1) && (inIndex<=(int)scylKinBodyPtrs.size()) )
                    {
                        if (in.get(5).asInt()==1)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Grab(scylKinBodyPtrs[inIndex-1]);
                            std::printf("The cylinder is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(scylKinBodyPtrs[inIndex-1]);
                            std::printf("The cylinder is released!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else out.addVocab(VOCAB_FAILED);
                }
                else if (in.get(3).asString()=="obj")
                {
                    OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(in.get(4).asString().c_str());
                    if(objPtr)
                    {
                        std::printf("[WorldRpcResponder] success: object %s exists.\n", in.get(4).asString().c_str());
                        if (in.get(5).asInt()==1)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            std::printf("The cylinder is grabbed!!\n");
                            pRobot->Grab(objPtr);
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            std::printf("The cylinder is released!!\n");
                            pRobot->Release(objPtr);
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else      // null pointer
                    {
                        std::printf("[WorldRpcResponder] warning: object %s does not exist.\n", in.get(3).asString().c_str());
                        out.addVocab(VOCAB_FAILED);
                    }
                }
                else out.addVocab(VOCAB_FAILED);

                // -- grab

            }
            else if (in.get(1).asString()=="whereis")
            {
                if (in.get(2).asString()=="obj")
                {
                    OpenRAVE::KinBodyPtr objPtr = pEnv->GetKinBody(in.get(3).asString().c_str());
                    std::printf("We want to know where is ->> %s\n", objPtr->GetName().c_str());
                    if(objPtr)
                    {
                        //Transform t = objPtr->GetTransform();
                        OpenRAVE::Vector tr = objPtr->GetTransform().trans;
                        std::printf("[WorldRpcResponder] success: object %s at %f, %f, %f.\n", in.get(3).asString().c_str(), tr.x,tr.y,tr.z);
                        yarp::os::Bottle trans;
                        trans.addDouble(tr.x);
                        trans.addDouble(tr.y);
                        trans.addDouble(tr.z);
                        out.addList() = trans;
                        out.addVocab(VOCAB_OK);
                    }
                    else      // null pointer
                    {
                        std::printf("[WorldRpcResponder] warning: object %s does not exist.\n", in.get(3).asString().c_str());
                        out.addVocab(VOCAB_FAILED);
                    }
                }
                else if (in.get(2).asString()=="tcp")
                {
                    std::vector<OpenRAVE::RobotBasePtr> robots;
                    pEnv->GetRobots(robots);
                    OpenRAVE::RobotBasePtr robotPtr = robots.at(0);  //-- For now, we use only the first robot
                    pRobotManip = robotPtr->GetManipulator(in.get(3).asString()); //-- <in.get(3).asString()> will have to be the robot manipulator used in XML file. E.g: rigthArm for TEO"
                    OpenRAVE::Transform ee = pRobotManip->GetEndEffector()->GetTransform();
                    OpenRAVE::Transform tool;
                    //tool.trans = Vector(0.0,0.0,1.3);
                    tool.rot = OpenRAVE::geometry::quatFromAxisAngle(OpenRAVE::Vector(0,0,0)); //-- Converts an axis-angle rotation into a quaternion.
                    tool.rot = ee.rot;
                    OpenRAVE::Transform tcp = ee * tool;
                    //Transform tcp = ee;
                    std::printf("[WorldRpcResponder] success: TCP at %f, %f, %f.\n", tcp.trans.x, tcp.trans.y, tcp.trans.z);
                    yarp::os::Bottle trans;
                    trans.addDouble(tcp.trans.x);
                    trans.addDouble(tcp.trans.y);
                    trans.addDouble(tcp.trans.z);
                    out.addList() = trans;
                    out.addVocab(VOCAB_OK);

                }
                else
                {
                    std::printf("[WorldRpcResponder] warning: where is what?\n");
                    out.addVocab(VOCAB_FAILED);
                }

            }
            else if (in.get(1).asString()=="draw")
            {
                if (in.get(2).asInt() == 0)
                {
                    std::printf("[WorldRpcResponder] success: Turning draw OFF.\n");
                    robotDraw = 0;
                    out.addVocab(VOCAB_OK);
                }
                else
                {
                    std::printf("[WorldRpcResponder] success: Turning draw ON.\n");
                    robotDraw = in.get(2).asInt();
                    if (in.size() >= 4) drawRadius = in.get(3).asDouble();
                    if (in.size() >= 7)
                    {
                        drawR = in.get(4).asDouble();
                        drawG = in.get(5).asDouble();
                        drawB = in.get(6).asDouble();
                    }
                    out.addVocab(VOCAB_OK);
                }

            }
            else out.addVocab(VOCAB_FAILED);
            out.write(*returnToSender);
            return true;
        }
        out.addVocab(VOCAB_FAILED);
        out.write(*returnToSender);
        return false;


    }


};


/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpWorldRpcResponder
 *
 * @brief Contains roboticslab::OpenraveYarpWorldRpcResponder.
 */

/**
 * @ingroup OpenraveYarpWorldRpcResponder
 * @brief Opens YARP RpcPort, to control environment.
 */
class OpenraveYarpWorldRpcResponder : public OpenRAVE::ModuleBase
{
public:

    OpenraveYarpWorldRpcResponder(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
    {
        __description = "OpenraveYarpWorldRpcResponder plugin.";
        OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpWorldRpcResponder::Open, this,_1,_2),"opens OpenraveYarpWorldRpcResponder");
    }

    virtual ~OpenraveYarpWorldRpcResponder()
    {
        worldRpcServer.close();
    }

    virtual void Destroy()
    {

        RAVELOG_INFO("module unloaded from environment\n");
    }

    /*int main(const string& cmd) {
        RAVELOG_INFO("module initialized cmd; %s\n", cmd.c_str());
        return 0;
    }*/


    bool Open(std::ostream& sout, std::istream& sinput)
    {

        std::vector<std::string> funcionArgs;
        while(sinput)
        {
            std::string funcionArg;
            sinput >> funcionArg;
            funcionArgs.push_back(funcionArg);
        }

        std::string portName("/openraveYarpWorldRpcResponder/rpc:s");

        if (funcionArgs.size() > 0)
        {
            if( funcionArgs[0][0] == '/')
                portName = funcionArgs[0];
        }
        RAVELOG_INFO("portName: %s\n",portName.c_str());

        if ( !yarp.checkNetwork() )
        {
            RAVELOG_INFO("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }


        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();


        //-- Get the robot
        std::vector<OpenRAVE::RobotBasePtr> robots;
        penv->GetRobots(robots);
        //-- Robot 0
        probot = robots.at(0);  // which is a RobotBasePtr
        std::printf("OpenRaveYarpWorldRpcResponder using robot 0 (%s) as main robot.\n", probot->GetName().c_str());

        //-- processor
        processor.setEnvironment(penv);
        processor.setRobot(probot);

        //-- world rpc server
        worldRpcServer.open(portName);
        worldRpcServer.setReader(processor);

        return true;

    }

private:
    yarp::os::Network yarp;
    yarp::os::RpcServer worldRpcServer;
    DataProcessor processor;

    OpenRAVE::RobotBasePtr probot;
};

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpworldrpcresponder" )
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpWorldRpcResponder(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpWorldRpcResponder");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
