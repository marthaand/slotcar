
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
        
public class DBtester {
        public static void main(String[] args) 
    {
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
        dbi.load();
        dbi.register();
        dbi.logon();
        common.sleepSec(5);
        dbi.setAutoCommit(false);
        DBinterface.log(dbi,"[Tst msg]",true);
        dbi.rollback();
        dbi.logoff();
    }
}

