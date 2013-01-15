#include "phishtmlgecko100.h"

PHISHtmlGecko100::PHISHtmlGecko100( const PHISRequest *req, const PHIBasePage *page )
    : PHISHtml5Base( req, page ), _isMacOSX( req->osType()==PHISRequest::MacOS )
{
}

PHISHtmlGecko100::~PHISHtmlGecko100()
{
}

QByteArray PHISHtmlGecko100::phisEngine() const
{
    static QByteArray arr="Phis "+PHIS::libVersion().toLatin1()+"/Gecko 100";
    return arr;
}

QRect PHISHtmlGecko100::adjustMultiLineEditSize() const
{
    if ( _isMacOSX ) return QRect( 0, 0, -2, -2 );
    return QRect( 0, 0, -6, -6 );
}

QByteArray PHISHtmlGecko100::effectStyle() const
{
    QByteArray arr;
    arr.reserve( 400 );
    if ( _it->hasGraphicEffect() ) {
        if ( _it->effect()->graphicsType()==PHIEffect::GTShadow ) {
            arr+=textShadowStyle();
            if ( arr.isEmpty() ) {
                QColor c;
                qreal xOff, yOff, radius;
                _it->effect()->shadow( c, xOff, yOff, radius );
                arr+="box-shadow:"+QByteArray::number( static_cast<int>(xOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(yOff) )+"px ";
                arr+=QByteArray::number( static_cast<int>(radius) )+"px ";
                arr+=c.name().toLatin1()+';';
            }
        }
    }

    QTransform t=transformation();
    if ( t.isIdentity() ) return arr;
    arr+="-moz-transform-origin:0 0;-moz-transform:matrix3d("
        +QByteArray::number( t.m11(), 'f' )+','
        +QByteArray::number( t.m12(), 'f' )+",0,"
        +QByteArray::number( t.m13(), 'f' )+','
        +QByteArray::number( t.m21(), 'f' )+','
        +QByteArray::number( t.m22(), 'f' )+",0,"
        +QByteArray::number( t.m23(), 'f' )+",0,0,1,0,"
        +QByteArray::number( t.m31(), 'f' )+','
        +QByteArray::number( t.m32(), 'f' )+",0,"
        +QByteArray::number( t.m33(), 'f' )+");";
    return arr;
}
