package com.foo_baz.v_q;


/**
* com/foo_baz/v_q/ivq.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from C:/Documents and Settings/pniewiad/Desktop/personal/projects/v-q/trunk/base/logger.idl
* Wednesday, December 14, 2005 9:01:53 PM CET
*/


/*@}*/
public interface ivq extends ivqOperations, org.omg.CORBA.Object, org.omg.CORBA.portable.IDLEntity 
{

  /// blocked pop3 access
  public static final short uif_pop3_blk = (short)(1);

  /// blocked smtp access
  public static final short uif_smtp_blk = (short)(2);

  /// admin (it was first added user)
  public static final short uif_admin = (short)(4);

  /// ftp access blocked
  public static final short uif_ftp_blk = (short)(8);

  /// ignore me (entry MUST be ignored)
  public static final short uif_ignore = (short)(16);

  /// super (server) admin
  public static final short uif_supadm = (short)(32);

  /// Mailbox options
  public static final short uc_redir = (short)(1);

  //!< redirection
  public static final short uc_maildir = (short)(2);

  //!< maildir
  public static final short uc_autoresp = (short)(3);

  //!< autorespond
  public static final short uc_external = (short)(127);
} // interface ivq
