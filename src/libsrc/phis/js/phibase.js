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
  this._x=0;this._y=0;this._w=0;this._h=0;this._a=false;
  this.wid=function(){return this._wid;};
  this.id=function(){return obj.id;};
  this.getContext=function(s){return phi.getElementById(obj.id).getContext(s);};
  this.on=function(t,f){j(id).on(t,f);return this;};
  this.off=function(t,f){j(id).off(t,f);return this;};
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
  this.moveTo=function( l, t, s, d, e ) {
    var o={queue:false,duration:(d?d:1000),easing:e};
    var x=this.x(),y=this.y(),_x=this._x,_y=this._y;
    setTimeout( function(){
      if (l!=x) j(id).animate({left:l+_x},o);
      if (t!=y) j(id).animate({top:t+_y},o);
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
    if(l===undefined)return {left:parseInt(j(id).css('left'))-this._x,
        top:parseInt(j(id).css('top'))-this._y};
    j(id).css({left:l+this._x,top:t+this._y});return this;
  };
  this.width=function(w){
    if(w===undefined)return parseInt(j(id).css('width'))-this._w;
    j(id).css('width',w+this._w); return this;
  };
  this.height=function(h){
    if(h===undefined)return parseInt(j(id).css('height'))-this._h;
    j(id).css('height',h+this._h); return this;
  };
  this.x=function(l){
    if(l===undefined)return parseInt(j(id).css('left'))-this._x;
    j(id).css('left',l+this._x); return this;
  };
  this.y=function(t){
    if(t===undefined)return parseInt(j(id).css('top'))-this._y;
    j(id).css('top',t+this._y); return this;
  };
  this.left=function(l){return this.x(l);};
  this.top=function(t){return this.y(t);};
  this.zIndex=function(i){
    if(i===undefined)return parseInt(j(id).css('zIndex'));
    j(id).css('zIndex',i); return this;
  };
  this.cursor=function( c ) {
    if(!c)return j(id).css('cursor');j(id).css({cursor:c});return this;
  };
  this.fadeIn=function( s, d, o, e ) {
    j(id).css({opacity:0});
    setTimeout( function(){
      j(id).css('visibility','visible').show().animate({opacity:(o?o:1)},
        {queue:false,duration:(d?d:1000),easing:e});
    },s?s:0 );return this;
  };
  this.fadeOut=function( s, d, o, e ) {
    j(id).css({opacity:1});
    setTimeout( function(){
      j(id).css('visibility','visible').show().animate({opacity:(o?o:0)},
        {queue:false,duration:(d?d:1000),easing:e,complete:function(){
            if (j(id).css('opacity')<0.1) j(id).hide();
        }
      });
    },s?s:0 );return this;
  };
  this.rotateIn=function( a, s, d, e ) {
    if ( !a ) a=0x2;
    var w=j(id).css('width');
    var h=j(id).css('height');
    j(id).hide();
    setTimeout( function(){
      if (a & 1){j(id).css('width',0).show().animate({width:w},{queue:false,duration:(d?d:1000),easing:e});}
      if (a & 2){j(id).css('height',0).show().animate({height:h},{queue:false,duration:(d?d:1000),easing:e});}
    },s?s:0 );return this;
  };
  this.rotateOut=function( a, s, d, e ) {
    if ( !a ) a=0x2;
    var w=j(id).css('width');
    var h=j(id).css('height');
    setTimeout( function(){
      if (a & 1){j(id).animate({width:0},{queue:false,duration:(d?d:1000),easing:e,complete:function()
        {j(id).hide().css({width:w});}});}
      if (a & 2){j(id).animate({height:0},{queue:false,duration:(d?d:1000),easing:e,complete:function()
        {j(id).hide().css({height:h});}});}
    },s?s:0 );return this;
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
    if(v===undefined)return j(id+'_phi').is(':checked');
    if(typeof(v)==='boolean'||typeof(v)==='number'){j(id+'_phi').prop('checked',v);return this};
    if(typeof(c)!=='boolean'&&typeof(c)!=='number')return j(id+'_phi [value="'+v+'"]').is(':checked');
    j(id+'_phi [value="'+v+'"]').prop('checked',c);return this;
  };
  this.selected=function( v, s ){
    if(v===undefined){
        var arr = []; var i=0;
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

function Phi(l,s){
  var j=jQuery;
  this.session=s;
  this.lang=l;
  this.xAxis=1;
  this.yAxis=2;
  this.zAxis=4;
  this.onload=function(){};
  this.onsubmit=function(){return true;};
  this.isHtml=function(){return true;};
  this.isNative=function(){return false;};
  this.getElementById=function( id ){return window.document.getElementById( id );};
  this.href=function( url ) {
    var link=String( url );
    if ( link.indexOf( 'philang' )===-1 ) {
      if ( link.indexOf( '?' )>-1 ) link=link+'&philang='+this.lang;
      else link=link+'?philang='+this.lang;
    }
    if ( this.session.length>0 ) link=link+'&phisid='+this.session;
    window.location.href=link;
  };
  this.createAjax=function(){
    var a=(window.XMLHttpRequest)?new XMLHttpRequest():((window.ActiveXObject)?
      new ActiveXObject('Microsoft.XMLHTTP'):false);
    if ( a ) a.onreadystatechange=function(){};
    return a;
  };
  this.ajax=this.createAjax();
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
  j( function($){ $(window).on( 'load', phi.onload ); });
  j.noConflict();
};

var $=function( id ){
  var j=jQuery;
  if ( typeof( id )==='string' ) {
      var o=j( 'body' ).data( 'phi'+id );
      if ( o!==undefined ) return o;
      o=phi.getElementById( id );
      if ( !o ) return undefined;
      o=new PhiItem( o );
      j( 'body' ).data( 'phi'+id, o );
      return o;
  }
  if ( id!==undefined ) { // dom el
      id=j(id).attr('id');
      return j( 'body' ).data( 'phi'+id );
  }
  return this;
};

$.slideShow=function( s, c ){
    var j=jQuery;
    for (i=1;i<c;i++) {$(s+'_phi_'+i).hide();}
    var o=$(s);
    o._fi=4000;o._ft=2000;o._cnt=c;o._cur=0;o._id=s;o._wid=44;
    o.count=function() { return o._cnt; };
    o.start=function() {
        var n=o._cur+1; if( n>=o._cnt ) n=0;
        $(o._id+'_phi_'+o._cur).fadeOut(0,o._ft);
        $(o._id+'_phi_'+n).fadeIn(0,o._ft);
        o._cur=n;
        o._t=setTimeout(o.start,o._fi);
    };
    o.stop=function() { clearTimeout( o._t );j('#'+s).stop(true); };
    o.display=function( c ){
        if ( c===undefined ) return o._cur;
        o.stop();
        o._cur=parseInt(c);
        if ( o._cur>=o._cnt ) o._cur=0;
        for ( i=0; i<o._cnt; i++ ) $(o._id+'_phi_'+i ).hide();
        $(o._id+'_phi_'+o._cur).show();
        return o;
    };
    o.fadeTime=function( f ){
        if ( f===undefined ) return parseInt(o._ft/1000);
        o._ft=f*1000; return o;
    };
    o.fadeTimeMS=function( f ){
        if ( f===undefined ) return o._ft;
        o._ft=f; return o;
    };
    o.fadeInterval=function( f ){
        if ( f===undefined ) return parseInt(o._fi/1000);
        o._fi=f*1000; return o;
    };
    o.fadeIntervalMS=function( f ){
        if ( f===undefined ) return o._ft;
        o._fi=f; return o;
    };
    o.titles=function( t ) {
        if ( t===undefined ) {
            var a=new Array;
            for ( i=0; i<o._cnt; i++ ) a[i]=$(o._id+'_phi_'+i).title();
            return a;
        }
        if ( j.isArray( t ) ) {
            for ( i=0; i<t.length; i++ ) $(o._id+'_phi_'+i).title( t[i] );
            return o;
        }
        var s=String( t );
        var l=s.split( ':' );
        for ( i=0; i<l.length; i++ ) $(o._id+'_phi_'+i).title( l[i] );
    };
    o.start();
    o.mouseover(function(){o.stop();});
    o.mouseout(function(){o.start();});
    return o;
};

$.phi=function( s, i, x, y, w, h ) {
    var o=$(s);
    if ( o===undefined ) return o;
    o._wid=i;
    if ( x ) o._x=x;
    if ( y ) o._y=y;
    if ( w ) o._w=w;
    if ( h ) o._h=h;
    if ( i<3 || (i>49 && i<55) ) {
        o.val=function( t ) {
            if ( t===undefined ) return j( '#'+s ).val();
            j( '#'+s ).val( t ); return o;
        };
        o.readOnly=function( r ) {
            if ( r===undefined ) return j( '#'+s ).prop( 'readOnly' );
            j( '#'+s ).prop( 'readOnly',r ); return o;
        };
        o.accessKey=function( a ) {
            if ( a===undefined ) return j( '#'+s ).attr( 'accesskey' );
            j( '#'+s ).attr( 'accesskey', a );
        };
    }
    return o;
};

$.table=function( s, m, d, n, v, h, ms, chk ) {
    var j=jQuery, o=$(s), gid='#'+s+'_phit';
    if ( o===undefined ) return o;
    o._h=h; o._l=[]; o._wid=(ms?43:46); o._v=v; o._c=[]; o._b=[]; o._chk=chk; o._hr=[];
    o._cellFn=function(r,v,u,cm) {
        r=parseInt(r); var c=parseInt(cm.name);
        var s='', t=o._c[r+'.'+c];
        if ( t!==undefined ) s='color:'+t+';'
        t=o._b[r+'.'+c];
        if ( t!==undefined ) s+='background-color:'+t;
        else s+='background-color:transparent;background-image:none';
        return 'style="'+s+'"';
    };
    o._selRowFn=function(id,c){
        if ( o._wid===46 ) j('#'+s+'_phi').val(o._v[parseInt(id)]);
        o.currentRowChanged(parseInt(id));
    };
    o._completeFn=function() {
        for ( i=0; i<o.rowCount(); i++ ) {
            if ( o._hr[i] ) o.hideRow( i );
            if ( ms ) {
                j('#jqg_'+j(gid)[0].id+'_'+i+'r' ).val(o._v[i])
                    .attr( 'name', s ).prop('checked',o._chk[i]);
            }
        }
    };
    o._getRowForName=function(r) {
        for ( i=0; i< o.rowCount(); i++ ) { if ( o._v[i]===r ) return i; }
        return undefined;
    };
    o.rowCount=function(){ return j(gid).jqGrid('getGridParam','reccount');};
    o.height=function(h){
        if(h===undefined) return parseInt(j('#'+s).css('height'));
        j('#'+s).css( 'height', h );
        j(gid).jqGrid('setGridHeight',h+o._h); return o;
    };
    o.label=function(c,l) {
        c=parseInt(c);
        if ( l===undefined ) return o._l[c];
        o._l[c]=l;
        j(gid).setLabel(c+'c',l);
        return o;
    };
    o.cellText=function(r,c,t){
        r=parseInt(r);c=parseInt(c);
        if ( t===undefined ) return j(gid).jqGrid('getCell',r+'r',c+'c');
        j(gid).jqGrid('setCell',r+'r',c+'c',t,'','',true);
        return o;
    };
    o.cellColor=function(r,c,t){
        r=parseInt(r);c=parseInt(c);
        o._c[r+'.'+c]=t;
        j(gid).jqGrid('setCell',r+'r',c+'c','',{color:t},'',false);
        return o;
    };
    o.cellBgColor=function(r,c,t){
        r=parseInt(r);c=parseInt(c);
        o._b[r+'.'+c]=t;
        j(gid).jqGrid('setCell',r+'r',c+'c','',{backgroundColor:t},'',false);
        return o;
    };
    o.checked=function(r,c) {
        if ( !ms ) return undefined;
        if ( c===undefined ) {
            if ( typeof(r)==='string' ) r=o._getRowForName(r);
            if ( typeof(r)==='number' ) return o._chk[r];
            return undefined;
        }
        if ( typeof(r)==='string' ) r=o._getRowForName(r);
        if ( typeof(r)!=='number' ) return o;
        o._chk[r]=c?1:0;
        j(gid).jqGrid().trigger('reloadGrid');
        return o;
    };
    o.currentRowChanged=function(id,v){};
    o.addRow=function(d) { o._v[o.rowCount()]=d;j(gid).jqGrid('addRowData',o.rowCount()+'r',{},'last'); return o; };
    o.hideCol=function(c) { j(gid).jqGrid('hideCol',parseInt(c)+'c');return o;};
    o.showCol=function(c) { j(gid).jqGrid('showCol',parseInt(c)+'c');return o;};
    o.hideRow=function(r) { o._hr[parseInt(r)]=1; j('#'+parseInt(r)+'r',gid).css({display:'none'});return o;};
    o.showRow=function(r) { o._hr[parseInt(r)]=0; j(gid).jqGrid().trigger('reloadGrid');return o; };
    o.clear=function(r) { o._v=[]; o._chk=[]; o._c=[]; o._b=[]; o._hr=[]; j(gid).jqGrid('clearGridData').trigger('reloadGrid');};
    var g=j(gid).jqGrid({colModel:m,datatype:'jsonstring',gridview:true,autowidth:true,height:o.height()+h,cmTemplate:{sortable:false,cellattr:o._cellFn},
        jsonReader:{root:'d',page:'p',total:'t',records:'r',cell:'c',id:'i'},datastr:d,viewsortcols:[true,'vertical',true],rownumbers:n,
        onSelectRow:o._selRowFn,sortable:true,multiselect:ms,gridComplete:o._completeFn});
    if ( ms ) {
        j('#cb_'+g[0].id).hide();
        j('#jqgh_'+g[0].id+'_cb').addClass('ui-jqgrid-sortable');
        var cm=g.jqGrid('getColProp','cb');
        cm.sortable=true;
        cm.sorttype=function(v,i) { return 'cb' in i && i.cb ? 1 : 0; };
    }
    return o;
};
