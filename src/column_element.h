#include <Eigen/Dense>

class ColumnElement
{

private:

public:

};

class Drift:
{
private:
public:

	double z_min;
	double z_max;
	double radius;

};

class Screen:
{
private:
public:

	double z_loc;
};

class Aperture: public ColumnElement
{

private:

public:

	double z_min;
	double z_max;
	double r_minor;
	double r_major;

};


class EinzelLens: public ColumnElement
{

private:

public:

	double z_min;
	double z_max;
	double radius;

	double electrode1_length;
	double insulator1_length;
	double electrode2_length;
	double insulator2_length;
	double electrode3_length;

	double electrode1_voltage = 0;
	double electrode2_voltage = 1;
	double electrode3_voltage = 0;

	int element_z_resolution;
	int element_t_resolution;

	int charge_z_resolution;
	int charge_t_resolution;
	int charge_r_resolution;

	double charge_z_min;
	double charge_t_min;
	double charge_r_min;
	double charge_z_max;
	double charge_t_max;
	double charge_r_max;

	Eigen::VectorXd element_x_coordinates;
	Eigen::VectorXd element_y_coordinates;
	Eigen::VectorXd element_z_coordinates;
	Eigen::VectorXd element_voltage;

	Eigen::VectorXd charge_x_coordinates;
	Eigen::VectorXd charge_y_coordinates;
	Eigen::VectorXd charge_z_coordinates;
	Eigen::VectorXd charge_density;


	EinzelLens()
	{
		std::cout << "initializing lens" << std::endl;
	}

};

class Multipole: public ColumnElement
{

private:

public:

};