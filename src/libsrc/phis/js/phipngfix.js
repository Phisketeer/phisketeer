function phipngfix() {
    for ( var i=0; i<document.images.length; i++ ) {
        var img=document.images[i];
        var imgName=img.src.toUpperCase();
        if ( imgName.indexOf( 'PNG' )!=-1 && img.style.cssText.indexOf( "opacity" )==-1 ) {
            var imgID=img.id ? 'id="'+img.id+'" ' : '';
            var imgStyle="display:inline-block; "+img.style.cssText;
            var imgClass=img.className ? 'class="'+img.className+'" ' : '';
            var imgTitle=img.title ? 'title="'+img.title+'" ' : '';
            var imgHref=""; var imgHrefEnd="";
            if ( img.parentElement.href ) {
                imgStyle="cursor:hand;"+imgStyle;
                imgHref='<a href="'+img.parent.href+'">';
                imgHrefEnd='</a>';
            }
            if ( img.onclick ) {
                imgStyle="cursor:hand;"+imgStyle;
                var imgurl=String( img.onclick );
                imgurl=imgurl.replace( /function anonymous../, '' );
                imgHref='<a href="javascript:'+imgurl+'">';
                imgHrefEnd='</a>';
            }
            var newPNG=imgHref+'<div '+imgID+imgClass+imgTitle
                +'style="'+'width: '+img.width
                +'px; height:'+img.height+'px;'+imgStyle
                +'; filter:progid:DXImageTransform.Microsoft.AlphaImageLoader'
                +"(src='"+img.src+"');"+'"></div>'+imgHrefEnd;
            img.outerHTML=newPNG;
            i=i-1;
        }
    }
}

window.addEventListener ? window.addEventListener( 'load', phipngfix, false ) : window.attachEvent( 'onload', phipngfix );
