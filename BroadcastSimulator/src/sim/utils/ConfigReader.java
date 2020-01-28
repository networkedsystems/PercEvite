package sim.utils;
import java.io.File;
import java.util.Scanner;

public final class ConfigReader{

    private ConfigReader(){}
    
    public static WiFiConfig readConfigFile(String filename){
        //Improve parsing:
        //- Add read of multiple configurations

        WiFiConfig res = null;
        File file = new File(filename);
        try(Scanner s = new Scanner(file)){
            char n;
            double[] Ps = null;
            double[] Pn = null;
            double[] Pb = null;
            int trx=100,tn=100,ttx=100;
            while(s.hasNextLine()){
                String tmp = s.nextLine();
                if(tmp.isBlank() || tmp.isEmpty()){
                    continue;
                }
                if(tmp.contains("Ttx")){
                    String[] split = tmp.split("=");
                    ttx = Integer.parseInt(split[1].strip());
                    continue;
                }
                else if(tmp.contains("Trx")) {
                    String[] split = tmp.split("=");
                    trx = Integer.parseInt(split[1].strip());
                    continue;
                }
                else if(tmp.contains("Tn")){
                    String[] split = tmp.split("=");
                    tn = Integer.parseInt(split[1].strip());
                    continue;
                }
                String [] split = tmp.split(";");
                n = split[0].charAt(0);
                switch(n){
                    case 'S':
                        Ps = new double[]{Double.valueOf(split[1]),Double.valueOf(split[2]),Double.valueOf(split[3])};
                        break;
                    case 'N':
                        Pn = new double[]{Double.valueOf(split[1]),Double.valueOf(split[2]),Double.valueOf(split[3])};
                        break;
                    case 'B':
                        Pb = new double[]{Double.valueOf(split[1]),Double.valueOf(split[2]),Double.valueOf(split[3])};
                        break;
                }
            }
            if(Ps != null && Pn != null && Pb != null){
                res = new WiFiConfig(trx,tn,ttx,Ps,Pn,Pb);
            }
            else{
                throw new Exception(" -> Incomplete configuration file <- ");
            }
            
        }
        catch(Exception e){
            System.err.println(e.getMessage());
        }
      
        return res;


    }

}