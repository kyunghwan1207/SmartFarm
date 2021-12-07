function XHR_write(data){
          var xhr = new XMLHttpRequest();
 
          xhr.open("GET","remote_con.php?" + data);
          xhr.send();
}
function XHR_read(data){
          var xhr = new XMLHttpRequest();
 
          xhr.open("GET","remote_con.php?" + data, false);
          xhr.send();
 
     return xhr.responseText;
}
 
function repeat(){
      TEMP();HUMI();
}
setInterval(function(){repeat();},500);
 
//--------------------------------------------------------//
function LEDON(){
     XHR_write('LEDON');
 
     document.LED.src='img/led_on.png';
}
 
function LEDOFF(){
     XHR_write('LEDOFF');
 
    document.LED.src='img/led_off.png';
}
 
function DCMON(){
     XHR_write('DCMON');
 
    document.DCM.src='img/dcm_on.png';
}
 
function DCMOFF(){
     XHR_write('DCMOFF');
 
     document.DCM.src='img/dcm_off.png';
}
