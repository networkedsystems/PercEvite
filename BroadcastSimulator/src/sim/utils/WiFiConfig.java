package sim.utils;


public class WiFiConfig{

    public final double[] Vs,Vb,Vn;
    public final int Trx,Tn,Ttx;
        
    public WiFiConfig(WiFiConfig s){
        Vs = s.Vs.clone();
        Vn = s.Vn.clone();
        Vb = s.Vb.clone();
        Trx = s.Trx;
        Tn = s.Tn;
        Ttx = s.Ttx;
    }
    
    public WiFiConfig(int trx, int tn, int ttx, double[] ps, double[] pn, double[] pb){
        Vs = ps.clone();
        Vn = pn.clone();
        Vb = pb.clone();
        Trx = trx;
        Tn = tn;
        Ttx = ttx;
            
    }
    
}