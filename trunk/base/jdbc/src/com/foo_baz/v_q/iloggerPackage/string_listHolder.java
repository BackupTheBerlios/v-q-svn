package com.foo_baz.v_q.iloggerPackage;


/**
* com/foo_baz/v_q/iloggerPackage/string_listHolder.java .
* Generated by the IDL-to-Java compiler (portable), version "3.1"
* from ../logger.idl
* piątek, 17 marzec 2006 20:09:56 CET
*/

public final class string_listHolder implements org.omg.CORBA.portable.Streamable
{
  public String value[] = null;

  public string_listHolder ()
  {
  }

  public string_listHolder (String[] initialValue)
  {
    value = initialValue;
  }

  public void _read (org.omg.CORBA.portable.InputStream i)
  {
    value = com.foo_baz.v_q.iloggerPackage.string_listHelper.read (i);
  }

  public void _write (org.omg.CORBA.portable.OutputStream o)
  {
    com.foo_baz.v_q.iloggerPackage.string_listHelper.write (o, value);
  }

  public org.omg.CORBA.TypeCode _type ()
  {
    return com.foo_baz.v_q.iloggerPackage.string_listHelper.type ();
  }

}
