//---------------------------------------------------------------------------
#ifndef propertyH
#define propertyH

#include <string>
#include <iostream>

#include "dumpable.h"

class Property : public Dumpable
{
        protected:
                string val;
                string realname;
                virtual bool chk( const AnsiString &, string & ) { return true; };
        public:
                string name, hint;
                Property() { realname = name = ""; };
                Property( const string & n, const string & v )
                        : realname(n), name(n), val(v), hint(v) {}
                Property( const string & r, const string & n, const string & v )
                        : realname(r), name(n), val(v), hint(r) {}
                Property( const string & r, const string & n, const string & v, const string & h )
                        : realname(r), name(n), val(v), hint(h) {}

                virtual string get() { return val; };
                virtual void set( bool & );
                virtual ostream & operator >> (ostream & o) { return (o << realname << ' ' << val << endl); };
                virtual ~Property() {};
};

/*
 * to samo co powy¿ej tylko wartoœæ 3-liczbowa
 */
class PropFloat : public Property
{
        protected:
                string decsep;
                bool chk( const AnsiString &, string & );
                string replaceAll(string s, string f, string r);
        public:
                PropFloat( string r, string n, string def )
                        : Property( r, n, def ) { decsep = DecimalSeparator; }
                PropFloat( string n, string def )
                        : Property( n , def ) { decsep = DecimalSeparator; }
                string get();
                void set( bool & );
};

/*
 * to samo co powy¿ej tylko wartoœæ 3-liczbowa
 */
class PropVect3 : public PropFloat
{
        public:
                PropVect3( string r, string n, string def )
                        : PropFloat( r, n, def ) {}
                PropVect3( string n, string def )
                        : PropFloat( n , def ) {}
                void set( bool & );
};

/*
 * to samo co powy¿ej tylko wartoœæ 3-liczbowa
 */
class PropVect2 : public PropFloat
{
        public:
                PropVect2( string r, string n, string def )
                        : PropFloat( r, n, def ) {}
                PropVect2( string n, string def )
                        : PropFloat( n , def ) {}
                void set( bool & );
};

/*
 * wszelkiego rodzaju obroty
 */
class PropRot : public PropFloat
{
        public:
                PropRot( string r, string n, string def )
                        : PropFloat( r, n, def ) {}
                PropRot( string n, string def )
                        : PropFloat( n , def ) {}
                void set( bool & );
};
/*
 * wszelkiego rodzaju wartoœci logiczne
 */
class PropBool : public Property
{
        public:
                PropBool( string r, string n, string def )
                        : Property( r, n, def ) {}
                PropBool( string n, string def )
                        : Property( n , def ) {}
                void set( bool & );
};
/*
 * to samo co powy¿ej tylko wartoœæ 3-liczbowa
 */
class PropCol : public PropFloat
{
        public:
                PropCol( string r, string n, string def )
                        : PropFloat( r, n, def ) {}
                PropCol( string n, string def )
                        : PropFloat( n , def ) {}
                void set( bool & );
};
//---------------------------------------------------------------------------

#endif
