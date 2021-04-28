#ifndef RATIONCREATEWINDOW_H
#define RATIONCREATEWINDOW_H

#include <QDialog>

namespace Ui {
class RationCreateWindow;
}

class RationCreateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RationCreateWindow(QWidget *parent = nullptr);
    ~RationCreateWindow();

private slots:
    void on_pushButton_createration_clicked();

private:
    Ui::RationCreateWindow *ui;
};

#endif // RATIONCREATEWINDOW_H
