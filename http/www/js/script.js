/*
    This file is part of Rekall.
    Copyright (C) 2013-2014

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
*		VARIABLES GLOBALES
*/



var rekall = undefined;
var rekall_common = new Object();
function documentReadyCommon() {
	var oldURL = window.document.location + "";
	var index = 0;
	var url = oldURL;
	index = oldURL.indexOf('?');
	if(index == -1)
	    index = oldURL.indexOf('#');
	if(index != -1)
	    url = oldURL.substring(0, index);
	if(!url.endsWith("/"))
		url = url + "/";
	rekall_common.isLocal = ((window.url("domain", url) == "localhost") || (window.url("domain", url) == "127.0.0.1"));
	rekall_common.jcropCoords = undefined;
}

function documentReadyRekall() {
	documentReadyCommon();
	moment.lang('fr');

	//Retina
	if(window.devicePixelRatio != undefined);
	Kinetic.pixelRatio = window.devicePixelRatio;

	if(rekall == undefined) {
		rekall = new Rekall();
		rekall.start();
		rekall.loadXMLFile();
	}
	else
	rekall.resize();
}

function documentReadyMenu() {
	documentReadyCommon();

	$.ajax("/projects", {
		type: "GET",
		dataType: "json",
		success: function(infos) {
			window.document.title = "Rekall de " + infos.owner;
			
			$("#menu_shares > div").html("");
			$.each(infos.ip, function(index, host) {
				if(window.url("domain", host.url) != window.url("domain", window.document.location+"")) {
					var verbosePre = "", verbosePost = "";
					if     (host.type == "lo")		verbosePre = "This computer access";
					else if(host.type == "lan")		verbosePre = "Local network access";
					else if(host.type == "wan")		verbosePre = "Internet access";
					verbosePre += " (" + host.name + ")";

					var addClass = "";
					if(host.reachable != "true") {
						verbosePost = "seems like your router doesn't allow external accesses<br/>(please read your internet box documentation in “NAT” or “port forwarding” section.)";
						addClass = "inaccessible";
					}

					$("#menu_shares > div").append("<div class='menu_shares_preinfo'>" + verbosePre + "</div><input class='" + addClass + "' type='text' value='" + host.url + "'/><div class='menu_shares_postinfo'>" + verbosePost + "</div>");
					$("#menu_shares").show();
					$("#rekall_port").text(host.port);
				}
			});
			$("#menu_shares h1").click(function() {
				if($("#menu_shares div").is(":visible")) {
					$("#menu_shares h1>span").html("&#x25B6;");
					$("#menu_shares div").slideUp();
					$("#menu_shares p").slideUp();
				}
				else {
					$("#menu_shares h1>span").html("&#x25BC;");
					$("#menu_shares div").slideDown();
					$("#menu_shares p").slideDown();
				}
			});
			$("#menu_tools h1").click(function() {
				if($("#menu_tools div").is(":visible")) {
					$("#menu_tools h1>span").html("&#x25B6;");
					$("#menu_tools div").slideUp();
					$("#menu_tools p").slideUp();
				}
				else {
					$("#menu_tools h1>span").html("&#x25BC;");
					$("#menu_tools div").slideDown();
					$("#menu_tools p").slideDown();
				}
			});
			
			$("#menu_projects > div").html("");
			$.each(infos.projects, function(index, project) {
				var html = "<div target='/" + project.name + "'>" + project.friendlyName;
				$.each(project.events, function(index, event) {
					html += "<br/><span>";
					if(event.file == undefined)		html += "<i>";

					html += event.name;
					if     (event.action == 0)		html += " removed";
					else if(event.action == 1)		html += " added";
					else if(event.action == 2)		html += " updated";

					if(event.author != "")			html += " by " + event.author;
					if(event.locationName != "")	html += " at " + event.locationName;
					html += ", " + moment(event.date, "YYYY:MM:DD HH:mm:ss").fromNow();

					html += "</i></span>";
				});
				
				
				$("#menu_projects > div").append()
				if(rekall_common.isLocal)
					html += "<div class='menu_renameproject invisible' title='Rename project name'>rename</div><div class='menu_finderproject invisible' title='Show the folder of this project'>show folder</div><div class='menu_removeproject invisible' title='Remove project'>&times;</div></div>";
				$("#menu_projects > div").append(html + "</div>");
			});
			if(infos.projects.length == 0)
				$("#menu_projects > div").append("<p>No project found.</p><p style='font-size: 12px; line-height: 120%;'><br/><br/><i>TODO: refaire ces phrases</i><br/><br/>A project is a folder on your hard drive where you store the files needed by your creative project.<br/><br/>Rekall will not alterate your files, it will only generate one folder called <i>rekall_cache</i> to store xtra informations (photo and video thumbnails, metadatas). Rekall monitors all the file operations (creation, update…) in the project folder and allows you to sort and visualize your creative process.<br/><br/><u onClick='javascript:$(\"#menu_addproject\").trigger(\"click\");'>Start a project now by selecting a folder</u><br/>and Rekall will make the first analysis of all the files. It can take a while, so have a look<br/>at the icon next to your computer clock!</p>");


			$("#menu_shares > div > input").click(function(event) {
				event.stopPropagation();
				$(this).select();
			});
			$("#menu_projects > div > div").click(function(event) {
				event.stopPropagation();
				window.document.location = $(this).attr("target");
			});
			$("#menu_projects .menu_renameproject").click(function(event) {
				event.stopPropagation();
				var friendlyName = prompt("Please enter the new project name", $(this).parent().clone().children().remove().end().text());
				if(friendlyName != null) {
					$.ajax($(this).parent().attr("target") + "/rename?friendlyName=" + friendlyName, {
						success: function() {
							reload();
						}
					});
				}
			});
			$("#menu_projects .menu_finderproject").click(function(event) {
				event.stopPropagation();
				$.ajax($(this).parent().attr("target") + "/finder");
			});
			$("#menu_projects .menu_removeproject").click(function(event) {
				event.stopPropagation();
				var sur = confirm("Are-you sure to remove the project " + $(this).parent().clone().children().remove().end().text() + "?");
				if(sur == true) {
					$.ajax($(this).parent().attr("target") + "/remove", {
						success: function() {
							reload();
						}
					});
				}
			});
			$("#menu_projects > div > div").mouseover(function(event) {
				$(this).find("div").show();
			});
			$("#menu_projects > div > div").mouseleave(function(event) {
				$(this).find("div").hide();
			});
			
			$("#menu_projects #menu_addproject").click(function(event) {
				event.stopPropagation();
				$.ajax("/add", {
					success: function() {
						setTimeout(reload, 500);
					}
				});
			});

		}
	});
	
	/*
	<img id='screenshot' src='/capture' width='100%'>
	<div onClick='alert(JSON.stringify(rekall_common.jcropCoords));'>OK</div>
	
	$('#screenshot').Jcrop({
		onChange:   function(c) {
			rekall_common.jcropCoords = c;
		},
		onSelect:   function(c) {
			rekall_common.jcropCoords = c;
		},
		onRelease:  function(c) {
			rekall_common.jcropCoords = undefined
		}
	});
	*/
}
function reload() {
	window.location.reload();
}

function documentReadyIntro() {
	documentReadyCommon();
	
	setTimeout(function() {
		$("#intro img").fadeIn(750, function() {
		});
		$("#intro h1").fadeIn(2000, function() {
			$("#intro p").fadeIn(1000, function() {

			});
		});
	}, 250);

	/*
	var scenarioTime = 0;
	var colors = ["#57AA8E", "#FBA682", "#F7634C", "#FCC376", "#F94B32", "#F87439", "#000000"];
	setInterval(function() {
		if((20 <= scenarioTime) && (scenarioTime < 26))
			$("#intro_letter" + (scenarioTime-20)).show();
		else if((26 <= scenarioTime) && (scenarioTime < (26+4*7)))
			$("#intro").css("background-color", colors[floor(scenarioTime-26)/4]);
		else if(26+4*7+3 == scenarioTime) {
			$(".intro_letters").animate({'left': '190px', 'opacity': '0'}, 1000);
			$("#intro_logo2").fadeIn(1000);
		}
		else if(26+4*7+15 == scenarioTime)
			$("#intro_logo3").fadeIn(500);
		else if(26+4*7+20 == scenarioTime)
			$("#intro_logo4").fadeIn(500);
		scenarioTime++;
	}, 60);
	*/
}