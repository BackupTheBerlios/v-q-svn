//----------------------------------------------------------------------
// Copyright (c) 2002-2004 IONA Technologies. All rights reserved.
// This software is provided "as is".
//----------------------------------------------------------------------

package com.iona.corbautil;

import org.omg.CORBA.*;
import org.omg.PortableServer.POAPackage.*;
import org.omg.PortableServer.*;
import com.ooc.OBPortableServer.*;





/**
 * ORBacus-specific class that simplifies the construction of POA hierarchies.
 * An instance of this class is created by calling
 * <code>PoaUtility.init()</code>, subject to system properties having
 * appropriate values.
 */
public class PoaUtilityOrbacusImpl extends PoaUtilityPortableImpl
{
    /**
     * This constructor is not intended to be called by "normal" user code.
     * Instead, it is called by <code>PoaUtility.init()</code>, subject to
     * system properties.
     *
     * @param orb		The ORB.
     */
    public PoaUtilityOrbacusImpl(org.omg.CORBA.ORB orb, int deployModel)
    		throws PoaUtilityException
    {
	super(orb, deployModel);

	//--------
	// Get access to the ORBacus POA Manager Factory
	//--------
	org.omg.CORBA.Object			tmpObj;
	try {
		tmpObj = orb().resolve_initial_references("POAManagerFactory");
		m_poaMgrFactory = POAManagerFactoryHelper.narrow(tmpObj);
	} catch (Exception ex) {
		throw new PoaUtilityException("resolve_initial_references"
			+ "(\"POAManagerFactory\") failed: "
			+ ex);
	}
    }





    /**
     * Create a POA Manager.
     *
     * @param label		A label (name) that is to be associated with
     *				the newly created POA Manager. If one of the
     *				"fixed port" deployment models is used then
     *				ORBacus-specific entries in a runtime
     *				configuration file are used to specify
     *				on which fixed port the POA Manager listens.
     * @return			The newly created POA manager.
     */
    public LabelledPOAManager
    createPoaManager(String label) throws PoaUtilityException
    {
	com.ooc.OBPortableServer.POAManager	poaManager;
	org.omg.PortableServer.POA		tmpPoa;
	Policy[]				tmpPolicies;
	LabelledPOAManager			result;

	result = null;
	if (deployModel() == FIXED_PORTS_NO_IMR
	    || deployModel() == FIXED_PORTS_WITH_IMR)
	{
		firstPoaMgr(false);
		try {
			//--------
			// We do NOT use root().the_POAManager() as the
			// first POA manager. This is because once
			// ORB.init() has been called (which it has by now),
			// there is no way for us to configure the port
			// number on which it should listen.
			//
			// Instead, we always use the Orbacus-proprietary
			// POAManagerFactory to create a POA Manager.
			//--------
			poaManager = m_poaMgrFactory.create_poa_manager(label);
			result = new LabelledPOAManager(label, poaManager);
		} catch(Exception ex) {
			throw new PoaUtilityException("Error occurred when "
					+ "creating POA Manager \""
					+ label + "\": " + ex.toString());
		}
	} else {
		//--------
		// We are deploying with random ports, so it is safe
		// to use the root POA Manager.
		//--------
		result = super.createPoaManager(label);
	}
	return result;
    }


    /*
     * ORBacus-proprietary POA Manager factory
     */
    private POAManagerFactory		m_poaMgrFactory;

}
