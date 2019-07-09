
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
import mmsys.core.lib.communication.jdbc.DatabaseInterface;


/**
 *
 * @author FPEDERSE
 */
public class DBinterfaceRFID {
    private static boolean Race=false;
    public static String sqlcnt="select count(*) from driver where activated=1";
    public static boolean raceManager(DatabaseInterface dbi,String msg,boolean printsql){
        if (msg.startsWith("Card UID:")){
            System.out.println("RFID:"+msg);
        }
        else return true;
         String rfid=msg.substring(10,30);
         System.out.println ("RFID ID:"+rfid+":");
         String sql="select count(*) from rfidtrackcard where rfid='"+rfid+"'";
         System.out.println (sql);
         int trackCnt=dbi.getInt(sql);
         System.out.println ("cnt:"+trackCnt);
         if (trackCnt==1){
             sql="select trackid from rfidtrackcard where rfid='"+rfid+"'";
             int trackId=dbi.getInt(sql);
             System.out.println ("Activate Track:"+trackId);
             dbi.setAutoCommit(true);
             sql="update rfidtrackcard set activated=null";
             dbi.dml(sql);
             sql="update rfidtrackcard set activated=1 where rfid='"+rfid+"'";
             dbi.dml(sql);
             dbi.setAutoCommit(false);
             System.out.println ("Activate Track Input Finished - enter driver card");
             return true;
         }
         String sqlp=" from driver where rfid='"+rfid+"'";
         sql="select count(*)"+sqlp; 
         int driverCnt=dbi.getInt(sql);
         if (driverCnt==0){
             System.out.println ("Unknnwn Card:"+rfid);
         }
         System.out.println ("Card identified as Drivercard:"+rfid);
         sql="select id" + sqlp;
         int driverId=dbi.getInt(sql);
         System.out.println (dbi.getString("select substr(firstname,1,1)||substr(lastname,1,1)"+sqlp));
         dbi.setAutoCommit(false);
         //sql="update driver set trackid=null where trackid is not null";
         //sql="update driver set trackid=null where trackid = "+
         //dbi.dml(sql);
         sql="update driver set trackid=(select trackid from rfidtrackcard where activated=1) where rfid='"+rfid+"'";
         int trackid=dbi.getInt("select trackid from rfidtrackcard where activated=1");
         dbi.dml(sql);
         sql="insert into driverhistory (driverid,trackid,ts) values ("+driverId+","+trackid+",sysdate)";
         dbi.dml(sql);
         dbi.setAutoCommit(true);                  
        return true;
    }
    public static boolean log(DatabaseInterface dbi,String msg,boolean printsql,boolean raceManager){
        printsql=true;
        // create table SClog(ts1 date,ts2 timestamp,msg varchar2(100),raceid number,trackid number,sensorid number,driverid1 number,driverid2 number,rfid1 varchar2(50),rfid2 varchar2(50),eventtype varchar2(10),ts3 number, eventvalue varchar2(50)) 
        //String sql="insert into sclog (ts1,ts2,msg) values (sysdate,systimestamp,'"+msg+"')";
        System.out.println ("RaceManageR:"+raceManager);
        if (raceManager){
            return raceManager(dbi,msg,printsql);
        }
        String sql="";
        boolean status=true;
        // 0123456789012345678901234567890
        // Card UID: 04 6D BA FA 58 49 80

        //if (msg.startsWith("In hex:")){
        if (msg.startsWith("Card UID:")){
            System.out.println("RFID:"+msg);
            if (Race==false){
                int cnt=dbi.getInt(sqlcnt);
                if (cnt!=1){
                    System.out.println ("==============================================================================");
                    System.out.println (" Dataerror : Multiple drivers activated - cannot register RFID card");
                    System.out.println ("------------------------------------------------------------------------------");
                    return(false);
                }
                //012345678901234567890
                //In hex:  8E D5 5A 68

                //String rfid=msg.substring(9,20);
                System.out.println ("msg:"+msg+":");
                String rfid=msg.substring(10,30);
                System.out.println ("RFID ID:"+rfid+":");
                sql="update driver set rfid='"+rfid+"' where activated=1 ";
                if (printsql) System.out.println (sql);
                status=dbi.dml(sql);
                if (status){
                    System.out.println ("==============================================================================");
                    System.out.println (" User updated with new RFID");
                    System.out.println ("------------------------------------------------------------------------------");
                    return true;
                }
 
            }
        }
        //System.out.println ("==============================================================================");
        //System.out.println ("Error canot handle msg:"+msg);
        //System.out.println ("------------------------------------------------------------------------------");
        return false;
    }
    
}
