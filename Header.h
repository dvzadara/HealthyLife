#pragma once
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING 
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "ortools/linear_solver/linear_solver.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;


// Класс представляет собой матрицу с составами продуктов
class Products
{
public:
	double matrix_products_compositions[500][18]; // Матрица с количеством веществ в продуктах(не больше 500 продуктов и 18 веществ)
	double products_portion_size[500]; // Информация о размере порции если равна 0 значит продукт не имеет размера порции
	double products_portion_info[500][2]; // Информация о ограничениях по количеству(min и max масса или min и max кол-во порций)
	double vector_product_costs[500]; // Вектор с ценами продуктов
	char* product_names[500]; // Вектор с именами продуктов
	int n = 0; // Количество продуктов


	// Конструктор класса
	// Принимает имя файла с составоми продуктов
	Products()
	{
		int a = 0;
	}
	Products(string filename)
	{
		setlocale(LC_ALL, "Russian");
		const char* str_products[500];

		ifstream in(filename); // окрываем файл с составами продуктов и считываем строки в str_products
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
		MPVariable** x; // Массив переменных x
		MPVariable** x_shtrih; // Массив бинарных переменных x`
		Products products_data;
		int N = 0; // Количество продуктов в решении
		int* products_mass; // Вектор решение с массами продуктов
		int* products_costs; // Вектор с ценами продуктов
		char** products_names; // Вектор с именами продуктов
		double* products_portion_size; // Информация о размере порции если равна 0 значит продукт не имеет размера порции

		create_diet(string filename, int proteins, int fats, int carbohydrates)
		{
			products_data = Products(filename); // Создание обьекта класса Products с списком продуктов
			setlocale(LC_ALL, "Russian");

			std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP")); // Создание решателя
			const double infinity = solver->infinity();
			this->x = new MPVariable * [products_data.n];
			this->x_shtrih = new MPVariable * [products_data.n];

			MPConstraint** const variables_restrictions_bot = new MPConstraint * [products_data.n]; // Создание ограничений для переменных x снизу
			MPConstraint** const variables_restrictions_top = new MPConstraint * [products_data.n]; // Создание ограничений для переменных x сверху
			for (int i = 0; i < products_data.n; i++)
			{
				if (products_data.products_portion_size[i] == 0.0) // Проверка вида порциональности
				{
					this->x[i] = solver->MakeNumVar(0, infinity, { 'x', char(i) }); // Создание переменной типа double
					this->x_shtrih[i] = solver->MakeBoolVar({ 'x', '`', char(i) }); // Создание переменной типа bin

					// Задание ограничений снизу и расставление коефициентов переменных x и x`
					variables_restrictions_bot[i] = solver->MakeRowConstraint(0.0, infinity, { char(i) });
					variables_restrictions_bot[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_bot[i]->SetCoefficient(this->x_shtrih[i], -products_data.products_portion_info[i][0]); // коефициент x` = -минимальное кол-во продукта i
					
					// Задание ограничений сверху и расставление коефициентов переменных x и x`
					variables_restrictions_top[i] = solver->MakeRowConstraint(-infinity, 0.0, { char(i) });
					variables_restrictions_top[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_top[i]->SetCoefficient(this->x_shtrih[i], -products_data.products_portion_info[i][1]); // коефициент x` = -максимальное кол-во продукта i
				}
				else
				{
					this->x[i] = solver->MakeIntVar(0, infinity, { 'x', char(i) }); // Создание переменной типа int
					this->x_shtrih[i] = solver->MakeBoolVar({ 'x', '`', char(i) }); // Создание переменной типа bon

					// Задание ограничений снизу и расставление коефициентов переменных x и x`
					variables_restrictions_bot[i] = solver->MakeRowConstraint(0.0, infinity, { char(i) });
					variables_restrictions_bot[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_bot[i]->SetCoefficient(this->x_shtrih[i], int(-products_data.products_portion_info[i][0])); // коефициент x` = -минимальное кол-во продукта i

					// Задание ограничений сверху и расставление коефициентов переменных x и x`
					variables_restrictions_top[i] = solver->MakeRowConstraint(-infinity, 0.0, { char(i) });
					variables_restrictions_top[i]->SetCoefficient(this->x[i], 1);
					variables_restrictions_top[i]->SetCoefficient(this->x_shtrih[i], int(-products_data.products_portion_info[i][1])); // коефициент x` = -максимальное кол-во продукта i
				}
			}

			const double substances_restrictions[18] = { proteins, fats, carbohydrates, 70, 1.5, 0, 17, 16, 900, 2400, 400, 1.8, 0, 3500, 1000, 1000, 300, 30 }; // Вектор ограничений по суммарному кол-ву веществ в рационе
			MPConstraint** const dietary_restriction = new MPConstraint * [18]; // Создание 18 ограничений по суммарному кол-ву веществ в рационе
			for (int i = 0; i < 3; i++) // Двухсторонние ограничения для белков, жиров и углеводов
			{
				dietary_restriction[i] = solver->MakeRowConstraint(substances_restrictions[i] * 0.95, substances_restrictions[i] * 1.05, { char(i) }); // Установление верхней и нижней границы для неравенства
				for (int j = 0; j < products_data.n; j++) // Добавление в неравенство переменных и коефициентов для них
				{
					if (products_data.products_portion_size[j] == 0.0)
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i]);
					else
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i] * products_data.products_portion_size[j]);
				}
			}

			for (int i = 3; i < 18; i++) // Нижние ограничения для витаминов и микроелементов
			{
				dietary_restriction[i] = solver->MakeRowConstraint(substances_restrictions[i], infinity, { char(i) }); // Установление нижней границы для неравенства
				for (int j = 0; j < products_data.n; j++)
				{
					if (products_data.products_portion_size[j] == 0.0) // Добавление в неравенство переменных и коефициентов для них
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i]);
					else
						dietary_restriction[i]->SetCoefficient(this->x[j], products_data.matrix_products_compositions[j][i] * products_data.products_portion_size[j]);
				}
			}

			MPObjective* const objective = solver->MutableObjective(); // Создание обьекта целевой функции
			for (int i = 0; i < products_data.n; i++) // Добавление переменных и их коефициентов в целевую функцию
			{
				if (products_data.products_portion_size[i] == 0.0) 
					objective->SetCoefficient(this->x[i], products_data.vector_product_costs[i]);
				else
					objective->SetCoefficient(this->x[i], products_data.vector_product_costs[i] * products_data.products_portion_size[i]);
			}
			objective->SetMinimization(); // 

			const MPSolver::ResultStatus result_status = solver->Solve(); // Создание обьекта рашения


			// cout << "Рацион:" << endl;
			this->products_mass = new int[products_data.n];
			this->products_costs = new int[products_data.n];
			this->products_names = new char* [products_data.n];
			this->products_portion_size = new double[products_data.n];
			// cout << "Optimal objective value = " << objective->Value() << endl;

			for (int i = 0; i < products_data.n; i++) // Заполение массивов переменных класса diet_creator
				if (this->x[i]->solution_value() != 0) // В массивы записываются продукты которые добавлены в рацион
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
			// Функция возращает массив размеров порций продуктов
		{
			return this->products_portion_size;
		}

		int get_number_of_products()
			// Функция возращает кол-во продуктов в рационе
		{
			return this->N;
		}

		char** get_products_names()
			// Функция возращает массив имен продуктов
		{
			return this->products_names;
		}

		int* get_products_mass()
			// Функция возращает массив масс продуктов
		{
			return this->products_mass;
		}

		int* get_products_costs()
			// Функция возращает массив цен за 100 гр продуктов
		{
			return products_costs;
		}
	};
}