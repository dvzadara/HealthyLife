#pragma once
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING 
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "ortools/linear_solver/linear_solver.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;


// ����� ������������ ����� ������� � ��������� ���������
class Products
{
public:
	double matrix_products_compositions[500][18]; // ������� � ����������� ������� � ���������(�� ������ 500 ��������� � 18 �������)
	double products_portion_size[500]; // ���������� � ������� ������ ���� ����� 0 ������ ������� �� ����� ������� ������
	double products_portion_info[500][2]; // ���������� � ������������ �� ����������(min � max ����� ��� min � max ���-�� ������)
	double vector_product_costs[500]; // ������ � ������ ���������
	char* product_names[500]; // ������ � ������� ���������
	int n = 0; // ���������� ���������


	// ����������� ������
	// ��������� ��� ����� � ��������� ���������
	Products()
	{
		int a = 0;
	}
	Products(string filename)
	{
		setlocale(LC_ALL, "Russian");
		const char* str_products[500];

		ifstream in(filename); // �������� ���� � ��������� ��������� � ��������� ������ � str_products
		if (in.is_open())
		{
			string line;
			getline(in, line);
			while (getline(in, line))
			{
				int line_leght = line.length();
				char split_line[23][50];
				int j = 0;
				int column = 0;
				for (int i = 0; i <= line_leght; i++)
				{
					if (line[i] != ""[0] && line[i] != ';')
					{
						split_line[column][j] = line[i];
						j++;
					}
					else
					{
						split_line[column][j] = ""[0];
						column++;
						j = 0;
					}
				}

				product_names[n] = new char[50];
				for(int i = 0; i < 50; i++)
					product_names[n][i] = split_line[0][i];

				vector_product_costs[n] = stod(split_line[1]);

				products_portion_size[n] = stod(split_line[2]);
				products_portion_info[n][0] = stod(split_line[3]);
				products_portion_info[n][1] = stod(split_line[4]);

				for (int i = 5; i < 23; i++)
				{
					matrix_products_compositions[n][i - 5] = stod(split_line[i]);
				}
				str_products[n] = line.c_str();
				n += 1;
			}
		}
		in.close();
	}
};


namespace operations_research {

	class create_diet
	{
	public:
		MPVariable** x; // ������ ���������� x
		MPVariable** x_shtrih; // ������ �������� ���������� x`
		Products products_data;
		int N = 0; // ���������� ��������� � �������
		int* products_mass; // ������ ������� � ������� ���������
		int* products_costs; // ������ � ������ ���������
		char** products_names; // ������ � ������� ���������
		double* products_portion_size; // ���������� � ������� ������ ���� ����� 0 ������ ������� �� ����� ������� ������

		create_diet(string filename, int proteins, int fats, int carbohydrates)
		{
			products_data = Products(filename); // �������� ������� ������ Products � ������� ���������
			setlocale(LC_ALL, "Russian");

			std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP")); // �������� ��������
			const double infinity = solver->infinity();
			this->x = new MPVariable * [products_data.n];
			this->x_shtrih = new MPVariable * [products_data.n];

			MPConstraint** const variables_restrictions_bot = new MPConstraint * [products_data.n]; // �������� ����������� ��� ���������� x �����
			MPConstraint** const variables_restrictions_top = new MPConstraint * [products_data.n]; // �������� ����������� ��� ���������� x ������
			for (int i = 0; i < products_data.n; i++)
			{
				if (products_data.products_portion_size[i] == 0.0) // �������� ���� ���������������
				{
					this->x[i] = solver->MakeNumVar(0, infinity, { 'x', char(i) }); // �������� ���������� ���� double
					this->x_shtrih[i] = solver->MakeBoolVar({ 'x', '`', char(i) }); // �������� ���������� ���� bin

					// ������� ����������� ����� � ������������ ������������ ���������� x � x`
					variables_restrictions_bot[i] = solver->MakeRowConstraint(0.0, infinity, { char(i) });
					variables_restrictions_bot[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_bot[i]->SetCoefficient(this->x_shtrih[i], -products_data.products_portion_info[i][0]); // ���������� x` = -����������� ���-�� �������� i
					
					// ������� ����������� ������ � ������������ ������������ ���������� x � x`
					variables_restrictions_top[i] = solver->MakeRowConstraint(-infinity, 0.0, { char(i) });
					variables_restrictions_top[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_top[i]->SetCoefficient(this->x_shtrih[i], -products_data.products_portion_info[i][1]); // ���������� x` = -������������ ���-�� �������� i
				}
				else
				{
					this->x[i] = solver->MakeIntVar(0, infinity, { 'x', char(i) }); // �������� ���������� ���� int
					this->x_shtrih[i] = solver->MakeBoolVar({ 'x', '`', char(i) }); // �������� ���������� ���� bon

					// ������� ����������� ����� � ������������ ������������ ���������� x � x`
					variables_restrictions_bot[i] = solver->MakeRowConstraint(0.0, infinity, { char(i) });
					variables_restrictions_bot[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_bot[i]->SetCoefficient(this->x_shtrih[i], int(-products_data.products_portion_info[i][0])); // ���������� x` = -����������� ���-�� �������� i

					// ������� ����������� ������ � ������������ ������������ ���������� x � x`
					variables_restrictions_top[i] = solver->MakeRowConstraint(-infinity, 0.0, { char(i) });
					variables_restrictions_top[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_top[i]->SetCoefficient(this->x_shtrih[i], int(-products_data.products_portion_info[i][1])); // ���������� x` = -������������ ���-�� �������� i
				}
			}

			const double substances_restrictions[18] = { proteins, fats, carbohydrates, 70, 1.5, 0, 17, 16, 900, 2400, 400, 1.8, 0, 3500, 1000, 1000, 300, 30 }; // ������ ����������� �� ���������� ���-�� ������� � �������
			MPConstraint** const dietary_restriction = new MPConstraint * [18]; // �������� 18 ����������� �� ���������� ���-�� ������� � �������
			for (int i = 0; i < 3; i++) // ������������� ����������� ��� ������, ����� � ���������
			{
				dietary_restriction[i] = solver->MakeRowConstraint(substances_restrictions[i] * 0.95, substances_restrictions[i] * 1.05, { char(i) }); // ������������ ������� � ������ ������� ��� �����������
				for (int j = 0; j < products_data.n; j++) // ���������� � ����������� ���������� � ������������ ��� ���
				{
					if (products_data.products_portion_size[j] == 0.0)
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i]);
					else
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i] * products_data.products_portion_size[j]);
				}
			}

			for (int i = 3; i < 18; i++) // ������ ����������� ��� ��������� � ��������������
			{
				dietary_restriction[i] = solver->MakeRowConstraint(substances_restrictions[i], infinity, { char(i) }); // ������������ ������ ������� ��� �����������
				for (int j = 0; j < products_data.n; j++)
				{
					if (products_data.products_portion_size[j] == 0.0) // ���������� � ����������� ���������� � ������������ ��� ���
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i]);
					else
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i] * products_data.products_portion_size[j]);
				}
			}

			MPObjective* const objective = solver->MutableObjective(); // �������� ������� ������� �������
			for (int i = 0; i < products_data.n; i++) // ���������� ���������� � �� ������������ � ������� �������
			{
				if (products_data.products_portion_size[i] == 0.0) 
					objective->SetCoefficient(this->x[i], products_data.vector_product_costs[i]);
				else
					objective->SetCoefficient(this->x[i], products_data.vector_product_costs[i] * products_data.products_portion_size[i]);
			}
			objective->SetMinimization(); // 

			const MPSolver::ResultStatus result_status = solver->Solve(); // �������� ������� �������


			// cout << "������:" << endl;
			this->products_mass = new int[products_data.n];
			this->products_costs = new int[products_data.n];
			this->products_names = new char* [products_data.n];
			this->products_portion_size = new double[products_data.n];
			// cout << "Optimal objective value = " << objective->Value() << endl;

			for (int i = 0; i < products_data.n; i++) // ��������� �������� ���������� ������ diet_creator
				if (this->x[i]->solution_value() != 0) // � ������� ������������ �������� ������� ��������� � ������
				{
					cout << "x" << i << " = " << this->x[i]->solution_value() << endl;
					if (products_data.products_portion_size[i] == 0.0)
					{
						this->products_mass[this->N] = int(x[i]->solution_value() * 100);
						this->products_costs[this->N] = products_data.vector_product_costs[i] * x[i]->solution_value();
					}
					else
					{
						this->products_mass[this->N] = int(x[i]->solution_value());
						this->products_costs[this->N] = products_data.vector_product_costs[i] * x[i]->solution_value() * products_data.products_portion_size[i];
					}
					this->products_names[this->N] = new char[50];
					for (int j = 0; j < 50; j++)
					{
						this->products_names[this->N][j] = char(products_data.product_names[i][j]);
					}
					this->products_portion_size[this->N] = products_data.products_portion_size[i];
					this->N++;
				}
		}


		double* get_products_portion_size()
			// ������� ��������� ������ �������� ������ ���������
		{
			return this->products_portion_size;
		}

		int get_number_of_products()
			// ������� ��������� ���-�� ��������� � �������
		{
			return this->N;
		}

		char** get_products_names()
			// ������� ��������� ������ ���� ���������
		{
			return this->products_names;
		}

		int* get_products_mass()
			// ������� ��������� ������ ���� ���������
		{
			return this->products_mass;
		}

		int* get_products_costs()
			// ������� ��������� ������ ��� �� 100 �� ���������
		{
			return products_costs;
		}
	};
}