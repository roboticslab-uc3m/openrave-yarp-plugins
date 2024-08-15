/*
 * ForceSensor.h
 *
 *  Created on: Jul 29, 2011
 *  Author:  Higinio Marti
 */
#ifndef FORCESENSOR_H_
#define FORCESENSOR_H_

#include <openrave/openrave.h>
#include <openrave/plugin.h>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>

using namespace OpenRAVE;
using namespace std;

enum FilterTypes {None=0,
    FirstOrderLowpass,
    MovingAverage
};

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
    virtual SensorGeometryConstPtr GetSensorGeometry(SensorType type);
    virtual SensorDataPtr CreateSensorData(SensorType type);
    virtual bool GetSensorData(SensorDataPtr psensordata);
    virtual void SetTransform(const Transform& trans);
    virtual const Transform& GetTransform();
    bool SetHistoryLength(std::ostream& os, std::istream& is);
    bool SetFilter(std::ostream& os, std::istream& is);
    bool GetHistory(std::ostream& os, std::istream& is);

    virtual bool Supports(SensorType type) { return type == ST_Force6D; }

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

     //Doesn't do anything yet
     Transform _trans;

     boost::shared_ptr<Force6DSensorData> _data;
     boost::circular_buffer<Force6DSensorData> _history;
     boost::circular_buffer<dReal> _timestamps;
     boost::shared_ptr<ForceSensorGeomData> _geom;

     KinBody::LinkConstPtr _sensorLink;
     //This doesn't seem to do anything at this point.
     KinBody::JointConstPtr _sensorJoint;

     mutable boost::mutex _mutexdata;
     bool _bRenderData, _bRenderGeometry, _bPower;
     FilterTypes _outfilt;
     Force6DSensorData _movingsum;

     friend class ForceSensorXMLReader;
};

class OpenraveYarpForceSensorPlugin : public RavePlugin
{
public:
    OpenRAVE::InterfaceBasePtr CreateInterface(OpenRAVE::InterfaceType type,
                                               const std::string & interfacename,
                                               std::istream & sinput,
                                               OpenRAVE::EnvironmentBasePtr penv) override;

    const InterfaceMap & GetInterfaces() const override;
    const std::string & GetPluginName() const override;
};

#endif
