package com.foo_baz.v_q.ivqPackage;


/**
* com/foo_baz/v_q/ivqPackage/user_conf_info.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from C:/Documents and Settings/pniewiad/Desktop/personal/projects/v-q/trunk/base/logger.idl
* Wednesday, December 14, 2005 9:01:53 PM CET
*/

public final class user_conf_info implements org.omg.CORBA.portable.IDLEntity
{
  public int id_conf = (int)0;

  //!< identificator
  public String val = null;

  //!< value
  public short type = (short)0;

  public user_conf_info ()
  {
  } // ctor

  public user_conf_info (int _id_conf, String _val, short _type)
  {
    id_conf = _id_conf;
    val = _val;
    type = _type;
  } // ctor

} // class user_conf_info
