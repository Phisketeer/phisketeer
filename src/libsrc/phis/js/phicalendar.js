function phiCalendar() {
    var now=new Date();
    this.today=new Date( now.getFullYear(), now.getMonth(), now.getDate() );
    //alert( 'phiCalendar');

    this.init=function( id, field, close, dayNames, monthNames, cur, start, end ) {
        this.parent=window.document.getElementById( id );
        this.parent.style.display='block';
        this.valueField=window.document.getElementById( field );
        this.monthNames=monthNames.split( ':' );
        this.dayNames=dayNames.split( ':' );
        this.curDate=new Date( cur.charAt(0)+cur.charAt(1)+cur.charAt(2)+cur.charAt(3), cur.charAt(5)+cur.charAt(6)-1, cur.charAt(8)+cur.charAt(9) );
        this.curYear=this.curDate.getFullYear();
        this.curMonth=this.curDate.getMonth();
        this.curDay=this.curDate.getDate();
        if ( start.length==10 ) this.startDate=new Date( start.charAt(0)+start.charAt(1)+start.charAt(2)+start.charAt(3), start.charAt(5)+start.charAt(6)-1, start.charAt(8)+start.charAt(9) );
        else this.startDate=this.curDate;
        if ( end.length==10 ) this.endDate=new Date( end.charAt(0)+end.charAt(1)+end.charAt(2)+end.charAt(3), end.charAt(5)+end.charAt(6)-1, end.charAt(8)+end.charAt(9) );
        else this.endDate=new Date( 9999, 11, 31 );
        this.setField( this.curDate );
        this.close=false;
        if ( close=='close' ) this.close=true;
        if ( this.close ) {
            //this.parent.onmouseout=function() { this.style.display='none'; }
        }
        this.show();
    };

    this.setField=function( date ) {
        var month=date.getMonth()+1;
        var day=date.getDate();
        var mm=new String( month ); var dd=new String( day );
        if ( month<10 ) mm='0'+month;
        if ( day<10 ) dd='0'+day;
        this.valueField.value=new String( date.getFullYear() )+'-'+mm+'-'+dd;
        if ( this.close ) {
            this.parent=this.removeAll( this.parent );
            this.parent.style.display='none';
            return;
        }
        this.curDate=date;
        this.show();
    };

    this.show=function() {
        this.parent=this.removeAll( this.parent );
        var table=window.document.createElement( 'table' );
        //table.style.display='block';
        table.className='phicalendar phibase';
        table.style.tableLayout='fixed';
        table.style.width='100%';
        table.style.height='100%';
        table.style.border='solid black 1px';
        table.appendChild( this.createHeader() );
        table.appendChild( this.createBody() );
        this.parent.appendChild( table );
    };

    this.switchMonth=function( todo ) {
        switch ( todo ) {
            case 'prevMonth':
                this.curYear=this.curMonth==0 ? this.curYear-1 : this.curYear;
                this.curMonth=this.curMonth==0 ? 11 : this.curMonth-1;
                break;
            case 'thisMonth':
                this.curMonth=this.curDate.getMonth();
                break;
            case 'nextMonth':
                this.curYear=this.curMonth==11 ? this.curYear+1 : this.curYear;
                this.curMonth=this.curMonth==11 ? 0 : this.curMonth+1;
                break;
            case 'prevYear':
                this.curYear=this.curYear-1;
                break;
            case 'thisYear':
                this.curYear=this.curDate.getFullYear();
                break;
            case 'nextYear':
                this.curYear=this.curYear+1;
                break;
        }
        this.show();
    };

    this.createHeader=function() {
        var thead=window.document.createElement( 'thead' );
        var tr=window.document.createElement( 'tr' );
        var th=window.document.createElement( 'th' );
        th.colSpan=7;
        th.className='phihighlight phihighlightedtext';
        thead.appendChild( tr );
        tr.appendChild( th );
        th.appendChild( this.createHeaderCell( '<', 'prevMonth' ) );
        th.appendChild( window.document.createTextNode( ' ' ) );
        th.appendChild( this.createHeaderCell( this.monthNames[this.curMonth], 'thisMonth' ) );
        th.appendChild( window.document.createTextNode( ' ' ) );
        th.appendChild( this.createHeaderCell( '>', 'nextMonth' ) );
        th.appendChild( window.document.createTextNode( ' ' ) );
        th.appendChild( this.createHeaderCell( '<', 'prevYear' ) );
        th.appendChild( window.document.createTextNode( ' ' ) );
        th.appendChild( this.createHeaderCell( this.curYear, 'thisYear' ) );
        th.appendChild( window.document.createTextNode( ' ' ) );
        th.appendChild( this.createHeaderCell( '>', 'nextYear' ) );
        return thead;
    };

    this.createBody=function() {
        var tbody=window.document.createElement( 'tbody' );
        var tr=window.document.createElement( 'tr' );
        var td;
        tbody.appendChild( tr );
        tr.className='phicalendarweek';
        for ( var i=0; i<7; i++ ) {
            td=window.document.createElement( 'td' );
            td.appendChild( window.document.createTextNode( this.dayNames[i] ) );
            td.style.margin='0px';
            tr.appendChild( td );
        }

        tr=window.document.createElement( 'tr' );
        var dayspermonth=[31,28,31,30,31,30,31,31,30,31,30,31];
        if ( (this.curYear%4==0) && ( (this.curYear%100!=0) || (this.curYear%400==0 ) ) ) dayspermonth[1]=29;
        var weekday=0;
        var begin=new Date( this.curYear, this.curMonth, 1 );
        var firstday=begin.getDay()-1;
        var lines=1;
        if ( firstday<0 ) firstday=6;
        if ( firstday==0 ) {
            for ( var i=0; i<7; i++) {
                var year=this.curMonth==0 ? this.curYear-1 : this.curYear;
                var month=this.curMonth==0 ? 11 : this.curMonth-1;
                var day=dayspermonth[month]-6+i;
                tr.appendChild( this.createBodyCell( new Date( year, month, day ), i ) );
            }
            tbody.appendChild( tr );
            tr=window.document.createElement( 'tr' );
            lines++;
        }
        for ( var i=0; i<firstday; i++, weekday++ ) {
            var year=this.curMonth==0 ? this.curYear-1 : this.curYear;
            var month=this.curMonth==0 ? 11 : this.curMonth-1;
            var day=dayspermonth[month]-firstday+i+1;
            tr.appendChild( this.createBodyCell( new Date( year, month, day ), i ) );
        }
        for ( var i=1; i<=dayspermonth[this.curMonth]; i++, weekday++ ) {
            if ( weekday == 7 ){
                tbody.appendChild( tr );
                tr=window.document.createElement( 'tr' );
                lines++;
                weekday=0;
            }
            tr.appendChild( this.createBodyCell( new Date( this.curYear, this.curMonth, i ), weekday ) );
        }
        var lastday=1;
        for ( var i=weekday; i<7; i++ ) {
            var year=this.curMonth==11 ? this.curYear+1 : this.curYear;
            var month=this.curMonth==11 ? 0 : this.curMonth+1;
            lastday=i-weekday+1;
            tr.appendChild( this.createBodyCell( new Date( year, month, lastday ), i ) );
        }
        if ( lines==5 ) {
            tbody.appendChild( tr );
            tr=window.document.createElement( 'tr' );
            for ( var i=0; i<7; i++ ) {
                var year=this.curMonth==11 ? this.curYear+1 : this.curYear;
                var month=this.curMonth==11 ? 0 : this.curMonth+1;
                tr.appendChild( this.createBodyCell( new Date( year, month, lastday ), i ) );
                lastday++;
            }
        }
        tbody.appendChild( tr );
        return tbody;
    };

    this.createHeaderCell=function( text, todo ) {
        var a=window.document.createElement( 'a' );
        var t=window.document.createTextNode( text );
        a.style.fontWeight='bold';
        a.appendChild( t );
        a.instance=this;
        a.setAttribute( 'href', 'javascript:' );
        a.onclick=function() { this.instance.switchMonth( todo ); }
        return a;
    };

    this.createBodyCell=function( date, pos ) {
        var td=window.document.createElement( 'td' );
        var num=window.document.createTextNode( date.getDate() );
        if ( pos>4 ) td.style.color='red';
        if ( date.getMonth()!=this.curMonth ) td.className='phicalendaroutdays';
        else td.className='phibase phitext';
        if ( date >= this.startDate  && date <= this.endDate ) {
            td.instance=this;
            td.onmouseover=function() { this.className='phihighlight phihighlightedtext'; }
            if ( date.getMonth()!=this.curMonth ) td.onmouseout=function() { this.className='phicalendaroutdays'; }
            else td.onmouseout=function() { this.className='phibase phitext'; }
            td.onclick=function() { this.instance.setField( date ); }
            td.style.cursor='pointer';
        } else {
            td.style.cursor='default';
            td.className='phicalendarweek';
        }
        if ( date.getFullYear()==this.curDate.getFullYear() && date.getMonth()==this.curDate.getMonth() && date.getDate()==this.curDate.getDate() ) {
            td.style.border='solid red 1px';
        } else if ( date.getFullYear()==this.today.getFullYear() && date.getMonth()==this.today.getMonth() && date.getDate()==this.today.getDate() ) {
            td.style.border='solid black 1px';
        } else {
            td.style.border='none';
        }
        td.appendChild( num );
        return td;
    };

    this.removeAll=function( obj ) {
        while( obj.childNodes.length > 0 )
            obj.removeChild( obj.childNodes[obj.childNodes.length-1] );
        return obj;
    }
}

