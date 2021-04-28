#include "Header.h"
#include "rationcreatewindow.h"
#include "ui_rationcreatewindow.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <sstream>

RationCreateWindow::RationCreateWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RationCreateWindow)
{
    ui->setupUi(this);

    this->setFixedSize(191,145);

    ui->line_proteins->setText("0"); //set value to 0
    ui->line_fats->setText("0");
    ui->line_carbohydrates->setText("0");

    ui->line_proteins->setInputMask("009"); //Mask 0-999
    ui->line_fats->setInputMask("009");
    ui->line_carbohydrates->setInputMask("009");



}

RationCreateWindow::~RationCreateWindow()
{
    delete ui;
}

void RationCreateWindow::on_pushButton_createration_clicked()
{
    int proteins = ui->line_proteins->text().toInt(); //input from textline, int
    int fats = ui->line_fats->text().toInt();
    int carbohydrates = ui->line_carbohydrates->text().toInt();

    //qDebug()<<proteins<<" "<<fats<<" "<<carbohydrates<<"\n";

    operations_research::create_diet diet = operations_research::create_diet("products list.csv", proteins, fats, carbohydrates);
    int n = diet.get_number_of_products(); // ���������� ���������
    char** products_names = diet.get_products_names(); // ������ ���� ���������
    int* products_costs = diet.get_products_costs();
    int* products_mass = diet.get_products_mass();
    int diet_cost = 0;

    
    string m_str = "";
    char s[4], s1[4];

    for (int i = 0; i < n; i++)
    {
        if (products_mass[i] != 0)
        {
            _itoa_s(products_mass[i], s, 10);
            _itoa_s(products_mass[i] * diet.products_portion_size[i] * 100, s1, 10);
            if (diet.products_portion_size[i] == 0)
                m_str += string(products_names[i]) + string(" ") + string(s) + string(u8"��\n");
            else
                m_str += string(products_names[i]) + string(" ") + string(s) + string(u8"�� (") + string(s1) + string(u8"��)\n");
            diet_cost += products_costs[i];
        }
    }
    _itoa_s(diet_cost, s, 10);
    m_str += string(u8"\n����: ") + string(s) + string(u8"���");
    if (n == 0)
        m_str = string(u8"��������� ������ ��������������� ����������� �� ������ ��������� �� ������ ����������.");

    QMessageBox ration;
    ration.setText(u8"������ �������:");
    ration.setInformativeText(m_str.c_str());
    ration.setStandardButtons(QMessageBox::Save | QMessageBox::Close);

    int res = ration.exec();
    if (res == QMessageBox::Save)
    {
        QMessageBox save;
        save.setText(u8"������ ������� ��������.");
        save.setStandardButtons(QMessageBox::Ok);
        save.exec();

        int num_ration = 1;
        while (1)
        {
            ifstream in(string("������ ������� ") + to_string(num_ration) + string(".txt"));
            if (in.is_open())
            {
                num_ration++;
            }
            else {
                ofstream out(string("������ ������� ") + to_string(num_ration) + string(".txt"));
                if (out.is_open())
                {
                    out << m_str.c_str();
                    break;
                }
            }
        }
    }

    this->close();
}
