/**
  @file
  @author Stefan Frings
*/

#include "fileuploadcontroller.h"

FileUploadController::FileUploadController() {}

void FileUploadController::service(HttpRequest& request, HttpResponse& response) {
    QMapIterator<QByteArray,QByteArray> paramsIterator(request.getParameterMap());
    while(paramsIterator.hasNext()) {
        paramsIterator.next();
        qDebug("\t%s = %s", qPrintable(paramsIterator.key()), qPrintable(paramsIterator.value()));
    }
    foreach(QTemporaryFile *file, request.getUploadedFiles())
        emit(fileUploaded(request.getParameter("gps"), request.getParameter("file"), file));

    if(request.getParameter("action") == "show") {
        if(request.getUploadedFiles().count())  response.write("ok");
        else                                    response.write("upload failed");
    }
    else {
        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        response.write("<html><body>");
        response.write("Upload a JPEG image file<p>");
        response.write("<form method=\"post\" enctype=\"multipart/form-data\">");
        response.write("  <input type=\"hidden\" name=\"action\" value=\"show\">");
        response.write("  File: <input type=\"file\" name=\"file1\"><br>");
        response.write("  <input type=\"submit\">");
        response.write("</form>");
        response.write("</body></html>",true);
    }
}

