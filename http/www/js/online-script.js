var rekall = new Rekall();
var rekall_common = new Object();
$(document).ready(function() {
	rekall_common.owner = {"canEdit": false, "author": "", "locationGps": "", "locationName": ""};

	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"status": 1},
		success: function(infos) {
			rekall_common = infos;
			if((rekall_common.owner.canEdit) && (navigator.geolocation))
				navigator.geolocation.getCurrentPosition(function(position) {
					rekall_common.owner.locationGps = position.coords.latitude + "," + position.coords.longitude;
					$.ajax("http://maps.googleapis.com/maps/api/geocode/json", {
						type: "GET",
						dataType: "json",
						data: {"latlng": rekall_common.owner.locationGps},
						success: function(infos) {
							if((infos.results != undefined) && (infos.results[0] != undefined) && (infos.results[0].formatted_address != undefined))
								rekall_common.owner.locationName = infos.results[0].formatted_address;
						},
						error: function() {
						}
					});	
				});

			rekall.loadXMLFile();
		},
		error: function() {
		}
	});	                                                
	
	//Drag&drop files
	$(document).on({
		dragenter: function(event) {
			event.stopImmediatePropagation();
			event.preventDefault();                              
		},
		dragleave: function(event) {
			event.stopImmediatePropagation();
			event.preventDefault();   
		},
		dragover: function(event) {
			event.stopImmediatePropagation();
			event.preventDefault();  
		},
		drop: function(event) {    
			if(event.originalEvent.dataTransfer.files.length) {
				event.stopImmediatePropagation();
				event.preventDefault();
				uploadFiles(event.originalEvent.dataTransfer.files);
			}
		}
	});   
	                     
	
	$("#flattentimeline").on({
		dragenter: function(event) {
			//$(".flattentimeline_item").removeClass("draggable").addClass("drag");
		},
		dragleave: function(event) {
			$("#flattentimeline").removeClass("draggable").removeClass("drag"); 
		},
		dragover: function(event) {
			$("#flattentimeline").removeClass("draggable").addClass("drag"); 
		},
		drop: function(event) {
			$("#flattentimeline").removeClass("draggable").removeClass("drag");  
		}
	});
	
	$(document).keyup(function(event) {  
		if(event.keyCode == 77) // M
			uploadFiles(["Mon marker"]);
		else if(event.keyCode == 69) // E
			embed();
	});
	                   
	
	$("#popupAlertSpace").click(function(){  
		event.stopPropagation();      
	});
	$("#popupAlertButtonOk").click(function(){  
		event.stopPropagation(); 
		$("#popupAlertSpace").hide();
	});
   
 	$("#popupSpace").click(function(){  
		event.stopPropagation();  
		closeEdit();              
	});                    
	$("#closePopupEdit").click(function(){
		event.stopPropagation();  
		closeEdit(); 
	});
	
	$("#popupEdit").click(function(){  
		event.stopPropagation(); 
		closeInputs();
	});
	            
	$("#popupNom").click(function(){  
		event.stopPropagation();     
		closeInputs();
		$(this).hide();
		$("#popupNomInput").show().focus(); 
	});       
	
	$(".popupTCdisplay").click(function(){ 
		event.stopPropagation();     
		closeInputs();
		$("#popupTC").hide();
		$("#popupTCedit").show();//.focus(); 
	});     
	
	$(".popupTCeditfield").click(function(){
		event.stopPropagation();
	});     
	$("#nowTCin").click(function(){
		event.stopPropagation();                                                            
		var timeCurrent = convertToTime(Math.round(rekall.timeline.timeCurrent));   
		$("#popupTCinMin").val(timeCurrent.split(":")[0]); 
		$("#popupTCinSec").val(timeCurrent.split(":")[1]); 
	});         
	$("#nowTCout").click(function(){
		event.stopPropagation();                                                             
		var timeCurrent = convertToTime(Math.round(rekall.timeline.timeCurrent));
		$("#popupTCoutMin").val(timeCurrent.split(":")[0]); 
		$("#popupTCoutSec").val(timeCurrent.split(":")[1]); 
	});                                                
	
	$("#TCvalidModif").click(function(){
		event.stopPropagation();      
		var keyDoc = $(this).parent().parent().attr("keydoc");    
		var inMin = $("#popupTCinMin").val();
		var inSec = $("#popupTCinSec").val();
		var outMin = $("#popupTCoutMin").val();
		var outSec = $("#popupTCoutSec").val();
		var TCin = (inMin*60)+(inSec*1);   
		var TCout = (outMin*60)+(outSec*1);   
		                                                                         
		var endVideo = rekall.videoPlayer.duration();
		
		if(TCin>TCout) openAlert("Start time must be set before end time", "ok"); 
		else if(TCout>endVideo) openAlert("End time must not be set after "+convertToTime(endVideo)+" (end of the video)", "ok");  
		else {
			setTCFromDom(keyDoc, TCin, TCout); 
		
			$("#popupTCin").html(inMin+":"+inSec);  
			$("#popupTCout").html(outMin+":"+outSec);  
			
			closeInputs();   
		}
	});
	$("#TCinvalidModif").click(function(){
		event.stopPropagation();  
		var TCin = $("#popupTCin").html().split(":"); 
		var TCout = $("#popupTCout").html().split(":");   
		$("#popupTCinMin").val(TCin[0]);
		$("#popupTCinSec").val(TCin[1]);
		$("#popupTCoutMin").val(TCout[0]);
		$("#popupTCoutSec").val(TCout[1]);
		closeInputs();  
	});
	           
	
	$("#popupLegende").click(function(){  
		event.stopPropagation();     
		closeInputs();
		$(this).hide();
		$("#popupLegendeInput").show().focus(); 
	});  
	
	
	$(".popupInput").click(function(){
		event.stopPropagation();
	});   
	 
	
	$("#popupNomInput").keyup(function(e){  
		event.stopPropagation();              
		if(e.which == 13) {    
			var keyDoc = $(this).parent().attr("keydoc"); 
			var newName = $(this).val().trim();        
			$(this).val(newName);
			setMetaFromDom(keyDoc, "Rekall->Name", newName); 
			
			if(newName!="") $("#popupNom").html(newName).removeClass("empty"); 
			else $("#popupNom").html("Add a name").addClass("empty");     
			closeInputs(); 
		}
	});    
	 
	$("#popupLegendeInput").keyup(function(e){  
		event.stopPropagation();              
		if(e.which == 13) {    
			var keyDoc = $(this).parent().attr("keydoc"); 
			var newComment = $(this).val().trim(); 
			$(this).val(newComment);
			setMetaFromDom(keyDoc, "Rekall->Comments", newComment); 
			
			if(newComment!="") $("#popupLegende").html(newComment).removeClass("empty"); 
			else $("#popupLegende").html("Add a comment").addClass("empty");     
			closeInputs(); 
		}
	});        
	 
	$("#popupSetHighlight").click(function(){
		event.stopPropagation(); 
		var keyDoc = $(this).parent().attr("keydoc");
		var isHL = $(this).attr("isHighlight");  
		if(isHL=="true") {
			setMetaFromDom(keyDoc, "Rekall->Highlight", "");  
			$(this).html("Highlight").attr("isHighlight", "false");  
		} else { 
			setMetaFromDom(keyDoc, "Rekall->Highlight", "true");  
			$(this).html("Un-Highlight").attr("isHighlight", "true");   	
		}
	});
	
	$("#popupEditSupprimer").click(function(){
		openAlert("Do youreally want to delete this file from the project ?", "yesnodelete");
	});       
	
	$("#popupAlertButtonCancel").click(function(){
		closeAlert();
	});      
	
	$("#popupAlertButtonYesdelete").click(function(){
		var keyDoc = $("#popupRight").attr("keyDoc");
		deleteFromDom(keyDoc);
	});       
	
	
	
});
             
function openAlert(message,buttons) {
	$("#popupAlertMessage").html(message);  
	if(!buttons) {
		$(".popupAlertButton").hide();
	} else if(buttons=="ok") {   
		$(".popupAlertButton").hide();  
		$("#popupAlertButtonOk").show(); 
	} else if(buttons=="yesnodelete") {   
		$(".popupAlertButton").hide();  
		$("#popupAlertButtonYesdelete").show(); 
		$("#popupAlertButtonCancel").show(); 
	}      
	$("#popupAlertSpace").show(); 
}       

function closeAlert() {
	$("#popupAlertMessage").html(""); 
	$(".popupAlertButton").hide();         
	$("#popupAlertSpace").hide(); 
}

function closeInputs() {
   	$(".popupInput").hide();
	$(".popupRightItem").show();  
	$("#popupTC").show();
	$("#popupTCedit").hide();
}       

function closeEdit() {
	closeInputs();   
	var isPaused = $("#popupEdit").attr("isPaused");   
	if(isPaused=="false") rekall.timeline.play();
	$("#popupSpace").hide();   
	$("#popupEdit").hide();
}                      
                           
function fillPopupEdit(tag) {                
	
	var isPaused = rekall.timeline.isPaused();  
	rekall.timeline.pause();   
	
	//alert(tag.document.key);  
	$("#popupEdit").css("background",tag.color).attr("isPaused",isPaused);     
	                         
	if(tag.thumbnail.url){        
		$("#popupImg").show();   
		$("#popupImg").attr("src",tag.thumbnail.url);  
		$("#popupImg").unbind( "click" );
		$("#popupImg").click(function(){
			tag.openBrowser(); 
		});     
	} else {
		$("#popupImg").hide(); 
	}                     
	
	$("#popupType").html(tag.getMetadata("Rekall->Type"));        
	var name = tag.getMetadata("Rekall->Name");    
	if(name!="") $("#popupNom").html(name).removeClass("empty");
	else $("#popupNom").html("Add a name").addClass("empty");  
	$("#popupNomInput").val(tag.getMetadata("Rekall->Name"));
	
	var startVerb = convertToTime(tag.getTimeStart());
	$("#popupTCin").html(startVerb);       
	$("#popupTCinMin").val(startVerb.split(":")[0]);
	$("#popupTCinSec").val(startVerb.split(":")[1]);
	
	var endVerb = convertToTime(tag.getTimeEnd());  
	$("#popupTCout").html(endVerb);      
	$("#popupTCoutMin").val(endVerb.split(":")[0]);
	$("#popupTCoutSec").val(endVerb.split(":")[1]);  
	
	var comments = tag.getMetadata("Rekall->Comments");
	if(comments!="") $("#popupLegende").html(comments).removeClass("empty");
	else $("#popupLegende").html("Add a comment").addClass("empty"); 
	$("#popupLegendeInput").html(comments);       
	
	var highlight = tag.getMetadata("Rekall->Highlight");        
	if(highlight=="true") $("#popupSetHighlight").html("Un-Highlight").attr("isHighlight","true");
	else $("#popupSetHighlight").html("Highlight").attr("isHighlight","false");
	
	$("#popupLeft").attr("keydoc",tag.document.key); 
	$("#popupRight").attr("keydoc",tag.document.key); 
	//tag.getMetadata("Rekall->Highlight") = "true" ou ""    
	
	$("#popupSpace").show();   
	$("#popupEdit").show();
}
          
function convertToTime(seconds) {    
	var minutes = Math.floor(seconds/60); 
	seconds = Math.floor(seconds-(minutes*60));   
	if(minutes<10) minutes="0"+minutes;
	if(seconds<10) seconds="0"+seconds;   
	var time = minutes+":"+seconds;
	return time;
}       


function setMetaFromDom(keyDoc, metaType, meta) {
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"key": keyDoc, "metadataKey": metaType, "metadataValue": meta.replace(/'/g, '’')},
		success: function(retour) {
			rekall.loadXMLFile();
		},
		error: function() {
			alert("Erreur lors de la mise à jour…");
		}
	});	
}
function setTCFromDom(keyDoc, TCin, TCout) {
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"key": keyDoc, "tcIn": TCin, "tcOut": TCout},
		success: function(retour) {
			rekall.loadXMLFile();
		},
		error: function() {
			alert("Erreur lors de la mise à jour…");
		}
	});	
}  

function deleteFromDom(keyDoc) {
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"key": keyDoc, "remove": 1},
		success: function(retour) {
			deleteFromDomFinished();
			rekall.loadXMLFile();
		},
		error: function() {
			alert("Erreur lors de la mise à jour…");
		}
	});	
}
function deleteFromDomFinished() {
	closeAlert(); 
	closeEdit();
}
        
                  
//Gestion d'upload        
var filesToUpload = [], fileIsUploading = false;
function uploadFiles(files) {
	$.each(files, function(index, file) {
		var formData = new FormData();
		
		if(file.name != undefined) {
			var colorCategory = rekall.sortings["colors"].categories[Sorting.prefix + file.type];
			if(colorCategory != undefined) {
				//alert(colorCategory.color);
			}
			var fileType     = (file.type.split("/"))[0];
			var fileDateTime = moment(file.lastModifiedDate);
			//alert("Chargement de " + file.name + " (" + fileType + ", " + file.size + " octets, date du " + fileDateTime.format("YYYY:MM:DD HH:mm:ss") + ")");
			
			alert(rekall_common.uploadMax);
			if (file.size > rekall_common.uploadMax) {
				alert("File size too large!");
				return;
			}
			
			//Données du formulaire
			if($('form')[0] != undefined)
				formData = new FormData($('form')[0]); //à vérifier
			else {
				formData.append("fileToUpload", file);
				formData.append("date", fileDateTime.format("YYYY:MM:DD HH:mm:ss"));
			}
		}
		else {
			formData.append("name", file);
		}
		
		formData.append("tc", 			rekall.timeline.timeCurrent);
		formData.append("author",       rekall_common.owner.author);
		formData.append("locationGps",  rekall_common.owner.locationGps);
		formData.append("locationName", rekall_common.owner.locationName);
		
		if(formData != undefined) {
			filesToUpload.push({
				url: 'php/upload.php',
				type: 'POST',
				xhr: function() {
					var myXhr = $.ajaxSettings.xhr();
					if(myXhr.upload) {
						myXhr.upload.addEventListener('progress', function(event) {
							window.document.title = "Téléchargement " + floor(event.loaded / event.total * 100) + "%";
						}, false);
					}
					return myXhr;
				},
				beforeSend: function(data) {
					//alert("beforeSend : " + data);
				},
				success:    function(data) {
					try {
						console.log(data);
						data = JSON.parse(data);
						if(data.files[0].code > 0)
							alert(data.files[0].metas["Rekall->Name"] + " téléchargé");
						else
							alert(data.files[0].error);
					}
					catch(err) {
						
					}
					window.document.title = "Rekall Online";
					fileIsUploading = false;
					uploadFilesNext();
				},
				error:      function(data) {
					alert("Erreur de téléchargement");
					window.document.title = "Rekall Online";
					fileIsUploading = false;
					uploadFilesNext();
				},
				data:		 formData,
				cache:       false,
				contentType: false,
				processData: false
			});
			uploadFilesNext();
		}
	});
}
function uploadFilesNext() {
	if(!fileIsUploading) {
		if(filesToUpload.length > 0) {
			fileIsUploading = true;
			$.ajax(filesToUpload[0]);
			filesToUpload.splice(0, 1);
			uploadFilesNext();
		}
		else {
			rekall.loadXMLFile();
		}
	}
}

//Code d'embarquement du projet Rekall
function embed() {
	var width = 960, height = round(width * 0.44);
	var embedUrl = '<iframe src="' + rekall.baseUrl + '" width="' + width + '" height="' + height + '" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe>';
	embedUrl += '<p><a href="' + rekall.baseUrl + '">' + "Mon Projet Rekall" + '</a></p>';
	alert("Code exporté dans la console");
	console.log(embedUrl);
}

function showInRuban(texte, time) {
	alert(texte);
}

function getParameterByName(name) {
    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
    return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

$(window).resize(function(e) {
	if(rekall.videoPlayer != undefined) {
		rekall.videoPlayer.width (($("#container").width() - $("#flattentimeline").width() - 5) + "px");
		rekall.videoPlayer.height(($("#container").height()) + "px");
	}
});
	