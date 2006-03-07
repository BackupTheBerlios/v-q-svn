package com.foo_baz.v_q;


/**
* com/foo_baz/v_q/ivqPOATie.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from /home/new/svn/berlios.de/v-q/trunk/base/logger.idl
* poniedziałek, 6 marzec 2006 22:05:07 CET
*/


/*@}*/
public class ivqPOATie extends ivqPOA
{

  // Constructors

  public ivqPOATie ( com.foo_baz.v_q.ivqOperations delegate ) {
      this._impl = delegate;
  }
  public ivqPOATie ( com.foo_baz.v_q.ivqOperations delegate , org.omg.PortableServer.POA poa ) {
      this._impl = delegate;
      this._poa      = poa;
  }
  public com.foo_baz.v_q.ivqOperations _delegate() {
      return this._impl;
  }
  public void _delegate (com.foo_baz.v_q.ivqOperations delegate ) {
      this._impl = delegate;
  }
  public org.omg.PortableServer.POA _default_POA() {
      if(_poa != null) {
          return _poa;
      }
      else {
          return super._default_POA();
      }
  }

  /// Add domain
  public com.foo_baz.v_q.ivqPackage.error dom_add (String dom, org.omg.CORBA.IntHolder dom_id) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dom_add(dom, dom_id);
  } // dom_add


  /// Removes domain
  public com.foo_baz.v_q.ivqPackage.error dom_rm (int dom_id) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dom_rm(dom_id);
  } // dom_rm


  /// Validates domain name
  public com.foo_baz.v_q.ivqPackage.error dom_val (String dom) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dom_val(dom);
  } // dom_val


  /// Gets ID of domain
  public com.foo_baz.v_q.ivqPackage.error dom_id (String dom, org.omg.CORBA.IntHolder dom_id) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dom_id(dom, dom_id);
  } // dom_id


  /// Gets name of a domain
  public com.foo_baz.v_q.ivqPackage.error dom_name (int dom_id, org.omg.CORBA.StringHolder dom_name) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dom_name(dom_id, dom_name);
  } // dom_name


  /// List all domains
  public com.foo_baz.v_q.ivqPackage.error dom_ls (com.foo_baz.v_q.ivqPackage.domain_info_listHolder dis) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dom_ls(dis);
  } // dom_ls


  /// add alias for a domain
  public com.foo_baz.v_q.ivqPackage.error da_add (int dom_id, String ali) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.da_add(dom_id, ali);
  } // da_add


  /// Removes alias
  public com.foo_baz.v_q.ivqPackage.error da_rm (String ali) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.da_rm(ali);
  } // da_rm


  /// Lists all aliases for a domain
  public com.foo_baz.v_q.ivqPackage.error da_ls_by_dom (int dom_id, com.foo_baz.v_q.ivqPackage.string_listHolder alis) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.da_ls_by_dom(dom_id, alis);
  } // da_ls_by_dom


  /// add IP address for a domain
  public com.foo_baz.v_q.ivqPackage.error dip_add (int dom_id, String ip) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dip_add(dom_id, ip);
  } // dip_add


  /// Removes IP address
  public com.foo_baz.v_q.ivqPackage.error dip_rm (String ip) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dip_rm(ip);
  } // dip_rm


  /// Lists all IP addresses for a domain
  public com.foo_baz.v_q.ivqPackage.error dip_ls_by_dom (int dom_id, com.foo_baz.v_q.ivqPackage.string_listHolder ips) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.dip_ls_by_dom(dom_id, ips);
  } // dip_ls_by_dom


  /// Adds user
  public com.foo_baz.v_q.ivqPackage.error user_add (com.foo_baz.v_q.ivqPackage.user_info ai, boolean is_banned) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_add(ai, is_banned);
  } // user_add


  /// Modify user
  public com.foo_baz.v_q.ivqPackage.error user_rep (com.foo_baz.v_q.ivqPackage.user_info ui, boolean password, boolean dir) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_rep(ui, password, dir);
  } // user_rep


  /// Removes user
  public com.foo_baz.v_q.ivqPackage.error user_rm (int dom_id, String login) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_rm(dom_id, login);
  } // user_rm


  /// Changes password
  public com.foo_baz.v_q.ivqPackage.error user_pass (int dom_id, String login, String pass) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_pass(dom_id, login, pass);
  } // user_pass


  /// Checks if user name is valid (not if user exists)
  public com.foo_baz.v_q.ivqPackage.error user_val (String user) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_val(user);
  } // user_val


  /// Gets informations about user
  public com.foo_baz.v_q.ivqPackage.error user_get (com.foo_baz.v_q.ivqPackage.user_infoHolder ai) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_get(ai);
  } // user_get


  /// Checks whether user exists
  public com.foo_baz.v_q.ivqPackage.error user_ex (int dom_id, String login) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.except, com.foo_baz.v_q.db_error
  {
    return _impl.user_ex(dom_id, login);
  } // user_ex


  /// get all users in specified domain
  public com.foo_baz.v_q.ivqPackage.error user_ls_by_dom (int dom_id, int start, int cnt, com.foo_baz.v_q.ivqPackage.user_info_listHolder uis) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_ls_by_dom(dom_id, start, cnt, uis);
  } // user_ls_by_dom


  /// get number of users in specified domain
  public com.foo_baz.v_q.ivqPackage.error user_cnt_by_dom (int dom_id, org.omg.CORBA.IntHolder cnt) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_cnt_by_dom(dom_id, cnt);
  } // user_cnt_by_dom


  //@{
  public com.foo_baz.v_q.ivqPackage.error eb_add (String re_domain, String re_login) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.eb_add(re_domain, re_login);
  } // eb_add

  public com.foo_baz.v_q.ivqPackage.error eb_rm (String re_domain, String re_login) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.eb_rm(re_domain, re_login);
  } // eb_rm

  public com.foo_baz.v_q.ivqPackage.error eb_ls (com.foo_baz.v_q.ivqPackage.email_banned_listHolder ebs) throws com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.eb_ls(ebs);
  } // eb_ls


  /// Adds mailbox configuration
  public com.foo_baz.v_q.ivqPackage.error user_conf_add (int dom_id, String user, String pfix, com.foo_baz.v_q.ivqPackage.user_conf_infoHolder ui) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_add(dom_id, user, pfix, ui);
  } // user_conf_add


  /// Removes all mailbox configuration entries
  public com.foo_baz.v_q.ivqPackage.error user_conf_rm_by_type (int dom_id, String user, String pfix, short ut) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_rm_by_type(dom_id, user, pfix, ut);
  } // user_conf_rm_by_type


  /// Removes mailbox configuration entry
  public com.foo_baz.v_q.ivqPackage.error user_conf_rm (int dom_id, String user, String pfix, int id) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_rm(dom_id, user, pfix, id);
  } // user_conf_rm


  /// Lists mailbox configuration
  public com.foo_baz.v_q.ivqPackage.error user_conf_ls (int dom_id, String user, String pfix, com.foo_baz.v_q.ivqPackage.user_conf_info_listHolder uis) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_ls(dom_id, user, pfix, uis);
  } // user_conf_ls


  /// Lists mailbox configuation of specified type
  public com.foo_baz.v_q.ivqPackage.error user_conf_ls_by_type (int dom_id, String user, String pfix, short ut, com.foo_baz.v_q.ivqPackage.user_conf_info_listHolder uis) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_ls_by_type(dom_id, user, pfix, ut, uis);
  } // user_conf_ls_by_type


  /// Changes existing entry
  public com.foo_baz.v_q.ivqPackage.error user_conf_rep (int dom_id, String user, String pfix, com.foo_baz.v_q.ivqPackage.user_conf_info ui) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_rep(dom_id, user, pfix, ui);
  } // user_conf_rep


  /// Gets existing entry
  public com.foo_baz.v_q.ivqPackage.error user_conf_get (com.foo_baz.v_q.ivqPackage.user_conf_infoHolder ui) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_get(ui);
  } // user_conf_get


  /// Checks whether mailbox has entry of specified type
  public com.foo_baz.v_q.ivqPackage.error user_conf_type_has (int dom_id, String user, String pfix, short ut) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_type_has(dom_id, user, pfix, ut);
  } // user_conf_type_has


  /// Counts entries of specified type
  public com.foo_baz.v_q.ivqPackage.error user_conf_type_cnt (int dom_id, String user, String pfix, short ut, org.omg.CORBA.IntHolder cnt) throws com.foo_baz.v_q.null_error, com.foo_baz.v_q.db_error, com.foo_baz.v_q.except
  {
    return _impl.user_conf_type_cnt(dom_id, user, pfix, ut, cnt);
  } // user_conf_type_cnt

  private com.foo_baz.v_q.ivqOperations _impl;
  private org.omg.PortableServer.POA _poa;

} // class ivqPOATie
