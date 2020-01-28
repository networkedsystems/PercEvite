package sim.utils;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Logger{
    String path;
    ArrayList<char[]> logContent;
    
    public Logger(String path){
        this.path = path;
        logContent = new ArrayList<>();

    }
    public Logger(String path, int size){
        this.path = path;
        logContent = new ArrayList<>(size);

    }

    public void log(String txt){
        logContent.add(txt.toCharArray());
    }
    public void log(char txt){
        logContent.add(new char[]{txt});
    }

    public void log(char[] txt){
        logContent.add(txt);
    }

    public void log(Object o){
        logContent.add(String.valueOf(o).toCharArray());
    }

    public void dump(boolean append){
        try(FileWriter fw = new FileWriter(path,append)){
            BufferedWriter bf = new BufferedWriter(fw);
            for(var s: logContent){
                
                bf.write(s);
            }
            logContent.clear();
            bf.close();
        }
        catch(IOException ioe){
            ioe.printStackTrace();
        }
        finally{
            System.out.println("Log "+path+"succesfully written");
        }
    }

    public void dump(String separator){
        try(FileWriter fw = new FileWriter(path,true)){
            BufferedWriter bf = new BufferedWriter(fw);
            bf.write(separator);
            for(var s: logContent){
                
                bf.write(s);
            }
            logContent.clear();
            bf.close();
        }
        catch(IOException ioe){
            ioe.printStackTrace();
        }
        finally{
            
            System.out.println("Log "+path+"succesfully written");
        }
    }

    public void byteDump(byte ts, byte tn, byte tb){
        try{
            byte[] out = encode(ts, tn, tb);
            Path pth = Paths.get(path);
            Files.write(pth, out); //creates, overwrites
            logContent.clear();
           
        }
        catch(Exception ioe){
            ioe.printStackTrace();
        }
        finally{
            System.out.println("Log "+path+"succesfully written");
        }
    }

    private byte[] encode(byte ts, byte tn, byte tb){
        byte[] output = new byte[logContent.size()];
        for(int i=0;i<output.length;i++){
            var ca = logContent.get(i);
            if(ca[0]=='S'){
                output[i] = (byte) -ts;
            }
            else if(ca[0]=='N'){
                output[i] = (byte) -tn;
            }
            else{
                for(byte j=0;j<tb;j++){
                    if(ca[j] == 'B'){
                        output[i] = j;
                        break;
                    }
                }
            }
        }
        return output;
    }
}