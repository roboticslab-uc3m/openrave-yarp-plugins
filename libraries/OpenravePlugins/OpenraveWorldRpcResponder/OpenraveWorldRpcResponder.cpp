#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind.hpp>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Vocab.h>


using namespace yarp::os;
using namespace OpenRAVE;


using namespace std;


class DataProcessor : public yarp::os::PortReader
{

public:

    int robotDraw;
    double drawRadius, drawR, drawG, drawB;

    // Register an OpenRAVE environment.

    void setEnvironment(EnvironmentBasePtr _pEnv)
    {
        pEnv = _pEnv;
    }

    // ---------------------------------------------

    void setRobot(RobotBasePtr _pRobot)
    {
        pRobot = _pRobot;
    }

    // ----------------------------------------------

    void setRobotManip(RobotBase::ManipulatorPtr _pRobotManip)
    {
        pRobotManip = _pRobotManip;
    }

private:

    // -- variables
    vector<int>* psqPainted;
    yarp::os::Semaphore* psqPaintedSemaphore;

    // box/sbox/cyl/scyl/sph/ssph
    std::vector<KinBodyPtr> boxKinBodyPtrs;
    std::vector<KinBodyPtr> sboxKinBodyPtrs;
    std::vector<KinBodyPtr> cylKinBodyPtrs;
    std::vector<KinBodyPtr> scylKinBodyPtrs;
    std::vector<KinBodyPtr> sphKinBodyPtrs;
    std::vector<KinBodyPtr> ssphKinBodyPtrs;
    std::vector<KinBodyPtr> meshKinBodyPtrs;
    std::vector<KinBodyPtr> objKinBodyPtrs;

    EnvironmentBasePtr pEnv;  // set in setEnvironment
    RobotBasePtr pRobot;  // set in setRobot
    RobotBase::ManipulatorPtr pRobotManip;  // set in setRobot


    // Implement the actual responder (callback on RPC).

    virtual bool read(yarp::os::ConnectionReader& connection)
    {

        Bottle in, out;
        in.read(connection);
        printf("[WorldRpcResponder] Got %s\n", in.toString().c_str());
        ConnectionWriter *returnToSender = connection.getWriter();
        if (returnToSender==NULL) return false;
        ConstString choice = in.get(0).asString();
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
                        KinBodyPtr boxKinBodyPtr = RaveCreateKinBody(pEnv,"");
                        ConstString boxName("box_");
                        std::ostringstream s;  // boxName += std::to_string(boxKinBodyPtrs.size()+1);  // C++11 only
                        s << boxKinBodyPtrs.size()+1;
                        boxName += s.str();
                        boxKinBodyPtr->SetName(boxName.c_str());
                        //
                        std::vector<AABB> boxes(1);
                        boxes[0].extents = Vector(in.get(3).asDouble(), in.get(4).asDouble(), in.get(5).asDouble());
                        boxes[0].pos = Vector(in.get(6).asDouble(), in.get(7).asDouble(), in.get(8).asDouble());
                        boxKinBodyPtr->InitFromBoxes(boxes,true);
                        boxKinBodyPtr->GetLinks()[0]->SetMass(1);
                        Vector inertia(1,1,1);
                        boxKinBodyPtr->GetLinks()[0]->SetPrincipalMomentsOfInertia(inertia);
                        Transform pose(Vector(1,0,0,0),Vector(0,0,0));
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
                        KinBodyPtr sboxKinBodyPtr = RaveCreateKinBody(pEnv,"");
                        ConstString sboxName("sbox_");
                        std::ostringstream s;  // sboxName += std::to_string(sboxKinBodyPtrs.size()+1);  // C++11 only
                        s << sboxKinBodyPtrs.size()+1;
                        sboxName += s.str();
                        sboxKinBodyPtr->SetName(sboxName.c_str());
                        //
                        std::vector<AABB> boxes(1);
                        boxes[0].extents = Vector(in.get(3).asDouble(), in.get(4).asDouble(), in.get(5).asDouble());
                        boxes[0].pos = Vector(in.get(6).asDouble(), in.get(7).asDouble(), in.get(8).asDouble());
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
                        KinBodyPtr ssphKinBodyPtr = RaveCreateKinBody(pEnv,"");
                        ConstString ssphName("ssph_");
                        std::ostringstream s;  // ssphName += std::to_string(ssphKinBodyPtrs.size()+1);  // C++11 only
                        s << ssphKinBodyPtrs.size()+1;
                        ssphName += s.str();
                        ssphKinBodyPtr->SetName(ssphName.c_str());
                        //
                        std::vector<Vector> spheres(1);
                        spheres.push_back( Vector(in.get(4).asDouble(), in.get(5).asDouble(), in.get(6).asDouble(), in.get(3).asDouble() ));
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
                        KinBodyPtr scylKinBodyPtr = RaveCreateKinBody(pEnv,"");
                        ConstString scylName("scyl_");
                        std::ostringstream s;  // scylName += std::to_string(scylKinBodyPtrs.size()+1);  // C++11 only
                        s << scylKinBodyPtrs.size()+1;
                        scylName += s.str();
                        scylKinBodyPtr->SetName(scylName.c_str());
                        //
                        std::list<KinBody::Link::GeometryInfo> scylInfoList;
                        KinBody::Link::GeometryInfo scylInfo;
                        scylInfo._type = KinBody::Link::GeomCylinder;
                        Transform pose(Vector(1,0,0,0),Vector(in.get(5).asDouble(),in.get(6).asDouble(),in.get(7).asDouble()));
                        scylInfo._t = pose;
                        Vector volume;
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
                        KinBodyPtr meshKinBodyPtr = RaveCreateKinBody(pEnv,"");
                        ConstString meshName("mesh_");
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
                        raveMesh.vertices[0] = Vector(1.0,1.0,1.0);
                        raveMesh.vertices[1] = Vector(1.0,1.5,1.0);
                        raveMesh.vertices[2] = Vector(1.5,1.0,1.0);
                        raveMesh.vertices[3] = Vector(1.0,1.5,1.0);
                        raveMesh.vertices[4] = Vector(1.5,1.0,1.0);
                        raveMesh.vertices[5] = Vector(1.5,1.5,1.5);
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
                        KinBodyPtr objKinBodyPtr = RaveCreateKinBody(pEnv,"");
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
                KinBodyPtr objPtr = pEnv->GetKinBody(in.get(2).asString().c_str());
                Transform T = objPtr->GetTransform();
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
                            printf("The box is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(boxKinBodyPtrs[inIndex-1]);
                            printf("The box is released!!\n");
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
                            printf("The sbox is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(sboxKinBodyPtrs[inIndex-1]);
                            printf("The sbox is released!!\n");
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
                            printf("The sphere is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(ssphKinBodyPtrs[inIndex-1]);
                            printf("The sphere is released!!\n");
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
                            printf("The cylinder is grabbed!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            pRobot->Release(scylKinBodyPtrs[inIndex-1]);
                            printf("The cylinder is released!!\n");
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else out.addVocab(VOCAB_FAILED);
                }
                else if (in.get(3).asString()=="obj")
                {
                    KinBodyPtr objPtr = pEnv->GetKinBody(in.get(4).asString().c_str());
                    if(objPtr)
                    {
                        printf("[WorldRpcResponder] success: object %s exists.\n", in.get(4).asString().c_str());
                        if (in.get(5).asInt()==1)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            printf("The cylinder is grabbed!!\n");
                            pRobot->Grab(objPtr);
                            out.addVocab(VOCAB_OK);
                        }
                        else if (in.get(5).asInt()==0)
                        {
                            pRobot->SetActiveManipulator(in.get(2).asString());
                            printf("The cylinder is released!!\n");
                            pRobot->Release(objPtr);
                            out.addVocab(VOCAB_OK);
                        }
                        else out.addVocab(VOCAB_FAILED);
                    }
                    else      // null pointer
                    {
                        printf("[WorldRpcResponder] warning: object %s does not exist.\n", in.get(3).asString().c_str());
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
                    KinBodyPtr objPtr = pEnv->GetKinBody(in.get(3).asString().c_str());
                    printf("We want to know where is ->> %s\n", objPtr->GetName().c_str());
                    if(objPtr)
                    {
                        //Transform t = objPtr->GetTransform();
                        Vector tr = objPtr->GetTransform().trans;
                        printf("[WorldRpcResponder] success: object %s at %f, %f, %f.\n", in.get(3).asString().c_str(), tr.x,tr.y,tr.z);
                        Bottle trans;
                        trans.addDouble(tr.x);
                        trans.addDouble(tr.y);
                        trans.addDouble(tr.z);
                        out.addList() = trans;
                        out.addVocab(VOCAB_OK);
                    }
                    else      // null pointer
                    {
                        printf("[WorldRpcResponder] warning: object %s does not exist.\n", in.get(3).asString().c_str());
                        out.addVocab(VOCAB_FAILED);
                    }
                }
                else if (in.get(2).asString()=="tcp")
                {
                    std::vector<RobotBasePtr> robots;
                    pEnv->GetRobots(robots);
                    RobotBasePtr robotPtr = robots.at(0);  //-- For now, we use only the first robot
                    pRobotManip = robotPtr->GetManipulator(in.get(3).asString()); //-- <in.get(3).asString()> will have to be the robot manipulator used in XML file. E.g: rigthArm for TEO"
                    Transform ee = pRobotManip->GetEndEffector()->GetTransform();
                    Transform tool;
                    //tool.trans = Vector(0.0,0.0,1.3);
                    tool.rot = quatFromAxisAngle(Vector(0,0,0)); //-- Converts an axis-angle rotation into a quaternion.
                    tool.rot = ee.rot;
                    Transform tcp = ee * tool;
                    //Transform tcp = ee;
                    printf("[WorldRpcResponder] success: TCP at %f, %f, %f.\n", tcp.trans.x, tcp.trans.y, tcp.trans.z);
                    Bottle trans;
                    trans.addDouble(tcp.trans.x);
                    trans.addDouble(tcp.trans.y);
                    trans.addDouble(tcp.trans.z);
                    out.addList() = trans;
                    out.addVocab(VOCAB_OK);

                }
                else
                {
                    printf("[WorldRpcResponder] warning: where is what?\n");
                    out.addVocab(VOCAB_FAILED);
                }

            }
            else if (in.get(1).asString()=="draw")
            {
                if (in.get(2).asInt() == 0)
                {
                    printf("[WorldRpcResponder] success: Turning draw OFF.\n");
                    robotDraw = 0;
                    out.addVocab(VOCAB_OK);
                }
                else
                {
                    printf("[WorldRpcResponder] success: Turning draw ON.\n");
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



class OpenraveWorldRpcResponder : public ModuleBase
{
public:

    OpenraveWorldRpcResponder(EnvironmentBasePtr penv) : ModuleBase(penv)
    {
        __description = "OpenraveWorldRpcResponder plugin.";
        RegisterCommand("open",boost::bind(&OpenraveWorldRpcResponder::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveWorldRpcResponder()
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


    bool Open(ostream& sout, istream& sinput)
    {

        vector<string> funcionArgs;
        while(sinput)
        {
            string funcionArg;
            sinput >> funcionArg;
            funcionArgs.push_back(funcionArg);
        }

        string portName("/worldRpcResponder/rpc:s");

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
        std::vector<RobotBasePtr> robots;
        penv->GetRobots(robots);
        //-- Robot 0
        probot = robots.at(0);  // which is a RobotBasePtr
        printf("OpenRaveWorldRpcResponder using robot 0 (%s) as main robot.\n", probot->GetName().c_str());

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

    RobotBasePtr probot;
};

InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv)
{
    if( type == PT_Module && interfacename == "openraveworldrpcresponder" )
    {
        return InterfaceBasePtr(new OpenraveWorldRpcResponder(penv));
    }
    return InterfaceBasePtr();
}

void GetPluginAttributesValidated(PLUGININFO& info)
{
    info.interfacenames[PT_Module].push_back("OpenraveWorldRpcResponder");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}