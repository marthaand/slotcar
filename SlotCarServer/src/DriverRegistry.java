
import java.io.IOException;
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
import mmsys.core.lib.basic.common;

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
import mmsys.core.lib.communication.jdbc.InterfaceParameters;



public class DriverRegistry {
    //private DatabaseInterface dbi;
    private SerialPortHandlerRFID sph;
    private boolean SerialOpen=false;
    
    public DriverRegistry(){
        
    } 
    public void setRaceManager(){
        sph.setRaceManager(true);
    }
 
    public boolean connectSerialPort(String args[],DatabaseInterface dbi) throws InterruptedException, IOException{
        sph=new SerialPortHandlerRFID();
        sph.openPort(args[0],9600);
        sph.setupSerialPort (args[0],"9600");
        sph.registerDBI(dbi);
        SerialOpen=true;
        return true;
    }
        public static void main(String[] args) throws InterruptedException, IOException 
    {
        DriverRegistry dr=new DriverRegistry();
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
            pwd="slotcar"; 
        }
        else{
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
        if (args.length>0){
            status=dr.connectSerialPort(args,dbi);
            if (status==false){
                 System.out.println ("Connect to serial port error - exit");
                mmsys.core.ll.lib.basic.Common.sleepSec(10);
                System.exit(100);      
            }
        }
                boolean cont=true;
        while (cont){
            //DBinterface.log(dbi,"[Tst msg]",true);
            //dbi.rollback();
            System.out.println ("*");
            mmsys.core.ll.lib.basic.Common.sleepSec(10000);
        }
        dbi.logoff();

    }
}


