package com.foo_baz.v_q.ivqPackage;

/**
* com/foo_baz/v_q/ivqPackage/user_conf_infoHolder.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from /home/new/svn/berlios.de/v-q/trunk/base/logger.idl
* poniedziałek, 6 marzec 2006 22:05:08 CET
*/

public final class user_conf_infoHolder implements org.omg.CORBA.portable.Streamable
{
  public com.foo_baz.v_q.ivqPackage.user_conf_info value = null;

  public user_conf_infoHolder ()
  {
  }

  public user_conf_infoHolder (com.foo_baz.v_q.ivqPackage.user_conf_info initialValue)
  {
    value = initialValue;
  }

  public void _read (org.omg.CORBA.portable.InputStream i)
  {
    value = com.foo_baz.v_q.ivqPackage.user_conf_infoHelper.read (i);
  }

  public void _write (org.omg.CORBA.portable.OutputStream o)
  {
    com.foo_baz.v_q.ivqPackage.user_conf_infoHelper.write (o, value);
  }

  public org.omg.CORBA.TypeCode _type ()
  {
    return com.foo_baz.v_q.ivqPackage.user_conf_infoHelper.type ();
  }

}
