package com.foo_baz.v_q.iloggerPackage;


/**
* com/foo_baz/v_q/iloggerPackage/err_codeHelper.java .
* Generated by the IDL-to-Java compiler (portable), version "3.1"
* from ../logger.idl
* piątek, 17 marzec 2006 20:09:56 CET
*/

abstract public class err_codeHelper
{
  private static String  _id = "IDL:vq/ilogger/err_code:1.0";

  public static void insert (org.omg.CORBA.Any a, com.foo_baz.v_q.ivqPackage.err_code that)
  {
    org.omg.CORBA.portable.OutputStream out = a.create_output_stream ();
    a.type (type ());
    write (out, that);
    a.read_value (out.create_input_stream (), type ());
  }

  public static com.foo_baz.v_q.ivqPackage.err_code extract (org.omg.CORBA.Any a)
  {
    return read (a.create_input_stream ());
  }

  private static org.omg.CORBA.TypeCode __typeCode = null;
  synchronized public static org.omg.CORBA.TypeCode type ()
  {
    if (__typeCode == null)
    {
      __typeCode = com.foo_baz.v_q.ivqPackage.err_codeHelper.type ();
      __typeCode = org.omg.CORBA.ORB.init ().create_alias_tc (com.foo_baz.v_q.iloggerPackage.err_codeHelper.id (), "err_code", __typeCode);
    }
    return __typeCode;
  }

  public static String id ()
  {
    return _id;
  }

  public static com.foo_baz.v_q.ivqPackage.err_code read (org.omg.CORBA.portable.InputStream istream)
  {
    com.foo_baz.v_q.ivqPackage.err_code value = null;
    value = com.foo_baz.v_q.ivqPackage.err_codeHelper.read (istream);
    return value;
  }

  public static void write (org.omg.CORBA.portable.OutputStream ostream, com.foo_baz.v_q.ivqPackage.err_code value)
  {
    com.foo_baz.v_q.ivqPackage.err_codeHelper.write (ostream, value);
  }

}
