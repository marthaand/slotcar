
import java.io.IOException;
import mmsys.core.lib.communication.jdbc.DatabaseInterface;
import mmsys.core.lib.communication.jdbc.InterfaceParameters;
import mmsys.core.lib.communication.jdbc.SelectCursor;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 * create table simulator(id number,msg varchar2(50),delayms number)
insert into simulator values (1,'[SC:SENSOR:1:1]',500);
insert into simulator values (2,'[SC:SENSOR:2:1]',500);
insert into simulator values (1,'[SC:SENSOR:1:0]',500);
insert into simulator values (2,'[SC:SENSOR:2:0]',5000);
 */
public class Simulator {
    DatabaseInterface dbiSim;
    public Simulator(){
    } 
    public boolean connectSimulatorDB(Server2 obj,String Args[]){
        InterfaceParameters i=obj.getIP(Args);
        dbiSim = new DatabaseInterface(i);
        boolean status=dbiSim.load();
        status=dbiSim.register();
        status=dbiSim.logon();
        //common.sleepSec(5);
        //dbi.setAutoCommit(false);
        //DBinterface.log(dbi,"[Tst msg]",true);
        //dbi.rollback();
        //dbi.logoff();
        return status;
    }
    public void simulatorLoop(Server2 obj) throws IllegalStateException, IOException{
        String sql="select id,msg,delayms from simulator order by id";
        System.out.println (sql);
        while (true){
        SelectCursor SC=new SelectCursor(dbiSim,sql);
         while (SC.next()){
             String msg=SC.getString(2);
             
            System.out.println (msg);
            obj.send2serial(msg);
            int delayms=SC.getInt(3);
            System.out.println ("WaitMS:"+delayms);
            mmsys.core.ll.lib.basic.Common.sleepMS(delayms);
         }
         
        }

    }
    
    public static void main(String args[])throws InterruptedException, IOException{
        Server2 obj=new Server2();   
        boolean status=obj.connectDb(args);
        if (status==false){
            System.out.println ("Connect db error - exit");
            mmsys.core.ll.lib.basic.Common.sleepSec(10);
            System.exit(100);      
        }
        if (args.length>0){
            status=obj.connectSerialPort(args);
            if (status==false){
                 System.out.println ("Connect to serial port error - exit");
                mmsys.core.ll.lib.basic.Common.sleepSec(10);
                System.exit(100);      
            }
        }
        obj.start(); 
        Simulator sim=new Simulator();
        status=sim.connectSimulatorDB(obj,args);
        if (status==false){
            System.out.println ("Connect simulator db error - exit");
            mmsys.core.ll.lib.basic.Common.sleepSec(10);
            System.exit(100);      
        }
        sim.simulatorLoop(obj);
        //obj.mainLoop();
        
  }    

}
