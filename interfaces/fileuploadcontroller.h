/**
  @file
  @author Stefan Frings
*/

#ifndef FILEUPLOADCONTROLLER_H
#define FILEUPLOADCONTROLLER_H

#include "interfaces/http/httprequest.h"
#include "interfaces/http/httpresponse.h"
#include "interfaces/http/httprequesthandler.h"

class FileUploadController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(FileUploadController);
public:

    /** Constructor */
    FileUploadController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);

signals:
    void fileUploaded(QString, QString, QTemporaryFile*);
};

#endif // FILEUPLOADCONTROLLER_H
