#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING 
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rationcreatewindow.h"
#include "Header.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QDebug>
#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(640,480);
    connect(ui->pushButton_standartproductslist, SIGNAL(clicked()), this, SLOT(on_pushButton_standartproductlist_clicked()));


    setlocale(LC_ALL, "Russian");
    Products products_list = Products("products list.csv");
    int n = products_list.n;

    ui->tableWidget_products->setRowCount(200);
    for (int i = 0; i < n; i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString(products_list.product_names[i]));
        ui->tableWidget_products->setItem(i, 0, item);
        item = new QTableWidgetItem(QString::number(products_list.products_portion_size[i] * 100));
        ui->tableWidget_products->setItem(i, 1, item);
        if (products_list.products_portion_size[i] == 0)
        {
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][0] * 100));
            ui->tableWidget_products->setItem(i, 2, item);
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][1] * 100));
            ui->tableWidget_products->setItem(i, 3, item);
        }
        else
        {
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][0]));
            ui->tableWidget_products->setItem(i, 2, item);
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][1]));
            ui->tableWidget_products->setItem(i, 3, item);
        }
        item = new QTableWidgetItem(QString::number(products_list.vector_product_costs[i]));
        ui->tableWidget_products->setItem(i, 4, item);
        for (int j = 0; j < 18; j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(products_list.matrix_products_compositions[i][j], 'f', 3));
            ui->tableWidget_products->setItem(i, j + 5, item);
        }
    }
    for (int i = n; i < 200; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString(""));
            ui->tableWidget_products->setItem(i, j, item);
        }
    }
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_pushButton_createration_clicked()
{
    RationCreateWindow rcreatorwindow;
    rcreatorwindow.setModal(true);
    rcreatorwindow.exec();

}

void MainWindow::on_pushButton_standartproductlist_clicked()
{
    ifstream in("standart products list.csv"); // окрываем файл с составами продуктов и считываем строки в str_products
    if (in.is_open())
    {
        ofstream out("products list.csv"); // окрываем файл с составами продуктов и считываем строки в str_products
        if (out.is_open())
        {
            string line;
            while (getline(in, line))
            {
                out << line << "\n";
            }
            out.close();
        }
    }
    in.close();

    Products products_list = Products("products list.csv");
    int n = products_list.n;

    ui->tableWidget_products->setRowCount(200);
    for (int i = 0; i < n; i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString(products_list.product_names[i]));
        ui->tableWidget_products->setItem(i, 0, item);
        item = new QTableWidgetItem(QString::number(products_list.products_portion_size[i] * 100));
        ui->tableWidget_products->setItem(i, 1, item);
        if (products_list.products_portion_size[i] == 0)
        {
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][0] * 100));
            ui->tableWidget_products->setItem(i, 2, item);
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][1] * 100));
            ui->tableWidget_products->setItem(i, 3, item);
        }
        else
        {
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][0]));
            ui->tableWidget_products->setItem(i, 2, item);
            item = new QTableWidgetItem(QString::number(products_list.products_portion_info[i][1]));
            ui->tableWidget_products->setItem(i, 3, item);
        }
        item = new QTableWidgetItem(QString::number(products_list.vector_product_costs[i]));
        ui->tableWidget_products->setItem(i, 4, item);
        for (int j = 0; j < 18; j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(products_list.matrix_products_compositions[i][j], 'f', 3));
            ui->tableWidget_products->setItem(i, j + 5, item);
        }
    }
    for (int i = n; i < 200; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString(""));
            ui->tableWidget_products->setItem(i, j, item);
        }
    }

    QMessageBox ration;
    ration.setText(u8"Стандартный рацион восстановлен");
    //ration.setInformativeText(u8"Стандартный рацион восстановлен");
    ration.setStandardButtons(QMessageBox::Ok);
    ration.exec();

}

void MainWindow::on_pushButton_saveproductslist_clicked()
{
    ofstream fout("products list.csv"); 
    if (fout.is_open())
    {
        fout << u8"name;Цена;Порционность;;;Белки;Жиры;Углеводы;Витамин C;Витамин B1;Витамин E;Витамин B3 (PP);Железо;Витамин А;Натрий;Магний;Витамин B2;Калорийность;Калий;Кальций;Фосфор;Холестерин;Клетчатка\n";
        string line;
        for (int i = 0; i < 200; i++)
        {
            if (ui->tableWidget_products->item(i, 0)->text() != QString(""))
            {
                line = ui->tableWidget_products->item(i, 0)->text().toStdString();
                fout << line;
                
                line = ui->tableWidget_products->item(i, 4)->text().toStdString();
                replace(line.begin(), line.end(), '.', ',');
                fout << ';';
                if (ui->tableWidget_products->item(i, 4)->text() != QString(""))
                    fout << line;
                else
                    fout << u8'0';

                char buffer[10];
                snprintf(buffer, sizeof(buffer), "%f", double(ui->tableWidget_products->item(i, 1)->text().toInt()) / 100);
                line = string(buffer);
                replace(line.begin(), line.end(), '.', ',');
                fout << ';' << line;
                if (ui->tableWidget_products->item(i, 1)->text() == QString("0") || ui->tableWidget_products->item(i, 1)->text() == QString(""))
                {
                    for (int j = 2; j < 4; j++)
                    {
                        snprintf(buffer, sizeof(buffer), "%f", double(ui->tableWidget_products->item(i, j)->text().toInt()) / 100);
                        line = string(buffer);
                        replace(line.begin(), line.end(), '.', ',');
                        fout << ';' << line;
                    }
                }
                else
                {
                    for (int j = 2; j < 4; j++)
                    {
                        line = ui->tableWidget_products->item(i, j)->text().toStdString();
                        replace(line.begin(), line.end(), '.', ',');
                        fout << ';';
                        if (ui->tableWidget_products->item(i, j)->text() != QString(""))
                            fout << line;
                        else
                            fout << u8'0';
                    }
                }



                for (int j = 5; j < 23; j++)
                {
                    line = ui->tableWidget_products->item(i, j)->text().toStdString();
                    replace(line.begin(), line.end(), '.', ',');
                    fout << ';';
                    if (ui->tableWidget_products->item(i, j)->text() != QString(""))
                        fout << line;
                    else
                        fout << u8'0';
                }
                fout << '\n';
            }
        }
    }
    fout.close();

    QMessageBox ration;
    ration.setText(u8"Рацион сохранен");
    //ration.setInformativeText(u8"Стандартный рацион восстановлен");
    ration.setStandardButtons(QMessageBox::Ok);
    ration.exec();
}