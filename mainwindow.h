#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow // Клас QMainWindow, з функціями головного вікна
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_createration_clicked();
    void on_pushButton_standartproductlist_clicked();
    void on_pushButton_saveproductslist_clicked();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
