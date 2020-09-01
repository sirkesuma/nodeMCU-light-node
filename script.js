// api url 
const api_url = 
	"http://192.168.43.234/"; 

// Defining async function 
async function getapi(url) { 
	
	// Storing response 
	const response = await fetch(url); 
	
	// Storing data in form of JSON 
	var data = await response.json(); 
	console.log(data); 
	if (response) { 
		hideloader(); 
	} 
	show(data); 
} 

async function ledPower(ledName, power) {
	const url = api_url.concat(ledName,"/",power);

	const r = await fetch(url);
}

// Calling that async function 
var myVar = setInterval(getData, 1000);

function getData(){
    getapi(api_url);
}

// Function to hide the loader 
function hideloader() { 
	document.getElementById('loading').style.display = 'none'; 
} 
// Function to define innerHTML for HTML table 
function show(data) { 
	let tab = 
		`<tr> 
		<th>Name</th> 
		<th>Voltage</th> 
		<th>Current</th> 
		</tr>`; 
	
	// Loop to access all rows 
	for (let r of data.Load) { 
		tab += `<tr><td>${r.name} </td> <td>${r.voltage} ${r.voltage_unit}</td> <td>${r.current} ${r.current_unit}</td>	</tr>`; 
	} 
	// Setting innerHTML as tab variable 
	document.getElementById("power_data").innerHTML = tab; 
	
} 


