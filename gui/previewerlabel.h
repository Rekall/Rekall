#ifndef PREVIEWERLABEL_H
#define PREVIEWERLABEL_H

#include <QLabel>

class PreviewerLabel : public QLabel {
    Q_OBJECT
public:
    explicit PreviewerLabel(QWidget *parent = 0);

signals:
    void mouseReleased();
    void mouseDoubleClick();

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
};

#endif // PREVIEWERLABEL_H
