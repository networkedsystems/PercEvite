package sim.utils;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;

public class CharLogger {
    char[] buffer;
    String path;
    int current_index;

    public CharLogger(String filename, int size){
        buffer = new char[size];
        Arrays.fill(buffer, 'X');
        path = filename;
        current_index = 0;
    }


    public void log(char[] txt){
        System.arraycopy(txt, 0, buffer, current_index, txt.length);
        current_index += txt.length-1;

    }

    public void dump(boolean append){
        char [] fin_buf = new char[current_index+1];
        System.arraycopy(buffer, 0, fin_buf, 0, fin_buf.length);
        try(FileWriter fw = new FileWriter(path,append)){
            BufferedWriter bf = new BufferedWriter(fw);
           
            bf.write(fin_buf);
            
            bf.close();
        }
        catch(IOException ioe){
            ioe.printStackTrace();
        }
        finally{
            //System.out.println("Log "+path+"succesfully written");
        }
    }


}