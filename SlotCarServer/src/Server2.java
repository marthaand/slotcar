
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
public class Server2 extends Thread{
    private static boolean runControl=true;
    private static int nextThreadId=0;
    private DatabaseInterface dbi;
    private SerialPortHandler sph;
    private boolean SerialOpen=false;
    public Server2(){
    }
    public void run(){
        mainLoop();
    }
    public void mainLoop(){
        int threadId=nextThreadId++;
        while (runControl){
            //System.out.println("My thread is in running state:"+threadId);  //Kommentert ut
            try{
                Thread.sleep(5000);
            }
            catch (Exception E){
                System.out.println ("Sleep Exception");
            }
        }
    }
    public InterfaceParameters getIP(String args[]){
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
        return i;
    }
    
    public boolean connectDb(String args[]){
        InterfaceParameters i=getIP(args);
        dbi = new DatabaseInterface(i);
        boolean status=dbi.load();
        status=dbi.register();
        status=dbi.logon();
        //common.sleepSec(5);
        //dbi.setAutoCommit(false);
        //DBinterface.log(dbi,"[Tst msg]",true);
        //dbi.rollback();
        //dbi.logoff();
        return status;
    }
    public boolean send2serial(String msg) throws IllegalStateException, IOException{
        if (SerialOpen) return sph.send(msg);
        return true;
    }

    public boolean connectSerialPort(String args[]) throws InterruptedException, IOException{
        sph=new SerialPortHandler();
        sph.openPort(args[0],9600);
        sph.setupSerialPort (args[0],"9600");
        sph.registerDBI(dbi);
        SerialOpen=true;
        return true;
    }
    @SuppressWarnings("empty-statement")
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
        obj.mainLoop();
        while (true){}
  }    
}
