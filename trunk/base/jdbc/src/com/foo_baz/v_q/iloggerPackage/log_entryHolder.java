package com.foo_baz.v_q.iloggerPackage;

/**
* com/foo_baz/v_q/iloggerPackage/log_entryHolder.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from /home/new/svn/berlios.de/v-q/trunk/base/logger.idl
* poniedziałek, 6 marzec 2006 22:05:09 CET
*/

public final class log_entryHolder implements org.omg.CORBA.portable.Streamable
{
  public com.foo_baz.v_q.iloggerPackage.log_entry value = null;

  public log_entryHolder ()
  {
  }

  public log_entryHolder (com.foo_baz.v_q.iloggerPackage.log_entry initialValue)
  {
    value = initialValue;
  }

  public void _read (org.omg.CORBA.portable.InputStream i)
  {
    value = com.foo_baz.v_q.iloggerPackage.log_entryHelper.read (i);
  }

  public void _write (org.omg.CORBA.portable.OutputStream o)
  {
    com.foo_baz.v_q.iloggerPackage.log_entryHelper.write (o, value);
  }

  public org.omg.CORBA.TypeCode _type ()
  {
    return com.foo_baz.v_q.iloggerPackage.log_entryHelper.type ();
  }

}
