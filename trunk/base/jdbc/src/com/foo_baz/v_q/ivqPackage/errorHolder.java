package com.foo_baz.v_q.ivqPackage;

/**
* com/foo_baz/v_q/ivqPackage/errorHolder.java .
* Generated by the IDL-to-Java compiler (portable), version "3.1"
* from ../logger.idl
* piątek, 17 marzec 2006 20:09:54 CET
*/


/// errors
public final class errorHolder implements org.omg.CORBA.portable.Streamable
{
  public com.foo_baz.v_q.ivqPackage.error value = null;

  public errorHolder ()
  {
  }

  public errorHolder (com.foo_baz.v_q.ivqPackage.error initialValue)
  {
    value = initialValue;
  }

  public void _read (org.omg.CORBA.portable.InputStream i)
  {
    value = com.foo_baz.v_q.ivqPackage.errorHelper.read (i);
  }

  public void _write (org.omg.CORBA.portable.OutputStream o)
  {
    com.foo_baz.v_q.ivqPackage.errorHelper.write (o, value);
  }

  public org.omg.CORBA.TypeCode _type ()
  {
    return com.foo_baz.v_q.ivqPackage.errorHelper.type ();
  }

}
