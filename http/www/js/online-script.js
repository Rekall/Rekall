var rekall = new Rekall();
var rekall_common = new Object();
$(document).ready(function() {
	rekall_common.owner = {"canEdit": false, "author": "", "locationGps": "", "locationName": ""};

	rouletteStart();
	var httpGetData = {"status": 1};
	if(getParameterByName("p") != "")
		httpGetData.p = (CryptoJS.SHA1(getParameterByName("p")) + "").toUpperCase();
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: httpGetData,
		success: function(infos) {
			rekall_common = infos;
			if(getParameterByName("w") != "")
				rekall_common.owner.canEdit = false;
			
			if(rekall_common.owner.canEdit) {
				//Mode preview or not
				$(".editmode").removeClass("editmode");     
			    $(".empty").show();
			    $("#watermark").hide();
				
				setEditionControls();
				
				//Geoloc en mode édition
				if((false) && (navigator.geolocation)) {
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

			} else {                                                    
				$(".empty").hide();  
			    $("#watermark").show(); 
			} 
			
			rouletteEnd();
			rekall.loadXMLFile();  

		},
		error: function() {
			rouletteEnd();
			rekall.loadXMLFile();
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
	         
	
	$("#left_menu_item_settings").click(function(event){  
		event.stopPropagation(); 
		$("#popupSettingsSpace").show();         
		                     
		if(rekall.project.metadata["Title"]!="") $("#popupSettingsTitle").html(rekall.project.metadata["Title"]).removeClass("empty"); 
		else $("#popupSettingsTitle").html("+ Add project title").addClass("empty");
		
		if(rekall.project.metadata["Author"]!="") $("#popupSettingsAuthor").html(rekall.project.metadata["Author"]).removeClass("empty"); 
		else $("#popupSettingsAuthor").html("+ Add project author").addClass("empty");
		
		if(rekall.project.metadata["Email"]!="") $("#popupSettingsEmail").html(rekall.project.metadata["Email"]).removeClass("empty"); 
		else $("#popupSettingsEmail").html("+ Add email address").addClass("empty");
		
		if(rekall.project.metadata["Comments"]!="") $("#popupSettingsCredits").html(rekall.project.metadata["Comments"]).removeClass("empty"); 
		else $("#popupSettingsCredits").html("+ Add project credits").addClass("empty");      
		
		if(rekall_common.owner.canEdit) {                    
	    	$(".empty").show();         
		}
		                                                                             
	});   
	
	$("#popupSettingsTitle").click(function(event){  
		event.stopPropagation();     
		closeSettingsInputs();
		$(this).hide();   
		if(!$(this).hasClass("empty")) $("#popupSettingsTitleInput").val($(this).html());
		$("#popupSettingsTitleInput").show().focus(); 
	});    
	$("#popupSettingsAuthor").click(function(event){  
		event.stopPropagation();     
		closeSettingsInputs();
		$(this).hide();          
		if(!$(this).hasClass("empty")) $("#popupSettingsAuthorInput").val($(this).html());
		$("#popupSettingsAuthorInput").show().focus(); 
	});  
	$("#popupSettingsEmail").click(function(event){  
		event.stopPropagation();     
		closeSettingsInputs();
		$(this).hide();            
		if(!$(this).hasClass("empty")) $("#popupSettingsEmailInput").val($(this).html());
		$("#popupSettingsEmailInput").show().focus(); 
	});       
	$("#popupSettingsCredits").click(function(event){  
		event.stopPropagation();     
		closeSettingsInputs();
		$(this).hide();      
		if(!$(this).hasClass("empty")) $("#popupSettingsCreditsInput").val($(this).html());
		$("#popupSettingsCreditsInput").show().focus(); 
	});                
	
	$(".popupSettingsInput").keyup(function(event){  
		event.stopPropagation();              
		if(event.which == 13) {                                                                      
			closeSettingsInputs();
		}
	});
	
	$("#popupSettings").click(function(event){  
		event.stopPropagation(); 
		closeSettingsInputs();
	});
	
	
	$("#popupSettingsSpace").click(function(event){  
		event.stopPropagation(); 
		closeSettingsPopup();
	});  
	
	$("#left_menu_item_preview").click(function(event){  
		event.stopPropagation();
		window.open("?w=1", '_blank');
		/*window.open("?w=1", "Preview Rekall", "menubar=no, status=no, scrollbars=no, menubar=no, width=1150, height=560");  */
	});
	$("#popupSettingsBtnEmbed").click(function(event){  
		event.stopPropagation();
		var tmp = shareEmbed();  
		openAlert("textarea",tmp);
	});
	$("#popupSettingsBtnShare").click(function(event){  
		event.stopPropagation();    
		var tmp = shareLink();  
		openAlert("input",tmp);
	});
	$("#popupSettingsBtnDownloadXml").click(function(event){  
		event.stopPropagation();
		window.open("php/project.php?downloadXML=1", '_self'); 
	});
	$("#popupSettingsBtnDelete").click(function(event){  
		event.stopPropagation();
		alert("DELETE");
	});                               
	                                           
	$("#left_menu_item_addnote").click(function(event){  
		event.stopPropagation();   
		uploadFiles(["New note"]);   
	}); 

	$("#left_menu_item_btn_addfile").change(function(event){   
		event.stopPropagation();                   
		uploadFiles($("#left_menu_item_btn_addfile").get(0).files);  
	});      
	
	$("#left_menu_item_addlink").click(function(event){   
		event.stopPropagation();   
		$("#popupAddLinkSpace").show();  
		$("#popupAddLinkInput").focus();
	});       
	    
	$("#popupAddLinkButtonCancel").click(function(event){   
		event.stopPropagation();            
		closeAddLinkPopup();
	});    
	$("#popupAddLinkSpace").click(function(event){   
		event.stopPropagation();            
		closeAddLinkPopup();
	});
	
	$("#popupAddLinkButtonOk").click(function(event){ 
		var myLink = $("#popupAddLinkInput").val();    
		addLink(myLink);
	}); 
	
	$("#popupAddLinkInput").keyup(function(event){  
		event.stopPropagation();              
		if(event.which == 13) {     
			var myLink = $("#popupAddLinkInput").val();
		    addLink(myLink);
		}
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
	
	$("#popupAuthor").click(function(event){  
		event.stopPropagation();     
		closeInputs();
		$(this).hide();
		$("#popupAuthorInput").show().focus(); 
	});      
	
	$("#popupLink").click(function(event){         
		event.stopPropagation();     
		closeInputs();
		$(this).hide();
		$("#popupLinkInput").show().focus();    
	});  
	
	
	$(".popupInput").click(function(event){
		event.stopPropagation();
	});   
	 
	
	$("#popupNomInput").keyup(function(event){  
		event.stopPropagation();              
		if(event.which == 13) {                                                      
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
			closeInputs(); 
		}
	});         
	
	$("#popupAuthorInput").keyup(function(event){  
		event.stopPropagation();              
		if(event.which == 13) {                                                       
			closeInputs(); 
		}
	});
	
	$("#popupLinkInput").keyup(function(event){       
		event.stopPropagation();              
		if(event.which == 13) {                                                        
			closeInputs(); 
		}                                         
	});
	 
	$("#popupSetHighlight").click(function(event){
		event.stopPropagation();
		var keyDoc = $(this).parent().attr("keydoc");
		var isHL = $(this).attr("isHighlight");  
		if(isHL=="true") {
			setMetaFromDom(keyDoc, "Rekall->Highlight", "");  
			$(this).attr("isHighlight", "false").removeClass("selected");  
			$("#popupEdit").removeClass("highlightPopup");
		} else { 
			setMetaFromDom(keyDoc, "Rekall->Highlight", "true");     
			var tmpColor = $("#popupNom").css("color");             
			$(this).attr("isHighlight", "true").addClass("selected");
			$("#popupEdit").addClass("highlightPopup"); 	
		}              
	});
	
	$("#popupEditSupprimer").click(function(){
		openAlert("Do you really want to delete this file from the project ?", "yesnodelete");
	});       

	
}  
         

function closeSettingsPopup() {
	$("#popupSettingsSpace").hide();
}

function addLink(url) {
	
	if(url.trim()=="") openAlert("Invalid URL");
	else {
		uploadFiles(["NewLink"+url]); 
		closeAddLinkPopup();
	}
}    

function closeAddLinkPopup() {
	$("#popupAddLinkInput").val("");  
	$("#popupAddLinkSpace").hide();
}


             
function openAlert(message, buttons) {
	//Rétro-compatibilité Rekall-Pro
	if((message == undefined) && (buttons == undefined))
		closeAlert();      
		
	if(message=="input"){      
		$("#popupAlertMessage").html("Copy this URL to share your project");      
		$(".popupAlertButton").hide();  
		$("#popupAlertButtonOk").show(); 
		
		$("#popupAlertInput").val(buttons);
		$("#popupAlertInput").show();           
		$("#popupAlertTextarea").hide(); 
		                                      
		$("#popupAlertSpace").show();     
		$("#popupAlertInput").focus().select();
		
	} else if(message=="textarea"){  
		$("#popupAlertMessage").html("Copy this code and paste it into your HTML for embed");
		$(".popupAlertButton").hide();  
		$("#popupAlertButtonOk").show(); 
		                                
		$("#popupAlertTextarea").val(buttons);
		$("#popupAlertInput").hide(); 
		$("#popupAlertTextarea").show();               
		                                  
		$("#popupAlertSpace").show();         
		$("#popupAlertTextarea").focus().select();
		
	} else {            
		$("#popupAlertInput").hide(); 
		$("#popupAlertTextarea").hide(); 
		
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
}       
function closeAlert() {
	$("#popupAlertMessage").html(""); 
	$(".popupAlertButton").hide();         
	$("#popupAlertSpace").hide(); 
}     

function closeSettingsInputs() {
	$.each($(".popupSettingsInput"), function() { 
		if($(this).css("display") != "none") {
			if($(this).attr("id")=="popupSettingsTitleInput") {                                           
				
   // 			var keyDoc = $(this).parent().attr("keydoc"); 
				var newName = $(this).val().trim();        
				$(this).val(newName);
   // 			setMetaFromDom(keyDoc, "Rekall->Name", newName); 

				if(newName!="") $("#popupSettingsTitle").html(newName).removeClass("empty"); 
				else $("#popupSettingsTitle").html("+ Add project name").addClass("empty");
				  
			} else if($(this).attr("id")=="popupSettingsAuthorInput") {  
				
	//			var keyDoc = $(this).parent().attr("keydoc"); 
				var newComment = $(this).val().trim();    
				$(this).val(newComment);
	//			setMetaFromDom(keyDoc, "Rekall->Comments", newComment.replace(/\n/gi, "<br/>")); 

				if(newComment!="") $("#popupSettingsAuthor").html(newComment.replace(/\n/gi, "<br/>")).removeClass("empty"); 
				else $("#popupSettingsAuthor").html("+ Add project author").addClass("empty"); 
				   
			} else if($(this).attr("id")=="popupSettingsEmailInput") {  
				
	  //  		var keyDoc = $(this).parent().attr("keydoc"); 
				var newAuthor = $(this).val().trim();        
				$(this).val(newAuthor);
	 //   		setMetaFromDom(keyDoc, "Rekall->Author", newAuthor); 

				if(newAuthor!="") $("#popupSettingsEmail").html(newAuthor).removeClass("empty"); 
				else $("#popupSettingsEmail").html("+ Add email address").addClass("empty");   
				  
			} else if($(this).attr("id")=="popupSettingsCreditsInput") {   
			
	//			var keyDoc = $(this).parent().attr("keydoc"); 
				var newLink = $(this).val().trim();        
				$(this).val(newLink);
	//			setMetaFromDom(keyDoc, "Rekall->Link", newLink); 

				if(newLink!="") $("#popupSettingsCredits").html(newLink).removeClass("empty"); 
				else $("#popupSettingsCredits").html("+ Add project credits").addClass("empty");
			
			}
		}
		
		//alert("ok"); 
	});         
   	$(".popupSettingsInput").hide();
	$(".popupSettingsTxt").show();  
}

function closeInputs() {  
	$.each($(".popupInput"), function() { 
		if($(this).css("display") != "none") {
			if($(this).attr("id")=="popupNomInput") { 
				
				var keyDoc = $(this).parent().attr("keydoc"); 
				var newName = $(this).val().trim();        
				$(this).val(newName);
				setMetaFromDom(keyDoc, "Rekall->Name", newName); 

				if(newName!="") $("#popupNom").html(newName).removeClass("empty"); 
				else $("#popupNom").html("+ Add a name").addClass("empty");
				  
			} else if($(this).attr("id")=="popupLegendeInput") {  
				
				var keyDoc = $(this).parent().attr("keydoc"); 
				var newComment = $(this).val().trim();    
				$(this).val(newComment);
				setMetaFromDom(keyDoc, "Rekall->Comments", newComment.replace(/\n/gi, "<br/>")); 

				if(newComment!="") $("#popupLegende").html(newComment.replace(/\n/gi, "<br/>")).removeClass("empty"); 
				else $("#popupLegende").html("+ Add a comment").addClass("empty"); 
				   
			} else if($(this).attr("id")=="popupAuthorInput") {  
				
				var keyDoc = $(this).parent().attr("keydoc"); 
				var newAuthor = $(this).val().trim();        
				$(this).val(newAuthor);
				setMetaFromDom(keyDoc, "Rekall->Author", newAuthor); 

				if(newAuthor!="") $("#popupAuthor").html(newAuthor).removeClass("empty"); 
				else $("#popupAuthor").html("+ Add an author").addClass("empty");   
				  
			} else if($(this).attr("id")=="popupLinkInput") {   
			
				var keyDoc = $(this).parent().attr("keydoc"); 
				var newLink = $(this).val().trim();    
				
				if(newLink.indexOf("http://")!=0) newLink = "http://"+newLink;
				    
				$(this).val(newLink);
				setMetaFromDom(keyDoc, "Rekall->Link", newLink); 

				if(newLink!="") {
					if(rekall_common.owner.canEdit) $("#popupLink").html(newLink).removeClass("empty");   
					else $("#popupLink").html("<a href='"+newLink+"' target='_blank'>"+newLink+"</a>").removeClass("empty");
				}
				else $("#popupLink").html("+ Add a link").addClass("empty");
			
			}
		}
		
		//alert("ok"); 
	});
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
                         
 //   var bgColor = "-webkit-linear-gradient(right bottom,  rgb(20,46,51) 0%, #757F81 150%)" ; //"rgb(20,46,51)";

	$("#popupEdit").attr("isPaused",isPaused);//.css("background",bgColor); 
	$("#popupTC").css("background",tag.color); 
	/*$("#popupType").css("color",tag.color);*/   
	                                                                                              
	var isOpera = !!window.opera || navigator.userAgent.indexOf(' OPR/') >= 0;  // Opera 8.0+ (UA detection to detect Blink/v8-powered Opera)
	var isFirefox = typeof InstallTrigger !== 'undefined';   // Firefox 1.0+
	var isSafari = Object.prototype.toString.call(window.HTMLElement).indexOf('Constructor') > 0;   // At least Safari 3+: "[object HTMLElementConstructor]"
	var isChrome = !!window.chrome && !isOpera;              // Chrome 1+
	var isIE = /*@cc_on!@*/false || !!document.documentMode; // At least IE6

	var bgColorLeft = tag.color.replace(/rgb/g, "rgba").replace(/\)/g, ",.35)");
	
    if(isOpera) bgColorLeft = "-o-linear-gradient(right bottom,  rgba(20,46,51,1) 0%, "+tag.color+" 100%)";
    else if(isFirefox) bgColorLeft = "-moz-linear-gradient(right bottom,  rgba(20,46,51,1) 0%, "+tag.color+" 100%)";          
    else if((isSafari)||(isChrome))  bgColorLeft = "-webkit-linear-gradient(right bottom,  rgba(20,46,51,1) 0%, "+tag.color+" 100%)"; 
	
	$("#popupLeft").css("background",bgColorLeft);
	                                              
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
	
	$("#popupNom").css("color",tag.color); 
	 
	$("#popupType").html(tag.getMetadata("Rekall->Type")).css("color",tag.color);         
	var name = tag.getMetadata("Rekall->Name");    
	if(name!="") $("#popupNom").html(name).removeClass("empty");
	else $("#popupNom").html("+ Add a name").addClass("empty");    

	var startVerb = convertToTime(tag.getTimeStart());
	$("#popupTCin").html(startVerb);       
	                                                 
	var endVerb = convertToTime(tag.getTimeEnd());  
	$("#popupTCout").html(endVerb);                  
	  
	var comments = tag.getMetadata("Rekall->Comments");
	if((comments)&&(comments!="")) $("#popupLegende").html(comments).removeClass("empty");
	else $("#popupLegende").html("+ Add a comment").addClass("empty"); 
	
	var author = tag.getMetadata("Rekall->Author");
	if((author)&&(author!="")) $("#popupAuthor").html(author).removeClass("empty");
	else $("#popupAuthor").html("+ Add an author").addClass("empty"); 
    
	var link = tag.getMetadata("Rekall->Link");
	if((link)&&(link!="")) {
		if(rekall_common.owner.canEdit) $("#popupLink").html(link).removeClass("empty");   
		else $("#popupLink").html("<a href='"+link+"' target='_blank'>"+link+"</a>").removeClass("empty");
	}
	else $("#popupLink").html("+ Add a link").addClass("empty");      
	
	                                         
	if(rekall_common.owner.canEdit) {       
		if(tag.isMarker()==true) $("#popupEditSupprimer").html("Delete Note");  
		else $("#popupEditSupprimer").html("Delete File");  
		                                  
    	$(".empty").show();            
		$("#watermark").hide();
		$("#popupNomInput").val(tag.getMetadata("Rekall->Name")); 
		
		$("#popupTCinMin").val(startVerb.split(":")[0]);
		$("#popupTCinSec").val(startVerb.split(":")[1]);
		
		$("#popupTCoutMin").val(endVerb.split(":")[0]);
		$("#popupTCoutSec").val(endVerb.split(":")[1]);
		
		$("#popupLegendeInput").val(""+comments.replace(/<br\/>/gi, '\n'));    
		$("#popupAuthorInput").val(""+author); 
		$("#popupLinkInput").val(""+link); 
		
		var highlight = tag.getMetadata("Rekall->Highlight");        
		if(highlight=="true") {
			$("#popupSetHighlight").attr("isHighlight","true").addClass("selected");   
			$("#popupEdit").addClass("highlightPopup"); 
		}
		else {
			$("#popupSetHighlight").attr("isHighlight","false").removeClass("selected");  
			$("#popupEdit").removeClass("highlightPopup"); 
		}
	} else {
		$(".empty").hide();
		$(".editmode").hide();
		$("#watermark").show();
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


function setProjectMeta(metaType, meta) {
	rouletteStart();
	$.ajax("php/project.php", {
		type: "POST",
		dataType: "json",
		data: {"metadataKey": metaType, "metadataValue": meta.replace(/'/g, '’')},
		success: function(retour) {
			rouletteEnd();
		},
		error: function() {
			openAlert("Server error. Try again.");
			rouletteEnd();
		}
	});	
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
			if($("uploadForm")[0] != undefined)
				formData = new FormData($('form')[0]); //à vérifier
			else {
				formData.append("fileToUpload", file);
				formData.append("date", fileDateTime.format("YYYY:MM:DD HH:mm:ss"));
			}
		}
		else if(file.indexOf("NewLink") == 0){
			formData.append("name", "NewLink");      
			formData.append("link", file.replace("NewLink",""));
			formData.append("type", "rekall/link");
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
					window.document.title = "Rekall";
					fileIsUploading = false;

					rouletteEnd();
					uploadFilesNext();
				},
				error: function(data) {
					openAlert("Uploading error. Try again.");
					window.document.title = "Rekall";
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
function shareEmbed() {
	var width = 960, height = round(width * 0.44);
	var embedUrl = '<iframe src="' + rekall.baseUrl + '" width="' + width + '" height="' + height + '" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe>';
	embedUrl += '<p><a href="' + rekall.baseUrl + '">' + "Mon Projet Rekall" + '</a></p>';
	console.log(embedUrl);
//	openAlert("Embed code in console");
	return embedUrl;
}
function shareLink() {
	var width = 960, height = round(width * 0.44);
	var embedUrl = rekall.baseUrl;
//	openAlert(embedUrl);
	return embedUrl;
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
		rekall.videoPlayer.width (($("#container").width() - ($("#left_menu").width()+3) - ($("#flattentimeline").width()+6)) + "px");
		rekall.videoPlayer.height(($("#container").height()) + "px");
	}
}); 
	