try {
var loco = new Loco;
//loco.cout.println(loco.p);
loco.p("CIAO");
loco.cbacks.push(function(){loco.cout.println("cback");});
Loco.prototype.cbacks.pop()();
loco.env.exec();
} catch(e) {
loco.cout.println(e);
}