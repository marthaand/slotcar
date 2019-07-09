/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 */
public class MessageBuffer {
    private static String msgBuffer="";
    public static void dumpBuffer(){
        System.out.println ("=================================================================================");
        System.out.println ("DumpBuffer:");
        System.out.println ("Size:"+msgBuffer.length());
        System.out.println ("-----------");
        System.out.println ("00000000001111111111222222222233333333334444444444555555555566666666667");
        System.out.println ("01234567890123456789012345678901234567890123456789012345678901234567890");
        System.out.println (msgBuffer);
        System.out.println ("=================================================================================");
    }
    public static void clearBuffer(){
        msgBuffer="";
    }
    public static boolean msgAvailable(){
        if (msgBuffer.contains("]")){
            if (msgBuffer.contains("[")){
                if (msgBuffer.indexOf('[')<msgBuffer.indexOf(']')){
                    return true;
                }
                
            }
        }
        return false;
    }
    public static void printMsgAvailable(){
        if (msgAvailable()) System.out.println ("Message available"); else System.out.println ("Message NOT available");
    }
    public static void addFragment(String frag){
        msgBuffer=msgBuffer+frag;
    }
    public static String getMsg(){
        if (msgAvailable()==false) return "";
        int start=msgBuffer.indexOf('[');
        int end=1+msgBuffer.indexOf(']');
        String msg=msgBuffer.substring(start, end);
        msgBuffer=msgBuffer.substring(end);
        return msg;
    }
    public static void printIfAvailable(){
        if (msgAvailable()){
            System.out.println (getMsg());
        }
        else System.out.println ("Msg NOT AVAILABLE");
        
    }
    public static void main(String args[]){  
        printMsgAvailable();
        dumpBuffer();
        printMsgAvailable();
        addFragment("test");
        printMsgAvailable();
        dumpBuffer();
        printMsgAvailable();
        clearBuffer();
        printMsgAvailable();
        dumpBuffer();
        printMsgAvailable();
        addFragment("[test]");
        dumpBuffer();
        printMsgAvailable();
        addFragment("[test1]");
        addFragment("[test2]");
        addFragment("[test3]");
        addFragment("[test4]");
        dumpBuffer();
        printIfAvailable();
        dumpBuffer();
        printIfAvailable();
    }    
    
    
    
}
