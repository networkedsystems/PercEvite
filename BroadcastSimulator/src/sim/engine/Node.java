package sim.engine;

import java.util.Arrays;
import java.util.Random;

public class Node {
	
	private char current;
	private double Ps,Pb;
	private Random r;
	private static long id_counter = 0;
	private long ID;
	private int Ttx;
	private double[] Vs;
	private double[] Vb;
	private double[] Vn;
	private char[] scan;
	private char[] net;
	private char[] broadcast;

	public Node(int Trx, int Tn, int Ttx){
		r = new Random();
		switch(r.nextInt(3)){
			case 0:
				current = 'S';
				break;
			case 1:
				current = 'N';
				break;
			case 2:
				current = 'B';
		}
		ID = id_counter;
		id_counter++;
		

		this.Ttx = Ttx;

		scan = new char[Trx];
		for(int i=0;i<Trx;i++) scan[i] = 'S';
		net = new char[Tn];
		for(int i=0;i<Tn;i++) net[i] = 'N';
		broadcast = new char[Ttx];
		for(int i=0;i<Ttx;i++) broadcast[i] = 'X';
		int t = r.nextInt(Ttx);
		broadcast[t] = 'B';

		Vs = new double[]{0.3,0.4,0.3};
		Vn = new double[]{0.3,0.4,0.3};
		Vb = new double[]{0.3,0.4,0.3};
		changeState(current);
		
	}

	public Node(int Trx, int Tn, int Ttx, double[]vs, double[]vn, double[]vb){
		r = new Random();
		switch(r.nextInt(3)){
			case 0:
				current = 'S';
				break;
			case 1:
				current = 'N';
				break;
			case 2:
				current = 'B';
		}
		ID = id_counter;
		id_counter++;

		this.Ttx = Ttx;

		
		Vs = vs.clone();
		Vn = vn.clone();
		Vb = vb.clone();
		scan = new char[Trx];
		for(int i=0;i<Trx;i++) scan[i] = 'S';
		net = new char[Tn];
		for(int i=0;i<Tn;i++) net[i] = 'N';
		broadcast = new char[Ttx];
		for(int i=0;i<Ttx;i++) broadcast[i] = 'X';
		int t = r.nextInt(Ttx);
		broadcast[t] = 'B';
		changeState(current);
		
	}

	private void changeState(char state){
		switch(state){
			case 'S':
				Ps = Vs[0];
				Pb = Vs[2];
				current = 'S';
				break;
			case 'N':
				Ps = Vn[0];
				Pb = Vn[2];
				current = 'N';
				break;
			case 'B':
				Ps = Vb[0];
				Pb = Vb[2];
				current = 'B';
				break;
		}
	}

	public void run(){
		
		double thr1 = Ps;
		double thr2 = Pb+thr1;
		double v = r.nextDouble();
		
		if(v < thr1){
			changeState('S');
			
		}
		else if(v < thr2){
			changeState('B');
			
		}
		else{
			changeState('N');
		}
		
	}

	public char[] getCurrentState(){

		switch(current){
			case 'S':
				return scan;
			case 'N':
				return net;
			case 'B':
				return broadcast;
		}

		return broadcast;
	}

	public double[][] getPm(){
		var m = new double[][]{Vs,Vn,Vb};
		return m;
	}

	public long getID(){
		return ID;
	}

	public void setScanTransitions(double Pss, double Psn, double Psb){
		Vs[0] = Pss;
		Vs[1] = Psn;
		Vs[2] = Psb;
	}
	public void setBroadcastTransitions(double Pbs, double Pbn, double Pbb){
		Vb[0] = Pbs;
		Vb[1] = Pbn;
		Vb[2] = Pbb;
	}
	public void setNetworkTransitions(double Pns, double Pnn, double Pnb){
		Vn[0] = Pns;
		Vn[1] = Pnn;
		Vn[2] = Pnb;
	}

	
}

