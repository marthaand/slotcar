/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author FPEDERSE
 */
public class Server extends Thread{
    private static boolean runControl=true;
    private static int nextThreadId=0;
    public Server(){
    }
    public void run(){
        mainLoop();
    }
    public void mainLoop(){
        int threadId=nextThreadId++;
        while (runControl){
            //System.out.println("My thread is in running state:"+threadId); //Kommentert ut
            try{
                Thread.sleep(5000);
            }
            catch (Exception E){
                System.out.println ("Sleep Exception");
            }
        }
    }
    public static void main(String args[]){  
        Server obj=new Server();   
        obj.start(); 
        obj.mainLoop();
  }    
}
