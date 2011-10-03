from openravepy import *
import time

if __name__ == "__main__":
    
    try:
        
        env = Environment()
        env.SetViewer('qtcoin')
        env.Load('physicsRobot.env.xml') 
        time.sleep(1)
        
        physics = RaveCreatePhysicsEngine(env,'ode')
        physics.SetGravity([0,-9.81,0])
        env.SetPhysicsEngine(physics)
        
        robot = env.GetRobots()[0]
        forceSensor = robot.GetAttachedSensors()[0].GetSensor()
    
        force = 0
        torque = 0
        sumForce = 0
        sumTorque = 0
        sumForceBody = 0
        sumTorqueBody = 0
        
        env.StopSimulation()
        env.StartSimulation(timestep=0.001)
        for i in xrange(10000):
            print "#######################################################"
            data = forceSensor.GetSensorData()
            print "Force: ",  data.force
            print "Torque: ", data.torque
            sumForce = sumForce + data.force
            sumTorque = sumTorque + data.torque
            
            [force,torque] = physics.GetLinkForceTorque( robot.GetLink('Wrist') )
            print "Force on body: ", force
            print "Torque on body: ", torque
            sumForceBody = sumForceBody + force
            sumTorqueBody = sumTorqueBody + torque
            
            
        print "######################################"
        print "Force: ", sumForce/10000
        print "Torque: ", sumTorque/10000
        print "Force on body: ", sumForceBody/10000
        print "Torque on body: ", sumTorqueBody/10000
        raw_input('')
    
    finally:
        env.Destroy()
