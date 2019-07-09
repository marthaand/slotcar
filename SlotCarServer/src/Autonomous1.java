
import java.io.IOException;
import java.sql.Connection;
import mmsys.core.lib.basic.common;
import mmsys.core.lib.communication.jdbc.DatabaseInterface;
import mmsys.core.lib.communication.jdbc.InterfaceParameters;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 */
public class Autonomous1 {
    private String RaceStatus="PREPARE";
    //private String RaceStatus="TEST";         //IF TESTING
    private int Stopms=0;
    public Autonomous1(){
    }
    public DatabaseInterface connectDB(String args[]){
         //       DriverRegistry dr=new DriverRegistry();
        String url="";
        String driver="";
        String user="";
        String pwd="";
        boolean oracle=true;
        if (oracle){
            driver="oracle.jdbc.driver.OracleDriver";
            url="jdbc:oracle:thin:@localhost:1521:XE";
            url="jdbc:oracle:thin:@";
            user="slotcar";
            pwd="slotcar#2019"; //Cloud
            pwd="slotcar";      //Local DB
        }
        else{                                       //Not in use
            driver="com.mysql.jdbc.Driver";
            url="jdbc:mysql://remote-mysql3.servage.net/mmweb";
            user="mmweb";
            pwd="Qwerty*12"; 
        }
        if (args.length>2){
            url=url+args[2];
        }
        else{
            //url=url+"127.0.0.1:1521:orcl";
            url=url+"127.0.0.1:1521/orcl";
            url="jdbc:oracle:thin:@130.61.112.73:1521/fpederse.secomsub3.secommonvnc.oraclevcn.com";
            url="jdbc:oracle:thin:@10.0.0.5:1521/pdb1.no.oracle.com";
            
        }
        //url="jdbc:oracle:thin:@localhost:1521/orcl";
        System.out.println ("URL:"+url);
        InterfaceParameters i=new InterfaceParameters(driver,url,user,pwd);
        DatabaseInterface dbi = new DatabaseInterface(i);
        boolean status=dbi.load();
        status=dbi.register();
        status=dbi.logon();
        common.sleepSec(5);
        status=dbi.setAutoCommit(true);
         //      Server2 obj=new Server2();   
        //boolean status=obj.connectDb(args);
        if (status==false){
            System.out.println ("Connect db error - exit");
            mmsys.core.ll.lib.basic.Common.sleepSec(10);
            System.exit(100);      
        }
        return dbi;
    }
    public String getSpeed(boolean simulator,DatabaseInterface dbi){                 // PEK TIL RENEE; DETTE MÅ BLI SMARTERE
        //String sql="select trackstatus*100 + nodeid*10+sensorid from sensordetected where trackid=2"; //HUSK Å SE PÅ SPORNUMMER. AUTONOM = 2
        String sql="select nodeid*10+sensorid from sensordetected where trackid=2";
        int sensor=dbi.getSQLInt(sql);
        if (simulator) System.out.println ("Sensor:"+sensor);
        //if (sensor==91) return "3";
        if (RaceStatus.equals("PREPARE")){
            if(sensor==34){                     //ENDRES TIL 44 OG KORTERE STOPPTID
                Stopms=2000;
                
                System.out.println("RaceStatus: "+ RaceStatus);   //FÅ INN ALLE STEDER VI BYTTER STATUS
                return "3";
            }
        }
        else {
        if (sensor==92) return "3";     //Start race
        if (sensor==98){ 
            if (RaceStatus.equals("READY")){
            RaceStatus = "PREPARE";
            System.out.println("RaceStatus: " + RaceStatus);
            return "3";
            }
        }      //Stop race
        if (sensor==22) return "5";     //BYGG INN KLOKKEFUNKSJON?
        if (sensor==32) return "5";
        //if (sensor==34) return "3";
        }
        return "3";
    }
    public void sendMessage(SerialPortHandler sph, boolean simulator,String newSpeed)throws InterruptedException, IOException{               
        if (simulator){
            System.out.println ("Simulator New Speed:"+newSpeed);
            }
            else{
                System.out.println ("New Speed:"+newSpeed);
                boolean test=sph.send(newSpeed);
                mmsys.core.lib.basic.common.sleepMS(100);
            }
        if (simulator) mmsys.core.lib.basic.common.sleepMS(1000);
    }
    public void connectSerial(DatabaseInterface dbi,boolean simulator,String args[])throws InterruptedException, IOException{
        System.out.print("Connect serial port :");
        SerialPortHandler sph=new SerialPortHandler();
        if (simulator){
            System.out.println ("Simulator mode");
        }
        else{
            System.out.println (args[0]);
            sph.openPort(args[0],9600);
            sph.setupSerialPort (args[0],"9600");
        }
        boolean test=true;       
        String speed="0";
        System.out.println ("Set  Speed:"+speed);
        if (simulator==false){
            test=sph.send("0");
            mmsys.core.lib.basic.common.sleepMS(1000);
            test=sph.send("0");
            mmsys.core.lib.basic.common.sleepMS(1000);
        }
        while (true){
            String newSpeed=getSpeed(simulator,dbi);
            if ((Stopms > 0) && (RaceStatus.equals("PREPARE"))) {
                RaceStatus = "READY";
                System.out.println("RaceStatus: " + RaceStatus);
                sendMessage(sph, simulator, newSpeed);
                mmsys.core.lib.basic.common.sleepMS(Stopms);
                Stopms=0;
                sendMessage(sph, simulator, "0");
            }
            if (newSpeed.equals(speed)==false){
                sendMessage(sph, simulator, newSpeed);
            }
            speed=newSpeed;
        }   
    }
    public static void main(String args[]) throws InterruptedException, IOException{
        boolean simulator=true;
        // Linux ???
        simulator=false;
        Autonomous1 a=new Autonomous1();
        DatabaseInterface dbi=a.connectDB(args);
        Connection con=dbi.dbConnection();
        a.connectSerial(dbi,simulator,args);
    }
}

    

