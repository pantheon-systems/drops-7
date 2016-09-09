// A LINKED LIST
function LinkedList() {
  
  // create a node for the list
  LinkedList.makeNode = function() { 
    return {value: null, previous: null, next: null}; 
  }; 

  //defaults
  this.start = null; 
  this.end = null; 

  // add a node...defaults to the end
  this.add = function(value) { 
    
    // if start is null, we haven't added anything yet
    if (this.start === null) { 
      // our new node will be the first
      this.start = LinkedList.makeNode(); 
      //and the last
      this.end = this.start; 
    } else { 
      
      this.end.next = LinkedList.makeNode(); // otherwise, we add a new node as the next to the end
      this.end.next.previous = this.end; // the new node's previous becomes the current end
      this.end = this.end.next; // then we set the end to the new node
    } ; 
    this.end.value = value; //and set the value to our new value
  }; 

  // delete a node by value
  this.delete = function(value) { 
    var current = this.start; //start at the beginning 
    while (current !== null) { //loop through the nodes
      if (value === current.value) { // if values match, we've found the one to delete
        if (current === this.start) { // if the match is the very first node 
          this.start = current.next; //set the start to the next node
          this.start.previous = null; //and there is no previous anymore
          return; 
        } 
        if (current === this.end){ // if the match is the very last node
          this.end = current.previous; // the end becomes the previous
          this.end.next = null; //and there is no next
          return; 
        }
        current = current.next; 
      }
    }
  }; 

//    this.insertAsFirst = function(d) { 
//      var temp = List.makeNode(); 
//      temp.next = this.start; 
//      this.start = temp; 
//      temp.value = d; 
//    }; 

//    this.insertAfter = function(t, d) { 
//      var current = this.start; 
//      while (current !== null) { 
//        if (current.value === t) { 
//          var temp = List.makeNode();
//          temp.value = d; 
//          temp.next = current.next; 
//          if (current === this.end) this.end = temp;
//          current.next = temp; 
//          return; 
//        } 
//        current = current.next; 
//      }
//    };

    this.itemByIndex = function(i) { 
      var current = this.start; 
      while (current !== null) { 
        i--; 
        if (i === 0) return current; 
        current = current.next; 
      } 
      return null; 
    }; 
    
    this.itemByValue = function(val) { 
      console.log("searching for " + val);
      var current = this.start; 
      while (current !== null) { 
//        console.log("checking if " + val + "===" + current.value);
        if (val == current.value){
          console.log("found match. returning current node");
          return current; 
        }
        current = current.next; 
      } 
      console.log("no match found. returning null");
      return null; 
    }; 

    // applies a function to each node
    this.each = function(f) {
      var current = this.start;
      while (current !== null) { 
        f(current); 
        current = current.next; 
      } 
    };
  }



function formatXml(xml) {

  var formatted = '';
  var reg = /(>)(<)(\/*)/g;
  xml = xml.replace(reg, '$1\r\n$2$3');
  var pad = 0;

  jQuery.each(xml.split('\r\n'), function(index, node) {
    var indent = 0;
    if (node.match(/.+<\/\w[^>]*>$/)) {
      indent = 0;
    } else if (node.match(/^<\/\w/)) {
      if (pad != 0) {
        pad -= 1;
      }
    } else if (node.match(/^<\w[^>]*[^\/]>.*$/)) {
      indent = 1;
    } else {
      indent = 0;
    }
    var padding = '';
    for (var i = 0; i < pad; i++) {
      padding += '  ';
    }
    formatted += padding + node + '\r\n';
    pad += indent;
  });

  return formatted;
}