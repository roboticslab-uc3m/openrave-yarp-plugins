#include "ForceSensor.h"

#include <boost/bind/bind.hpp>

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
    _history.resize(10);

    _movingsum.force=Vector(0,0,0);
    _movingsum.torque=Vector(0,0,0);

    _timestamps.resize(10);
    _outfilt=None;

    _bPower = false;
    _bRenderData = false;
    _firstStep = true;
    using namespace boost::placeholders;
    RegisterCommand("histlen",boost::bind(&ForceSensor::SetHistoryLength,this,_1,_2),
                "Format: histlen len\n Assign a history depth in solver steps to the internal circular buffer. Note that this should not be done online");
    RegisterCommand("gethist",boost::bind(&ForceSensor::GetHistory,this,_1,_2),
                "Format: gethist\n Serialize the data history and timestamps and return");

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
    Vector zero (0,0,0);
    _movingsum.force=zero;
    _movingsum.torque=zero;

    FOREACH(it,_history){
        it->force=zero;
        it->torque=zero;
    }
};

bool ForceSensor::SimulationStep(OpenRAVE::dReal fTimeElapsed){

    //What does this do?
    if( _firstStep ) {
        _firstStep = false;
        if(!Init("ForceSensor"))
            return false;
    }
    dReal time=GetEnv()->GetSimulationTime();

    if(fTimeElapsed<0.0) return false;

    // Read the force and torque applied to a given link
    Vector force;
    Vector torque;
    //TODO: record a history of forces/ torques in a circular buffer
    //TODO: Filter results on-demand based on FIR filter
    Force6DSensorData data;

    if (_bPower ){
        GetEnv()->GetPhysicsEngine()->GetLinkForceTorque( _sensorLink, force,torque );
        data.force = force;
        data.torque = torque;

        _movingsum.force-=_history.back().force;
        _movingsum.torque-=_history.back().torque;
        _history.push_front(data);
        _timestamps.push_front(time);
        _movingsum.force+=data.force;
        _movingsum.torque+=data.torque;
    }
    return true;

};

SensorBase::SensorGeometryConstPtr ForceSensor::GetSensorGeometry(SensorType type){
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
    //First, filter the data on demand based on enabled filter
    _data->force=_movingsum.force;
    _data->force/=(dReal)_history.size();

    _data->torque=_movingsum.torque;
    _data->torque/=(dReal)_history.size();

    boost::mutex::scoped_lock lock(_mutexdata);
    *boost::dynamic_pointer_cast<Force6DSensorData>(psensordata) = *_data;

    return true;
};

void ForceSensor::SetTransform(const Transform& trans){
    _trans = trans;
}

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 25, 2)
const Transform& ForceSensor::GetTransform() {
#else
Transform ForceSensor::GetTransform() {
#endif
    return _trans;
};

bool ForceSensor::SetHistoryLength(std::ostream& os, std::istream& is){
    int l=0;
    if (!_bPower && !!is){
        is >> l;
        if (l>0){
            RAVELOG_DEBUG("Sensor off, changing history depth to %d\n",l);
            _history.resize(l);
            _timestamps.resize(l);
            Reset(0);
            return true;
        }
        else   RAVELOG_DEBUG("Depth %d is invalid, ignoring...\n",l);
    }
    return false;
}

bool ForceSensor::GetHistory(std::ostream& os, std::istream& is){

    //TODO:Lock this? could change halfway through..
    for (size_t i = 0; i < _history.size();++i){
        os << _timestamps[i] << " ";
        os << _history[i].force[0] << " ";
        os << _history[i].force[1] << " ";
        os << _history[i].force[2] << " ";
        os << _history[i].torque[0] << " ";
        os << _history[i].torque[1] << " ";
        os << _history[i].torque[2] << "\n";
    }
    return true;
}

