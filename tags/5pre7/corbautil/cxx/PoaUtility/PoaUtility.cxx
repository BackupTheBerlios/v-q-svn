//----------------------------------------------------------------------
// Copyright (c) 2002-2004 IONA Technologies. All rights reserved.
// This software is provided "as is".
//
// File:	PoaUtility.h
//
// Description: Class that provides operations to simplify the building
//		of POA hierarchies
//----------------------------------------------------------------------





//--------
// #include's
//--------
#include "PoaUtility.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "p_iostream.h"
#include "p_strstream.h"





namespace corbautil
{

//----------------------------------------------------------------------
// Function:	stringToDeploymentModel()
//
// Description:	Convert the (case insensitive) stringified version of
//		a deployment model to the corresponding enum value.
//----------------------------------------------------------------------
PoaUtility::DeploymentModel
PoaUtility::stringToDeploymentModel(const char * str)
						throw(PoaUtilityException)
{
	strstream	buf;
	char *		u_str;
	int		i;
	int		len;
	char		ch;

	len = strlen(str);
	u_str = new char[len+1];
	for (i = 0; i < len; i++) {
		ch = str[i];
		if (islower(ch)) { ch = toupper(ch); }
		u_str[i] = ch;
	}
	u_str[len] = '\0';

	if (strcmp(u_str, "RANDOM_PORTS_NO_IMR") == 0) {
		delete [] u_str;
		return RANDOM_PORTS_NO_IMR;
	}
	else if (strcmp(u_str, "RANDOM_PORTS_WITH_IMR") == 0) {
		delete [] u_str;
		return RANDOM_PORTS_WITH_IMR;
	}
	else if (strcmp(u_str, "FIXED_PORTS_NO_IMR") == 0) {
		delete [] u_str;
		return FIXED_PORTS_NO_IMR;
	}
	else if (strcmp(u_str, "FIXED_PORTS_WITH_IMR") == 0) {
		delete [] u_str;
		return FIXED_PORTS_WITH_IMR;
	}
	buf	<< "Invalid DeploymentModel '" << str << "'" << ends;
	throw PoaUtilityException(buf);
}





//----------------------------------------------------------------------
// Function:	Constructor
//
// Description:	
//----------------------------------------------------------------------

PoaUtility::PoaUtility(CORBA::ORB_ptr orb, DeploymentModel deployModel)
		throw (PoaUtilityException)
{
	CORBA::Object_var		tmpObj;

	m_deployModel = deployModel;
	m_firstPoaMgr = 1; // true
	m_orb = CORBA::ORB::_duplicate(orb);
	try {
		tmpObj = m_orb->resolve_initial_references("RootPOA");
		m_root = POA::_narrow(tmpObj);
		assert(!CORBA::is_nil(m_root));
	} catch (const CORBA::Exception & ex) {
		strstream	buf;

		buf	<< "resolve_initial_references(\"RootPOA\") failed: "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}

#if defined(P_USE_ORBIX)
	//--------
	// Get access to the Orbix configuration API
	//--------
	try {
		tmpObj = orb->resolve_initial_references("IT_Configuration");
		m_cfg = IT_Config::Configuration::_narrow(tmpObj);
	} catch (const CORBA::Exception & ex) {
		strstream	buf;

		buf	<< "resolve_initial_references(\"IT_Configuration\") "
			<< "failed: "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}
	assert(!CORBA::is_nil(m_cfg));
#endif

#if defined(P_USE_ORBACUS)
	//--------
	// Get access to the ORBacus POA Manager Factory
	//--------
	try {
		tmpObj = m_orb->resolve_initial_references("POAManagerFactory");
		m_poaMgrFactory = OBPortableServer::POAManagerFactory::
					_narrow(tmpObj);
	} catch (const CORBA::Exception & ex) {
		strstream	buf;

		buf	<< "resolve_initial_references(\"POAManagerFactory\") "
			<< "failed: "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}
	assert(!CORBA::is_nil(m_poaMgrFactory));
#endif
}





//----------------------------------------------------------------------
// Function:	Destructor
//
// Description:	
//----------------------------------------------------------------------

PoaUtility::~PoaUtility()
{
	//--------
	// Nothing to do
	//--------
}





//----------------------------------------------------------------------
// Function:	createPoa()
//
// Description:	
//----------------------------------------------------------------------

POA_ptr
PoaUtility::createPoa(
	const char *		poa_name,
	POA_ptr			parent_poa,
	LabelledPOAManager &	labelled_mgr,
	const char *		policies_str) throw (PoaUtilityException)
{
	CORBA::PolicyList_var	policies;
	POA_var			poa;
	CORBA::ULong		len;
	CORBA::ULong		i;

	try {
		policies = createPolicyList(labelled_mgr.label(),
					      policies_str);
		poa = parent_poa->create_POA(poa_name, labelled_mgr.mgr(),
							policies.in());
		len = policies->length();
		for (i = 0; i < len; i++) {
			policies[i]->destroy();
		}
	} catch (PortableServer::POA::InvalidPolicy ex) {
		strstream		buf;
		CORBA::String_var	full_poa_name;

		full_poa_name = getFullPoaName(poa_name, parent_poa);
		buf	<< "Error occurred when creating POA \""
			<< full_poa_name.in()
			<< "\": InvalidPolicy (index "
			<< ex.index 
			<< " in \""
			<< policies_str
			<< "\")"
			<< ends;
		throw PoaUtilityException(buf);
	} catch(const CORBA::Exception & ex) {
		strstream		buf;
		CORBA::String_var	full_poa_name;

		full_poa_name = getFullPoaName(poa_name, parent_poa);
		buf	<< "Error occurred when creating POA '"
			<< full_poa_name.in()
			<< "': "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	} catch(const PoaUtilityException & ex) {
		strstream		buf;
		CORBA::String_var	full_poa_name;

		full_poa_name = getFullPoaName(poa_name, parent_poa);
		buf	<< "Error occurred when creating POA '"
			<< full_poa_name.in()
			<< "': "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}

	return poa._retn();
}





//----------------------------------------------------------------------
// Function:	getFullPoaName()
//
// Description:	
//----------------------------------------------------------------------

char *
PoaUtility::getFullPoaName(
	const char *			local_name,
	POA_ptr				parent_poa)
{
	strstream			buf;
	char *				buf_str;
	char *				result;
	CORBA::StringSeq		seq(5); // pre-allocate some space
	CORBA::ULong			len;
	int				i;

	//--------
	// Obtain the hierarchical names of all the ancestors and
	// store them in a sequence
	//--------
	len = 0;
	seq.length(len);
	while (!parent_poa->_is_equivalent(m_root)) {
		len++;
		seq.length(len);
		seq[len-1] = parent_poa->the_name();
		parent_poa = parent_poa->the_parent();
	}

	//--------
	// Now iterate over the sequence backwards to form the
	// name in the correct format.
	//--------
	for (i = len-1; i >= 0; i--) {
		buf << seq[(CORBA::ULong)(i)] << "/";
	}
	buf << local_name << ends;
	buf_str = buf.str();
	result = CORBA::string_dup(buf_str);
	buf.rdbuf()->freeze(0);
	return result;
}





//----------------------------------------------------------------------
// Function:	createPolicyList()
//
// Description:	
//----------------------------------------------------------------------

CORBA::PolicyList *
PoaUtility::createPolicyList(
	const char *		poa_mgr_label,
	const char *		policy_list_str) throw(PoaUtilityException)
{
	CORBA::PolicyList_var	seq;
	CORBA::Boolean		has_persistent_policy;
	char *			split_policies_str;
	CORBA::ULong		i;
	int			len;
	char *			p;
	char			ch;
	CORBA::ULong		seq_len;

	//--------
	// Replace all occurances of ' ', '+' and "," with '\0'
	// so we can handle policy as a null terminated string.
	//--------
	split_policies_str = CORBA::string_dup(policy_list_str);
	len = strlen(policy_list_str);
	for (i = 0; i < len; i++) {
		ch = split_policies_str[i];
		if (ch == ' ' || ch == '+' || ch == ',') {
			split_policies_str[i] = '\0';
		}
	} 

	//--------
	// Iterate over "split_policies_str", copying in the relevant
	// policies into "seq".
	//--------
	seq = new CORBA::PolicyList(7); // pre-allocate some space
	seq_len = 0;
	seq->length(seq_len);
	has_persistent_policy = 0; // false
	p = split_policies_str;
	while (p < split_policies_str + len) {
		if (*p == '\0') {
			p++;
		} else {
			seq_len ++;
			seq->length(seq_len);
			seq[(CORBA::ULong)(seq_len-1)]
					= createPolicy(p, policy_list_str);
			if (strcmp(p, "persistent") == 0) {
				has_persistent_policy = 1; // true
			}
			p += strlen(p);
		}
	}
	CORBA::string_free(split_policies_str);

#if defined(P_USE_ORBIX)
	//--------
	// To use fixed port numbers and/or deploy persistent POAs without
	// and IMR in an Orbix application, you have to apply proprietary
	// policies to POAs.
	//--------
	if (m_deployModel   == FIXED_PORTS_NO_IMR
	   || m_deployModel == RANDOM_PORTS_NO_IMR)
	{
		CORBA::Any	tmp_any;

		if (has_persistent_policy) {
			tmp_any <<= IT_PortableServer::DIRECT_PERSISTENCE;
			seq_len++;
			seq->length(seq_len);
			seq[seq_len-1] = m_orb->create_policy(
				IT_PortableServer::PERSISTENCE_MODE_POLICY_ID,
				tmp_any);
		}
	}

	if (m_deployModel    == FIXED_PORTS_NO_IMR
	    || m_deployModel == FIXED_PORTS_WITH_IMR)
	{
		CORBA::Any	tmp_any;
		tmp_any <<= CORBA::Any::from_string(
				CORBA::string_dup(poa_mgr_label), 0, 1);
		seq_len++;
		seq->length(seq_len);
		seq[seq_len-1] = m_orb->create_policy(
			IT_CORBA::WELL_KNOWN_ADDRESSING_POLICY_ID,
			tmp_any);
	}
#endif
	return seq._retn();
}





//----------------------------------------------------------------------
// Function:	createPolicy()
//
// Description:	
//----------------------------------------------------------------------

CORBA::Policy_ptr
PoaUtility::createPolicy(
	const char *			name,
	const char *			policy_list_str)
		throw (PoaUtilityException)
{
	CORBA::Policy_var		result;

	if (strcmp(name, "orb_ctrl_model") == 0) {
		result = m_root->create_thread_policy(ORB_CTRL_MODEL);
	} else if (strcmp(name, "single_thread_model") == 0) {
		result = m_root->create_thread_policy(SINGLE_THREAD_MODEL);
	} else if (strcmp(name, "transient") == 0) {
		result = m_root->create_lifespan_policy(TRANSIENT);
	} else if (strcmp(name, "persistent") == 0) {
		result = m_root->create_lifespan_policy(PERSISTENT);
	} else if (strcmp(name, "unique_id") == 0) {
		result = m_root->create_id_uniqueness_policy(UNIQUE_ID);
	} else if (strcmp(name, "multiple_id") == 0) {
		result = m_root->create_id_uniqueness_policy(MULTIPLE_ID);
	} else if (strcmp(name, "user_id") == 0) {
		result = m_root->create_id_assignment_policy(USER_ID);
	} else if (strcmp(name, "system_id") == 0) {
		result = m_root->create_id_assignment_policy(SYSTEM_ID);
	} else if (strcmp(name, "implicit_activation") == 0) {
		result = m_root->create_implicit_activation_policy(
					IMPLICIT_ACTIVATION);
	} else if (strcmp(name, "no_implicit_activation") == 0) {
		result = m_root->create_implicit_activation_policy(
					NO_IMPLICIT_ACTIVATION);
	} else if (strcmp(name, "retain") == 0) {
		result = m_root->create_servant_retention_policy(RETAIN);
	} else if (strcmp(name, "non_retain") == 0) {
		result = m_root->create_servant_retention_policy(NON_RETAIN);
	} else if (strcmp(name, "use_active_object_map_only") == 0) {
		result = m_root->create_request_processing_policy(
					USE_ACTIVE_OBJECT_MAP_ONLY);
	} else if (strcmp(name, "use_default_servant") == 0) {
		result = m_root->create_request_processing_policy(
					USE_DEFAULT_SERVANT);
	} else if (strcmp(name, "use_servant_manager") == 0) {
		result = m_root->create_request_processing_policy(
					USE_SERVANT_MANAGER);
#if defined(P_USE_ORBIX)
	} else if (strcmp(name, "deliver") == 0) {
		CORBA::Any	a;
		a <<= IT_PortableServer::DELIVER;
		result = m_orb->create_policy(
			IT_PortableServer::OBJECT_DEACTIVATION_POLICY_ID, a);
	} else if (strcmp(name, "discard") == 0) {
		CORBA::Any	a;
		a <<= IT_PortableServer::DISCARD;
		result = m_orb->create_policy(
			IT_PortableServer::OBJECT_DEACTIVATION_POLICY_ID, a);
	} else if (strcmp(name, "hold") == 0) {
		CORBA::Any	a;
		a <<= IT_PortableServer::HOLD;
		result = m_orb->create_policy(
			IT_PortableServer::OBJECT_DEACTIVATION_POLICY_ID, a);
#endif
	} else {
		//--------
		// Illegal policy name. Through back a descriptive exception.
		//--------
		strstream	buf;

		buf << "illegal policy name '" << name << "' in list '"
		    << policy_list_str << "'"
		    << ends;
		throw PoaUtilityException(buf);
	}
	return result._retn();
}





#if defined(P_USE_ORBACUS)
//----------------------------------------------------------------------
// Function:	createPoaManager()
//
// Description:	ORBacus-proprietary implementation of createPoaManager()
//----------------------------------------------------------------------

LabelledPOAManager
PoaUtility::createPoaManager(const char * label) throw (PoaUtilityException)
{
	LabelledPOAManager		result;

	try {
		result.m_label = CORBA::string_dup(label);
		if (m_deployModel    == FIXED_PORTS_NO_IMR
		    || m_deployModel == FIXED_PORTS_WITH_IMR)
		{
			//--------
			// We do NOT use m_root->the_POAManager() as the
			// first POA Manager. This is because once
			// ORB_init() has been called (which it has by now),
			// there is no way for us to configure the port
			// number on which is should listen.
			//
			// Instead, we use the Orbacus-proprietary
			// POAManagerFactory to create a new POA Manager.
			//--------
			result.m_mgr = m_poaMgrFactory->
						create_poa_manager(label);
		} else if (m_firstPoaMgr) {
			//--------
			// We are deploying with random ports, so it is
			// safe to use the root POA Manager.
			//--------
			result.m_mgr = m_root->the_POAManager();
		} else {
			POA_var				tmp_poa;
			CORBA::PolicyList_var		tmp_policies;

			//--------
			// We create a POA Manager by creating a
			// (temporary) POA with a nil reference for its
			// POA Manager. When we have what we want we
			// then destroy() the temporary POA.
			//--------
			tmp_policies = new CORBA::PolicyList(0);
			tmp_poa = m_root->create_POA(
					"_tmp_POA_for_POAManager_creation",
					POAManager::_nil(),
					tmp_policies.in());
			result.m_mgr = tmp_poa->the_POAManager();
			tmp_poa->destroy(0, 1);
		}
	} catch(CORBA::Exception & ex) {
		strstream	buf;

		buf	<< "Error occurred when creating POA Manager '"
			<< label
			<< "': "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}

	m_firstPoaMgr = 0; // false
	return result;
}




#else
//----------------------------------------------------------------------
// Function:	createPoaManager()
//
// Description:	Portable implementation of createPoaManager()
//----------------------------------------------------------------------

LabelledPOAManager
PoaUtility::createPoaManager(const char * label) throw (PoaUtilityException)
{
	LabelledPOAManager		result;
	CORBA::ULong			i;
	CORBA::ULong			len;

	try {
		result.m_label = CORBA::string_dup(label);
		if (m_firstPoaMgr) {
			m_firstPoaMgr = 0; // false
			result.m_mgr = m_root->the_POAManager();
		} else {
			POA_var				tmp_poa;
			CORBA::PolicyList_var		tmp_policies;

			//--------
			// We create a POA Manager by creating a
			// (temporary) POA with a nil reference for its
			// POA Manager. When we have what we want we
			// then destroy() the temporary POA.
			//--------
			tmp_policies = createPolicyList(result.label(), "");
			tmp_poa = m_root->create_POA(
					"_tmp_POA_for_POAManager_creation",
					POAManager::_nil(),
					tmp_policies.in());
			result.m_mgr = tmp_poa->the_POAManager();
			tmp_poa->destroy(0, 1);
			len = tmp_policies->length();
			for (i = 0; i < len; i++) {
				tmp_policies[i]->destroy();
			}
		}
	} catch(CORBA::Exception & ex) {
		strstream	buf;

		buf	<< "Error occurred when creating POA Manager '"
			<< label
			<< "': "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}

	return result;
}
#endif /* if/else defined(P_USE_ORBACUS) */





#if defined(P_USE_ORBIX)
//----------------------------------------------------------------------
// Function:	createAutoWorkQueue()
//
// Description:	
//----------------------------------------------------------------------

LabelledOrbixWorkQueue
PoaUtility::createAutoWorkQueue(
	const char *				label,
	CORBA::Long				max_size,
	CORBA::ULong				initial_thread_count,
	CORBA::Long				high_water_mark,
	CORBA::Long				low_water_mark,
	CORBA::Long				thread_stack_size_kb)
					throw(PoaUtilityException)
{
	CORBA::Object_var			tmpObj;
	AutomaticWorkQueueFactory_var		factoryObj;
	AutomaticWorkQueue_var			wqObj;
	LabelledOrbixWorkQueue			result;

	//--------
	// Let the specified parameters be overridden by configuration
	// values, if available
	//--------
	max_size = getConfigLong(label, "max_size", max_size);
	initial_thread_count = getConfigLong(label, "initial_thread_count",
						initial_thread_count);
	high_water_mark = getConfigLong(label, "high_water_mark",
						high_water_mark);
	low_water_mark = getConfigLong(label, "low_water_mark", low_water_mark);
	thread_stack_size_kb = getConfigLong(label, "thread_stack_size_kb",
						thread_stack_size_kb);

	try {
		//--------
		// Find the factory
		//--------
		tmpObj = m_orb->resolve_initial_references(
			"IT_AutomaticWorkQueueFactory");
		factoryObj = AutomaticWorkQueueFactory::_narrow(tmpObj);
		assert(!CORBA::is_nil(factoryObj));

		//--------
		// Create the work queue
		//--------
		wqObj = factoryObj->create_work_queue_with_thread_stack_size(
						max_size,
						initial_thread_count,
						high_water_mark,
						low_water_mark,
						thread_stack_size_kb * 1024);
		assert(!CORBA::is_nil(wqObj));

	} catch (const CORBA::Exception &) {
		abort(); // Bug!
	}

	result.m_label   = CORBA::string_dup(label);
	result.m_wq      = wqObj._retn();
	return result;
}





//----------------------------------------------------------------------
// Function:	createManualWorkQueue()
//
// Description:	
//----------------------------------------------------------------------

LabelledOrbixWorkQueue
PoaUtility::createManualWorkQueue(
	const char *				label,
	CORBA::Long				max_size)
				throw(PoaUtilityException)
{
	CORBA::Object_var			tmpObj;
	ManualWorkQueueFactory_var		factoryObj;
	ManualWorkQueue_var			wqObj;
	LabelledOrbixWorkQueue			result;

	//--------
	// Let the specified parameters be overridden by configuration
	// values, if available
	//--------
	max_size = getConfigLong(label, "max_size", max_size);

	try {
		//--------
		// Find the factory
		//--------
		tmpObj = m_orb->resolve_initial_references(
			"IT_ManualWorkQueueFactory");
		factoryObj = ManualWorkQueueFactory::_narrow(tmpObj);
		assert(!CORBA::is_nil(factoryObj));

		//--------
		// Create the work queue
		//--------
		wqObj = factoryObj->create_work_queue(max_size);
		assert(!CORBA::is_nil(wqObj));

	} catch (const CORBA::Exception &) {
		abort(); // Bug!
	}

	result.m_label   = CORBA::string_dup(label);
	result.m_wq      = wqObj._retn();
	return result;
}





//----------------------------------------------------------------------
// Function:	getConfigLong()
//
// Description:	
//----------------------------------------------------------------------

CORBA::Long
PoaUtility::getConfigLong(
		const char *	the_namespace,
		const char *	entry_name,
		CORBA::Long	default_value)
{
	CORBA::String_var		name;
	CORBA::Long			result;

	if (strcmp(the_namespace, "") == 0) {
		return default_value;
	}
	name = CORBA::string_alloc(strlen(the_namespace)+strlen(entry_name)+1);
	sprintf(name.inout(), "%s:%s", the_namespace, entry_name);
	try {
		if (!m_cfg->get_long(name.in(), result)) {
			//--------
			// The entry is missing. Use the default value
			//--------
			result = default_value;
		}
	} catch (const CORBA::Exception &) {
		//--------
		// Something went wrong. Use the default value
		//--------
		result = default_value;
	}
	return result;
}





//----------------------------------------------------------------------
// Function:	createPoa()
//
// Description:	
//----------------------------------------------------------------------

POA_ptr
PoaUtility::createPoa(
	const char *			poa_name,
	POA_ptr				parent_poa,
	LabelledPOAManager &		labelled_mgr,
	const char *			policies_str,
	LabelledOrbixWorkQueue &	labelled_wq)
		throw (PoaUtilityException)
{
	CORBA::PolicyList_var	policies;
	POA_var			poa;
	CORBA::ULong		len;
	CORBA::ULong		i;
	CORBA::Any		wq_any;

	try {
		//--------
		// Create the normal policy list
		//--------
		policies = createPolicyList(labelled_mgr.label(),
					      policies_str);

		//--------
		// Now add a work queue policy item to the list
		// Note: the unsigned long constant that we have to
		// pass as a parameter has changed beween ASP 5.1
		// and 6.0.
		//--------
		len = policies->length();
		len ++;
		policies->length(len);
		wq_any <<= labelled_wq.wq();
		policies[len-1] = m_orb->create_policy(
#if P_ORBIX_VERSION < 60
			IT_WorkQueue::WORK_QUEUE_POLICY_ID,
#else
			IT_PortableServer::DISPATCH_WORKQUEUE_POLICY_ID,
#endif
			wq_any);

		//--------
		// Create the POA
		//--------
		poa = parent_poa->create_POA(poa_name, labelled_mgr.mgr(),
							policies.in());
		len = policies->length();
		for (i = 0; i < len; i++) {
			policies[i]->destroy();
		}
	} catch (PortableServer::POA::InvalidPolicy ex) {
		strstream		buf;
		CORBA::String_var	full_poa_name;

		full_poa_name = getFullPoaName(poa_name, parent_poa);
		buf	<< "Error occurred when creating POA \""
			<< full_poa_name.in()
			<< "\": InvalidPolicy (index "
			<< ex.index 
			<< " in \""
			<< policies_str
			<< "\")"
			<< ends;
		throw PoaUtilityException(buf);
	} catch(const CORBA::Exception & ex) {
		strstream		buf;
		CORBA::String_var	full_poa_name;

		full_poa_name = getFullPoaName(poa_name, parent_poa);
		buf	<< "Error occurred when creating POA '"
			<< full_poa_name.in()
			<< "': "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	} catch(const PoaUtilityException & ex) {
		strstream		buf;
		CORBA::String_var	full_poa_name;

		full_poa_name = getFullPoaName(poa_name, parent_poa);
		buf	<< "Error occurred when creating POA '"
			<< full_poa_name.in()
			<< "': "
			<< ex
			<< ends;
		throw PoaUtilityException(buf);
	}

	return poa._retn();
}
#endif /* P_USE_ORBIX */





}; // namespace corbautil
