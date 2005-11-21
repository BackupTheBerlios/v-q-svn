/**
 * Simple example of using v-q from Java
 * javac -classpath ../../base/jdbc/jdbcd.jar DomVal1.java
 * java -Djava.naming.factory.initial=com.sun.jndi.cosnaming.CNCtxFactory -Djava.naming.provider.url=iiop://localhost:2809 -classpath ../../base/jdbc/jdbcd.jar:. DomVal1
 * @author Pawe³ Niewiadomski
 */

import com.foo_baz.v_q.*;
import com.foo_baz.v_q.ivqPackage.*;
import javax.naming.*;
import java.util.*;

public class DomVal1 {

	static public void main( String [] args ) throws Exception {
		org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args, null);
		String stage = "Init";
		try {
			stage = "acquiring InitialContext";
			Hashtable env = new Hashtable();
			env.put("java.naming.corba.orb", orb);
			Context ic = new InitialContext(env);

			stage = "lookup";
			ivq vqo = ivqHelper.narrow((org.omg.CORBA.Object) ic.lookup("VQ.ivq"));

			stage = "performing action";
			String dom = "123123.asd.com.pl";
			error err = vqo.dom_val(dom);
			if( err.ec == err_code.err_no ) {
				System.out.println("It's a valid domain name");
			} else {
				System.out.println("It's an invalid domain name");
			}
		} catch( Exception e ) {
			System.out.println("Exception in stage: "+stage);
			e.printStackTrace(System.out);
		}
		System.exit(0);
	}
}
