//---------------------------------------------------------------------------
#ifndef vobjH
#define vobjH

#include <vector>
#include <iostream>

#include "dumpable.h"
#include "property.h"

/*******************************************************************
 * bazowa klasa dla wszystkich obiektow VRML
 *******************************************************************/
class VObj : public Dumpable
{
        protected:
                string realname;
                void del();
                VObj *_parent;
        public:
                string name, def;
                VObj( VObj *p, const string & r, const string & n )
                        : _parent(p), realname(r), name(n) {}
                VObj( VObj *p, const string & n )
                        : _parent(p), realname(n), name(n) {}
                VObj( VObj *p )
                        : _parent(p) {}

                vector < Property * > props;
                vector < VObj * > subs;

                virtual ostream & operator >> (ostream & o);
                virtual ~VObj() { del(); }
                virtual VObj * newinst()=0;
                virtual VObj * newinst( VObj * )=0;
                const VObj * parent() const { return _parent; }
};
/*********************************************************************
 * obiekt TextureTransform
 *********************************************************************/
class VTeTrans : public VObj
{
        protected:
                void init();
        public:
                VTeTrans();
                VTeTrans( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};

/*********************************************************************
 * ogolnie znany obiekt Material
 *********************************************************************/
class VMaterial : public VObj
{
        protected:
                void init();
        public:
                VMaterial();
                VMaterial( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};

/*********************************************************************
 * ogolnie znany obiekt Appearance
 *********************************************************************/
class VAppear : public VObj
{
        protected:
                void init();
        public:
                VAppear();
                VAppear( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};

/*********************************************************************
 * ogolnie znany obiekt Sphere
 *********************************************************************/
class VSphere : public VObj
{
        public:
                VSphere();
                VSphere( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};
class VShapeSphere : public VObj
{
        public:
                VShapeSphere();
                VShapeSphere( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};

/*********************************************************************
 * ogolnie znany obiekt Box
 *********************************************************************/
class VBox : public VObj
{
        protected:
                void init();
        public:
                VBox();
                VBox( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};
class VShapeBox : public VObj
{
        public:
                VShapeBox();
                VShapeBox( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};
/*********************************************************************
 * obiekt Cone
 *********************************************************************/
class VCone : public VObj
{
        protected:
                void init();
        public:
                VCone();
                VCone( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};
class VShapeCone : public VObj
{
        public:
                VShapeCone();
                VShapeCone( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};
/*********************************************************************
 * obiekt Cylinder
 *********************************************************************/
class VCyl : public VObj
{
        protected:
                void init();
        public:
                VCyl();
                VCyl( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};
class VShapeCyl : public VObj
{
        protected:
                void init();
        public:
                VShapeCyl();
                VShapeCyl( VObj * );
                VObj * newinst();
                VObj * newinst( VObj * );
};

/**********************************************************************
 * Transformacja pojedyñczego obiektu
 **********************************************************************/
class VTrans : public VObj
{
        protected:
                void init();
        public:
                VTrans( VObj * p, string n );
                VTrans( VObj * p, string r, string n );
                VObj * newinst();
                VObj * newinst( VObj * );
                void addchd( VObj * );
                ostream & operator >> (ostream & o);
};
//---------------------------------------------------------------------------

#endif
