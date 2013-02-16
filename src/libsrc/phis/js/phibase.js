/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This JS library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
function PhiItem( obj ) {
  var j=jQuery;
  var id='#'+obj.id;
  _e=function(e){
    var o=j('#phihtml').offset();
    e.pageX=e.pageX-parseInt(o.left+0.5);
    e.pageY=e.pageY-parseInt(o.top+0.5);
    return e;
  };
  //this.getContext=function(s){return phi.getElementById(obj.id).getContext(s);};
  this.on=function(e,f){j(id).on(e,f);return this;};
  this.off=function(e,f){j(id).off(e,f);return this;};
  this.trigger=function(f,a){if(typeof(f)!=='string')return this;j(id).trigger(f,a);return this;};
  this.toggle=function(){j(id).toggle();return this;};
  this.hide=function(eff){j(id).hide(eff);return this;};
  this.show=function(eff){j(id).css('visibility','visible').show(eff);return this;};
  this.visible=function(b){
    if(b===undefined)return !(j(id).is(':hidden')||j(id).css('visibility')==='hidden');
    b?this.show():this.hide();
    return this;
  };
  this.stop=function(){j(id).stop(true);return this;};
  this.opacity=function(o){if(o===undefined)return j(id).css('opacity');phi.setOpacity(obj.id,o);return this;};
  this.slide=function( t, d, e ) {
    var o={queue:true,duration:(d?d:400),easing:e};
    if ( t==='down' ) j(id).hide().css('visibility','visible').slideDown(o);
    else j(id).slideUp(o);
    return this;
  };
  this.rotateIn=function( axis, s, d, e ) {phi.rotateIn( obj.id, axis, s, d, e );return this;};
  this.rotateOut=function( axis, s, d, e ) {phi.rotateOut( obj.id, axis, s, d, e );return this;};
  this.moveTo=function( l, t, s, d, e ) {
    var o={queue:false,duration:(d?d:1000),easing:e};
    setTimeout( function(){
      if (l!=parseInt(j(id).css('left')))j(id).animate({left:l},o);
      if (t!=parseInt(j(id).css('top')))j(id).animate({top:t},o);
    },s?s:0 );return this;
  };
  this.moveBy=function( dx, dy, dw, dh, s, d, e ) {
    var x,y,w,h,o={queue:false,duration:(d?d:1000),easing:e};
    dx>=0?x='+='+dx:x='-='+Math.abs(dx);
    dy>=0?y='+='+dy:y='-='+Math.abs(dy);
    dw>=0?w='+='+dw:w='-='+Math.abs(dw);
    dh>=0?h='+='+dh:h='-='+Math.abs(dh);
    setTimeout( function(){
      if (dx)j(id).animate({left:x},o);
      if (dy)j(id).animate({top:y},o);
      if ( dw && dh ) j(id).animate({width:w,height:h},o);
      else { /* jQuery bug? */
        if (dw)j(id).animate({width:w,height:'+=0'},o);
        if (dh)j(id).animate({height:h,width:'+=0'},o);
      };
    },s?s:0 );return this;
  };
  this.pos=function(l,t){
    if(l===undefined)return {left:parseInt(j(id).css('left')),top:parseInt(j(id).css('top'))};
    j(id).css({left:l,top:t});return this;
  };
  this.width=function(w){
    if(w===undefined)return parseInt(j(id).css('width'));
    j(id).css('width',w); return this;
  };
  this.height=function(h){
    if(h===undefined)return parseInt(j(id).css('height'));
    j(id).css('height',h); return this;
  };
  this.left=function(l){
    if(l===undefined)return parseInt(j(id).css('left'));
    j(id).css('left',l); return this;
  };
  this.top=function(t){
    if(t===undefined)return parseInt(j(id).css('top'));
    j(id).css('top',t); return this;
  };
  this.zIndex=function(i){
    if(i===undefined)return parseInt(j(id).css('zIndex'));
    j(id).css('zIndex',i); return this;
  };
  this.cursor=function( c ) {
    if(!c)return j(id).css('cursor');j(id).css({cursor:c});return this;
  };
  this.fadeIn=function( s, d, o, e ) { phi.fadeIn( obj.id, s, d, o, e );return this; };
  this.fadeOut=function( s, d, o, e ) { phi.fadeOut( obj.id, s, d, o, e );return this; };
  this.html=function( v ) {
    if(!j(id).hasClass('phi_41')&&!j(id).hasClass('phi_29'))return this;
    if(v===undefined)return j(id).html();
    j(id).html(v);return this;
  };
  this.title=function( t ) {
    if(t===undefined)return j(id).attr('title');
    j(id).attr('title',t);return this;
  };
  this.progress=function( v ) {
    if(!j(id).hasClass('phi_59'))return this;
    if(v===undefined)return j(id).progressbar('option','value');
    j(id).progressbar('option','value',v);return this;
  };
  this.checked=function( v, c ){
    if(v===undefined)return j(id).is(':checked');
    if(typeof(v)==='boolean'){j(id).attr('checked',v);return this};
    if(typeof(c)!=='boolean')return j(id+' [value="'+v+'"]').is(':checked');
    j(id+' [value="'+v+'"]').attr('checked',c);return this;
  };
  this.selected=function( v, s ){
    if(v===undefined){
        var arr=new Array(); var i=0;
        j(id+' option:selected').each( function(){ arr[i++]=j(this).attr('value') });
        return arr;
    }
    if(typeof(s)!=='boolean')return j(id+' option[value="'+v+'"]').is(':selected');
    j(id+' option[value="'+v+'"]').attr('selected',s);return this;
  };
  this.dblclick=function( f ){if(f)j(id).dblclick(function(e){f(_e(e));});else j(id).dbclick();return this;};
  this.click=function( f ){if(f)j(id).click(function(e){f(_e(e));});else j(id).click();return this;};
  this.mouseover=function( f ){if(f)j(id).mouseenter(function(e){f(_e(e));});else j(id).mouseenter();return this;};
  this.mouseout=function( f ){if(f)j(id).mouseleave(function(e){f(_e(e));});else j(id).mouseleave();return this;};
  this.mousemove=function( f ){if(f)j(id).mousemove(function(e){f(_e(e));});else j(id).mousemove();return this;};
  this.mousedown=function( f ){if(f)j(id).mousedown(function(e){f(_e(e));});else j(id).mousedown();return this;};
  this.mouseup=function( f ){if(f)j(id).mouseup(function(e){f(_e(e));});else j(id).mouseup();return this;};
  this.keyup=function( f ){if(f)j(id).keyup(function(e){f(_e(e));});else j(id).keyup();return this;};
  this.keydown=function( f ){if(f)j(id).keydown(function(e){f(_e(e));});else j(id).keydown();return this;};
  this.keypress=function( f ){if(f)j(id).keypress(function(e){f(_e(e));});else j(id).keypress();return this;};
  this.change=function( f ){if(f)j(id).change(function(e){f(_e(e));});else j(id).change();return this;};
  this.focus=function( f ){if(f)j(id).focus(function(e){f(_e(e));});else j(id).focus();return this;};
  this.blur=function( f ){if(f)j(id).blur(function(e){f(_e(e));});else j(id).blur();return this;};
  this.drop=function( f ){
    if(typeof(f)!=='function')return this;
    j(id).on('drop',function(e,ui)
      {o={source:ui.draggable.attr('id'),target:obj.id,offset:ui.offset,position:ui.position};f(_e(e),o);});
    return this;
  };
  this.dragEnabled=function( b ){
    if ( b===undefined) return !j(id).draggable('option','disabled');
    j(id).draggable('option','disabled',!b);
    return this;
  };
  this.dropEnabled=function( b ){
    if ( b===undefined) return !j(id).droppable('option','disabled');
    j(id).droppable('option','disabled',!b);
    return this;
  };
  this.val=function( v ){
    if ( v===undefined ) return j(id).val();
    j(id).val(v);return this;
  };
  this.selectOptions=function(v,d){phi.setData(obj.id,v,d);return this;};
  this.color=function( c ){
    if ( c===undefined ) return j(id).css('color');
    j(id).css('color',c); return this;
  };
  this.bgColor=function( c ){
    if ( c===undefined ) return j(id).css('background-color');
    j(id).css('background-color',c); return this;
  };
};
function PhiBase(){
  var j=jQuery;
  this.session='';
  this.lang='C';
  this.xAxis=1;
  this.yAxis=2;
  this.zAxis=4;
  this.onload=function(){};
  this.onsubmit=function(){return true;};
  this.isXhtml=function(){return true;};
  this.isHtml=function(){return true;};
  this.isNative=function(){return false;};
  this.alert=function( text ){window.alert( text );};
  this.getElementById=function( id ){return window.document.getElementById( id );};
  this.setOpacity=function( id, o ){j('#'+id).css({opacity:o});};
  this.move=function( id, l, t, s, d, e ) {
    var o={queue:false,duration:(d?d:1000),easing:e};
    setTimeout( function(){
      if (l!=parseInt(j('#'+id).css('left')))j('#'+id).animate({left:l},o);
      if (t!=parseInt(j('#'+id).css('top')))j('#'+id).animate({top:t},o);
    },s?s:0 );
  };
  this.setVisible=function( id, vis ) {
    if ( vis ) j('#'+id).show();
    else j('#'+id).hide();
  };
  this.setSession=function( s ){this.session=new String( s );};
  this.setLang=function( l ){this.lang=new String( l );};
  this.href=function( url ) {
    var link=new String( url );
    if ( link.indexOf( 'philang' )===-1 ) {
      if ( link.indexOf( '?' )>-1 ) link=link+'&philang='+this.lang;
      else link=link+'?philang='+this.lang;
    }
    if ( this.session.length>0 ) link=link+'&phisid='+this.session;
    window.location.href=link;
  };
  this.fadeIn=function( id, s, d, o, e ) {
    j('#'+id).css({opacity:0});
    setTimeout( function(){
      j('#'+id).css('visibility','visible').show().animate({opacity:(o?o:1)},
        {queue:false,duration:(d?d:1000),easing:e});
    },s?s:0 );
  };
  this.fadeOut=function( id, s, d, o, e ) {
    j('#'+id).css({opacity:1});
    setTimeout( function(){
      j('#'+id).css('visibility','visible').show().animate({opacity:(o?o:0)},
      {queue:false,duration:(d?d:1000),easing:e,complete:function(){
        if (j('#'+id).css('opacity')<0.1) j('#'+id).hide();
      }});
    },s?s:0 );
  };
  this.createAjax=function(){
    var a=(window.XMLHttpRequest)?new XMLHttpRequest():((window.ActiveXObject)?
      new ActiveXObject('Microsoft.XMLHTTP'):false);
    if ( a ) a.onreadystatechange=function(){};
    return a;
  };
  this.ajax=this.createAjax();
  this.stopAnimations=function( id ) {
    j('#'+id).stop( true );
  };
  this.rotateIn=function( id, axis, s, d, e ) {
    if ( !axis ) axis=0x2;
    var w=j('#'+id).css('width');
    var h=j('#'+id).css('height');
    j('#'+id).hide();
    setTimeout( function(){
      if (axis & 1){j('#'+id).css('width',0).show().animate({width:w},{queue:false,duration:(d?d:1000),easing:e});}
      if (axis & 2){j('#'+id).css('height',0).show().animate({height:h},{queue:false,duration:(d?d:1000),easing:e});}
    },s?s:0 );
  };
  this.rotateOut=function( id, axis, s, d, e ) {
    if ( !axis ) axis=0x2;
    var w=j('#'+id).css('width');
    var h=j('#'+id).css('height');
    setTimeout( function(){
      if (axis & 1){j('#'+id).animate({width:0},{queue:false,duration:(d?d:1000),easing:e,complete:function()
        {j('#'+id).hide().css({width:w});}});}
      if (axis & 2){j('#'+id).animate({height:0},{queue:false,duration:(d?d:1000),easing:e,complete:function()
        {j('#'+id).hide().css({height:h});}});}
    },s?s:0 );
  };
  this.setData=function( id, data, d ) {
    var obj=this.getElementById( id );
    if ( !obj || obj.type.indexOf('select')===-1 ) return;
    if ( !d ) d='\n';
    while ( obj.options.length ) obj.options[0]=null;
    if ( !data ) return;
    var arr=data.split( d );
    while ( arr[0] ) {
      var sel=false; var val=arr[0]; var txt=val;
      var s=txt.indexOf( '[' );
      if ( s>-1 ) {
        txt=txt.substring( 0, s );
        e=val.indexOf( ']' );
        if ( e>-1 ) {
          val=val.substring( s+1, e );
          s=arr[0].indexOf( '[', e );
          if ( s>-1 ) {
            e=arr[0].indexOf( ']', s );
            if ( e>-1 ) {
              var str=arr[0].substring( s+1, e );
              if ( str.toLowerCase()==='true' ) sel=true;
              else if ( parseInt( str ) ) sel=true;
            }
          }
        }
      }
      obj.options[obj.options.length]=new Option( txt, val, false, sel );
      arr.shift();
    };
  };
};
jQuery.noConflict();
var phi=new PhiBase();
var $=function(id){
  var o;
  if ( typeof(id)==='string' ) o=phi.getElementById(id);
  else o=id;
  if ( !o ) return undefined;
  return new PhiItem(o);
};
