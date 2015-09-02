/*
*		VARIABLES GLOBALES
*/

var clock = undefined;
function documentReady() {
	if(clock == undefined) {
		openAlert("Share this page with computers on stage through WiFi (see Rekall welcome page > Share) to have a synced clock");
		
		clock = new Object();
		clock.lastCheck = 0;
		updateClock();
		$("body").css("background-color", "black");

		function resetClock(event) {
			$.ajax("/clock_data?reset=0");
		}
		$("#clock_sync").mousedown(function(event) {
			event.stopImmediatePropagation();
			if(clock.type == 'rst') {
				resetClock();
				$("#clock_menu li[clockMenuId='elp']").trigger("mousedown");
			}
			else if(clock.type == 'clk')
				$("#clock_menu li[clockMenuId='elp']").trigger("mousedown");
			else if(clock.type == 'elp')
				$("#clock_menu li[clockMenuId='txt']").trigger("mousedown");
			else if(clock.type == 'txt')
				$("#clock_menu li[clockMenuId='clk']").trigger("mousedown");
		});
		$("#clock_menu li").mousedown(function(event) {
			$("#clock_menu li").removeClass("selected");
			$(this).addClass("selected");
			clock.type = $(this).attr("clockMenuId");
			if(clock.type == 'rst')
				$("#clock_sync").text("VALIDATE?");
			else if(clock.type == 'txt') {
				openAlert("Send OSC data on <span style='text-transform:none;'>/rekall/clock/text</span> to Rekall running computer on port " + window.url("port", window.document.location+"") + "");
				$("#clock_sync").text("");
			}
			else
				$("#clock_sync").text("");
		});
		$("#clock_menu li[clockMenuId='clk']").trigger("mousedown");
	}
	$("#clock_sync").css("font-size", $("#clock_sync").height() + "px");
	$("#ruban").click(function() {
		rubanTimeoutTime = 99999;
	});
}
function updateClock() {
	function updateClockAsk() {
		var newCheck = 40 - (moment() - clock.lastCheck);
		if(newCheck <= 0)	updateClock();
		else				setTimeout(updateClock, newCheck);
	}
	clock.lastCheck = moment();
	$.ajax("/clock_data", {
		type: "GET",
		dataType: "json",
		success: function(clock_data) {
			clock_data.clockMoment = moment(clock_data.clock);
			clock_data.elapsed = ((clock_data.clock - clock_data.clockReset) / 1000.) + "";
			if(clock.type == "clk")
				$("#clock_sync").html(clock_data.clockMoment.format("HH:mm:ss") + "&nbsp;<span>" + clock_data.clockMoment.format("SSS") + "</span>");
			if(clock.type == "elp")
				$("#clock_sync").html(clock_data.elapsed.toHHMMSS(false, true) + "&nbsp;<span>" + clock_data.elapsed.tommm() + "</span>");
			if(clock.type == "txt") {
				if(clock_data.text.trim() == "")
					$("#clock_sync").html("<span>Waiting for data…</span>");
				else
					$("#clock_sync").html("<span>" + clock_data.text + "</span>");
			}
			updateClockAsk();
		},
		error: function() {
			updateClockAsk();
		}
	});
}
