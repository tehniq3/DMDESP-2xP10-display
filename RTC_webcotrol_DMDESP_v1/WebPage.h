
String message, XML;

char setwaktu[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<meta name='viewport' content='width=device-width, initial-scale=1'>

<head>
<title>NTP_clock_control</title>
<style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>

<body onload='process()'>

<h1>SETTING THE CLOCK AND DATE</h1>
<table width='100%'>
  <tr>
    <td>
      <form>
        <h4>Date</h4>  
        <input type='date' name='date' min='2020-01-01'><br><br>
        <input type='submit' value='Update Date'> 
      </form>  
    </td>
    <td>
      <form>  
        <h4>Clock</h4>
        <input type='TIME' name='time'><br><br>
        <input type='submit' value='Update Clock'> 
      </form>
    </td>
  </tr>
</table>
<br><br>
<h5><a class ='tombol' href="/setdisplay">Display</a></h5>
<br><br>
<h5><a class ='tombol' href="/setinfo">Information</a></h5>
</body>
</html>
<script>

var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }
 setTimeout('process()',1000);
}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rYear');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('year').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rMonth');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('month').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rDay');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('day').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rHour');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('hour').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rMinute');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('minute').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rSecond');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('second').innerHTML=message;
 }
}
</script>
</html>
)=====";


char setdisplay[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<head>
  <title>NTP_clock_control</title>
  <style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>BRIGHTNESS SETTINGS</h1>
<form id='data_form' name='frmText'>
  <label>Minimum<input id="cerah" type='range' name='Cerah' min='18' max='255'>Maximum
  <br>
</form>
<br>
<button onClick="setDisp()"> Save </button><span> </span>
<br><br>
<h5><a class ='tombol' href="/">Back</a></h5>
<br><br>
<h5><a class ='tombol' href="/setinfo">Information</a></h5>
</body>

<script>

function setDisp() {

  console.log("tombol telah ditekan!");
  
  var cerah = document.getElementById("cerah").value;
  var datadisp = {cerah:cerah};

  var xhr = new XMLHttpRequest();
  var url = "/settingdisp";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(datadisp));
  
}

var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }
}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rCerah');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('cerah').value=message;

 }
}
</script>
</html>

)=====";

char setinfo[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<head>
<title>NTP_clock_control</title>
<style> body { width: 97% ; text-align: center; } td {text-align: center;} input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1,h2,h3,h4 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>INFORMATION</h1>

<form>
<div>
  <label for="info1">Custom text</label>
  <input id="info1" placeholder="Put new text here.. "/>
</div>
<br>
<div>
  <button onClick="setInfo()"> Change </button>
</div>
</form>
<br><br>

<h5><a class ='tombol' href="/">Back</a></h5>
<br><br>
<h5><a class ='tombol' href="/setdisplay">Display</a></h5>

</body>
<script>


var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }

}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rInfo1');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('info1').value=message;

 }
}

function setInfo() {
  console.log("tombol telah ditekan!");

  var info1 = document.getElementById("info1").value;
  var datainfo = {info1:info1};

  var xhr = new XMLHttpRequest();
  var url = "/settinginfo";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(datainfo));
};
</script>
</html>
)=====";
