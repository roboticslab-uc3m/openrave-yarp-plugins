#include "ForceSensor.h"

using namespace OpenRAVE;

#define FOREACH(it, v) for(typeof((v).begin()) it = (v).begin(); it != (v).end(); (it)++)
#define FOREACH_NOINC(it, v) for(typeof((v).begin()) it = (v).begin(); it != (v).end(); )

#define FOREACHC FOREACH
#define FOREACHC_NOINC FOREACH_NOINC

//Reader
BaseXMLReader::ProcessElement ForceSensor::ForceSensorXMLReader::startElement(const std::string& name, const std::list<std::pair<std::string,std::string> >& atts)
{
    if( !!_pcurreader ) {
        if( _pcurreader->startElement(name,atts) == PE_Support )
            return PE_Support;
        return PE_Ignore;
    }

    if( name != "sensor" ) {
        return PE_Pass;
    }
    ss.str("");
    return PE_Support;
}

bool ForceSensor::ForceSensorXMLReader::endElement(const std::string& name)
{
    if( !!_pcurreader ) {
        if( _pcurreader->endElement(name) )
            _pcurreader.reset();
        return false;
    }
    else if( name == "sensor" ){
        return true;
    }
    else
        RAVELOG_WARNA(str(boost::format("bad tag: %s")%name));

    if( !ss )
        RAVELOG_WARNA(str(boost::format("ForceSensor error parsing %s\n")%name));

    return false;
}

void ForceSensor::ForceSensorXMLReader::characters(const std::string& ch)
{
    if( !!_pcurreader )
        _pcurreader->characters(ch);
    else {
        ss.clear();
        ss << ch;
    }
}

//Tactile Sensor
ForceSensor::ForceSensor(EnvironmentBasePtr penv) : SensorBase(penv)
{
    RAVELOG_INFOA("Creating a Force Sensor\n");

    _data.reset(new Force6DSensorData());
    _geom.reset(new ForceSensorGeomData());

    _bPower = false;
    _bRenderData = false;
    _firstStep = true;


}

//Sensor Interface
bool ForceSensor::Init(const string& args){

    //Get all sensor in robots and find the one with the same name
    string name = this->GetName();

    RAVELOG_DEBUGA("SensorName: %s\n", name.c_str());

    if(strcmp(name.c_str(),"(NULL)")==0 || name == "" )
    {
        RAVELOG_INFOA("Sensor Name NULL!\n");
        RAVELOG_WARNA("Failed to init force sensor: %s. Make sure you named the sensor. \n", name.c_str());
    }

    else {

        this->Configure(SensorBase::CC_PowerOn, true);

        vector<RobotBasePtr> vrobots;
        GetEnv()->GetRobots(vrobots);
        FOREACHC(itrobot, vrobots)
        {
            FOREACH(itsensor, (*itrobot)->GetAttachedSensors())
            {
                if(this->GetName() == (*itsensor)->GetSensor()->GetName())
                {
                    _sensorLink = (*itsensor)->GetAttachingLink();
                }
            }

        }
    }

    return true;
};

void ForceSensor::Reset(int options){

};

bool ForceSensor::SimulationStep(OpenRAVE::dReal fTimeElapsed){
    
    //What does this do?
    if( _firstStep ) {
        _firstStep = false;
        if(!Init("ForceSensor"))
            return false;
    }

    if(GetEnv()->GetSimulationTime()<0.0) return false;

    // Read the force and torque in the joint
    Vector force;
    Vector torque;
    GetEnv()->GetPhysicsEngine()->GetLinkForceTorque( _sensorLink, force,torque );
    _data->force[0] = force[0];
    _data->force[1] = force[1];
    _data->force[2] = force[2];

    _data->torque[0] = torque[0];
    _data->torque[1] = torque[1];
    _data->torque[2] = torque[2];

    return true;
};

SensorBase::SensorGeometryPtr ForceSensor::GetSensorGeometry(SensorType type){
    if( type == ST_Invalid || type == ST_Force6D ) {

        ForceSensorGeomData* pgeom = new ForceSensorGeomData();
        *pgeom = *_geom;
        return SensorGeometryPtr(pgeom);
    }
    return SensorGeometryPtr();
};

SensorBase::SensorDataPtr ForceSensor::CreateSensorData(SensorType type){
    if(( type == ST_Invalid) ||( type == ST_Force6D) ) {
        return SensorDataPtr(new Force6DSensorData());
    }
    return SensorDataPtr();
};

bool ForceSensor::GetSensorData(SensorDataPtr psensordata){
    boost::mutex::scoped_lock lock(_mutexdata);
    *boost::dynamic_pointer_cast<Force6DSensorData>(psensordata) = *_data;

    return true;
};

void ForceSensor::SetTransform(const Transform& trans){
    _trans = trans;
}

Transform ForceSensor::GetTransform() {
    return _trans;
};
