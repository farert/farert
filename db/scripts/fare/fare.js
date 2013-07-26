function divopenclose(objId){
	if(document.all){
		objIdStyle = document.all(objId).style;
	}
	else{
		if(document.getElementById){
			objIdStyle = document.getElementById(objId).style;
		}
	}
	if(objIdStyle){
		objIdStyle.display=='none'?objIdStyle.display='':objIdStyle.display='none';
	}
}

function bunhistory(strTitle, strName, intWidth, intHeight){
	var bunsnap;
	bunsnap=window.open("", "_blank", "toolbar=no, location=no, scrollbars=no, resizable=no, width=" + (intWidth + 24) + ", height=" + (intHeight + 34 + 56));
	bunsnap.document.open();
	bunsnap.document.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	bunsnap.document.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
	bunsnap.document.write("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"ja\" lang=\"ja\">\n\n");
	bunsnap.document.write("<head>\n");
	bunsnap.document.write("	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
	bunsnap.document.write("	<title>" + strTitle + "</title>\n");
	bunsnap.document.write("	<link rel=\"stylesheet\" type=\"text/css\" href=\"http://ni-a.jp/ni-after.css\" />\n");
	bunsnap.document.write("</head>\n\n");
	bunsnap.document.write("<body><div class=\"printbody\">\n");
	bunsnap.document.write("	<h1 class=\"printtitle\">" + strTitle + "</h1>\n");
	bunsnap.document.write("	<img src=\"png/" + strName + ".png\"");
	bunsnap.document.write(" alt=\"" + strTitle + "\" width=\"" + intWidth + "\" height=\"" + intHeight + "\" />\n");
	bunsnap.document.write("</div></body>\n");
	bunsnap.document.write("</html>\n");
	bunsnap.document.close();
}

function fncCalcFare(){
	var a1, a2, a3, dc2, dm, l, calcfareform;
	var fc1, fc2, fc3, fc4, fc5, fc6, fc7, fc8, fc9;

	l=1;
	a1=16.2;
	a2=12.85;
	a3=7.05;
	dc2=0;
	dm=1;

	calcfareform = document.forms['calcfare'];

	while(dc2 < calcfareform.d0.value){			// ex. d0 = 601
		if(dc2<6){
			l=3;
			dm=1;
		}
		else if(dc2<10){
			l=4;
			dm=0;
		}
		else if(dc2<50){
			l=5;
			dm=1;
		}
		else if(dc2<100){
			l=10;
			dm=0;
		}
		else if(dc2<600){
			l=20;
		}
		else{
			l=40;
		}
		dc2+=l;
	}
	dc1=dc2-l+1;
	calcfareform.d1.value=dc1;      // æŽÔŒ”‚Í dc1 ` dc2 ƒLƒ‹æŠÔ‚Å‚·.   601 ` 640
	calcfareform.d2.value=dc2;	// ‚»‚Ì•½‹Ï’l dc3 ‚©‚ç‹‚ß‚Ü‚·         620
	dc3=(dc2-(l-dm)/2)
	calcfareform.d3.value=dc3;
	if(dc3>=300){
		bc1=300;
		if(dc3>=600){
			bc2=300;
			bc3=dc3-600;
		}
		else {
			bc2=dc3-300;
			bc3=0;
		}
	}
	else {
		bc1=dc3;
		bc2=bc3=0;
	}
	calcfareform.b1.value=bc1;     // <16.2> * 300 + <12.85> * 300 + <7.05> * 20
	calcfareform.b2.value=bc2;
	calcfareform.b3.value=bc3;
	fc1=a1*bc1+a2*bc2+a3*bc3;
	calcfareform.f1.value=fc1;
	if(dc1<100){
		fc2=1;
		fc3="Ø‚èã‚°‚é";
		fc4=Math.round(fc1/10+.4999)*10;
		fc9=1;
	}
	else {
		fc2=10;
		fc3="ŽlŽÌŒÜ“ü‚·‚é";
		fc4=Math.round(fc1/100)*100;
		fc9=Math.round(dc2/200+.4999)+1;
	}
	calcfareform.f2.value=fc2;
	calcfareform.f3.value=fc3;
	calcfareform.f4.value=fc4;
	fc5=fc4*1.05;
	calcfareform.f5.value=fc5;
	fc6=Math.round(fc5/10)*10;
	calcfareform.f6.value=fc6;
	if(dc1<10){
		fc7="‚µ‚©‚µ“Á—á‚É‚æ‚è‚±‚Ì‹æŠÔ‚Ì‰^’À‚Í";
		if(dc1<3){
			fc8=140;
		}
		else if(dc1<6){
			fc8=180;
		}
		else {
			fc8=190;
		}
	}
	else {
		fc7="‚æ‚Á‚Ä‚±‚Ì‹æŠÔ‚Ì•’ÊæŽÔŒ”‚Ì‰^’À‚Í";
		fc8=fc6;
	}
	calcfareform.f7.value=fc7;
	calcfareform.f8.value=fc8;
	calcfareform.f9.value=fc9;
}