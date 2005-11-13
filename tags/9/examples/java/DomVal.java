/**
 * Simple example of using v-q from Java
 * @author Pawe³ Niewiadomski
 */
import com.foo_baz.v_q.*;
import com.foo_baz.v_q.ivqPackage.*;
import org.omg.CORBA.*;
import org.omg.CosNaming.*;

public class DomVal {

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
		
		String dom = "123123.asd.com.pl";
		error err = vqo.dom_val(dom);
		if( err.ec == err_code.err_no ) {
			System.out.println("It's a valid domain name");
		} else {
			System.out.println("It's an invalid domain name");
		}
		System.exit(0);
	}
}
