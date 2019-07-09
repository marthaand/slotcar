
import com.pi4j.io.serial.Baud;
import com.pi4j.io.serial.DataBits;
import com.pi4j.io.serial.FlowControl;
import com.pi4j.io.serial.Parity;
import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialConfig;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataEventListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.StopBits;
import java.io.IOException;
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
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 */
import com.pi4j.io.serial.*;
import com.pi4j.io.serial.Baud;
import com.pi4j.io.serial.DataBits;
import com.pi4j.io.serial.FlowControl;
import com.pi4j.io.serial.Parity;
import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialConfig;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataEventListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.StopBits;
import java.io.IOException;
import mmsys.core.lib.communication.jdbc.DatabaseInterface;
public class SerialPortHandlerRFID {
    private final Serial serial = SerialFactory.createInstance();
    private boolean write2Console=true;
    private boolean write2Buffer=true;
    private DatabaseInterface dbi;
    private boolean connected2DB=false;
        private boolean RaceManager=false;


    SerialPortHandlerRFID(){
    }
    public void registerDBI(DatabaseInterface newDBI){
        dbi=newDBI;
        connected2DB=true;
    }
    public boolean openPort (String name,int baud){
        return true;
    }
   public boolean setupSerialPort (String portNameParam,String portSpeed) throws InterruptedException, IOException {
    
            serial.addListener(new SerialDataEventListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                try {
                    String msgReceived=event.getAsciiString();
                    System.out.println (msgReceived);
                    if (connected2DB){
                        DBinterfaceRFID.log(dbi,msgReceived,true,RaceManager);
                    }
                }
                    catch (IOException e) {
                    e.printStackTrace();
                }
                catch(Exception e){System.out.println ("Serial Port error");
                        
               }
            }
       });

        try {
            SerialConfig config = new SerialConfig();
            config.device(portNameParam)
                  .baud(Baud._9600)
                  .dataBits(DataBits._8)
                  .parity(Parity.NONE)
                  .stopBits(StopBits._1)
                  .flowControl(FlowControl.NONE);
            System.out.println("[MONITOR] Connecting to: " + config.toString());
           serial.open(config);
           
        }
        catch(IOException ex) {
            System.out.println("[MONITOR] ==>> SERIAL SETUP FAILED : " + ex.getMessage());
            return false;
        }
        return true;
        
    }
   public boolean send(String msg) throws IllegalStateException, IOException{
       serial.write(msg);
       serial.flush();
       return true;
   }
   public void setRaceManager(boolean b){
       RaceManager=true;
   }

    public static void main(String args[]) throws InterruptedException, IOException{
        SerialPortHandler sph=new SerialPortHandler();
        sph.openPort(args[0],9600);
        sph.setupSerialPort (args[0],"9600");
        while (true){}
    }
}
