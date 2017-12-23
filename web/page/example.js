//Copyright (C) 2016 <>< Charles Lohr, see LICENSE file for more info.
//
//This particular file may be licensed under the MIT/x11, New BSD or ColorChord Licenses.

function initSevenSeg() {
	var sevenItm = `
	<tr><td width=1><input type=submit onclick="ShowHideEvent('SevenSeg');SevenSegTick();" value="SevenSeg"></td><td>
	<div id="SevenSeg" class="collapsible">
		<div class="sevenSegContainer">
			<div id="sevenSegArray" class="sevenSegArray4"></div>
		</div>
		<br/>
		<input type=button id=SevenSegBtn value="Refresh"> <input type=button value="Clock (cdt)" onclick="doSend('cdt');">
		<br/>
		<input type=button value="Year (cdy)" onclick="doSend('cdy');"> <input type=button value="Month (cdM)" onclick="doSend('cdM');">
		<br/>
		<input type=button value="Seconds (cds)" onclick="doSend('cds');"> <input type=button value="Date (cdd)" onclick="doSend('cdd');">
		<br/>
		<input type=button value="HH:M_ (cdn)" onclick="doSend('cdn');"> <input type=button value="HH:MM (cdm)" onclick="doSend('cdm');">
		<p id=SevenSegInfoDspl>&nbsp;</p>
	</div>
	</td></tr>`;
	$('#MainMenu > tbody:last').after( sevenItm );

	$("#sevenSegArray").sevenSeg({ digits: 4, pattern: 0x01200104, decimalPoint: false });

	$('#SevenSegBtn').click( function(e) {
		$('#SevenSegInfoDspl').html('...');
		QueueOperation( "7i", clbSevenSegData ); // Send info request to ESP
	});
}

window.addEventListener("load", initSevenSeg, false);

function clbSevenSegData(req,data){
	var params = data.trim().split( "\t" );
	//console.log("clbSevenSegData",req,data,params);
	$('#SevenSegInfoDspl').html('SystemDateTime: "'+ params[3] +'"');
	$("#sevenSegArray").sevenSeg({ digits: params[1], pattern: params[2]});
}

function SevenSegTick()
{
	if( IsTabOpen('SevenSeg') )
	{
		QueueOperation( "7i", clbSevenSegData );
		setTimeout( SevenSegTick, 1000 );
	}
	else
	{
		//Stop.
	}
}