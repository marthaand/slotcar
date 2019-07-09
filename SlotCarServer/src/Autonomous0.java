
import java.io.IOException;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 */
public class Autonomous0 {
    public static void main(String args[]) throws InterruptedException, IOException{
        SerialPortHandler sph=new SerialPortHandler();
        sph.openPort(args[0],9600);
        sph.setupSerialPort (args[0],"9600");
        boolean test=true;
        while (true){
            test=sph.send("0");
            mmsys.core.lib.basic.common.sleepMS(3000);
            test=sph.send("5");
            mmsys.core.lib.basic.common.sleepMS(3000);
        }   
    }
}
