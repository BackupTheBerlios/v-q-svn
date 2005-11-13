/**
 * Simple example of using v-q from Java
 * @author Pawe³ Niewiadomski
 */

import com.foo_baz.v_q.*;
import com.foo_baz.v_q.ivqPackage.*;
import org.omg.CORBA.*;
import org.omg.CosNaming.*;

public class DomList {

	static public void main( String [] args ) throws Exception {
		ORB orb = ORB.init(args, null);

		org.omg.CORBA.Object obj = orb.resolve_initial_references("NameService");

		NamingContext rootCtx = NamingContextHelper.narrow(obj);
		if( rootCtx == null ) {
			System.out.println("Failed to narrow the root naming context");
			System.exit(1);
		}

		NameComponent name = new NameComponent("VQ.ivq", "");
		NameComponent [] path = { name };
		ivq vqo = ivqHelper.narrow(rootCtx.resolve(path));
	
		domain_info_listHolder diList = new domain_info_listHolder();
		error err = vqo.dom_ls(diList);
		if( err.ec == err_code.err_no ) {
			System.out.println("Following domains are registered within system:");
			for( int i=0, s=diList.value.length; i<s; ++i ) {
				System.out.println(diList.value[i].domain);
			}
		} else {
			System.out.println("Can't get list of domains.");
		}
		System.exit(0);
	}
}
