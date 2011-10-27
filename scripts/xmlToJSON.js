/*function xmlToJSON( xml ) {
  var obj = {};
  var i;
  if( xml.nodeType === 3 ) { //text
    obj = xml.nodeValue;
  } else if( xml.nodeType === 1 ) { //element
    if( xml.hasAttributes() && xml.attributes.length > 0 ) {
      var attr = xml.attributes;
      for( i in attr ) obj[ "@attributes" ][ i ] = attr[ i ];     
    }
  }

  if( xml.hasChildNodes() && xml.childNodes.length > 0 ) {
    var children = xml.childNodes;
    for( var i = 0; i != children.length; i += 1 ) {
      var child = children.item( i );
      var xc = xmlToJSON( child );  
      if( obj[ child.nodeName ].length ) obj[ child.nodeName ].push( xc );
      else { 
        obj[ child.nodeName ] = [];
        obj[ child.nodeName ].push( xc );
      }
    } 
  }
  return obj;
};*/

function jsonToString( obj, indent ) {
  indent = indent || "";
  var s = "";
  for( var i in obj ) {
    s += indent + i + ": ";
    if( obj[ i ] && typeof obj[ i ] !== 'object' ) {
      s += obj[ i ];
    } else if( obj[ i ] ) s += "\n" + jsonToString( obj[ i ], indent + " " ); 
  }
  return s + "\n";
};


function jsonTraverse( obj, visitor ) {
  visitor( obj );
  if( typeof obj === 'object' ) { 
    for( var i in obj ) {
      if( obj[ i ] ) jsonTraverse( obj[ i ], visitor );
    }
  }
  return visitor;
};


// Changes XML to JSON
function xmlToJson(xml) {

  // Create the return object
  var obj = {};

  if (xml.nodeType === 1) { // element
    // do attributes
    if ( xml.attributes && xml.attributes.length > 0) {
      obj["@attributes"] = {};
      for (var j = 0; j < xml.attributes.length; j += 1) {
        var attribute = xml.attributes.item(j);
        obj["@attributes"][attribute.name] = attribute.value;
      }
    }
  } else if (xml.nodeType === 3) { // text
    obj = xml.nodeValue;
  }

  // do children
  if (xml.hasChildNodes() && xml.childNodes ) {
    for(var i = 0; i < xml.childNodes.length; i += 1) {
      var item = xml.childNodes.item(i);
      if( item.nodeType === 3 ) {
        if( !item.nodeValue.match( /\S/ ) ) continue;
      } 
      if( !item ) continue; 
      var nodeName = item.nodeName.replace( /#/, "" );
      if( !nodeName ) continue;
      var ji = xmlToJson(item);
      if ( typeof obj[nodeName] === "undefined" ) {
        obj[nodeName] = ji;
      } else {
        if ( !(obj[nodeName] instanceof Array) ) {
          obj[nodeName] = [ obj[ nodeName ] ];
        }
        obj[nodeName].push( ji );
      }
    }
  }
  return obj;
}

