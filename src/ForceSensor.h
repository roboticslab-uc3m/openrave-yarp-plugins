/*
 * ForceSensor.h
 *
 *  Created on: Jul 29, 2011
 *  Author:  Higinio Marti
 */
#ifndef FORCESENSOR_H_
#define FORCESENSOR_H_

#include <rave/rave.h>
#include <boost/bind.hpp>

using namespace OpenRAVE;
using namespace std;

class ForceSensor : public SensorBase
{
protected:
	class ForceSensorXMLReader : public BaseXMLReader
	    {
	    public:
	    	ForceSensorXMLReader(boost::shared_ptr<ForceSensor> psensor) : _psensor(psensor) {}

	    	virtual ProcessElement startElement(const std::string& name, const std::list<std::pair<std::string,std::string> >& atts);
	    	virtual bool endElement(const std::string& name);
	    	virtual void characters(const std::string& ch);

	    protected:
	        BaseXMLReaderPtr _pcurreader;
	        boost::shared_ptr<ForceSensor> _psensor;
	        stringstream ss;
	    };

public:

	/// permanent properties of the sensor
	class ForceSensorGeomData : public Force6DGeomData
	{
	public:
		virtual SensorType GetType() { return ST_Force6D; }
	};

	 static BaseXMLReaderPtr CreateXMLReader(InterfaceBasePtr ptr, const std::list<std::pair<std::string,std::string> >& atts)
	{
		return BaseXMLReaderPtr(new ForceSensorXMLReader(boost::dynamic_pointer_cast<ForceSensor>(ptr)));
	}

	/************************************************************************/
    ForceSensor(EnvironmentBasePtr penv);
    virtual ~ForceSensor(){};
    
    //Sensor Interface
	virtual bool Init(const string& args);
	virtual void Reset(int options);
	virtual bool SimulationStep(OpenRAVE::dReal fTimeElapsed);
	virtual SensorGeometryPtr GetSensorGeometry(SensorType type);
	virtual SensorDataPtr CreateSensorData(SensorType type);
	virtual bool GetSensorData(SensorDataPtr psensordata);
	virtual void SetTransform(const Transform& trans);
	virtual Transform GetTransform();

	virtual bool Supports(SensorType type) { return type == ST_Force6D; }

    bool MyCommand(std::ostream& sout, std::istream& sinput)
    {
        std::string input;
        sinput >> input;
        sout << "output";
        return true;
    }

    virtual int Configure(ConfigureCommand command, bool blocking)
	{
		switch(command) {
		case CC_PowerOn:
			_bPower = true;
			return _bPower;
		case CC_PowerOff:
			_bPower = false;
			//_Reset();
			return _bPower;
		case CC_PowerCheck:
			return _bPower;
		case CC_RenderDataOn:
			_bRenderData = true;
			return _bRenderData;
		case CC_RenderDataOff: {
			boost::mutex::scoped_lock lock(_mutexdata);
			_bRenderData = false;
			return _bRenderData;
		}
		case CC_RenderDataCheck:
			return _bRenderData;
		case CC_RenderGeometryOn:
			_bRenderGeometry = true;
			return _bRenderData;
		case CC_RenderGeometryOff: {
			_bRenderGeometry = false;
			return _bRenderData;
		}
		case CC_RenderGeometryCheck:
			return _bRenderGeometry;
		}
		throw openrave_exception(str(boost::format("SensorBase::Configure: unknown command 0x%x")%command));
	}

protected:

     bool _firstStep;

	 Transform _trans;

	 boost::shared_ptr<Force6DSensorData> _data;
	 boost::shared_ptr<ForceSensorGeomData> _geom;

	 KinBody::LinkConstPtr _sensorLink;
     //This doesn't seem to do anything at this point. 
	 KinBody::JointConstPtr _sensorJoint;

	 mutable boost::mutex _mutexdata;
	 bool _bRenderData, _bRenderGeometry, _bPower;

	 friend class ForceSensorXMLReader;
};

#endif
