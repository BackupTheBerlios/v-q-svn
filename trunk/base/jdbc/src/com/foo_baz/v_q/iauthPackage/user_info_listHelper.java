package com.foo_baz.v_q.iauthPackage;


/**
* com/foo_baz/v_q/iauthPackage/user_info_listHelper.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from /home/new/svn/berlios.de/v-q/trunk/base/auth.idl
* poniedziałek, 6 marzec 2006 22:05:03 CET
*/

abstract public class user_info_listHelper
{
  private static String  _id = "IDL:vq/iauth/user_info_list:1.0";

  public static void insert (org.omg.CORBA.Any a, com.foo_baz.v_q.ivqPackage.user_info[] that)
  {
    org.omg.CORBA.portable.OutputStream out = a.create_output_stream ();
    a.type (type ());
    write (out, that);
    a.read_value (out.create_input_stream (), type ());
  }

  public static com.foo_baz.v_q.ivqPackage.user_info[] extract (org.omg.CORBA.Any a)
  {
    return read (a.create_input_stream ());
  }

  private static org.omg.CORBA.TypeCode __typeCode = null;
  synchronized public static org.omg.CORBA.TypeCode type ()
  {
    if (__typeCode == null)
    {
      __typeCode = com.foo_baz.v_q.ivqPackage.user_infoHelper.type ();
      __typeCode = org.omg.CORBA.ORB.init ().create_sequence_tc (0, __typeCode);
      __typeCode = org.omg.CORBA.ORB.init ().create_alias_tc (com.foo_baz.v_q.ivqPackage.user_info_listHelper.id (), "user_info_list", __typeCode);
      __typeCode = org.omg.CORBA.ORB.init ().create_alias_tc (com.foo_baz.v_q.iauthPackage.user_info_listHelper.id (), "user_info_list", __typeCode);
    }
    return __typeCode;
  }

  public static String id ()
  {
    return _id;
  }

  public static com.foo_baz.v_q.ivqPackage.user_info[] read (org.omg.CORBA.portable.InputStream istream)
  {
    com.foo_baz.v_q.ivqPackage.user_info value[] = null;
    value = com.foo_baz.v_q.ivqPackage.user_info_listHelper.read (istream);
    return value;
  }

  public static void write (org.omg.CORBA.portable.OutputStream ostream, com.foo_baz.v_q.ivqPackage.user_info[] value)
  {
    com.foo_baz.v_q.ivqPackage.user_info_listHelper.write (ostream, value);
  }

}
