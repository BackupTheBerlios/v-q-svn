//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "vobj.h"

#pragma package(smart_init)

ostream & VObj::operator >> (ostream & o)
{
        if( ! def.empty() ) o<< "DEF " << def << " ";
        o << realname << " {" << endl;
        unsigned i , s;
        for( i=0, s=subs.size(); i < s; i++ )
                (*subs[i]) >> o;

        for( i=0, s=props.size(); i < s; i++ ) {
                o << '\t';
                (*props[i]) >> o;
        }
        o << "}" << endl;
        return o;
}
//--------------------------------------------------------------------------
void VObj::del()
{
        unsigned i = props.size();
        if( i ) {
                i--;
                do {
                        delete props[i];
                } while(i--);
        }
        i = subs.size();
        if( i ) {
                i--;
                do {
                        delete subs[i];
                } while(i--);
        }
}

/**************************************************************************
 *
 **************************************************************************/
void VMaterial::init()
{
        props.push_back( new PropFloat( "ambientIntensity", "ambientIntensity", "0.2") );
        props.push_back( new PropCol( "diffuseColor", "diffuseColor", "0.8 0.8 0.8") );
        props.push_back( new PropCol( "emissiveColor", "emissiveColor", "0 0 0") );
        props.push_back( new PropFloat( "shininess", "shininess", "0.2") );
        props.push_back( new PropCol( "specularColor", "specularColor", "0 0 0") );
        props.push_back( new PropFloat( "transparency", "przezroczystoœæ", "0") );
}

VMaterial::VMaterial() : VObj( NULL, "material Material", "Materia³" )
{
        init();
}

//--------------------------------------------------------------------------
VMaterial::VMaterial( VObj * p ) : VObj( p, "material Material", "Materia³" )
{
        init();
}

//---------------------------------------------------------------------------
VObj *VMaterial::newinst()
{
        return new VMaterial();
}
//---------------------------------------------------------------------------
VObj *VMaterial::newinst( VObj * p )
{
        return new VMaterial( p );
}

/**************************************************************************
 *
 **************************************************************************/
void VTeTrans::init()
{
        props.push_back( new PropVect2( "center", "œrodek", "0 0") );
        props.push_back( new PropFloat( "rotation", "obrót", "0") );
        props.push_back( new PropVect2( "scale", "skala", "1 1") );
        props.push_back( new PropVect2( "translation", "przesuniêcie", "0 0") );
}

VTeTrans::VTeTrans() : VObj( NULL, "textureTransform textureTransform", "Przekszta³cenia tekstury" )
{
        init();
}

//--------------------------------------------------------------------------
VTeTrans::VTeTrans( VObj * p ) : VObj( p, "textureTransform textureTransform", "Przekszta³cenia tekstury" )
{
        init();
}

//---------------------------------------------------------------------------
VObj *VTeTrans::newinst()
{
        return new VTeTrans();
}
//---------------------------------------------------------------------------
VObj *VTeTrans::newinst( VObj * p )
{
        return new VTeTrans( p );
}

/**************************************************************************
 *
 **************************************************************************/
VSphere::VSphere() : VObj( NULL, "geometry Sphere", "Kula" )
{
        props.push_back( new PropFloat( "radius", "promieñ", "1") );
}

//--------------------------------------------------------------------------
VSphere::VSphere( VObj * p ) : VObj( p, "geometry Sphere", "Kula" )
{
        props.push_back( new PropFloat( "radius", "promieñ", "1") );
}

//---------------------------------------------------------------------------
VObj *VSphere::newinst()
{
        return new VSphere();
}
//---------------------------------------------------------------------------
VObj *VSphere::newinst( VObj * p )
{
        return new VSphere( p );
}
//--------------------------------------------------------------------------
VShapeSphere::VShapeSphere() : VObj( NULL, "Shape", "Kula" )
{
        subs.push_back( new VSphere(this) );
        subs.push_back( new VAppear(this) );
}

//--------------------------------------------------------------------------
VShapeSphere::VShapeSphere( VObj * p ) : VObj( p, "Shape", "Kula" )
{
        subs.push_back( new VSphere(this) );
        subs.push_back( new VAppear(this) );
}

//---------------------------------------------------------------------------
VObj *VShapeSphere::newinst()
{
        return new VShapeSphere();
}
//---------------------------------------------------------------------------
VObj *VShapeSphere::newinst( VObj * p )
{
        return new VShapeSphere( p );
}

/***************************************************************************
 *
 ***************************************************************************/
void VBox::init()
{
        props.push_back( new PropFloat( "size", "rozmiar", "2 2 2") );
}

VBox::VBox() : VObj( NULL, "geometry Box", "Prostopad³oœcian" )
{
        init();
}
//--------------------------------------------------------------------------
VBox::VBox( VObj * p ) : VObj( p, "geometry Box", "Prostopad³oœcian" )
{
        init();
}

//---------------------------------------------------------------------------
VObj *VBox::newinst()
{
        return new VBox();
}
//---------------------------------------------------------------------------
VObj *VBox::newinst( VObj * p )
{
        return new VBox( p );
}
//--------------------------------------------------------------------------
VShapeBox::VShapeBox() : VObj( NULL, "Shape", "Prostopad³oœcian" )
{
        subs.push_back( new VBox(this) );
        subs.push_back( new VAppear(this) );
}

//--------------------------------------------------------------------------
VShapeBox::VShapeBox( VObj * p ) : VObj( p, "Shape", "Prostopad³oœcian" )
{
        subs.push_back( new VBox(this) );
        subs.push_back( new VAppear(this) );
}

//---------------------------------------------------------------------------
VObj *VShapeBox::newinst()
{
        return new VShapeBox();
}
//---------------------------------------------------------------------------
VObj *VShapeBox::newinst( VObj * p )
{
        return new VShapeBox( p );
}

/***************************************************************************
 *
 ***************************************************************************/
void VCone::init()
{
        props.push_back( new PropFloat( "bottomRadius", "dolny promieñ", "1") );
        props.push_back( new PropFloat( "height", "wysokoœæ", "2") );
        props.push_back( new PropBool( "side", "side", "TRUE") );
        props.push_back( new PropBool( "bottom", "bottom", "TRUE") );
}

VCone::VCone() : VObj( NULL, "geometry Cone", "Sto¿ek" )
{
        init();
}
//--------------------------------------------------------------------------
VCone::VCone( VObj * p ) : VObj( p, "geometry Cone", "Sto¿ek" )
{
        init();
}

//---------------------------------------------------------------------------
VObj *VCone::newinst()
{
        return new VCone();
}
//---------------------------------------------------------------------------
VObj *VCone::newinst( VObj * p )
{
        return new VCone( p );
}
//--------------------------------------------------------------------------
VShapeCone::VShapeCone() : VObj( NULL, "Shape", "Sto¿ek" )
{
        subs.push_back( new VCone(this) );
        subs.push_back( new VAppear(this) );
}

//--------------------------------------------------------------------------
VShapeCone::VShapeCone( VObj * p ) : VObj( p, "Shape", "Sto¿ek")
{
        subs.push_back( new VCone(this) );
        subs.push_back( new VAppear(this) );
}

//---------------------------------------------------------------------------
VObj *VShapeCone::newinst()
{
        return new VShapeCone();
}
//---------------------------------------------------------------------------
VObj *VShapeCone::newinst( VObj * p )
{
        return new VShapeCone( p );
}
/***************************************************************************
 *
 ***************************************************************************/
void VCyl::init()
{
        props.push_back( new PropBool( "bottom", "bottom", "TRUE") );
        props.push_back( new PropFloat( "height", "wysokoœæ", "2") );
        props.push_back( new PropFloat( "radius", "promieñ", "1") );
        props.push_back( new PropBool( "side", "side", "TRUE") );
        props.push_back( new PropBool( "top", "top", "TRUE") );
}

VCyl::VCyl() : VObj( NULL, "geometry Cylinder", "Cylinder" )
{
        init();
}
//--------------------------------------------------------------------------
VCyl::VCyl( VObj * p ) : VObj( p, "geometry Cylinder", "Cylinder" )
{
        init();
}

//---------------------------------------------------------------------------
VObj *VCyl::newinst()
{
        return new VCyl();
}
//---------------------------------------------------------------------------
VObj *VCyl::newinst( VObj * p )
{
        return new VCyl( p );
}
//--------------------------------------------------------------------------
VShapeCyl::VShapeCyl() : VObj( NULL, "Shape", "Cylinder" )
{
        subs.push_back( new VCyl(this) );
        subs.push_back( new VAppear(this) );
}

//--------------------------------------------------------------------------
VShapeCyl::VShapeCyl( VObj * p ) : VObj( p, "Shape", "Cylinder")
{
        subs.push_back( new VCyl(this) );
        subs.push_back( new VAppear(this) );
}

//---------------------------------------------------------------------------
VObj *VShapeCyl::newinst()
{
        return new VShapeCone();
}
//---------------------------------------------------------------------------
VObj *VShapeCyl::newinst( VObj * p )
{
        return new VShapeCyl( p );
}

/***************************************************************************
 *
 ***************************************************************************/
void VAppear::init()
{
        subs.push_back( new VMaterial(this) );
//        subs.push_back( new VTeTrans(this) );        
}

VAppear::VAppear() : VObj( NULL, "appearance Appearance", "Wygl¹d" )
{
        init();
}
//--------------------------------------------------------------------------
VAppear::VAppear( VObj * p ) : VObj( p, "appearance Appearance", "Wygl¹d" )
{
        init();
}

//---------------------------------------------------------------------------
VObj *VAppear::newinst()
{
        return new VAppear();
}
//---------------------------------------------------------------------------
VObj *VAppear::newinst( VObj * p )
{
        return new VAppear( p );
}
//---------------------------------------------------------------------------
void VTrans::init()
{
        props.push_back( new PropVect3( "translation", "przesuniêcie", "0 0 0") );
        props.push_back( new PropVect3( "center", "œrodek", "0 0 0") );
        props.push_back( new PropRot( "rotation", "obrót", "0 0 1 0") );
        props.push_back( new PropVect3( "scale", "skalowanie", "1 1 1") );
        props.push_back( new PropRot( "scaleOrientation", "kierunek skalowania", "0 0 1 0") );
        props.push_back( new PropVect3( "bboxCenter", "bboxCenter", "0 0 0") );
        props.push_back( new PropVect3( "bboxSize", "bboxSize", "-1 -1 -1") );
}

//---------------------------------------------------------------------------
VTrans::VTrans( VObj * p, string n )
        : VObj( p, "Transform", n )
{
        init();
}
//---------------------------------------------------------------------------
VTrans::VTrans( VObj * p, string r, string n )
        : VObj( p, r, n )
{
        init();
}
//---------------------------------------------------------------------------
VObj *VTrans::newinst()
{
        VTrans *tmp = new VTrans( NULL, realname, name );
        for( unsigned i=0, s=subs.size(); i < s; i ++ )
                tmp->addchd( subs[i]->newinst(tmp) );
        return tmp;
}
//---------------------------------------------------------------------------
VObj *VTrans::newinst( VObj * p )
{
        VTrans *tmp = new VTrans( NULL, realname, name );
        for( unsigned i=0, s=subs.size(); i < s; i ++ )
                tmp->addchd( subs[i]->newinst(tmp) );
        return tmp;
}
//---------------------------------------------------------------------------
void VTrans::addchd( VObj * v )
{
        subs.push_back(v);
}
//---------------------------------------------------------------------------
ostream & VTrans::operator >> (ostream & o)
{
        if( ! def.empty() ) o<< "DEF " << def << " ";
        o << realname << " {" << endl;
        unsigned i , s;
        o << "children [" << endl;
        for( i=0, s=subs.size(); i < s; i++ )
                (*subs[i]) >> o;
        o << "]" << endl;
        for( i=0, s=props.size(); i < s; i++ ) {
                o << '\t';
                (*props[i]) >> o;
        }
        o << "}" << endl;
        return o;
}
//---------------------------------------------------------------------------
