var rekall = new Rekall();
var rekall_common = new Object();
$(document).ready(function() {
	rekall_common.owner = {"canEdit": false, "author": "", "locationGps": "", "locationName": ""};

	rouletteStart();
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"status": 1, "password": getParameterByName("password")},
		success: function(infos) {
			rekall_common = infos;
			if(rekall_common.owner.canEdit) {
				//Mode preview or not
				$(".editmode").removeClass("editmode");   
				
				setEditionControls();
				
				//Geoloc en mode édition
				if(navigator.geolocation) {
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
				}

			}
			rouletteEnd();
			rekall.loadXMLFile();
		},
		error: function() {
			rouletteEnd();
		}
	});
	   

		
  
	    
	
	$("#popupAlertSpace").click(function(event){  
		event.stopPropagation();      
	});
	$("#popupAlertButtonOk").click(function(event){  
		event.stopPropagation(); 
		$("#popupAlertSpace").hide();
	});
   
 	$("#popupSpace").click(function(event){  
		event.stopPropagation();  
		closeEdit();              
	});                    
	$("#closePopupEdit").click(function(event){
		event.stopPropagation();  
		closeEdit(); 
	});
	
	$("#popupAlertButtonCancel").click(function(){
		closeAlert();
	});      
	
	$("#popupAlertButtonYesdelete").click(function(){
		var keyDoc = $("#popupRight").attr("keyDoc");
		deleteFromDom(keyDoc);
	});       
	

	
});    

function setEditionControls() {  
	
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
	   
	                                           
	$("#left_menu_item_addnote").click(function(event){  
		event.stopPropagation();   
		uploadFiles(["New note"]);   
	});
	
	
	$("#popupEdit").click(function(event){  
		event.stopPropagation(); 
		closeInputs();
	});
	            
	$("#popupNom").click(function(event){  
		event.stopPropagation();     
		closeInputs();
		$(this).hide();
		$("#popupNomInput").show().focus(); 
	});       
	
	$("#popupTC").click(function(event){        
		event.stopPropagation();     
		closeInputs();
		$("#popupTC").hide();
		$("#popupTCedit").show();//.focus(); 
	});     
	
	$(".popupTCeditfield").click(function(event){
		event.stopPropagation();
	});     
	$("#nowTCin").click(function(event){
		event.stopPropagation();                                                            
		var timeCurrent = convertToTime(Math.round(rekall.timeline.timeCurrent));   
		$("#popupTCinMin").val(timeCurrent.split(":")[0]); 
		$("#popupTCinSec").val(timeCurrent.split(":")[1]); 
	});         
	$("#nowTCout").click(function(event){
		event.stopPropagation();                                                             
		var timeCurrent = convertToTime(Math.round(rekall.timeline.timeCurrent));
		$("#popupTCoutMin").val(timeCurrent.split(":")[0]); 
		$("#popupTCoutSec").val(timeCurrent.split(":")[1]); 
	});                                                
	
	$("#TCvalidModif").click(function(event){
		event.stopPropagation();      
		var keyDoc = $(this).parent().parent().attr("keydoc");    
		var inMin = $("#popupTCinMin").val();
		var inSec = $("#popupTCinSec").val();
		var outMin = $("#popupTCoutMin").val();
		var outSec = $("#popupTCoutSec").val();
		var TCin = (inMin*60)+(inSec*1);   
		var TCout = (outMin*60)+(outSec*1);   
		                                                                         
		var endVideo = rekall.videoPlayer.duration();
		
		if(TCin>TCout) 			openAlert("Start time must be set before end time", "ok"); 
		else if(TCout>endVideo) openAlert("End time must not be set after " + convertToTime(endVideo) + " (end of the video)", "ok");  
		else {
			setTCFromDom(keyDoc, TCin, TCout); 
		
			$("#popupTCin").html(inMin+":"+inSec);  
			$("#popupTCout").html(outMin+":"+outSec);  
			
			closeInputs();   
		}
	});
	$("#TCinvalidModif").click(function(event){
		event.stopPropagation();  
		var TCin = $("#popupTCin").html().split(":"); 
		var TCout = $("#popupTCout").html().split(":");   
		$("#popupTCinMin").val(TCin[0]);
		$("#popupTCinSec").val(TCin[1]);
		$("#popupTCoutMin").val(TCout[0]);
		$("#popupTCoutSec").val(TCout[1]);
		closeInputs();  
	});
	           
	
	$("#popupLegende").click(function(event){  
		event.stopPropagation();     
		closeInputs();
		$(this).hide();
		$("#popupLegendeInput").show().focus(); 
	});  
	
	
	$(".popupInput").click(function(event){
		event.stopPropagation();
	});   
	 
	
	$("#popupNomInput").keyup(function(event){  
		event.stopPropagation();              
		if(event.which == 13) {    
			var keyDoc = $(this).parent().attr("keydoc"); 
			var newName = $(this).val().trim();        
			$(this).val(newName);
			setMetaFromDom(keyDoc, "Rekall->Name", newName); 
			
			if(newName!="") $("#popupNom").html(newName).removeClass("empty"); 
			else $("#popupNom").html("Add a name").addClass("empty");     
			closeInputs(); 
		}
	});    
	 
	$("#popupLegendeInput").keyup(function(event){    
		event.stopPropagation(); 
		
		var isEnter = false; 
		
		if (event.key !== undefined) {
		       if (event.key === 'Enter' && event.altKey) {
		          //openAlert('Alt + Enter pressed!');
		       } else if(event.key === 'Enter') isEnter = true; 
		
		    } else if (event.keyIdentifier !== undefined) {
		       if (event.keyIdentifier === "Enter" && event.altKey) {
		          //openAlert('Alt + Enter pressed!');
		       } else if(event.keyIdentifier === 'Enter') isEnter = true;

		    } else if (event.keyCode !== undefined) {
		       if (event.keyCode === 13 && event.altKey) {
		          //openAlert('Alt + Enter pressed!');
		    } else if(event.keyCode === 13) isEnter = true; 
		}
		             
		if(isEnter == true) {    
			var keyDoc = $(this).parent().attr("keydoc"); 
			var newComment = $(this).val().trim();    
			$(this).val(newComment);
			setMetaFromDom(keyDoc, "Rekall->Comments", newComment.replace(/\n/gi, "<br/>")); 
			
			if(newComment!="") $("#popupLegende").html(newComment.replace(/\n/gi, "<br/>")).removeClass("empty"); 
			else $("#popupLegende").html("Add a comment").addClass("empty");     
			closeInputs(); 
		}
	});        
	 
	$("#popupSetHighlight").click(function(event){
		event.stopPropagation();
		var keyDoc = $(this).parent().attr("keydoc");
		var isHL = $(this).attr("isHighlight");  
		if(isHL=="true") {
			setMetaFromDom(keyDoc, "Rekall->Highlight", "");  
			$(this).html("&#9734;&nbsp;Highlight").attr("isHighlight", "false").removeClass("selected");  
		} else { 
			setMetaFromDom(keyDoc, "Rekall->Highlight", "true");  
			$(this).html("&#9733;&nbsp;Highlight").attr("isHighlight", "true").addClass("selected");   	
		}              
	});
	
	$("#popupEditSupprimer").click(function(){
		openAlert("Do youreally want to delete this file from the project ?", "yesnodelete");
	});       

	
}
             
function openAlert(message, buttons) {
	//Rétro-compatibilité Rekall-Pro
	if((message == undefined) && (buttons == undefined))
		closeAlert();
	
	$("#popupAlertMessage").html(message);  
	if(buttons == "nobuttons") {
		$(".popupAlertButton").hide();
	}
	else if(buttons == "yesnodelete") {   
		$(".popupAlertButton").hide();  
		$("#popupAlertButtonYesdelete").show(); 
		$("#popupAlertButtonCancel").show(); 
	}      
	else {
		$(".popupAlertButton").hide();  
		$("#popupAlertButtonOk").show(); 
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
	
	//if(rekall_common.owner.canEdit) { alert("edit mode !"); }      
	var isPaused = rekall.timeline.isPaused();  
	rekall.timeline.pause();   
                                    
	$("#popupEdit").css("background",tag.color).attr("isPaused",isPaused); 
	$("#popupTC").css("background",tag.color); 
	/*$("#popupType").css("color",tag.color);*/
	                                              
	if(tag.isMarker()==true){         
		$("#popupImg").show();   
		$("#popupImg").attr("src","css/images/img-note.png");  
		$("#popupImg").unbind( "click" );   
		
	} else {                
		if(tag.thumbnail.url){        
		   	$("#popupImg").attr("src",tag.thumbnail.url);    
			   
		} else {                   
			var type = tag.getMetadata("Rekall->Type");
			//alert(type);        
			if(type.indexOf("image") > -1) $("#popupImg").attr("src","css/images/img-image.png");  //alert("image");
			else if(type.indexOf("pdf") > -1) $("#popupImg").attr("src","css/images/img-pdf.png");  //alert("pdf"); 
			else if(type.indexOf("audio") > -1) $("#popupImg").attr("src","css/images/img-music.png");  //alert("son");  
			else if(type.indexOf("vcard") > -1) $("#popupImg").attr("src","css/images/img-user.png");  //alert("user"); 
			else if(type.indexOf("video") > -1) $("#popupImg").attr("src","css/images/img-video.png");  //alert("video");  
			else if(type.indexOf("msword") > -1) $("#popupImg").attr("src","css/images/img-word.png");  //alert("word");  
			else $("#popupImg").attr("src","css/images/img-document.png");  //alert(type);
		}    
		$("#popupImg").unbind( "click" );
		$("#popupImg").click(function(event){ 
			event.stopPropagation(); 
			tag.openBrowser(); 
		});                               
	}     
	 
	$("#popupType").html(tag.getMetadata("Rekall->Type"));        
	var name = tag.getMetadata("Rekall->Name");    
	if(name!="") $("#popupNom").html(name).removeClass("empty");
	else $("#popupNom").html("+ Add a name").addClass("empty");    

	var startVerb = convertToTime(tag.getTimeStart());
	$("#popupTCin").html(startVerb);       
	                                                 
	var endVerb = convertToTime(tag.getTimeEnd());  
	$("#popupTCout").html(endVerb);                  
	  
	var comments = tag.getMetadata("Rekall->Comments");
	if(comments!="") $("#popupLegende").html(comments).removeClass("empty");
	else $("#popupLegende").html("+ Add a comment").addClass("empty");        
	
	                                         
	if(rekall_common.owner.canEdit) {       
		if(tag.isMarker()==true) $("#popupEditSupprimer").html("&#10761;&nbsp;&nbsp;Delete Note");  
		else $("#popupEditSupprimer").html("&#10761;&nbsp;&nbsp;Delete File");  
		
		$("#popupNomInput").val(tag.getMetadata("Rekall->Name")); 
		
		$("#popupTCinMin").val(startVerb.split(":")[0]);
		$("#popupTCinSec").val(startVerb.split(":")[1]);
		
		$("#popupTCoutMin").val(endVerb.split(":")[0]);
		$("#popupTCoutSec").val(endVerb.split(":")[1]);
		
		$("#popupLegendeInput").html(comments.replace(/<br\/>/gi, '\n'));   
		
		var highlight = tag.getMetadata("Rekall->Highlight");        
		if(highlight=="true") $("#popupSetHighlight").html("&#9733;&nbsp;Highlight").attr("isHighlight","true").addClass("selected");  
		else $("#popupSetHighlight").html("&#9734;&nbsp;Highlight").attr("isHighlight","false").removeClass("selected");
	}
	
	
	$("#popupLeft").attr("keydoc",tag.document.key); 
	$("#popupRight").attr("keydoc",tag.document.key);        

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
	rouletteStart();
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"key": keyDoc, "metadataKey": metaType, "metadataValue": meta.replace(/'/g, '’')},
		success: function(retour) {
			rouletteEnd();
			rekall.loadXMLFile();
		},
		error: function() {
			openAlert("Server error. Try again.");
			rouletteEnd();
		}
	});	
}
function setTCFromDom(keyDoc, TCin, TCout) {
	rouletteStart();
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"key": keyDoc, "tcIn": TCin, "tcOut": TCout},
		success: function(retour) {
			rouletteEnd();
			rekall.loadXMLFile();
		},
		error: function() {
			openAlert("Server error. Try again.");
			rouletteEnd();
		}
	});	
}  

function deleteFromDom(keyDoc) {
	rouletteStart();
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"key": keyDoc, "remove": 1},
		success: function(retour) {
			deleteFromDomFinished();
			rouletteEnd();
			rekall.loadXMLFile();
		},
		error: function() {
			openAlert("Server error. Try again.");
			rouletteEnd();
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
			}
			var fileType     = (file.type.split("/"))[0];
			var fileDateTime = moment(file.lastModifiedDate);
			
			if (file.size > rekall_common.uploadMax) {
				openAlert("File size is too large! Maximum is " + rekall_common.uploadMax + " bytes.");
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
				file: 		 file,
				url:  		 'php/upload.php',
				type: 		 'POST',
				data:		 formData,
				cache:       false,
				contentType: false,
				processData: false,
				xhr: function() {
					var myXhr = $.ajaxSettings.xhr();
					if(myXhr.upload) {
						myXhr.upload.addEventListener('progress', function(event) {
							rouletteProgress(floor(event.loaded / event.total * 100));
						}, false);
					}
					return myXhr;
				},
				beforeSend: function(data) {
				},
				success:    function(data) {
					try {
						console.log(data);
						data = JSON.parse(data);
						Tag.keyToOpenAfterLoading = data.files[0].key;
						if(data.files[0].code <= 0)
							openAlert("Uploading error. " + data.files[0].error);
					}
					catch(err) {
						openAlert("Uploading error. Try again.");
					}
					window.document.title = "Rekall Online";
					fileIsUploading = false;

					rouletteEnd();
					uploadFilesNext();
				},
				error: function(data) {
					openAlert("Uploading error. Try again.");
					window.document.title = "Rekall Online";
					fileIsUploading = false;

					rouletteEnd();
					uploadFilesNext();
				}
			});
			uploadFilesNext();
		}
	});
}
function uploadFilesNext() {
	if(!fileIsUploading) {
		if(filesToUpload.length > 0) {
			fileIsUploading = true;
			rouletteStart(true);
			$.ajax(filesToUpload[0]);
			
			if(filesToUpload[0].file.name != undefined)
				openAlert("Starting upload of " + filesToUpload[0].file.name);
			else
				openAlert("Creation of " + filesToUpload[0].file + " in progress");
			
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
	console.log(embedUrl);
	openAlert("Embed code in console");
}

function rouletteStart(isProgress) {
	if(isProgress == undefined)
		console.log("Début de la roulette infinie");
	else
		console.log("Début de la roulette avec progression");
}
function rouletteProgress(progress) {
	console.log("Progression de la roulette @ " + progress + "%");
	window.document.title = "Téléchargement " + progress + "%";
}
function rouletteEnd() {
	console.log("Fin de la roulette");
	closeAlert();
}

function getParameterByName(name) {
    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
    return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

$(window).resize(function(e) {
	if(rekall.videoPlayer != undefined) {
		rekall.videoPlayer.width (($("#container").width() - $("#left_menu").width() - $("#flattentimeline").width() - 10) + "px");
		rekall.videoPlayer.height(($("#container").height()) + "px");
	}
}); 
	