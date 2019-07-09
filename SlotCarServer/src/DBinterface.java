
import mmsys.core.lib.communication.jdbc.DatabaseInterface;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 */
public class DBinterface { 
    public static boolean sensorlog(DatabaseInterface dbi,int nodeid, int sensorid,boolean printsql) {
        int trackid = 1;
        if (sensorid==2) trackid = 2;
        if (sensorid==4) trackid = 2;
        if (sensorid==6) trackid = 2;
        String sql = "update SENSORDETECTED set nodeid="+ nodeid + ", sensorid="+ sensorid + " where trackid="+trackid;
        if (printsql) System.out.println ("*" + sql);
        boolean status = dbi.dml(sql);
        return status;
    }
    public static boolean log(DatabaseInterface dbi,String msg,boolean printsql){
        // create table SClog(ts1 date,ts2 timestamp,msg varchar2(100),raceid number,trackid number,sensorid number,driverid1 number,driverid2 number,rfid1 varchar2(50),rfid2 varchar2(50),eventtype varchar2(10),ts3 number, eventvalue varchar2(50)) 
        String sql="insert into sclog (ts1,ts2,msg) values (sysdate,systimestamp,'"+msg+"')";
        if (printsql) System.out.println ("*" + sql);
        boolean status = dbi.dml(sql);
        if (msg.contains("[RACE:START:")){
            status=sensorlog(dbi,9,1,printsql);
            status=sensorlog(dbi,9,2,printsql);
        }
        if (msg.contains("detect track 1[FINISH TIME:")){
            status = sensorlog(dbi,9,8,printsql);           //MÅ LEGGES INN FOR TRACK 1 VED to SELVKJØRENDE
        }
        if (msg.contains("[NODE:1:SENSOR:1]")) status=sensorlog(dbi,1,1,printsql);
        if (msg.contains("[NODE:1:SENSOR:2]")) status=sensorlog(dbi,1,2,printsql);
        if (msg.contains("[NODE:1:SENSOR:3]")) status=sensorlog(dbi,1,3,printsql);
        if (msg.contains("[NODE:1:SENSOR:4]")) status=sensorlog(dbi,1,4,printsql);
        if (msg.contains("[NODE:1:SENSOR:5]")) status=sensorlog(dbi,1,5,printsql);
        if (msg.contains("[NODE:1:SENSOR:6]")) status=sensorlog(dbi,1,6,printsql);
        if (msg.contains("[NODE:2:SENSOR:1]")) status=sensorlog(dbi,2,1,printsql);
        if (msg.contains("[NODE:2:SENSOR:2]")) status=sensorlog(dbi,2,2,printsql);
        if (msg.contains("[NODE:2:SENSOR:3]")) status=sensorlog(dbi,2,3,printsql);
        if (msg.contains("[NODE:2:SENSOR:4]")) status=sensorlog(dbi,2,4,printsql);
        if (msg.contains("[NODE:2:SENSOR:5]")) status=sensorlog(dbi,2,5,printsql);
        if (msg.contains("[NODE:2:SENSOR:6]")) status=sensorlog(dbi,2,6,printsql);
        if (msg.contains("[NODE:3:SENSOR:1]")) status=sensorlog(dbi,3,1,printsql);
        if (msg.contains("[NODE:3:SENSOR:2]")) status=sensorlog(dbi,3,2,printsql);
        if (msg.contains("[NODE:3:SENSOR:3]")) status=sensorlog(dbi,3,3,printsql);
        if (msg.contains("[NODE:3:SENSOR:4]")) status=sensorlog(dbi,3,4,printsql);
        if (msg.contains("[NODE:3:SENSOR:5]")) status=sensorlog(dbi,2,5,printsql);
        if (msg.contains("[NODE:3:SENSOR:6]")) status=sensorlog(dbi,2,6,printsql);
        if (msg.contains("[NODE:4:SENSOR:1]")) status=sensorlog(dbi,4,1,printsql);
        if (msg.contains("[NODE:4:SENSOR:2]")) status=sensorlog(dbi,4,2,printsql);
        if (msg.contains("[NODE:4:SENSOR:3]")) status=sensorlog(dbi,4,3,printsql);
        if (msg.contains("[NODE:4:SENSOR:4]")) status=sensorlog(dbi,4,4,printsql);
        if (msg.contains("[NODE:4:SENSOR:5]")) status=sensorlog(dbi,4,5,printsql);
        if (msg.contains("[NODE:4:SENSOR:6]")) status=sensorlog(dbi,4,6,printsql);
        return status;                                                  
    }
    
}
