/**
Class implements getopt interface. Placed in public domain.
@author Pawel Niewiadomski &lt;paweln@iidea.pl&gt;
@version 1
*/
package com.foo_baz;

public class Getopt  {
    /**
    if you want to parse another command line, set it to 0
    */
    public int optind = 0;
    /// current option, you can set it to 0 to start processing new set of options
    public String optarg;
    /// if true prints error messages
    public boolean verbose = true;
    /**
    parses command line, acts like UNIX getopt function
    @return -1 on End of options (or --)
    */
    public int parse(String args[], String opts) {
        if( optind < args.length
            && args[optind].charAt(0) == '-' 
            && args[optind].length() >= 2 
            && args[optind].charAt(1) != '-' ) {
            
            for(int i=0; i<opts.length(); i++ ) {
                if(opts.charAt(i) == ':') continue;
                if(args[optind].charAt(1) == opts.charAt(i)) {
                    if( opts.length()>i+1 && opts.charAt(i+1) == ':') {
                        if(args.length<= optind+1) {
                            optarg = "";
                            if(verbose)
                                System.err.println("option: "+args[optind]+" without argument");
                            optind++;
                            return '?';
                        }
                        optarg = args[++optind];
                    } else optarg = "";
                    optind ++;
                    return opts.charAt(i);
                }
            }
            if(verbose) System.err.println("unknown option: "+args[optind]);
            optind++;
            return '?';
        }
        return -1;
    }
/*
    static public void main(String args[]) {
            getopt go = new getopt();
            int opt;
            while((opt=go.parse(args, "hb:"))!= -1) {
                switch(opt) {
                case 'b':
                    System.out.println(go.optarg);
                    break;
                case 'h':
                    System.out.println("h");
                    break;
                case '?':
                    System.out.println("?");
                    break;
                }
            }
    }*/
}
