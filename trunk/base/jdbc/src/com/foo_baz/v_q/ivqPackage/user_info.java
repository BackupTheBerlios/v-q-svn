package com.foo_baz.v_q.ivqPackage;


/**
* com/foo_baz/v_q/ivqPackage/user_info.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from C:/Documents and Settings/pniewiad/Desktop/personal/projects/v-q/trunk/base/logger.idl
* Wednesday, December 14, 2005 9:01:53 PM CET
*/

public final class user_info implements org.omg.CORBA.portable.IDLEntity
{
  public int id_domain = (int)0;

  //!< domain
  public String login = null;
  public String pass = null;

  //!< password
  public String dir = null;

  //!< mailbox directory
  public short flags = (short)0;

  //!< additional flags
  public short gid = (short)0;
  public short uid = (short)0;

  public user_info ()
  {
  } // ctor

  public user_info (int _id_domain, String _login, String _pass, String _dir, short _flags, short _gid, short _uid)
  {
    id_domain = _id_domain;
    login = _login;
    pass = _pass;
    dir = _dir;
    flags = _flags;
    gid = _gid;
    uid = _uid;
  } // ctor

} // class user_info
