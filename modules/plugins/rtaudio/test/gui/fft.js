//<script type="text/javascript">

var screenx=512, screeny=200, offset=10, x;
var yr = new Array(512); yi = new Array(512);
var n=1024, g=10, p=2*Math.PI/n, g1, l, m, k, k1, k2, k3;
var y1, y2, j;

var dend = "px; position:absolute; top:1px; width:2px; height:2px; font-size=1; background-color:#0000ff\'></div>"
var fend = "px; position:absolute; top:1px; width:1px; height:2px; font-size=1; background-color:#0000ff\'></div>"

  	for (var i=1; i<=screenx; i++)	{
  	x=i + offset;

	document.write("<div id=\'d"+i+"\' style=\' left:"+x+dend);
	document.write("<div id=\'f"+i+"\' style=\' left:"+x+fend);
	
	}

replot();

fft();

function replot() {
		
	var xmin=1 * document.f.xmin.value;
	var xmax=1 * document.f.xmax.value;
	var ymin=1 * document.f.ymin.value;
	var ymax=1 * document.f.ymax.value;
	
	if (ymin==ymax) { ymin=-Math.abs(ymin); ymax=Math.abs(ymax); 
	document.f.ymin.value = ymin;
	document.f.ymax.value = ymax;		}
	
	if (xmin==xmax) { xmin=-Math.abs(xmin); xmax=Math.abs(xmax); 
	document.f.xmin.value = xmin;
	document.f.xmax.value = xmax;		}
	
	var func=document.f.func.value;
	
	var xf=(xmax-xmin)/screenx, yf=screeny/(ymax-ymin);
  	var y, x1, y1, obj, ptr;
  	var scry = screeny + offset;
  	
  	with (Math) {
  	
  	for (var i=1; i<=screenx; i++)	{

   	x = i*xf + xmin;
  	y =eval(func); 
   	if (y>ymax) y=ymax;
  	if (y<ymin) y=ymin;
  	y1=scry-(y-ymin)*yf;
  	ptr = "d"+i;
  	obj = document.getElementById(ptr);
  	obj.style.top = y1;
  	
		} } }
		
function fft()	{

	var func=document.f.func.value;
	var xmin=1 * document.f.xmin.value;
	var xmax=1 * document.f.xmax.value;	
	var xf=(xmax-xmin)/n, mg1;
	
  	with (Math) {
  	
  	for (var i=1; i<=n; i++)	{

   	x = i*xf + xmin;
  	y =eval(func); 
  	yr[i]=y; yi[i]=0;
  	
  } 
}
		
with (Math)	{

  for (l=0; l<=(g-1); l++) {
    g1 = pow(2,(g-l-1));
    m=0;

    for (i=1; i<=pow(2,l); i++) {
    k1=floor(m/g1);
    kaylyn();
    y1=cos(p*k2);
    y2=-sin(p*k2);
	
for (j=1; j<=g1; j++)  {
m=m+1;
mg1=m+g1;
y3=yr[mg1]*y1-yi[mg1]*y2;
y4=yr[mg1]*y2+yi[mg1]*y1;
yr[mg1]=yr[m]-y3;
yi[mg1]=yi[m]-y4;
yr[m]=yr[m]+y3;
yi[m]=yi[m]+y4;
	
}
	
	m = m + g1;
		
	}	}
	
	for (i=0; i<=(n-1); i++)	{
	
	k1 = i;
	kaylyn();
	
	if (k2>=i) continue;
			
	k3 = yr[i+1];
	yr[i+1] = yr[k2+1];
	yr[k2+1] = k3;
	k3 = yi[i+1];
	yi[i+1] = yi[k2+1];
	yi[k2+1] = k3;
	
		}

	var ymax=0;	

	for (i=1; i<=n/2; i++)	{
	
	yr[i] = Math.sqrt((yr[i]*yr[i] + yi[i]*yi[i]))*2/n;
	ymax=max(yr[i], ymax);
		
		}
		
	yr[1] = yr[1]/2;
	var ymaxstr = Math.ceil(ymax*1.1*100)/100;
	document.f.Fmax.value = ymaxstr.toString();
		
	replot2();
	
	export1();
		
	} }
	
function kaylyn()	{

	k2=0;
	
	for (k=1; k<=g; k++) {

	k3=Math.floor(k1/2);
	k2=2*(k2-k3) + k1;
	k1=k3;
		
		}
	}
	
function replot2() {
	
	var xmin = 1 * document.f.xmin.value;
	var xmax = 1 * document.f.xmax.value;
	var ymin=0;
	var ymax=1 * document.f.Fmax.value;
	var numax = n/2/(xmax-xmin);	
	var omega = numax*2*Math.PI;
	
	var numaxstr = Math.round(numax*10)/10;
	document.f.numax.value = numaxstr.toString();

	var omegastr = Math.round(omega*10)/10;
	document.f.omega.value = omegastr.toString();
	
	var yf=screeny/(ymax-ymin);
  	var y, x1, y1, obj, ptr;
  	var yoff = 220+screeny;
  	
  	with (Math) {
  	
  	for (var i=1; i<=n/2; i++)	{
  	
  	y = yr[i];
  	
   	if (y>ymax) y=ymax;
  	if (y<ymin) y=ymin;
  	y1=round(yoff-(y-ymin)*yf);
  	
  	ptr="f"+i;
  	obj=document.getElementById(ptr);
  	obj.style.top=y1;
  	obj.style.height=yoff-y1+1;
  	
		} } }
		
function export1()	{

	var longstr="";
  	
  	for (var i=1; i<=n/2; i++)	{

	longstr += i-1 + ", " + yr[i] + "\n";
  	
  	}
  			
  	document.f.exportfield.value = longstr;
  	
	}

//</script>